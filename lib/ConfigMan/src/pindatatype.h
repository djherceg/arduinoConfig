#ifndef _PINDATATYPE_H
#define _PINDATATYPE_H

#include <stdint.h>

#define PINID_INVALID 0xFF   // za nepoznati pinId

enum PinDataType {
  pdtBool = 0,
  pdtSByte = 1,
  pdtByte = 2,
  pdtInt16 = 3,
  pdtUInt16 = 4,
  pdtInt32 = 5,
  pdtUInt32 = 6,
  pdtByteArray = 9,
  pdtString = 10,
  pdtFloat = 11,
  pdtTime16 = 12,
  pdtDate16 = 13,
  pdtUnknown = 254
};


enum PinMode {
  pmOutput = 0,
  pmInput = 1
};


// TODO: Refaktorisati tako da sledeći metodi postanu članovi neke klase.
//       Uputstva su ovde: https://stackoverflow.com/questions/21295935/can-a-c-enum-class-have-methods

/**
 * Konvertuje int u PinDataType. 
 * Ako ne uspe, vraća PinDataType::pdtUnknown.
 */
PinDataType itoPdt(int i);

/** Vraća broj bajtova potreban za pdt. ByteArray i String su dužine 0, tj. korisnik mora sam da zada dužinu. */
uint8_t pdtLen(PinDataType pdt);

#endif