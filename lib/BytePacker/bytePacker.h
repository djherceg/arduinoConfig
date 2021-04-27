/*
  ByteParser
  10.3.2020

  Služi za parsiranje podataka iz bafera i prevođenje u tipove podataka iz PinDataType.h
  Sadrži posebne funkcije za parsiranje pojedinačnih podataka i za ceo bafer
*/

#ifndef _BYTEPACKSER_
#define _BYTEPACKSER_

#include <stdint.h>
#include "pindatatype.h"

class BytePacker
{
private:
  uint8_t *buf;     // input buffer
  uint8_t len;      // buffer length
  uint8_t pos;      // current position in buffer

public:
  /** Assign the output buffer of the specified length */
  void init(uint8_t *buffer, uint8_t length);

  /** Clears the uffer */
  void clear();

  /** can the output buffer receive n bytes? */
  bool isfreen(uint8_t n);

  /** current position in the output buffer */
  uint8_t getPos();

  /** Writes a byte into the output buffer. Returns true on success, false if the buffer is full. */
  bool write(uint8_t b);

  /** Writes id, pinDataType::pdtBool and b to the output buffer. Returns true on success, flase if the buffer is full. */
  bool putBool(uint8_t id, bool b);

  bool putByte(uint8_t id, uint8_t b);
  bool putInt16(uint8_t id, int16_t i);
  bool putUInt16(uint8_t id, uint16_t i);
  bool putInt32(uint8_t id, int32_t l);
  bool putUInt32(uint8_t id, uint32_t l);
  bool putFloat(uint8_t id, float f);
  bool putDate16(uint8_t id, int year, int month, int day);
  bool putTime16(uint8_t id, uint8_t hour, uint8_t minute, uint8_t second);
  bool putPString(const char *src);
  
  /** Copy C-style string to the output buffer in Pascal format. Returns true on success, false if the buffer is full. TODO: check the code! */
  bool putString(const char *src);
  /** Copy C-style string from PROGMEM to the output buffer in Pascal format. Returns true on success, false if the buffer is full. TODO: check the code! */
  bool putString_P(const char *src);


  bool appendFrom(uint8_t *src, uint8_t count);
};

#endif