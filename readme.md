# MZ-RH1 Custom Firmware

This project implements firmware extensions for the MZ-RH1.

## Project Structure

- **`tools/`** - Firmware utility tools
  - `firmware-patcher/` - Patches firmware binaries
  - `firmware-updater-web/` - Web-based flasher interface
  - `firmware-updater-py/` - Python firmware update utilities

  
- **`src/`** - Device firmware source code
  - `extension-code/` - Custom firmware extensions for the RH1
  - `updater-device-code/` - Device-side updater firmware, responsible for unlocking, erasing, and flashing
  - `patches.toml` - Patches for the base firmware
  
- **`doc/`** - Documentation
  - `jtag/` - JTAG interface pinout and configuration
  
- **`base-fw/`** - Base firmware binaries
- **`build/`** - Build artifacts

## Usage

### Web Flasher

A web-based firmware flasher is available in `tools/firmware-updater-web/flasher.html` for a user-friendly update interface.

### Python Tools

The main firmware update tools are located in `tools/firmware-updater-py/`:

- `rh1trigger.py` - Enable DFU mode
- `rh1write.py` - Write firmware to the RH1 in DFU mode

This tool is experimental, and it used for testing a lot of random features. It is recommended to use the web flasher for most use cases.

### Requirements

- Python 3.7+
- libusb (for USB communication with RH1)
- pycryptodome (for firmware verification)

## Development

### Building Device Firmware

Device firmware is written in C and compiled for the CXD2687. Build configurations are located in `src/extension-code/` and `src/updater-device-code/`.

### Patching Firmware

Firmware modifications are configured through `src/patches.toml`. Use the firmware patcher to apply custom modifications:

```bash
python tools/firmware-patcher/patcher.py src/patches.toml
```

## References

- [Custom Firmware for MZ-RH1](https://sir68k.re/rh1/fw/)
- [Custom Firmware Development for MZ-RH1 - Ready for Testing!](https://sir68k.re/posts/rh1-firmware-available/)
- [RH1 Display Module Project](https://sir68k.re/rh1)

## License

See LICENSE file for details.
