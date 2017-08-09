/* Arduino SPIFlash Library v.3.0.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by @boseji <salearj@hotmail.com> - 02/03/2017
 * Modified by Prajwal Bhattaram - 09/08/2017
 *
 * This file is part of the Arduino SPIFlash Library. This library is for
 * Winbond NOR flash memory modules. In its current form it enables reading
 * and writing individual data variables, structs and arrays from and to various locations;
 * reading and writing pages; continuous read functions; sector, block and chip erase;
 * suspending and resuming programming/erase and powering down for low power operation.
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License v3.0
 * along with the Arduino SPIFlash Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef SPIFLASH_H
#define SPIFLASH_H
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Uncomment the code below to run a diagnostic if your flash 	  //
//                         does not respond                           //
//                                                                    //
//      Error codes will be generated and returned on functions       //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define RUNDIAGNOSTIC                                               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//   Uncomment the code below to increase the speed of the library    //
//                  by disabling _notPrevWritten()                    //
//                                                                    //
// Make sure the sectors being written to have been erased beforehand //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//#define HIGHSPEED                                                   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#include <Arduino.h>
#include "defines.h"

#if defined (ARDUINO_ARCH_SAM)
  #include <malloc.h>
  #include <stdlib.h>
  #include <stdio.h>
#endif

  #include <SPI.h>

#if defined (ARDUINO_ARCH_SAM) || defined (ARDUINO_ARCH_SAMD) || defined (ARDUINO_ARCH_ESP8266) || defined (SIMBLEE) || defined (ARDUINO_ARCH_ESP32) || defined (BOARD_RTL8195A)
// RTL8195A included - @boseji <salearj@hotmail.com> 02.03.17
 #define _delay_us(us) delayMicroseconds(us)
#else
 #include <util/delay.h>
#endif
// Includes specific to RTL8195A to access GPIO HAL - @boseji <salearj@hotmail.com> 02.03.17
#if defined (BOARD_RTL8195A)
#ifdef __cplusplus
extern "C" {
#endif

#include "gpio_api.h"
#include "PinNames.h"

#ifdef __cplusplus
}
#endif

#endif

#ifdef ARDUINO_ARCH_AVR
  #define CHIP_SELECT   *cs_port &= ~cs_mask;
  #define CHIP_DESELECT *cs_port |=  cs_mask;
  #define xfer(n)   _spi->transfer(n)
  #define BEGIN_SPI _spi->begin();
#elif defined (ARDUINO_ARCH_SAM)
  #define CHIP_SELECT   digitalWrite(csPin, LOW);
  #define CHIP_DESELECT digitalWrite(csPin, HIGH);
  #define xfer   _dueSPITransfer
  #define BEGIN_SPI _dueSPIBegin();
// Specific access configuration for Chip select pin - @boseji <salearj@hotmail.com> 02.03.17
#elif defined (BOARD_RTL8195A)
  #define CHIP_SELECT   gpio_write(&csPin, 0);
  #define CHIP_DESELECT gpio_write(&csPin, 1);
  #define xfer(n)   _spi->transfer(n)
  #define BEGIN_SPI _spi->begin();
#else //#elif defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_SAMD)
  #define CHIP_SELECT   digitalWrite(csPin, LOW);
  #define CHIP_DESELECT digitalWrite(csPin, HIGH);
  #define xfer(n)   _spi->transfer(n)
  #define BEGIN_SPI _spi->begin();
#endif

#define LIBVER 3
#define LIBSUBVER 0
#define BUGFIXVER 0

#if defined (ARDUINO_ARCH_SAM)
  extern char _end;
  extern "C" char *sbrk(int i);
  //char *ramstart=(char *)0x20070000;
  //char *ramend=(char *)0x20088000;
#endif

class SPIFlash {
public:
  //----------------------------------------------Constructor-----------------------------------------------
  //New Constructor to Accept the PinNames as a Chip select Parameter - @boseji <salearj@hotmail.com> 02.03.17
  #if !defined (BOARD_RTL8195A)
  SPIFlash(uint8_t cs = CS, SPIClass *spiinterface=&SPI);
  #else
  SPIFlash(PinName cs = CS, SPIClass *spiinterface=&SPI);
  #endif
  //----------------------------------------Initial / Chip Functions----------------------------------------//
  bool     begin(void);
  void     setClock(uint32_t clockSpeed);
  bool     libver(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  uint8_t  error(bool verbosity = false);
  uint16_t getManID(void);
  uint32_t getJEDECID(void);
  uint32_t getAddress(uint16_t size);
  uint16_t sizeofStr(String &inputStr);
  uint32_t getCapacity(void);
  uint32_t getMaxPage(void);
  //-------------------------------------------Write / Read Bytes-------------------------------------------//
  bool     writeByte(uint32_t _addr, uint8_t data, bool errorCheck = true);
  uint8_t  readByte(uint32_t _addr, bool fastRead = false);
  //----------------------------------------Write / Read Byte Arrays----------------------------------------//
  bool     writeByteArray(uint32_t _addr, uint8_t *data_buffer, uint16_t bufferSize, bool errorCheck = true);
  bool     readByteArray(uint32_t _addr, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead = false);
  //-------------------------------------------Write / Read Chars-------------------------------------------//
  bool     writeChar(uint32_t _addr, int8_t data, bool errorCheck = true);
  int8_t   readChar(uint32_t _addr, bool fastRead = false);
  //----------------------------------------Write / Read Char Arrays----------------------------------------//
  bool     writeCharArray(uint32_t _addr, char *data_buffer, uint16_t bufferSize, bool errorCheck = true);
  bool     readCharArray(uint32_t _addr, char *data_buffer, uint16_t buffer_size, bool fastRead = false);
  //------------------------------------------Write / Read Shorts------------------------------------------//
  bool     writeShort(uint32_t _addr, int16_t data, bool errorCheck = true);
  int16_t  readShort(uint32_t _addr, bool fastRead = false);
  //-------------------------------------------Write / Read Words-------------------------------------------//
  bool     writeWord(uint32_t _addr, uint16_t data, bool errorCheck = true);
  uint16_t readWord(uint32_t _addr, bool fastRead = false);
  //-------------------------------------------Write / Read Longs-------------------------------------------//
  bool     writeLong(uint32_t _addr, int32_t data, bool errorCheck = true);
  int32_t  readLong(uint32_t _addr, bool fastRead = false);
  //--------------------------------------Write / Read Unsigned Longs---------------------------------------//
  bool     writeULong(uint32_t _addr, uint32_t data, bool errorCheck = true);
  uint32_t readULong(uint32_t _addr, bool fastRead = false);
  //-------------------------------------------Write / Read Floats------------------------------------------//
  bool     writeFloat(uint32_t _addr, float data, bool errorCheck = true);
  float    readFloat(uint32_t _addr, bool fastRead = false);
  //------------------------------------------Write / Read Strings------------------------------------------//
  bool     writeStr(uint32_t _addr, String &inputStr, bool errorCheck = true);
  bool     readStr(uint32_t _addr, String &outStr, bool fastRead = false);
  //------------------------------------------Write / Read Anything-----------------------------------------//
  template <class T> bool _write(uint32_t _addr, const T& value, uint8_t _sz, bool errorCheck);
  template <class T> bool writeAnything(uint32_t _addr, const T& value, bool errorCheck = true);
  template <class T> bool _read(uint32_t _addr, T& value, uint8_t _sz, bool fastRead = false);
  template <class T> bool readAnything(uint32_t _addr, T& value, bool fastRead = false);
  //--------------------------------------------Erase functions---------------------------------------------//
  bool     eraseSector(uint32_t _addr);
  bool     eraseBlock32K(uint32_t _addr);
  bool     eraseBlock64K(uint32_t _addr);
  bool     eraseChip(void);
  //---------------------------------------------Power functions--------------------------------------------//
  bool     suspendProg(void);
  bool     resumeProg(void);
  bool     powerDown(void);
  bool     powerUp(void);
  //-------------------------------------Public Arduino Due Functions-------------------------------------//
//#if defined (ARDUINO_ARCH_SAM)
  //uint32_t freeRAM(void);
//#endif
  //-------------------------------------------Public variables-------------------------------------//

private:
#if defined (ARDUINO_ARCH_SAM)
  //-------------------------------------Private Arduino Due Functions--------------------------------------//
  void     _dmac_disable(void);
  void     _dmac_enable(void);
  void     _dmac_channel_disable(uint32_t ul_num);
  void     _dmac_channel_enable(uint32_t ul_num);
  bool     _dmac_channel_transfer_done(uint32_t ul_num);
  void     _dueSPIDmaRX(uint8_t* dst, uint16_t count);
  void     _dueSPIDmaRX(char* dst, uint16_t count);
  void     _dueSPIDmaTX(const uint8_t* src, uint16_t count);
  void     _dueSPIDmaCharTX(const char* src, uint16_t count);
  void     _dueSPIBegin(void);
  void     _dueSPIInit(uint8_t dueSckDivisor);
  uint8_t  _dueSPITransfer(uint8_t b);
  uint8_t  _dueSPIRecByte(void);
  uint8_t  _dueSPIRecByte(uint8_t* buf, size_t len);
  int8_t   _dueSPIRecChar(void);
  int8_t   _dueSPIRecChar(char* buf, size_t len);
  void     _dueSPISendByte(uint8_t b);
  void     _dueSPISendByte(const uint8_t* buf, size_t len);
  void     _dueSPISendChar(char b);
  void     _dueSPISendChar(const char* buf, size_t len);
#endif
  //----------------------------------------Private functions----------------------------------------//
  void     _troubleshoot(uint8_t _code, bool printoverride = false);
  void     _printErrorCode(void);
  void     _printSupportLink(void);
  void     _endSPI(void);
  bool     _prep(uint8_t opcode, uint32_t _addr, uint32_t size = 0);
  bool     _startSPIBus(void);
  bool     _beginSPI(uint8_t opcode);
  bool     _noSuspend(void);
  bool     _notBusy(uint32_t timeout = BUSY_TIMEOUT);
  bool     _notPrevWritten(uint32_t _addr, uint32_t size = 1);
  bool     _writeEnable(uint32_t timeout = 10L);
  bool     _writeDisable(void);
  bool     _getJedecId(void);
  bool     _getManId(uint8_t *b1, uint8_t *b2);
  bool     _getSFDP(void);
  bool     _chipID(void);
  bool     _transferAddress(void);
  bool     _addressCheck(uint32_t _addr, uint32_t size = 1);
  uint8_t  _nextByte(uint8_t data = NULLBYTE);
  uint16_t _nextInt(uint16_t = NULLINT);
  void     _nextBuf(uint8_t opcode, uint8_t *data_buffer, uint32_t size);
  uint8_t  _readStat1(void);
  uint8_t  _readStat2(void);
  template <class T> bool _writeErrorCheck(uint32_t _addr, const T& value);
  template <class T> bool _writeErrorCheck(uint32_t _addr, const T& value, uint8_t _sz);
  //-------------------------------------------Private variables------------------------------------------//
  #ifdef SPI_HAS_TRANSACTION
    SPISettings _settings;
  #endif
  //If multiple SPI ports are available this variable is used to choose between them (SPI, SPI1, SPI2 etc.)
  SPIClass *_spi;
  #if !defined (BOARD_RTL8195A)
  uint8_t     csPin;
  #else
  // Object declaration for the GPIO HAL type for csPin - @boseji <salearj@hotmail.com> 02.03.17
  gpio_t      csPin;
  #endif
  volatile uint8_t *cs_port;
  bool        pageOverflow, SPIBusState;
  uint8_t     cs_mask, errorcode, state, _SPCR, _SPSR, _a0, _a1, _a2;
  struct      chipID {
                uint8_t manufacturerID;
                uint8_t memoryTypeID;
                uint8_t capacityID;
                uint16_t supported;
                uint32_t sfdp;
                uint32_t capacity;
                uint32_t eraseTime;
              };
              chipID _chip;
  uint32_t    currentAddress, _currentAddress = 0;
  const uint8_t _capID[12]   = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x43, 0x4B};
  const uint32_t _memSize[12]  = {64L * K, 128L * K, 256L * K, 512L * K, 1L * M, 2L * M, 4L * M, 8L * M,
                                16L * M, 32L * M, 8L * M, 8L * M};
  const uint32_t _eraseTime[12] = {1L * S, 2L * S, 2L * S, 4L * S, 6L * S, 10L * S, 15L * S, 100L * S, 200L * S, 400L * S, 50L * S, 50L * S}; //Erase time in milliseconds
};

//----------------------------------------Public Templates----------------------------------------//

// Writes any type of data to a specific location in the flash memory.
// Takes three arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. T& value --> Variable to write
//  3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
//      Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
template <class T> bool SPIFlash::writeAnything(uint32_t _addr, const T& value, bool errorCheck) {
  uint8_t _sizeofvalue = sizeof(value);
  return _write(_addr, value, _sizeofvalue, errorCheck);
}

// Reads any type of data from a specific location in the flash memory.
// Takes three arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. T& value --> Variable to return data into
//  3. fastRead --> defaults to false - executes _beginFastRead() if set to true
template <class T> bool SPIFlash::readAnything(uint32_t _addr, T& value, bool fastRead) {
  uint8_t _sizeofvalue = sizeof(value);
  return _read(_addr, value, _sizeofvalue, fastRead);
}

//----------------------------------------Private Templates---------------------------------------//

// Checks for errors in writing data to flash memory.
// Takes three arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. const T& value --> Variable with the data to be error checked
//  3. _sz --> Size of the data variable to be error checked, in bytes (1 byte = 8 bits)
// Private template to check for errors in writing to flash memory
template <class T> bool SPIFlash::_writeErrorCheck(uint32_t _addr, const T& value, uint8_t _sz) {
  if (!_notBusy()) {
    return false;
  }
  _currentAddress = _addr;
  const uint8_t* p = (const uint8_t*)(const void*)&value;
  CHIP_SELECT
  _nextByte(READDATA);
  _transferAddress();
  for (uint16_t i = 0; i < _sz; i++) {
    if (*p++ != _nextByte()) {
      _troubleshoot(ERRORCHKFAIL);
      _endSPI();
      return false;
    }
  }
  _endSPI();
  return true;
}

// Writes any type of data to a specific location in the flash memory.
// Takes four arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. T& value --> Variable to write
//  3. _sz --> Size of variable in bytes (1 byte = 8 bits)
//  4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
//      Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)

template <class T> bool SPIFlash::_write(uint32_t _addr, const T& value, uint8_t _sz, bool errorCheck) {
  if (!_prep(PAGEPROG, _addr, _sz)) {
    return false;
  }
  const uint8_t* p = ((const uint8_t*)(const void*)&value);

  if (!SPIBusState) {
    _startSPIBus();
  }
  CHIP_SELECT
  _nextByte(PAGEPROG);
  _transferAddress();
    //If data is only one byte (8 bits) long
  if (_sz == 0x01) {
    _nextByte(*p);
    CHIP_DESELECT
  }
  else { //If data is longer than one byte (8 bits)
    uint32_t length = _sz;
    uint16_t maxBytes = PAGESIZE-(_addr % PAGESIZE);  // Force the first set of bytes to stay within the first page

    if (maxBytes > length) {
      for (uint16_t i = 0; i < length; ++i) {
        _nextByte(*p++);
      }
      CHIP_DESELECT
    }
    else {
      uint32_t writeBufSz;
      uint16_t data_offset = 0;

      do {
        writeBufSz = (length<=maxBytes) ? length : maxBytes;

        for (uint16_t i = 0; i < writeBufSz; ++i) {
          _nextByte(*p++);
        }
        CHIP_DESELECT
        _currentAddress += writeBufSz;
        data_offset += writeBufSz;
        length -= writeBufSz;
        maxBytes = 256;   // Now we can do up to 256 bytes per loop
        if(!_notBusy() || !_writeEnable()) {
          return false;
        }
      } while (length > 0);
    }
  }
  if (!errorCheck) {
    _endSPI();
    return true;
  }
  else {
    return _writeErrorCheck(_addr, value, _sz);
  }
}

// Reads any type of data from a specific location in the flash memory.
// Takes four arguments -
//  1. _addr --> Any address from 0 to maxAddress
//  2. T& value --> Variable to return data into
//  3. _sz --> Size of the variable in bytes (1 byte = 8 bits)
//  4. fastRead --> defaults to false - executes _beginFastRead() if set to true
template <class T> bool SPIFlash::_read(uint32_t _addr, T& value, uint8_t _sz, bool fastRead) {
  if (_prep(READDATA, _addr, _sz)) {
    uint8_t* p = (uint8_t*)(void*)&value;
    CHIP_SELECT
    switch (fastRead) {
      case false:
      _nextByte(READDATA);
      break;

      case true:
      _nextByte(FASTREAD);
      break;

      default:
      break;
    }
    _transferAddress();
    for (uint16_t i = 0; i < _sz; i++) {
      *p++ =_nextByte();
    }
    _endSPI();
    return true;
  }
  else {
    return false;
  }
}

#endif // _SPIFLASH_H_
