# How to compile the code

The code is based on an example keyboard code from WCH. You can use the MounRiver IDE to compile it.

This guide is only for Windows. There is a Linux version of MounRiver, too. But you will have to do you own research if you would like to use Linux.

Install Moun River
----------

Download the [Windows Installer](http://mounriver.com/download) and follow instructions.

Download the code and open with MounRiver
----------

You can either use "Download ZIP" from the repository's web page, or use `git clone https://github.com/pymo/wch_micro_kbd.git` to download it to local disk.

Open MounRiver, go to File>Load->Project, choose the `src/APP/wch_micro_kbd.wvproj` file in the repository.

Compilation options
----------

There are several macros I defined for each type of the keyboard, you need to configure these macros before compiling.

![macros](/doc/images/project_properties.jpg "macros")

In MounRiver, go to Project->Properties->C/C++ Build->Settings, on the right side choose GNU RISC-V Cross C Compiler->Preprocessor, you can see a list of macros defined in the "Defined Symbols". I'll explain some relevant ones here:

| Macro | Value | Note | 
| :-------: | :-----------------------------------------------------------------: | :------: |
| `KEYBOARD_TYPE_PPK` or `KEYBOARD_TYPE_G750` or `KEYBOARD_TYPE_ULTRATHIN`| N/A | The category of keyboard you want to compile for |
| `PPK_TYPE_HANDSPRING` or `PPK_TYPE_JORNADA` | N/A | If you are compiling for the Handspring Visor version or Jornada version of the Palm portable keyboard, define this. `KEYBOARD_TYPE_PPK` must also be defined if this is defined. If no `PPK_TYPE_*` is defined, it will compile for all other PPK types. |
| `PCB_REV` | 1, 2 or 3 | Please check the PCB revision at the upper layer silkscreen of the PCB. (1) silkscreen `CH582F`: no DC-DC circuitry on PCB, also some GPIO wirings are different from later revisions. (2) silkscreen `CH582Fv2`: Pull down GPIO A14 to power on the keyboard. (3) silkscreen `CH582Fv3`: Pull up GPIO A14 to power on the keyboard|
| `KEYBOARD_LAYOUT_ISO` | N/A | If defined, will compile for the ISO (common in Europe) keyboard layout; if not defined, will compile for ANSI (US) layout.|
| `ENABLE_NUMLOCK` | N/A | If defined, will support the NumLock feature of the keyboard. Not recommended because numpad keys co-exist with normal keys and it will cause confusion.|
| `DEBUG` | N/A | If defined, will output serial debug information from TXD1. Don't enable this when compiling for release.|

(Optional) Customizing keyboard layout
--------------

You can change the `key8_table` variable and `init_fn_key_table()` function in `src/drivers/key_scan/key_table.c` to change the mapping, read the code's comment to learn more.

- `key8_table`: the mapping for normal keys
- `fn_key_table`: the mapping for fn shortcuts.

Compile
--------------
Use Project->Build All to build. If no compilation error happens, there will be a firmware generated at `src/APP/obj/wch_micro_kbd.hex`. This is the firmware that can be downloaded in the next step.

Download the firmware to the board
--------------

Please refer to [the download guide](wch_isp_tool.md).
