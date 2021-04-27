#include "bytePacker.h"
#include <string.h>
//#include "avr/pgmspace.h"
#include <Arduino.h>

void BytePacker::init(uint8_t *buffer, uint8_t length)
{
  buf = buffer;
  len = length;
  pos = 0;
}

void BytePacker::clear()
{
  len = 0;
  pos = 0;
}

bool BytePacker::isfreen(uint8_t n)
{
  return pos + n <= len;
}

uint8_t BytePacker::getPos()
{
  return pos;
}

bool BytePacker::write(uint8_t b)
{
  if (isfreen(1))
  {
    buf[pos++] = b;
    return true;
  }
  else
  {
    return false;
  }
}

bool BytePacker::putBool(uint8_t id, bool b)
{
  if (isfreen(3))
  {
    write(id);
    write(PinDataType::pdtBool);
    write(b);
    return true;
  }
  else
  {
    return false;
  }
}

bool BytePacker::putByte(uint8_t id, uint8_t b)
{
  if (isfreen(3))
  {
    write(id);
    write(PinDataType::pdtByte);
    write(b);
    return true;
  }
  else
  {
    return false;
  }
}

bool BytePacker::putInt16(uint8_t id, int16_t i)
{
  if (isfreen(4))
  {
    write(id);
    write(PinDataType::pdtInt16);
    memcpy(buf + pos, &i, 2);
    pos += 2;
    return true;
  }
  else
  {
    return false;
  }
}

bool BytePacker::putUInt16(uint8_t id, uint16_t i)
{
  if (isfreen(4))
  {
    write(id);
    write(PinDataType::pdtInt16);
    memcpy(buf + pos, &i, 2);
    pos += 2;
    return true;
  }
  else
  {
    return false;
  }
}

bool BytePacker::putInt32(uint8_t id, int32_t l)
{
  if (isfreen(6))
  {
    write(id);
    write(PinDataType::pdtInt32);
    memcpy(buf + pos, &l, 4);
    pos += 4;
    return true;
  }
  else
  {
    return false;
  }
}

bool BytePacker::putUInt32(uint8_t id, uint32_t l)
{
  if (isfreen(6))
  {
    write(id);
    write(PinDataType::pdtUInt32);
    memcpy(buf + pos, &l, 4);
    pos += 4;
    return true;
  }
  else
  {
    return false;
  }
}

bool BytePacker::putFloat(uint8_t id, float f)
{
  if (isfreen(6))
  {
    write(id);
    write(PinDataType::pdtFloat);
    memcpy(buf + pos, &f, 4);
    pos += 4;
    return true;
  }
  else
  {
    return false;
  }
}

bool BytePacker::putDate16(uint8_t id, int year, int month, int day)
{
  // Date16: [yyyyyyyM][MMMddddd], 2000 added to year

  if (pos + 1 >= len)
  {
    return false;
  }

  // TODO: provera tacnog broja dana za svaki mesec
  if ((year < 2000) || (year > 2127) || (month < 1) || (month > 12) || (day < 1) || (day > 31))
  {
    return false;
  }

  buf[pos++] = ((year - 2000) << 1) | (month >> 3);
  buf[pos++] = ((month & 7) << 5) | day;
  return true;
}

bool BytePacker::putTime16(uint8_t id, uint8_t hour, uint8_t minute, uint8_t second)
{
  // Time16: [hhhhhmmm][mmmsssss], seconds multiplied by 2
  if (pos + 1 >= len)
  {
    return false;
  }

  if ((hour < 0) || (hour > 23) || (minute < 0) || (minute > 59) || (second < 0) || (second > 59))
  {
    return false;
  }

  buf[pos++] = (hour << 3) | (minute >> 3);
  buf[pos++] = ((minute & 7) << 5) | (second / 2);
  return true;
}

// put null-terminated string from PROGMEM into the buffer
// string is stored as a Pascal-style string in the buffer (first byte contains the number of characters)
bool BytePacker::putPString(const char *src)
{
  int slen = strlen_P(src);
  //Serial.print(F("pos:"));
  //Serial.println(pos);
  //Serial.print(F("putPString strlen_P returned "));
  //Serial.print(slen);
  //Serial.print(F("  chars: "));
  if (pos > len - slen - 1)
  {
    //Serial.println(F("false\n"));
    return false;
  }

  write((uint8_t)slen); // pascal-style string length
  for (int i = 0; i < slen; i++)
  {
    char c = pgm_read_byte_near(src + i);
    //Serial.print(c);
    write(c); // string chars
  }
  //Serial.println(F("true\n"));
  return true;
}

bool BytePacker::putString(const char *src)
{
  int l = strlen(src);
  if ((l < 255) && (isfreen(l + 1))) // ako bafer ima mesta za string + 0x0 i ako je string kraći od 255 znakova
  {
    write((uint8_t)l);              // upiši dužinu stringa.
    strcpy((char *)buf + pos, src); // kopiraj string
    //pos += l + 1;                     // dodajemo l+1 zbog 0x0 na kraju
    pos += l; // poslednji 0x0 se zanemaruje
    return true;
  }
  else
    return false;
}

bool BytePacker::putString_P(const char *src)
{
  int l = strlen_P(src);
  if ((l < 255) && (isfreen(l + 1))) // ako bafer ima mesta za string + 0x0 i ako je string kraći od 255 znakova
  {
    write((uint8_t)l);                // upiši dužinu stringa.
    strcpy_P((char *)buf + pos, src); // kopiraj string (voditi računa da iza stringa ide i 0x0 pa treba bajt više u baferu!)
    //pos += l + 1;                     // dodajemo l+1 zbog 0x0 na kraju
    pos += l; // poslednji 0x0 se zanemaruje
    return true;
  }
  else
    return false;
}

bool BytePacker::appendFrom(uint8_t *src, uint8_t count)
{
  if (pos > len - count)
    return false;

  memcpy(buf + pos, src, count); // copy the source buffer into data buffer
  pos += count;                  // adjust the data length
  return true;
}
