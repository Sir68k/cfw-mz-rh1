import tomllib
import hashlib
import struct
import sys
from pathlib import Path


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()


def load_symbols(symbols_path: Path) -> dict[str, int]:
    symbols = {}
    with symbols_path.open("r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            if len(parts) >= 3:
                addr_str, typ, name = parts[0], parts[1], ' '.join(parts[2:])
                try:
                    addr = int(addr_str, 16)
                    symbols[name] = addr
                except ValueError:
                    pass  # skip invalid lines
    return symbols


def resolve_symbol(value, symbols: dict[str, int]) -> int:
    if isinstance(value, str):
        value = value.strip()
        if '+' in value:
            parts = value.split('+', 1)
            sym = parts[0].strip()
            offset = int(parts[1].strip(), 0)
            if sym in symbols:
                return symbols[sym] + offset
            else:
                raise ValueError(f"Symbol '{sym}' not found in symbols")
        else:
            if value in symbols:
                return symbols[value]
            else:
                raise ValueError(f"Symbol '{value}' not found in symbols")
    elif isinstance(value, int):
        return value
    else:
        raise ValueError(f"Invalid value type: {type(value)}")


def load_replacement(patch: dict, symbols: dict[str, int], addr: int) -> bytes:
    kinds = [k for k in ("value", "bytes", "file", "chars", "bl", "b") if k in patch]
    if len(kinds) != 1:
        raise ValueError("Patch must contain exactly one of: value, bytes, file, chars, bl, b")

    if "value" in patch:
        val = resolve_symbol(patch["value"], symbols)
        return struct.pack("<I", val)

    if "bytes" in patch:
        return bytes(patch["bytes"])
    
    if "chars" in patch:
        return bytes(patch["chars"], "ascii")

    if "file" in patch:
        return Path(patch["file"]).read_bytes()

    if "bl" in patch:
        target = resolve_symbol(patch["bl"], symbols)
        offset = (target - (addr + 4)) // 2
        if offset < 0:
            raise ValueError(f"BL offset is negative")
        if offset > 0x3FFFFF:  # 22 bits max
            raise ValueError(f"BL offset too large")
        first  = 0xF000 | ((offset >> 11) & 0x3FF)
        second = 0xF800 | (offset & 0x7FF)
        return struct.pack("<HH", first, second)

    if "b" in patch:
        target = resolve_symbol(patch["b"], symbols)

        # Check if addr is aligned to 4 bytes (32-bit word boundary)
        if addr % 4 != 0:
            raise ValueError(f"'b' patch address {hex(addr)} is not 32-bit aligned")

        #   addr+0: ldr r2, [pc, #0]
        #   addr+2: bx r2
        #   addr+4: .word target

        ldr = 0x4A00 
        bx  = 0x4710

        return (
            struct.pack("<HH", ldr, bx) +
            struct.pack("<I", target)
        )

def main(cfg_path: Path):
    with cfg_path.open("rb") as f:
        cfg = tomllib.load(f)

    fw_cfg = cfg["firmware"]
    fw_path = Path(fw_cfg["path"])
    out_path = Path(fw_cfg["output"])

    symbols = {}
    if "symbols" in fw_cfg:
        symbols_path = Path(fw_cfg["symbols"])
        symbols = load_symbols(symbols_path)
        print(f"Loaded {len(symbols)} symbols from {symbols_path}")

    firmware = bytearray(fw_path.read_bytes())

    # --- validate firmware hash ---
    actual_hash = sha256_file(fw_path)
    expected_hash = fw_cfg["sha256"].lower()

    if actual_hash != expected_hash:
        raise RuntimeError(
            f"Firmware hash mismatch!\n"
            f"Expected: {expected_hash}\n"
            f"Actual:   {actual_hash}"
        )

    print("Firmware hash verified")

    # --- apply patches ---

    for idx, patch in enumerate(cfg["patch"], start=1):
        label = patch.get("name", f"patch_{idx}")
        addresses = patch["address"]
        if not isinstance(addresses, list):
            addresses = [addresses]

        for addr_val in addresses:
            addr = resolve_symbol(addr_val, symbols)
            data = load_replacement(patch, symbols, addr)
            end = addr + len(data)

            if end > len(firmware):
                raise RuntimeError(
                    f"[{label}] Out of range: {hex(addr)} + {len(data)} bytes"
                )

            # --- expected-original-bytes check ---
            if "original" in patch:
                o = patch["original"]
                if isinstance(o, str):
                    expected = bytes(o, "ascii")
                else:
                    expected = bytes(o)
                actual = firmware[addr : addr + len(expected)]
                if actual != expected:
                    raise RuntimeError(
                        f"[{label}] Original bytes mismatch at {hex(addr)}\n"
                        f"Expected: {expected.hex()}\n"
                        f"Actual:   {actual.hex()}"
                    )

            print(
                f"- [{label}] "
                f"{hex(addr)} .. {hex(end)} "
                f"({len(data)} bytes)"
            )

            firmware[addr:end] = data

    out_path.write_bytes(firmware)
    print(f"Patched firmware written to {out_path}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: patch_firmware.py patches.toml")
        sys.exit(1)

    main(Path(sys.argv[1]))
