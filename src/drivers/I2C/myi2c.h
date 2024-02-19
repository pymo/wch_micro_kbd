#ifndef __MY_I2C_H
#define __MY_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFFER_LENGTH   32
#define TWI_READY       0
#define TWI_MRX         1
#define TWI_MTX         2
#define TWI_SRX         3
#define TWI_STX         4

#define I2C_WR  0       /* Ð´¿ØÖÆbit */
#define I2C_RD  1       /* ¶Á¿ØÖÆbit */



typedef struct TwoWire {
    uint8_t rxBuffer[BUFFER_LENGTH];
    uint8_t rxBufferIndex ;
    uint8_t rxBufferLength;

    uint8_t txAddress;
    uint8_t txBuffer[BUFFER_LENGTH];
    uint8_t txBufferIndex;
    uint8_t txBufferLength;

    uint8_t transmitting;

    void (*user_onRequest)(void);
    void (*user_onReceive)(int len);


}TwoWire;

void myi2c_Init(void);
void myi2c_beginTransmission(uint8_t address);
uint8_t myi2c_endTransmission(bool sendStop);
uint8_t myi2c_requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop);
size_t myi2c_write(uint8_t data);
int myi2c_available(void);
int myi2c_read(void);
int myi2c_peek(void);

void onRequestService(void);
void onReceiveService(uint8_t* inBytes, int numBytes);


void myi2c_test(void);

#endif /*__MY_I2C_H*/
