#ifndef __DEVIDE_CONFIG_H
#define __DEVIDE_CONFIG_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#define  CHECKVALUE                 0x23

#define MODE_BLE                         0x11
#define MODE_USB                         0x6b
#define MODE_RF24                        0x24
#define MODE_TSET                        0x84

#define DEFAULT_MAC         0x8E89be38

typedef struct
{
    uint8_t Pair_flag;
    uint32_t Mac_addr;
}RF_Pair_Info_t;

typedef struct
{
    bool isbond;
    uint8_t remote_addr_type;
    uint8_t remote_addr[6];
    uint8_t local_addr[6];
}Device_ID_t;

typedef struct
{
    uint8_t ID_Num;
    Device_ID_t ID[4];

}__attribute__((packed)) Device_bond_t;

typedef struct{
    bool led_en;
    uint8_t led_level;
}Led_info_t;


extern __attribute__((aligned(4))) uint8_t device_mode;
extern __attribute__((aligned(4))) Device_bond_t device_bond;
extern __attribute__((aligned(4))) Led_info_t device_led;
extern __attribute__((aligned(4))) RF_Pair_Info_t device_pair;

uint8_t SaveDeviceInfo( char * device_info );
void ResetDeviceInfo(char * device_info);
void ReadDeviceInfo( char * device_info );
void Mode_Init(uint8_t mod);

#ifdef __cplusplus
}
#endif

#endif  /*__DEVIDE_CONFIG_H*/
