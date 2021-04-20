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
  uint8_t *buf;     // bafer kao niz bajtova
  uint8_t len;      // dužina bafera u bajtovima
  uint8_t pos;      // trenutna pozicija u baferu

public:
  /** povezuje se na izlazni bafer zadate dužine */
  void init(uint8_t *buffer, uint8_t length);

  /** briše bafer */
  void clear();

  /** da li u baferu ima mesta za još n bajtova */
  bool isfreen(uint8_t n);

  /** trenutna dužina poruke u izlaznom baferu */
  uint8_t getPos();

  /** Upisuje jedan bajt u izlazni bafer. Vraća false ako u baferu nema mesta. */
  bool write(uint8_t b);

  /** Upisuje id, pinDataType::pdtBool i b u izlazni bafer. Vraća false ako u baferu nema mesta. */
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
  
  /** Kopira string u izlazni bafer u Pascal formatu. Vraća false ako u baferu nema mesta. TODO: proveriti! */
  bool putString(const char *src);
  /** Kopira PROGMEM string u izlazni bafer u Pascal formatu. Vraća false ako u baferu nema mesta. TODO: proveriti! */
  bool putString_P(const char *src);


  bool appendFrom(uint8_t *src, uint8_t count);
};

#endif