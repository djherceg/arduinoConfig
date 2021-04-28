#include <Arduino.h>
#include <ConfigInfo.h>
#include "serialbuf.h"
#include <ByteArray.h>
#ifndef ENV_ESP32DEV
#include "avr/pgmspace.h"
#endif
#include "cmdProc.h"

SerialBuf sbuf;

ByteParser parser;
uint8_t buf[50];
ByteArray dest(100);

const char deviceName[] PROGMEM{"ConfigMan+CmdProc Arduino test device"};

bool sw1 = false;
bool sw1old = false;
int count1 = 42;
int count1old = 42;
float r = 37.2;
float rold = 37.2;
uint32_t lng = 1000;
uint32_t lngold = 1000;
char text1[] = {"this is a text._____"}; // pad the text to 20 chars
char text1old[] = {"this is an old text."};
#ifdef ENV_ESP32DEV
const char sw1name[]{"Switch1"};
const char count1name[]{"Counter1"};
const char rname[]{"Temp1"};
const char lngname[]{"LongCounter"};
const char text1name[]{"Text1"};

#else
const char sw1name[] PROGMEM{"Switch1"};
const char count1name[] PROGMEM{"Counter1"};
const char rname[] PROGMEM{"Temp1"};
const char lngname[] PROGMEM{"LongCounter"};
const char text1name[] PROGMEM{"Text1"};
#endif

ConfigInfo config(5, toDate16(2020, 11, 25), toTime16(20, 12, 00), 1, 0, deviceName);

void callback(PinInfo *cfi);    // forward deklaracija za callback
void pinCallback(PinInfo *cfi); // callback za pojedinacni pin

CmdProc::Proc cmdProc; // komandni procesor
const char infoCmd[] PROGMEM{"info"};
const char queryCmd[] PROGMEM{"query"};
const char setCmd[] PROGMEM{"set"};
const char verCmd[] PROGMEM{"ver"};
const char startbinCmd[] PROGMEM{"startbin"};
const char resetCmd[] PROGMEM{"reset"};

// forward deklaracije komandi
int cmdVer(CmdProc::Proc *c);
int cmdInfo(CmdProc::Proc *c);
int cmdQuery(CmdProc::Proc *c);
int cmdSet(CmdProc::Proc *c);
int cmdStartbin(CmdProc::Proc *c);
int cmdReset(CmdProc::Proc *c);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);

  config.addPin(201, sw1name, &sw1, &sw1old, PinDataType::pdtBool, PinMode::pmOutput, pinCallback);
  config.addPin(202, count1name, &count1, &count1old, PinDataType::pdtInt16, PinMode::pmOutput, pinCallback);
  config.addPin(203, rname, &r, &rold, PinDataType::pdtFloat, PinMode::pmOutput, pinCallback);
  config.addPin(204, lngname, &lng, &lngold, PinDataType::pdtUInt32, PinMode::pmOutput, pinCallback);
  config.addPin(205, text1name, text1, text1old, PinDataType::pdtString, PinMode::pmOutput, pinCallback);
  config.setEventHandler(callback);

  // initialize the SerialBuffer
  sbuf.init();
  sbuf.textMode();

  // initialize the command processor
  cmdProc.Init(6);
  cmdProc.Add(verCmd, cmdVer, 1, 1);
  cmdProc.Add(infoCmd, cmdInfo, 1, 2);
  cmdProc.Add(queryCmd, cmdQuery, 2, 2);
  cmdProc.Add(setCmd, cmdSet, 3, 3);
  cmdProc.Add(startbinCmd, cmdStartbin, 1, 1);
  cmdProc.Add(resetCmd, cmdReset, 1, 1);
}

void callback(PinInfo *pin)
{
  //Serial.println(pin.getId());
}

void pinCallback(PinInfo *cfi)
{
  //Serial.println(cfi.getId());

  //Serial.println(cfi.getName());
  //Serial.println(reinterpret_cast<const __FlashStringHelper *>(cfi.getName()));
  //Serial.print("Old val: ");
  //Serial.println(count1old);
  //Serial.print("New val: ");
  //Serial.println(count1);
}

void loop()
{
  // put your main code here, to run repeatedly:
  sbuf.loop();
  if (sbuf.isavailable())
  {
    //Serial.println(sbuf.buffer);

    if (sbuf.getMode() == SERIALBUF_BINARYMODE)
    {
      if (sbuf.buffer[0] == 'x')
      {
        sbuf.textMode();
        return;
      }
    }

    switch (sbuf.buffer[0])
    {
    case '~': // config                         // Local Config command
    {
      uint8_t buflen = sbuf.getLength();
      if (buflen > 2)
      {
        uint8_t payloadLen = sbuf.buffer[1]; // second byte is payload length
        if (payloadLen + 2 == buflen)        // is payload length the same as advertised?
        {
          uint8_t rez = config.processCommand((uint8_t *)sbuf.buffer + 2, buflen - 2, dest.buffer, dest.getMaxLen());
          dest.setLen(rez);
          if (rez > 0)
          {
            Serial.write('~');              // Local Config response
            Serial.write(rez);              // response length
            Serial.write(dest.buffer, rez); // actual response
          }
        }
      }
      break;
    }
    default:
      if (sbuf.getMode() == SERIALBUF_TEXTMODE)
      {
        int rez = cmdProc.Parse(sbuf.buffer);
        Serial.println(rez);
        Serial.println(F("ready"));
      }
      break;
    }
    sbuf.clear();
  }
}

int cmdVer(CmdProc::Proc *c)
{
  return 0;
}

void printStr_P(const char *p)
{
  char *ptr = (char *)malloc(strlen_P(p) + 1);
  if (ptr != NULL)
  { // if memory allocation is successful
    strcpy_P(ptr, p);
    Serial.print(ptr); // function overload
    free(ptr);
  }
}

void printPdtValue(PinDataType pdt, void *val)
{
  switch (pdt)
  {
  case PinDataType::pdtBool:
  case PinDataType::pdtByte:
    Serial.print(*(uint8_t *)val);
    break;

  case PinDataType::pdtSByte:
    Serial.print(*(int8_t *)val);
    break;

  case PinDataType::pdtInt16:
    Serial.print(*(int16_t *)val);
    break;

  case PinDataType::pdtUInt16:
    Serial.print(*(uint16_t *)val);
    break;

  case PinDataType::pdtInt32:
    Serial.print(*(int32_t *)val);
    break;

  case PinDataType::pdtUInt32:
    Serial.print(*(uint32_t *)val);
    break;

  case PinDataType::pdtFloat:
    Serial.print(*(float *)val, 7);
    break;

  case PinDataType::pdtTime16:

    break;

  case PinDataType::pdtString:
    Serial.print((char *)val);
    break;

  default:
    break;
  }
}

void printPin(PinInfo *pi)
{
  Serial.print(F("Pin id: "));
  Serial.print(pi->id);
  Serial.print(F(" Name: "));
  printStr_P(pi->name);
  Serial.print(F(" Type: "));
  Serial.print(pi->type);
  if ((pi->type == PinDataType::pdtByteArray) || (pi->type == PinDataType::pdtString))
  {
    Serial.print(F(" MaxLen: "));
    Serial.print(pi->maxLen);
  }
  Serial.print(F(" Value: "));
  printPdtValue(pi->type, pi->getValue());
  Serial.println();
}

int cmdInfo(CmdProc::Proc *c)
{
  char *s;
  if ((s = c->GetNextToken()) == nullptr)
  {
    Serial.print(F("Pins: "));
    Serial.println(config.getPinCount());
    return 0;
  }
  else
  {
    int pin;
    if (CmdProc::tryParseInt(s, pin))
    {
      PinInfo *pi = config.getPinByIndex(pin);
      if (pi != nullptr)
      {
        printPin(pi);

        Serial.println();
        return 0;
      }
    }
  }
  return CMD_ERR_GENERALERROR;
}

int cmdQuery(CmdProc::Proc *c)
{
  char *s;
  s = c->GetNextToken();
  int pin;
  if (CmdProc::tryParseInt(s, pin))
  {
    PinInfo *pi = config.getPinById(pin);
    if (pi != nullptr)
    {
      printPin(pi);
    }
    else
    {
      return CMD_ERR_GENERALERROR;
    }
  }
  return 0;
}

int cmdSet(CmdProc::Proc *c)
{
  int i;
  int32_t l;
  float f;
  char *s;
  s = c->GetNextToken();
  int pin;
  if (CmdProc::tryParseInt(s, pin))
  {
    PinInfo *pi = config.getPinById(pin);
    s = c->GetNextToken();
    switch ((pi->type))
    {
    case PinDataType::pdtBool:
      if (CmdProc::tryParseInt(s, i))
      {
        bool b = (bool)i;
        //pi->setValue(&b);
        config.setValue(pi, &b);
        return 0;
      }
      break;

    case PinDataType::pdtByte:
    case PinDataType::pdtSByte:
      if (CmdProc::tryParseInt(s, i, true))
      {
        uint8_t b = i;
        //pi->setValue(&b);
        config.setValue(pi, &b);
        return 0;
      }
      break;

    case PinDataType::pdtInt16:
    case PinDataType::pdtUInt16:
      if (CmdProc::tryParseInt(s, i, true))
      {
        //pi->setValue(&i);
        config.setValue(pi, &i);
        return 0;
      }
      break;

    case PinDataType::pdtInt32:
    case PinDataType::pdtUInt32:
      if (CmdProc::tryParseInt32(s, l, true))
      {
        //pi->setValue(&l);
        config.setValue(pi, &l);
        return 0;
      }
      break;

    case PinDataType::pdtFloat:
      if (CmdProc::tryParseFloat(s, f))
      {
        Serial.print(F("ParseFloat returned "));
        Serial.print(f, 7);
        //pi->setValue(&f);
        config.setValue(pi, &f);
        return 0;
      }

    case PinDataType::pdtString:
      //pi->setValue(s, strlen(s));
      config.setValue(pi, s, strlen(s));
      return 0;
      break;

    case PinDataType::pdtTime16:
    {
      int hour, minute, second;
      Serial.print(F("set time:"));
      bool ok = CmdProc::tryParseTime(s, hour, minute, second);
      Serial.println(ok);
      if (ok)
      {
        uint16_t t = toTime16(hour, minute, second);
        config.setValue(pi, &t);
        return 0;
      }
      break;
    }

    case PinDataType::pdtDate16:
    {
      int day, month, year;
      Serial.print(F("set date:"));
      bool ok = CmdProc::tryParseDate(s, day, month, year);
      Serial.println(ok);
      if (ok)
      {
        uint16_t t = toDate16(day, month, year);
        config.setValue(pi, &t);
        return 0;
      }
      break;
    }

    case PinDataType::pdtByteArray:
    {
      int n = c->GetTokenCount() - 2; // remaining tokens minus 'set' and 'pinId'
      Serial.print(F("tokens-2:"));
      Serial.println(n);
      uint8_t *bytes = new uint8_t(n);
      for (int j = 0; j < n; j++)
        if (CmdProc::tryParseInt(s, i, false))
        {
          Serial.print(i);
          Serial.print(',');
          bytes[j] = (uint8_t)i;
          s = c->GetNextToken();
        }
        else
        {
          delete[] bytes;
          return 0;
        }
      config.setValue(pi, bytes, n);
      delete[] bytes;
    }

    case PinDataType::pdtUnknown:
      return CMD_ERR_INVALIDVALUE;
    }
  }
  return CMD_ERR_GENERALERROR;
}

int cmdStartbin(CmdProc::Proc *c)
{
  sbuf.binaryMode();
  return 0;
}

int cmdReset(CmdProc::Proc *c)
{
  return 0;
}

/*
void testConfiga()
{
  Serial.print(sw1name);
  Serial.print(": ");
  Serial.println(sw1);
  Serial.print(count1name);
  Serial.print(": ");
  Serial.println(count1);

  buf[0] = 1;
  buf[1] = 0;
  buf[2] = 1;

  buf[3] = 2;
  buf[4] = 3;
  buf[5] = 1;
  buf[6] = 2;

  Serial.println("Process message");
  parser.init(buf, 7);
  Serial.println(parser.checkAllValues());

  config.processSet(buf, 7);

  Serial.print(sw1name);
  Serial.print(": ");
  Serial.println(sw1);
  Serial.print(count1name);
  Serial.print(": ");
  Serial.println(count1);

  memset(dest, 0, 40);
  buf[0] = 1;
  buf[1] = 2;
  config.processQuery(buf, 2, dest, 50);
  for (int i = 0; i < 40; i++)
  {
    Serial.print(dest[i], DEC);
    Serial.print(", ");
  }

  memset(dest, 0, 40);

  int len = config.processInfo(buf, 0, dest, 50);
  for (int i = 0; i < len; i++)
  {
    Serial.print(dest[i], DEC);
    Serial.print(", ");
  }

  buf[0] = 1;
  len = config.processInfo(buf, 1, dest, 50);
  Serial.println(len);
  for (int i = 0; i < len; i++)
  {
    Serial.print(dest[i], DEC);
    Serial.print(", ");
  }

  buf[1] = 2;
  len = config.processInfo(buf, 2, dest, 50);
  Serial.println(len);
  for (int i = 0; i < len; i++)
  {
    Serial.print(dest[i], DEC);
    Serial.print(", ");
  }

  buf[0] = 3;
  len = config.processInfo(buf, 1, dest, 50);
  for (int i = 0; i < len; i++)
  {
    Serial.print(dest[i], DEC);
    Serial.print(", ");
  }
}

void testParsera()
{
  buf[0] = 1;
  buf[1] = 0;
  buf[2] = 0;

  buf[3] = 2;
  buf[4] = 0;
  buf[5] = 1;

  buf[6] = 3;
  buf[7] = 3;
  buf[8] = 1;
  buf[9] = 2;

  buf[10] = 4;
  buf[11] = 10; // pdtString "BAC"
  buf[12] = 3;
  buf[13] = 66;
  buf[14] = 65;
  buf[15] = 67;
  buf[16] = 64;

  Serial.print("checkAllValues vraca ");
  Serial.println(parser.checkAllValues());

  bool q1 = true, q2 = false;
  uint16_t u = 42;
  uint8_t pinId = 0;

  Serial.print("q1:");
  Serial.print(q1);
  Serial.print(" q2:");
  Serial.println(q2);

  uint8_t l;
  PinDataType pdt;
  parser.init(buf, 16);

  Serial.print("Pos: ");
  Serial.print(parser.getPos());
  Serial.print(" Len: ");
  Serial.println(parser.getPos());

  if (parser.getNextValue(&q1, sizeof(q1), pinId, pdt, l))
  {
    Serial.print("getNextValue za q1 daje pinId:");
    Serial.print(pinId);
    Serial.print(" pdt: ");
    Serial.print(pdt);
    Serial.print(" value: ");
    Serial.println(q1);
    parser.moveNext();
  }
  else
  {
    Serial.println("getNextValue za q1 failed");
  }

  Serial.print("Pos: ");
  Serial.print(parser.getPos());
  Serial.print(" Len: ");
  Serial.println(parser.getLen());
  if (parser.getNextValue(&q2, sizeof(q2), pinId, pdt, l))
  {
    Serial.print("getNextValue za q2 daje pinId:");
    Serial.print(pinId);
    Serial.print(" pdt: ");
    Serial.print(pdt);
    Serial.print(" value: ");
    Serial.println(q2);
    parser.moveNext();
  }
  else
  {
    Serial.println("getNextValue za q2 failed");
  }

  Serial.print("Pos: ");
  Serial.print(parser.getPos());
  Serial.print(" Len: ");
  Serial.println(parser.getLen());
  if (parser.getNextValue(&u, sizeof(u), pinId, pdt, l))
  {
    Serial.print("getNextValue za u daje pinId:");
    Serial.print(pinId);
    Serial.print(" pdt: ");
    Serial.print(pdt);
    Serial.print(" value: ");
    Serial.println(u);
    parser.moveNext();
  }
  else
  {
    Serial.println("getNextValue za u failed");
  }

  Serial.print("Pos: ");
  Serial.print(parser.getPos());
  Serial.print(" Len: ");
  Serial.println(parser.getLen());
  if (parser.getNextValue(dest, 4, pinId, pdt, l))
  {
    Serial.print("getNextValue za string daje pinId:");
    Serial.print(pinId);
    Serial.print(" pdt: ");
    Serial.print(pdt);
    Serial.print(" length: ");
    Serial.print(l);
    Serial.print(" bajtova: ");
    Serial.println((char *)dest);
    parser.moveNext();
    
    //for (int i = 0; i < l; i++)
    //{
    //  Serial.print(dest[i], DEC);
    //  Serial.print(", ");
    //}
    //Serial.println();
    
  }
  else
  {
    Serial.println("getNextValue za string failed");
  }
  Serial.print("Pos: ");
  Serial.print(parser.getPos());
  Serial.print(" Len: ");
  Serial.println(parser.getLen());

  //cfg.init();
  //Serial.println("Entering cfg.loop()");
  //cfg.loop();
  //Serial.println("Finished cfg.loop()");
}
*/