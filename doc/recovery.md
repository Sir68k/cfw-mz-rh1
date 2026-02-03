# MZ-RH1 Firmware Recovery

- To recover an MZ-RH1 with corrupt firmware, you need to bridge HSALF and WDT to ground. See recovery.png for locating these points on the PCB.
- HSALF can also be found on the back of the PCB at TP8232, which is easier to solder to. However, it requires you to remove the entire board, which is not without risks either.
- When bridged, connect a battery and USB. The device should become available on the USB bus. If not, try pressing the REC button. Without battery the device will refuse to boot.
- From this point on you can flash the firmware as you would do normally. On Windows you need to make sure that the device is configured with Zadig, on Linux the udev config may need to be tweaked.
- After flashing, you should remove the bridges.