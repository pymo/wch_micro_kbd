# WCH-Micro

A WCH CH582F based Bluetooth controller for wireless keyboards. It is ideal for converting Serial keyboard to Bluetooth keyboard. It includes a LiPo charging circuit. also a inverter to handle RS-232 level Serial. It is super thin so can be installed directly inside the keyboard you want to mod.

This board is used in two of my projects: [ppk_bluetooth](https://github.com/pymo/ppk_bluetooth), and [g750_bluetooth](https://github.com/pymo/g750_bluetooth). It can potentially be used to convert Serial mouse, or even PS/2 or ADB keyboard/mouse to Bluetooth, but you would need to implement the protocol yourself.

The firmware can also work on CH582-based dev boards with some modifications. For example, it can work on the Palm Ultra-thin Keybaord's Bluetooth board.

![PCB of WCH-Micro version 3](/wch_micro.jpg "PCB of WCH-Micro version 3")

How to compile
----------
Please refer to [the compilation guide](doc/compile.md).

How to flash the firmware
----------
Please refer to [the download guide](doc/wch_isp_tool.md).
