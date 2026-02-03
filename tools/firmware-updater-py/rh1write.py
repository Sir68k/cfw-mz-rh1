from io import StringIO
import usb1
import libusb1
import struct
from time import sleep
import argparse
import hashlib
import sys
import os
import shlex
import cmd
from Crypto.Cipher import DES

_ord = ord
ord = lambda x: x if type(x) is int else _ord(x)

def dump(data):
    if isinstance(data, str):
        result = ' '.join(['%02x' % (ord(x), ) for x in data])
    else:
        result = ' '.join(['%02x' % (x, ) for x in data])

    return result

KNOWN_USB_ID_SET = frozenset([
     (0x054c, 0x0286), # Sony MZ-RH1
     (0x054c, 0x0223), # Sony MZ-RH1 in boot loader mode
     (0x054c, 0x017e), # RH1 in programming mode
     (0x054c, 0xffff), # HiMD in programming mode
])

ENDPOINT_READ = 0x80
BULK_WRITE_ENDPOINT = 0x2
INT_ENDPOINT = 0x81


# generic commands
CMD_DOWN     = (6, b"DOWN")   # Download data to device, expects 32-bit address, 32-bit length, followed by bulk data
CMD_DOWN_ENC = (1, b"DOWN")   # Same as above, but for encrypted data
CMD_EXEC     = (3, b"EXEC")   # Execute code at address, expects 32-bit address

# CXD2681 devices / AH10
CMD_WRITE    = (2, b"INTENTIALLY_LEFT_OUT") #WRIT

# CXD2683 / DH10P
CMD_CAMW = (64, b"CAMW")
CMD_DESW = (65, b"DESW")

# RH1
CMD_HASH     = (7, b"HASH")
CMD_VER      = (8, b"VER ")

# Constants for memory locations and defaults
DEFAULT_FLASHER_PATH = 'src/updater-device-code/build/updater.bin'

FLASHER_ADDR       = 0x0200_0000          # where to upload/execute flasher
FIRMWARE_ADDR      = 0x0210_0000          # where to upload firmware payload
FLASHER_WAIT_LOOPS = 200                  # number of readData polling iterations
READ_TIMEOUT       = 1                    # seconds between read attempts


def iterdevices(usb_context, bus=None, device_address=None):
    for device in usb_context.getDeviceList():
        if bus is not None and bus != device.getBusNumber():
            continue

        if device_address is not None and \
           device_address != device.getDeviceAddress():
            continue

        if (device.getVendorID(), device.getProductID()) in KNOWN_USB_ID_SET:
            yield UpdMD(device.open())


# ==== Hashing ====

def chunks(data, size=8):
    for i in range(0, len(data), size):
        yield data[i:i+size]

def xor_bytes(a, b):
    return bytes(x ^ y for x, y in zip(a, b))

HASH_KEY_DES = b'\x01\x23\x45\x67\x89\xab\xcd\xef'

def sony_hash(data):
    cipher = DES.new(HASH_KEY_DES, DES.MODE_ECB)
    iv = b'\x00' * 8
    for block in chunks(data[:len(data)//8*8], 8):  # drop remainder
        iv = cipher.encrypt(xor_bytes(iv, block))
    return iv[4:]

# ==== DFU Protocol ====

class UpdMD(object):
    def __init__(self, usb_handle, interface=0):
        self.usb_handle = usb_handle
        self.interface = interface

        usb_handle.setConfiguration(1)
        usb_handle.claimInterface(interface)


    def __del__(self):
        try:
            self.usb_handle.resetDevice()
            self.usb_handle.releaseInterface(self.interface)
        except: # Should specify an usb exception
            pass

    def readData(self, endpoint=ENDPOINT_READ):
        #controlRead(self, request_type, request, value, index, length, timeout=0):
        reply = self.usb_handle.controlRead(libusb1.LIBUSB_TYPE_VENDOR | \
                                            libusb1.LIBUSB_RECIPIENT_INTERFACE,
                                            endpoint, 0, 0, 4)
        return reply


    def interruptRead(self, count):
        reply = self.usb_handle.interruptRead(INT_ENDPOINT, count);
        return reply

 

    def sendCommand(self, command, args):
        endpoint = command[0]
        data     = bytes(command[1]) + bytes(args)

        self.usb_handle.controlWrite(libusb1.LIBUSB_TYPE_VENDOR | \
                                     libusb1.LIBUSB_RECIPIENT_INTERFACE,
                                     endpoint, 0, 0, data)

    def readBulk(self, length):
        result = StringIO()
        self.readBulkToFile(length, result)
        return result.getvalue()

    def readBulkToFile(self, length, outfile, chunk_size=0x10000, callback=lambda: None):
        """
          Read bulk data from device, and write it to a file.
          length (int)
            Length of data to read.
          outfile (str)
            Path to output file.
          chunk_size (int)
            Keep this much data in memory before flushing it to file.
        """
        done = 0
        while done < length:
            received = self.usb_handle.bulkRead(BULK_READ_ENDPOINT,
                min((length - done), chunk_size))
            done += len(received)
            outfile.write(received)
            callback(done)

    def writeBulk(self, data):
        """
          Write data to device.
          data (str)
            Data to write.
        """
        self.usb_handle.bulkWrite(BULK_WRITE_ENDPOINT, data)


def toBytes(n):
  return [
    (n >> 0)  & 0xFF,
    (n >> 8)  & 0xFF,
    (n >> 16) & 0xFF,
    (n >> 24) & 0xFF
  ]


READER_ADDR = 0x0201_0000            # where we upload read/write stub
READER_EXEC = 0x0201_0001            # entrypoint in read stub used to read memory
WRITER_EXEC = 0x0201_0005            # entrypoint in write stub used to write memory

# ldr r0, [r1]
# bx  lr
# str r1, [r2]
# bx  lr

CODE_READ_WRITE = [0x08, 0x68, 0x70, 0x47, 0x0a, 0x60, 0x70, 0x47, 0x0a, 0x70, 0x70, 0x47]

def ensure_reader(device):
    """Upload the small read/write stub"""
    code = bytes(CODE_READ_WRITE)
    device.sendCommand(CMD_DOWN, toBytes(READER_ADDR) + toBytes(len(code)))
    device.writeBulk(code)
    res = device.readData()
    print("wrote read/write routines:", dump(res))


def read_region(device, addr, length):
    """Read `length` bytes starting at `addr` using the read_stub previously uploaded
    """
    out = bytearray()
    pos = addr
    end = addr + length
    calls = 0
    while pos < end:
        if pos % 0x1000 == 0:
            print(f"read_region: reading at {hex(pos)} ({len(out)}/{length} bytes)")

        device.sendCommand(CMD_EXEC, toBytes(READER_EXEC) + toBytes(0) + toBytes(pos))
        #device.sendCommand(CMD_EXEC, toBytes(FLASHER_ADDR) + toBytes(pos))
        res = device.readData()
        if not res:
            break
        out.extend(res)
        pos += len(res)
        calls += 1
        if calls > (length // 4 + 1000):
            raise RuntimeError('read_region: too many calls, aborting')
    return bytes(out[:length])


def upload_blob_old(device, addr, data):
    device.sendCommand(CMD_DOWN, toBytes(addr) + toBytes(len(data)))
    device.writeBulk(bytes(data))
    res = device.readData()
    print(f"upload to {hex(addr)} ack:", dump(res))

def upload_blob(device, addr, data):
    device.sendCommand(CMD_DOWN, toBytes(addr) + toBytes(len(data)))

    CHUNK_SIZE = 0x10000
    total_size = len(data)
    offset = 0

    print(f"Starting upload: addr={hex(addr)}, size={total_size} bytes")

    while offset < total_size:
        chunk = data[offset:offset + CHUNK_SIZE]
        chunk_addr = addr + offset

        print(
            f"Uploading chunk: "
            f"offset={hex(offset)} "
            f"addr={hex(chunk_addr)} "
            f"size={len(chunk)} "
            f"({offset + len(chunk)}/{total_size})"
        )
        device.writeBulk(bytes(chunk))

        offset += len(chunk)

    res = device.readData()
    print(f"  ack:", dump(res))

    print("Upload complete")

def verify_region(device, addr, data, desc=None):
    if desc is None:
        desc = hex(addr)
    print(f"Verifying {hex(addr)} ({len(data)} bytes)")

    read_back = read_region(device, addr, len(data))
    if read_back != data:
        # find first mismatch
        for i, (a, b) in enumerate(zip(read_back, data)):
            if a != b:
                print(f"Mismatch at offset {i}: device=0x{a:02x} expected=0x{b:02x}")
                break
        else:
            if len(read_back) != len(data):
                print(f"Readback size mismatch: {len(read_back)} != {len(data)}")
        raise RuntimeError('verification failed')
    print("Verification OK")


def cmd_hash(device, addr, length):
    device.sendCommand(CMD_HASH, toBytes(addr) + toBytes(length))
    return device.readData()


def verify_region_with_hash(device, addr, data, desc=None):
    if desc is None:
        desc = hex(addr)
    print(f"Verifying {desc} at {hex(addr)} ({len(data)} bytes)")

    local_hash  = sony_hash(data)    
    device_hash = cmd_hash(device, addr, len(data))

    print("local hash: ", dump(local_hash))
    print("device hash: ", dump(device_hash))

    if local_hash == device_hash:
        print("Verification OK")
    else:
        raise RuntimeError('verification failed')


def parse_num(s):
    s = str(s).strip()
    if s.startswith('0x') or s.startswith('0X'):
        return int(s, 16)
    if s.isdigit():
        return int(s, 10)
    try:
        return int(s, 0)
    except Exception:
        raise ValueError('invalid number: %s' % s)


def _data_from_arg(arg):
    if os.path.exists(arg) and os.path.isfile(arg):
        with open(arg, 'rb') as f:
            return f.read()
    s = arg.replace('0x', '').replace('0X', '').replace(' ', '').replace('\n','').replace(':','').replace('_','')
    try:
        return bytes.fromhex(s)
    except Exception:
        raise ValueError('invalid data or file not found: %s' % arg)


class RH1Cmd(cmd.Cmd):
    """Interactive REPL for recovery operations"""
    intro = 'Entering recovery REPL. Type "help" for commands.'
    prompt = 'rh1> '

    def __init__(self, device):
        super().__init__()
        self.device = device

    def do_read(self, arg):
        """read <addr> [<len>] [out.bin] - read memory and optionally save to file"""
        try:
            parts = shlex.split(arg)
            if len(parts) < 1:
                print('usage: read <addr> [<len>] [out.bin]')
                return
            addr = parse_num(parts[0])
            length = parse_num(parts[1]) if len(parts) >= 2 else 4
            data = read_region(self.device, addr, length)
            if len(parts) >= 3:
                with open(parts[2], 'wb') as f:
                    f.write(data)
                print('Wrote', parts[2])
            else:
                print(dump(data))
        except Exception as e:
            print('Error:', e)

    def do_write(self, arg):
        """write <addr> <file-or-hex> - write bytes from file or hex string to addr"""
        try:
            parts = shlex.split(arg)
            if len(parts) < 2:
                print('usage: write <addr> <file-or-hex>')
                return
            addr = parse_num(parts[0])
            data = _data_from_arg(parts[1])
            upload_blob(self.device, addr, data)
        except Exception as e:
            print('Error:', e)

    def do_verify(self, arg):
        """verify <addr> <file-or-hex> - verify memory contents against file/hex"""
        try:
            parts = shlex.split(arg)
            if len(parts) < 2:
                print('usage: verify <addr> <file-or-hex>')
                return
            addr = parse_num(parts[0])
            data = _data_from_arg(parts[1])
            try:
                verify_region(self.device, addr, data, desc='REPL verify')
            except Exception as e:
                print('verify failed:', e)
        except Exception as e:
            print('Error:', e)

    def do_exec(self, arg):
        """exec <addr> [args...] - execute code at addr with optional 32-bit args"""
        try:
            parts = shlex.split(arg)
            if len(parts) < 1 or not parts[0]:
                print('usage: exec <addr> [args...]')
                return
            addr = parse_num(parts[0])
            args32 = []
            for a in parts[1:]:
                args32 += toBytes(parse_num(a))
            self.device.sendCommand(CMD_EXEC, toBytes(addr) + args32)
            print('exec sent, reading response...')
            try:
                res = self.device.readData()
                print('response:', dump(res))
            except Exception as e:
                print('no response or error:', e)
        except Exception as e:
            print('Error:', e)

    def do_hash(self, arg):
        """hash <addr> <len> - request device-side HASH"""
        try:
            parts = shlex.split(arg)
            if len(parts) < 2:
                print('usage: hash <addr> <len>')
                return
            addr = parse_num(parts[0])
            length = parse_num(parts[1])
            try:
                h = cmd_hash(self.device, addr, length)
                print('hash:', dump(h))
            except Exception as e:
                print('hash failed:', e)
        except Exception as e:
            print('Error:', e)

    def do_quit(self, arg):
        """quit - leave REPL"""
        return True

    def do_exit(self, arg):
        """exit - leave REPL"""
        return True

    def do_EOF(self, arg):
        """Handle EOF (Ctrl-D) to exit."""
        print('')
        return True

    def default(self, line):
        """Fallback for unknown commands."""
        cmd = line.split()[0] if line.strip() else ''
        if cmd:
            print('Unknown command:', cmd)


def repl_loop(device):
    repl = RH1Cmd(device)
    try:
        repl.cmdloop()
    except KeyboardInterrupt:
        print('\nExiting REPL')


def go():
    parser = argparse.ArgumentParser(description='Upload firmware and flasher, run flasher, and verify result')
    parser.add_argument('firmware', help='path to firmware binary')
    args = parser.parse_args()


    context = usb1.LibUSBContext()
    device = list(iterdevices(context))[0]

    # upload read/write stubs so we can read memory back for verification
    print('Uploading read/write helper to', hex(READER_ADDR))
    ensure_reader(device)

    if True:
      repl_loop(device)
      return

    # read firmware file
    with open(args.firmware, 'rb') as f:
        fw = f.read()

    # upload flasher
    with open(DEFAULT_FLASHER_PATH, 'rb') as f:
        flasher = f.read()

    print(f"Uploading flasher ({len(flasher)} bytes) to {hex(FLASHER_ADDR)}")
    upload_blob(device, FLASHER_ADDR, flasher)

    verify_region_with_hash(device, FLASHER_ADDR, flasher, desc='flasher (RAM)')

    # upload firmware
    print(f"Uploading firmware {args.firmware} ({len(fw)} bytes) to {hex(FIRMWARE_ADDR)}")
    upload_blob(device, FIRMWARE_ADDR, fw)

    #if True:
    #  repl_loop(device)
    #  return


    verify_region_with_hash(device, FIRMWARE_ADDR, fw, desc='firmware (RAM)')


    # execute flasher (pass source addr, length, and destination addr in registers r0,r1,r2)
    args.flash_dest = 0x0
    dest_for_flash = 0x0 # args.flash_dest
    exec_args = toBytes(FLASHER_ADDR) + toBytes(FIRMWARE_ADDR) + toBytes(len(fw)) + toBytes(dest_for_flash)
    print(f"Executing flasher at {hex(FLASHER_ADDR)} with src={hex(FIRMWARE_ADDR)}, len={len(fw)}, dest={hex(dest_for_flash)}")
    
    device.sendCommand(CMD_EXEC, toBytes(FLASHER_ADDR))

    for i in range(FLASHER_WAIT_LOOPS):
        try:
            res = device.readData()
        except Exception:
            sleep(READ_TIMEOUT)
            continue

        resp = int.from_bytes(res, byteorder='little', signed=False)
        resp_t = resp >> 28
        addr = resp & 0x00FFFFFF

        if resp_t == 1:
            print("erasing sector:", f"0x{addr:08x}")
        if resp_t == 2:
            print("writing sector:", f"0x{addr:08x}")

        if resp == 0xCAFEBABE:
            print("Flashing complete")
            break
        sleep(READ_TIMEOUT)

    print(f"Attempting to verify flashed contents at {hex(args.flash_dest)}")
    verify_region_with_hash(device, 0x0100_0000, fw, desc='firmware (FLASH)' )

    print('Done')

    #if args.repl:
    repl_loop(device)


if __name__ == '__main__':
    go()