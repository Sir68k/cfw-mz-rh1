#!/usr/bin/env python3

import fw_tools as fw
import time
import struct
import time
import sys

_ord = ord
ord = lambda x: x if type(x) is int else _ord(x)

def dump(data):
    if isinstance(data, str):
        result = ' '.join(['%02x' % (ord(x), ) for x in data])
    else:
        result = ' '.join(['%02x' % (x, ) for x in data])

    return result

def main(bus=None, device_address=None, show_uuids=False):
    md_iface = fw.connect()

    go(md_iface)


def factory_read_dram(md_iface, addr, size=0x10, s=False):
    DIV = 2368

    lo = int(addr % DIV)
    hi = int(addr / DIV)

    lo_a = (lo     ) & 0xFF
    lo_b = (lo >> 8) & 0xFF
    hi_a = (hi     ) & 0xFF
    hi_b = (hi >> 8) & 0xFF

    md_iface.sendFactoryCommand([0x18,0x24,0xff,hi_a,hi_b,lo_a,lo_b,size])
    reply = md_iface.readFactoryReply()

    if s:
        return reply[8:]
    else:
        return [ord(x) for x in reply[8:]]

def factory_write_dram(md_iface, addr, data, size=0x10):
    DIV = 2368

    lo = int(addr % DIV)
    hi = int(addr / DIV)

    lo_a = (lo     ) & 0xFF
    lo_b = (lo >> 8) & 0xFF
    hi_a = (hi     ) & 0xFF
    hi_b = (hi >> 8) & 0xFF

    md_iface.sendFactoryCommand([0x18,0x25,0xff,hi_a,hi_b,lo_a,lo_b,size] + data)
    return md_iface.readFactoryReply()

def patch(md_iface, addr, val, patch_nr = 7):
    addr_b = fw.getWordBytes(addr)

    base    = 0x01804000 + patch_nr * 0x10 # relative to dram base
    control = 0x01804100                   # relative to dram base

    # write 5, 12 to main control
    factory_write_dram(md_iface, control, [5], size=1)
    factory_write_dram(md_iface, control, [12], size=1)

    # write 0 to patch ctl/set
    factory_write_dram(md_iface, base, [0], size=1)

    # write patch addr
    factory_write_dram(md_iface, base + 4, addr_b, size=4)
    # write path val
    factory_write_dram(md_iface, base + 8, val[:4], size=4)

    # write 1 to patch ctl/set
    factory_write_dram(md_iface, base, [1], size=1)

    # write 5, 9 to main control
    factory_write_dram(md_iface, control, [5], size=1)
    factory_write_dram(md_iface, control, [9], size=1)


def install_exec_patch(md_iface):
    patch(md_iface, 0x0007db00, [0x12, 0x48, 0x00, 0x47], patch_nr=1)  #\xff\xf7\xfe\xff
    #patch(md_iface, 0x0007db00, [0xff, 0xf7, 0xfe, 0xff], patch_nr=1) 


def execute(md_iface, code, read_reply=True):
    md_iface.sendCommand([0x18,0xd2,0xff] + code)

    if read_reply:
        return md_iface.readReply_raw()
    else:
        return

def exec_func(md_iface, addr, arg1):
    call_func = [0x08,0x30,0x9f,0xe5,0x08,0x00,0x9f,0xe5,0x08,0x10,0x9f,0xe5,0x13,0xff,0x2f,0xe1,
        addr & 0xFF, (addr >> 8) & 0xFF, (addr >> 16) & 0xFF, (addr >> 24) & 0xFF,
        arg1 & 0xFF, (arg1 >> 8) & 0xFF, (arg1 >> 16) & 0xFF, (arg1 >> 24) & 0xFF]

    return execute(md_iface, call_func)


def exec_func3(md_iface, addr, a, b, c, xtra=0):
    call_func = [0x0c, 0x30, 0x9f, 0xe5, 0x0c, 0x00, 0x9f, 0xe5, 
                0x0c, 0x10, 0x9f, 0xe5, 0x0c, 0x20, 0x9f, 0xe5, 
                0x13, 0xff, 0x2f, 0xe1, 
                addr & 0xFF, (addr >> 8) & 0xFF, (addr >> 16) & 0xFF, (addr >> 24) & 0xFF,
                a & 0xFF, (a >> 8) & 0xFF, (a >> 16) & 0xFF, (a >> 24) & 0xFF,
                b & 0xFF, (b >> 8) & 0xFF, (b >> 16) & 0xFF, (b >> 24) & 0xFF,
                c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF, (c >> 24) & 0xFF] + xtra*[0]
    


    return execute(md_iface, call_func)


def exec_func4(md_iface, addr, a, b, c, d):
    call_func = [0x10, 0x40, 0x9f, 0xe5, 0x10, 0x00, 0x9f, 0xe5, 
                0x10, 0x10, 0x9f, 0xe5, 0x10, 0x20, 0x9f, 0xe5, 
                0x10, 0x30, 0x9f, 0xe5, 0x14, 0xff, 0x2f, 0xe1,
                addr & 0xFF, (addr >> 8) & 0xFF, (addr >> 16) & 0xFF, (addr >> 24) & 0xFF,
                a & 0xFF, (a >> 8) & 0xFF, (a >> 16) & 0xFF, (a >> 24) & 0xFF,
                b & 0xFF, (b >> 8) & 0xFF, (b >> 16) & 0xFF, (b >> 24) & 0xFF,
                c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF, (c >> 24) & 0xFF,
                d & 0xFF, (d >> 8) & 0xFF, (d >> 16) & 0xFF, (d >> 24) & 0xFF]

    return execute(md_iface, call_func)

    

def write_single(md_iface, addr, arg1, size = 4):
    if   size == 1:
        code = [0x08,0x00,0x9f,0xe5,0x08,0x10,0x9f,0xe5,0x00,0x10,0xc0,0xe5,0x1e,0xff,0x2f,0xe1]
    elif size == 2:
        code = [0x08,0x00,0x9f,0xe5,0x08,0x10,0x9f,0xe5,0xb0,0x10,0xc0,0xe1,0x1e,0xff,0x2f,0xe1]
    else:
        code = [0x08,0x00,0x9f,0xe5,0x08,0x10,0x9f,0xe5,0x00,0x10,0x80,0xe5,0x1e,0xff,0x2f,0xe1]

    code = code + [
        addr & 0xFF, (addr >> 8) & 0xFF, (addr >> 16) & 0xFF, (addr >> 24) & 0xFF,
        arg1 & 0xFF, (arg1 >> 8) & 0xFF, (arg1 >> 16) & 0xFF, (arg1 >> 24) & 0xFF]
    try:
        execute(md_iface, code)
    except:
        return


def toBytes(s):
    return [b for b in bytes(s)]


def set_bit(v, index, x):
  """Set the index:th bit of v to 1 if x is truthy, else to 0, and return the new value."""
  mask = 1 << index   # Compute mask, an integer with just bit 'index' set.
  v &= ~mask          # Clear the bit indicated by the mask (if x is False)
  if x:
    v |= mask         # If x was True, set the bit indicated by the mask.
  return v            # Return the result, we're done.







addr_memcpy = 0x000b7f08 | 1
data_offset = 32
addr_usb_buffer = 0x0200613c + data_offset


def read_fast(md_iface, src, size):
    res = exec_func3(md_iface, addr_memcpy, addr_usb_buffer, src, size, xtra=size-4)
    return toBytes(res[data_offset:data_offset+size])

def read_fast_raw(md_iface, src, size):
    res = exec_func3(md_iface, addr_memcpy, addr_usb_buffer, src, size, xtra=size-4)
    return res[data_offset:data_offset+size]

addr_read_eeprom = 0x000263f8 | 1

def read_eeprom(md_iface, src, size):
    res = exec_func3(md_iface, addr_read_eeprom, src, addr_usb_buffer, size, xtra=size-4)
    return toBytes(res[data_offset:data_offset+size])


def load_file_to_dram(md_iface, file, offset):
    file = open(file, 'rb')
    data = file.read()

    for i in range(0, len(data), 16):
        chunk = list(data[i:i+16])
        #print(offset + i, chunk, 16)
        factory_write_dram(md_iface, offset + i, chunk, 16)




def toBytesX(n):
  return [
    (n >> 0)  & 0xFF,
    (n >> 8)  & 0xFF,
    (n >> 16) & 0xFF,
    (n >> 24) & 0xFF
  ]

queue_disp_cmd  = 0xb6648 + 1;
import random


command_bytes = [0x03, 0x32, 0x30, 0xe0, 0xe2, 0xe7, 0x53, 0x56, 0x57, 0x21, 0x60, 0x61, 0x63, 0x65, 0x66]

def getRandom():
    found = False
    rnd   = 0
    while not found:
        rnd = random.randint(0, 0xFF)
        if not (rnd in command_bytes):
            break
    return rnd

def goo(md_iface):
    fw.prep_auth(md_iface, himd=True)

    install_exec_patch(md_iface)

    patch(md_iface, 0x0003d424, [0x70, 0x47, 0x00, 0x00], patch_nr=2) # bx lr; movs r0, r0
    patch(md_iface, 0x0003d498, [0,0,0,0], patch_nr=3)
    #patch(md_iface, 0x0003d39c, [0,0, 0x79, 0xf0], patch_nr=0)

    write_single(md_iface, 0x008057a5, 0, size=1)
    

    cmd = [
        0x32,
        0x01,
        0x30,
        0x1b
    ] + [
        0x11,
        0x01,

       # 0x57,
       # 0x01, #01 -> right, #02 <-left
       # 0x03, #speed 0-3
       # 0x05, #also speed?

    ]



    factory_write_dram(md_iface, 0x9010, [0xAA]*16)


    counter = 0x0
    while True:
        cmd = [
            0x30,0x1b,
            0xe0,
            0x02,
            0x04,
            0x00,
        ] + [ 0x8e, 0x9e, 0x8a, 0xd4]


        counter = (counter + 1) % 256
        

        amount = 5
        #cmd = cmd + [random.randint(1, 255) for i in range(amount)]
        print(dump(cmd))

        
        factory_write_dram(md_iface, 0x9000, cmd)

        
        for i in range(1):
            write_single(md_iface, 0x00802594, 0, size=1)
            exec_func3(md_iface, queue_disp_cmd, 0x0200_9000, len(cmd), 0)
            write_single(md_iface, 0x00802594, 1, size=1)   

        break
        #input()
        #time.sleep(0.1)
        if (counter % 100) == 0:
            print("==== break time ====")
            time.sleep(1)

    print("exec done")



def go_d(md_iface):
    fw.prep_auth(md_iface, himd=True)
    install_exec_patch(md_iface)


    factory_write_dram(md_iface, 0, [ 0x00, 0x30, 0x9f, 0xe5, 0x06, 0xf0, 0x5e, 0xe2, 0xef, 0xbe, 0xad, 0xde ]) # set r0 to deadbeef, skip instruction?
    patch(md_iface, 0x10, [0x02, 0xf4, 0xa0, 0xe3], patch_nr=0) #jump to 0x0200_0000




    #write_single(md_iface, 0x0081_4000, 0xCAFEBEBE)
    prev_valid = False

    for i in range(0x0000_0000, 0x1000_0000, 0x1000):
        if i % 0x100_0000 == 0:
            print("Scanning at %08x" % (i, ))

        res = read_fast(md_iface, i, 4)
        if res != [0xef, 0xbe, 0xad, 0xde]:
            #if not prev_valid:
            print("Found mapped at %08x: %s" % (i, dump(res)))
            prev_valid = True
        #elif prev_valid:
        #    print("End mapped at %08x" % (i))
        #    prev_valid = False
            
    print("done")


    #while False:
    #    time.sleep(1)


def go_ddd(md_iface):
    fw.prep_auth(md_iface, himd=True)
    install_exec_patch(md_iface)


    # todo: open file rom.bin, fast read 0x00000000 - 0x0080_0000 in chunks 0x10, write to file
    with open("rom_dump_0300_f000.bin", "wb") as f:
        for addr in range(0x0100_f000, 0x0101_0000, 0x10):
            data = factory_read_dram(md_iface, addr, 0x10, s=True)
            f.write(bytes(data))
            if addr % 0x10000 == 0:
                print("Dumped %08x" % (addr, ))

def randomByte():
    import random
    return random.randint(0, 255)

def go(md_iface):
    fw.prep_auth(md_iface, himd=True)
    install_exec_patch(md_iface)


    buff = 0x8023cc


    # start offset: 0x48
    # len maybe 0x4c
    # track id: 0x58 


    for xx in range(4):
        for i in range(0x0,128,4):
            write_single(md_iface, buff + i, 0)

        #for i in range(0x60,0x64,4):
        #    write_single(md_iface, buff + i, 1)
        
        write_single(md_iface, buff + 0x56, 0x01,size=1) # needs to be 1
        write_single(md_iface, buff + 0x60, 0x01,size=1) # needs to be 1
        #write_single(md_iface, buff + 0x58, 0x01,size=1) # track id
        #write_single(md_iface, buff + 0x73, 0x10,size=1) # should be 16

        #write_single(md_iface, buff + 0x60, 8, size=1)

        #write_single(md_iface, buff + 0x48, 0)
        #write_single(md_iface, buff + 0x4c, 0)

        write_single(md_iface, 0x0080064c, 0) 

        mode  = 0x1A
        val   = 0xA5
        dtype = 2
        idx = 6

        write_single(md_iface, 0x0080067c, mode) ## set mode

        f = 0xb2f74 | 1 
        res=exec_func3(md_iface, f, dtype, idx, val)

        #f = 0x0003b2f4 | 1
        #res=exec_func3(md_iface, f, 1, 6, 0)

        addr = buff
        res = read_fast_raw(md_iface, addr, 128)
        print("%08x: %s" % (addr, res.decode('sjis', errors='ignore')))

def go_dump_reg(md_iface):
    fw.prep_auth(md_iface, himd=True)
    install_exec_patch(md_iface)



    dest_register = 0x0080067c

    for i in range(128):
        write_single(md_iface, dest_register, i)
        # delay

        addr = 0x8023cc
        res = read_fast_raw(md_iface, addr, 256)
        print("%08x: %s" % (i, res.decode('sjis', errors='ignore')))
        print(dump(res))
        
        i = i + 1



if __name__ == '__main__':
    main()