/*
  DateTime16.h
  Funkcije za konverziju datuma i vremena u Date16 i Time16 format.
*/


#ifndef _DATETIME16_H_
#define _DATETIME16_H_

#include "stdint.h"


uint16_t toTime16(uint16_t hour, uint16_t minute, uint16_t second);

uint16_t toDate16(uint16_t year, uint16_t month, uint16_t day);

void fromDate16(uint16_t date, uint16_t &outYear, uint16_t &outMonth, uint16_t &outDay);

void fromTime16(uint16_t time, uint16_t &outHour, uint16_t &outMinute, uint16_t &outSecond);



#endif