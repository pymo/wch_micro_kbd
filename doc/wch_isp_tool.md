# How to download firmware from PC to CH582 chip

1. Go to the WCH official website to download WCHISPTool download tool,
(https://www.wch-ic.com/download/WCHISPTool_Setup_exe.html)
Open it to install, the setting instructions are as follows:
- Select “CH58x” series and "CH582" chip;
- Select “USB” in Download Port;
- Check "Automatic Download When Device Connect";
- "Object File1" selects the firmware file (*.hex) to be burned;
- Set the "Download Config" section as shown in Figure 1

![Figure 1. WCHISPTool UI](/doc/images/wch_isp_tool.jpg "Figure 1. WCHISPTool UI")

2. Use a USB type-C cable to connect the keyboard module to the PC.

3. Press and hold the reset button with a needle/paper clip, then insert the module into the keyboard (which is to turn the module on), then release the reset button.

![Figure 2. Enter Download mode](/doc/images/enter_download_mode.jpg "Figure 2. Enter Download mode")

4. The firmware should start download automatically. There should be a prompt indicating that the download has succeeded, as shown in Figure 2.

5. After the firmware download, the new firmware should start running automatically. If it didn't , re-attach the module to power cycle it.

6. After the firmware download, the Bluetooth pairing information will be lost on the module. You need to "forget the device" on the Bluetooth hosts it has paired with, and then pair again.
