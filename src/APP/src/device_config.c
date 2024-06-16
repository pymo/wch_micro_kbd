#include "HAL_FLASH/include/easyflash.h"
#include "device_config.h"
#include "HAL.h"
#include "soc.h"
#include "hidkbd.h"
#include "hiddev.h"
#include "USB_task/USB_kbd.h"
#include "RF_task/rf_dev.h"
#include <string.h>
#include "test_dtm_port.h"
#include "mode/mode.h"

__attribute__((aligned(4))) uint8_t device_mode;
__attribute__((aligned(4))) Device_bond_t device_bond;
__attribute__((aligned(4))) Led_info_t device_led;
__attribute__((aligned(4))) RF_Pair_Info_t device_pair;


uint8_t SaveDeviceInfo( char * device_info )
{
    uint8_t ret = SUCCESS;
    bool is_all = false;

    if(strcmp(device_info, "all") == 0){
        is_all = true;
    }

    if(is_all || (strcmp(device_info, "device_mode") == 0))
        ret |= ef_set_env_blob("device_mode", &device_mode, sizeof(device_mode));
    if(is_all || (strcmp(device_info, "device_bond") == 0))
        ret |= ef_set_env_blob("device_bond", &device_bond, sizeof(device_bond));
    if(is_all || (strcmp(device_info, "device_led") == 0))
        ret |= ef_set_env_blob("device_led", &device_led, sizeof(device_led));
    if(is_all || (strcmp(device_info, "device_pair") == 0))
        ret |= ef_set_env_blob("device_pair", &device_pair, sizeof(device_pair));

    return ret;
}


void ResetDeviceInfo(char * device_info)
{
    if(strcmp(device_info, "device_mode") == 0 || strcmp(device_info, "all") == 0){
        device_mode = MODE_BLE;
    }

    if(strcmp(device_info, "device_bond") == 0 || strcmp(device_info, "all") == 0){
        __attribute__((aligned(4))) uint8_t mac_addr[6]; 

        tmos_memset(&device_bond, 0, sizeof(device_bond));
        GetMACAddress( mac_addr );
        for (int i = 0; i < 4; i++) {
            tmos_memcpy(device_bond.ID[i].local_addr, mac_addr, 6);
            device_bond.ID[i].local_addr[2] = 0;
            device_bond.ID[i].local_addr[1] = i;
        }
    }

    if(strcmp(device_info, "device_led") == 0 || strcmp(device_info, "all") == 0){
        device_led.led_en = 0;
        device_led.led_level = 48;
    }

    if(strcmp(device_info, "device_pair") == 0 || strcmp(device_info, "all") == 0){
        device_pair.Pair_flag = 0;
        device_pair.Mac_addr = DEFAULT_MAC;
    }
    SaveDeviceInfo(device_info);
}

void ReadDeviceInfo( char * device_info )
{
    bool is_all = false;

    if(strcmp(device_info, "all") == 0){
        is_all = true;
    }

    if(is_all || (strcmp(device_info, "device_mode") == 0))
    if(ef_get_env_blob("device_mode", &device_mode, sizeof(device_mode), NULL) == 0){
        device_mode = MODE_BLE;
        SaveDeviceInfo("device_mode");
    }

    if(is_all || (strcmp(device_info, "device_bond") == 0))
    if(ef_get_env_blob("device_bond", &device_bond, sizeof(device_bond), NULL) == 0){
        __attribute__((aligned(4))) uint8_t mac_addr[6]; 

        tmos_memset(&device_bond, 0, sizeof(device_bond));
        GetMACAddress( mac_addr );

        for (int i = 0; i < 4; i++) {
            tmos_memcpy(device_bond.ID[i].local_addr, mac_addr, 6);
            device_bond.ID[i].local_addr[2] = 0;
            device_bond.ID[i].local_addr[1] = i;
        }

        SaveDeviceInfo("device_bond");
    }

    if(is_all || (strcmp(device_info, "device_led") == 0))
    if(ef_get_env_blob("device_led", &device_led, sizeof(device_led), NULL) == 0){
        device_led.led_en = 0;
        device_led.led_level = 48;
        SaveDeviceInfo("device_led");
    }

    if(is_all || (strcmp(device_info, "device_pair") == 0))
    if(ef_get_env_blob("device_pair", &device_pair, sizeof(device_pair), NULL) == 0){
        device_pair.Pair_flag = 0;
        device_pair.Mac_addr = DEFAULT_MAC;
        SaveDeviceInfo("device_pair");
    }

    LOG_INFO("my mac addr=%#x", device_pair.Mac_addr);
}


void Mode_Init(uint8_t mod)
{
    rf_dev_uinit();
    HidEmu_Unit();
    USB_Task_Uinit();

    CH57X_BLEInit( );
    HAL_Init( );

    GAPRole_PeripheralInit( );
    RF_RoleInit( );
    mode_ctrl_init();

    switch (mod) {
        case MODE_BLE:
            PRINT("BLE MODE\n");
            HidDev_Init();
            HidEmu_Init();
            break;

        case MODE_RF24:
            PRINT("RF24\n");
            rf_dev_init();
            break;

        case MODE_USB:
            PRINT("USB\n");
            USB_Task_Init();
            break;

        case MODE_TSET:
            test_dtm_usb_init();
            break;

        default:
            break;
    }
}
