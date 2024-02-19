#include "myi2c.h"
#include "twi.h"
#include "CH58x_common.h"

static TwoWire myi2c = {
        .rxBufferIndex = 0,
        .rxBufferLength = 0,
        .transmitting = 0,
        .txAddress = 0,
        .txBufferIndex = 0,
        .txBufferLength = 0,
};

void RequestCB(void){
    PRINT("RequestCB\n");
    /*slave transmit*/
    uint8_t testbuf[5] = {0x12,0x22,0x33,0x22,0x33};
    twi_transmit(testbuf, sizeof(testbuf));

}

void ReceiveCB( int len){
    PRINT("read data=[");
    for(int i = 0; i < len; i++){
        if(i) PRINT(" ");
        PRINT("%#x", myi2c_read());
    }PRINT("]\n");
}

void myi2c_Init(void){
    myi2c.rxBufferIndex = 0;
    myi2c.rxBufferLength = 0;

    myi2c.txBufferIndex = 0;
    myi2c.txBufferLength = 0;

    myi2c.user_onRequest = RequestCB;
    myi2c.user_onReceive = ReceiveCB;

    twi_init();
    twi_attachSlaveTxEvent(onRequestService); // default callback must exist
    twi_attachSlaveRxEvent(onReceiveService); // default callback must exist
}


void myi2c_beginTransmission(uint8_t address){
    // indicate that we are transmitting
    myi2c.transmitting = 1;
    // set address of targeted slave
    myi2c.txAddress = address;
    // reset tx buffer iterator vars
    myi2c.txBufferIndex = 0;
    myi2c.txBufferLength = 0;
}

uint8_t myi2c_endTransmission(bool sendStop)
{
  // transmit buffer (blocking)
  uint8_t ret = twi_writeTo(myi2c.txAddress, myi2c.txBuffer, myi2c.txBufferLength, 1, sendStop);

  // reset tx buffer iterator vars
  myi2c.txBufferIndex = 0;
  myi2c.txBufferLength = 0;
  // indicate that we are done transmitting
  myi2c.transmitting = 0;
  return ret;
}

uint8_t myi2c_requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
  if (isize > 0) {
    // send internal address; this mode allows sending a repeated start to access
    // some devices' internal registers. This function is executed by the hardware
    // TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)
    myi2c_beginTransmission(address);

    // the maximum size of internal address is 3 bytes
    if (isize > 3){
      isize = 3;
    }

    // write internal register address - most significant byte first
    while (isize-- > 0)
        myi2c_write((uint8_t)(iaddress >> (isize*8)));
        
    myi2c_endTransmission(false);
  }

  // clamp to buffer length
  if(quantity > BUFFER_LENGTH){
    quantity = BUFFER_LENGTH;
  }
  // perform blocking read into buffer
  uint8_t read = twi_readFrom(address, myi2c.rxBuffer, quantity, sendStop);
  // set rx buffer iterator vars
  myi2c.rxBufferIndex = 0;
  myi2c.rxBufferLength = read;

  return read;
}

size_t myi2c_write(uint8_t data)
{
  if(myi2c.transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(myi2c.txBufferLength >= BUFFER_LENGTH){
      PRINT("IIC ERROR!\n");
      return 0;
    }
    // put byte in tx buffer
    myi2c.txBuffer[myi2c.txBufferIndex] = data;
    ++myi2c.txBufferIndex;
    // update amount in buffer
    myi2c.txBufferLength = myi2c.txBufferIndex;
  }else{
  // in slave send mode
    // reply to master
        twi_transmit(&data, 1);
        PRINT("Slave mode, should not be here!\n");
  }
  return 1;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int myi2c_available(void)
{
  return myi2c.rxBufferLength - myi2c.rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int myi2c_read(void)
{
  int value = -1;

  // get each successive byte on each call
  if(myi2c.rxBufferIndex < myi2c.rxBufferLength){
    value = myi2c.rxBuffer[myi2c.rxBufferIndex++];
  }
  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int myi2c_peek(void)
{
  int value = -1;

  if(myi2c.rxBufferIndex < myi2c.rxBufferLength){
    value = myi2c.rxBuffer[myi2c.rxBufferIndex];
  }

  return value;
}

// behind the scenes function that is called when data is received
void onReceiveService(uint8_t* inBytes, int numBytes)
{
  // don't bother if user hasn't registered a callback
  if(!myi2c.user_onReceive){
    return;
  }
  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if(myi2c.rxBufferIndex < myi2c.rxBufferLength){
    return;
  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
  for(uint8_t i = 0; i < numBytes; ++i){
      myi2c.rxBuffer[i] = inBytes[i];
  }
  // set rx iterator vars
  myi2c.rxBufferIndex = 0;
  myi2c.rxBufferLength = numBytes;
  // alert user program
  myi2c.user_onReceive(numBytes);
}

// behind the scenes function that is called when data is requested
void onRequestService(void)
{
  // don't bother if user hasn't registered a callback
  if(!myi2c.user_onRequest){
    return;
  }
  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
  myi2c.txBufferIndex = 0;
  myi2c.txBufferLength = 0;
  // alert user program
  myi2c.user_onRequest();
}



void myi2c_test(void){
#define I2C_MODE     1
    myi2c_Init();

#if I2C_MODE
    /*master read*/
    uint8_t subAddress = 0x11;
    myi2c_beginTransmission(TWI_ADDR);         // Initialize the Tx buffer
    myi2c_write(subAddress);                  // Put slave register address in Tx buffer
    myi2c_endTransmission(0);             // Send the Tx buffer, but send a restart to keep connection alive
    uint8_t len = myi2c_requestFrom(TWI_ADDR, 3,0,0,1);  // Read from slave register address

    PRINT("master read %d bytes from %#x=[", len, TWI_ADDR);
    for(int i = 0; i < len; i++){
        if(i) PRINT(" ");
        PRINT("%#x", myi2c_read());
    }PRINT("]\n");

    /*master write*/
    uint8_t tmp[5] = {1,2,3,4,5};
    myi2c_beginTransmission(TWI_ADDR);         // Initialize the Tx buffer
    myi2c_write(tmp[0]);                  // Put slave register address in Tx buffer
    myi2c_write(tmp[1]);
    myi2c_write(tmp[2]);
    myi2c_write(tmp[3]);
    myi2c_write(tmp[4]);
    myi2c_endTransmission(1);

#else
    /*default slave mode*/
#endif




    while(1);

}
