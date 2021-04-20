#include "datetime16.h"

uint16_t toTime16(uint16_t hour, uint16_t minute, uint16_t second)
{
  return (hour << 11) | (minute << 5) | (second / 2);
}

uint16_t toDate16(uint16_t year, uint16_t month, uint16_t day)
{
  // Date16: [yyyyyyyM][MMMddddd], 2000 added to year
  return ((year - 2000) << 9) | (month << 5) | day;
}

void fromDate16(uint16_t date, uint16_t &outYear, uint16_t &outMonth, uint16_t &outDay)
{
  outYear = (date >> 9) + 2000;
  outMonth = (date >> 5) & 0xF;
  outDay = date & 0x1F;
}

void fromTime16(uint16_t time, uint16_t &outHour, uint16_t &outMinute, uint16_t &outSecond)
{
  outHour = time >> 11;
  outMinute = (time >> 5) & 0x3F;
  outSecond = (time & 0xF) * 2;
}