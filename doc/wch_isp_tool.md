# How to download firmware from PC to CH582 chip

1. Go to the WCH official website to download WCHISPTool download tool,
(https://www.wch-ic.com/download/WCHISPTool_Setup_exe.html)
Open it to install, the setting instructions are as follows:
- Select “CH32Fx series” chip;
- Chip Optional “CH32F103”;
- Select “Serial Port” in Download Type;
- Select “COM3” serial port displayed in step 4 in the serial device list, if not, click search;
- “User File” selects the firmware file (*.bin) to be burned;
- Pay attention to enable read protection settings when downloading configuration:
  - Check Enable read protection, after downloading the program successfully, you cannot use SWD to download the program, unless the read protection is lifted
protect. To prevent others from reading the contents of the flash program, you can check this setting.
  - Uncheck Enable read protection, after downloading the program successfully, you can continue to use SWD to download the program. If you are debugging a program and need to download and verify repeatedly, it is recommended not to check this setting.
- The above settings are shown in the figure 1
![WCHISPTool UI](/images/wch_isp_tool.jpg "WCHISPTool UI")

2. Use a USB type-C cable to connect the CH582 module to the PC.

3. Press and hold the boot button with a needle/paper clip, then insert the module into the keyboard (that is, turn the module on) while boot button is kept pressed, then release the boot button.

4. The firmware should start download automatically. There should be a prompt indicating that
the download is complete and the number of successes: 1, the download is successful, as
shown in the figure 4.

5. After the program is downloaded, the new firmware should start running automatically. If it didn't , power off then on the module again.

6. After the firmware download, the paired Bluetooth host information will be lost on the module, you need to "forget the device" on the Bluetooth host side, and pair again.
