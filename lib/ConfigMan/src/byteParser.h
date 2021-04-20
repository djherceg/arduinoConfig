/*
  ByteParser
  10.3.2020

  Služi za parsiranje podataka iz bafera i prevođenje u tipove podataka iz PinDataType.h
  Sadrži posebne funkcije za parsiranje pojedinačnih podataka i za ceo bafer
*/

#ifndef _BYTEPARSER_
#define _BYTEPARSER_

#include <stdint.h>
#include "pindatatype.h"

class ByteParser
{
private:
  uint8_t *buf;            // bafer kao niz bajtova
  uint8_t len;             // dužina bafera u bajtovima
  uint8_t pos;             // trenutna pozicija u baferu
  uint8_t posSkip;         // broj bajtova do sledećeg paketa (treba dodati na pos)
  bool isnextn(int n);     // da li u baferu postoji još najmanje n bajtova
  bool isnext();           // da li u baferu postoji još jedan bajt
  
  /** proverava da li n-ti bajt od tekućeg postoji u baferu */
  /*bool canPeek(uint8_t n);    */
 
  uint8_t read();      // čita sledeći bajt iz bafera. Vraća 0 ako nema više.
  
  /** čita n-ti bajt od tekućeg iz bafera. Ne utiče na pos. Ako ne uspe, vraća 0. Pre čitanja proveriti isnextn(). */
  uint8_t peek(uint8_t n);    

public:
  void init(uint8_t *buffer, uint8_t length);
  
  /** Čita jedan bajt iz bafera. Vraća false ako ne uspe. */
  bool getNextByte(uint8_t &byteOut);


  /** Resetuje poziciju za čitanje na prvi paket u baferu */
  void moveFirst();

  /** Prelazi na sledeći paket u baferu. Vraća true ako uspe, a inače false. */
  bool moveNext();

  /**
    * Čita sledeći paket iz bafera. Vraća true ako uspe i smešta rezultat u var. Vraća false ako ne uspe.
    * @@var bafer u koji se smešta pročitana vrednost
    * @@maxlen maksimalna dozvoljena dužina bafera
    * @@pinIdOut ID pina kojem se dodeljuje vrednost 
    * @@pdtOut Tip pročitane vrednosti (?)
    * @@lenOut Dužina pročitane vrednosti
  */
  bool getNextValue(void *var, uint8_t maxlen, uint8_t &pinIdOut, PinDataType &pdtOut, uint8_t &lenOut);

  /**
    * Čita sledeći paket iz bafera. Vraća true ako uspe i smešta rezultat u var. Vraća false ako ne uspe.
    * @@var bafer u koji se smešta pročitana vrednost
    * @@oldVar bafer za staru vrednost
    * @@maxlen maksimalna dozvoljena dužina bafera
    * @@pinIdOut ID pina kojem se dodeljuje vrednost 
    * @@pdtOut Tip pročitane vrednosti (?)
    * @@lenOut Dužina pročitane vrednosti
  */
  bool getNextValue(void *var, void *oldVar, uint8_t maxlen, uint8_t &pinIdOut, PinDataType &pdtOut, uint8_t &lenOut);
  
  /**
    * Čita sledeći paket iz bafera. Vraća true ako uspe i smešta rezultat u valueBufferOut. Vraća false ako ne uspe.

    * @@pinIdOut ID pina kojem se dodeljuje vrednost 
    * @@pdtOut Tip pročitane vrednosti (?)
    * @@lenOut Dužina pročitane vrednosti
    * @@valueBuffer bafer u koji se smešta pročitana vrednost
  */
  bool getNextValue(uint8_t &pinIdOut, PinDataType &pdtOut, uint8_t &lenOut, void* valueBuffer);

  bool checkNextValue(uint8_t &pinIdOut, PinDataType &pdtOut, uint8_t &lenOut);
  bool checkAllValues();
  
  /** Vraća poziciju u baferu */
  uint8_t getPos();

  /** Vraća dužinu bafera */
  uint8_t getLen();
  
  /** Da li je parser na kraju bafera (End Of Buffer) */
  bool isEOB();
};

#endif