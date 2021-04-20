#include "cmdProc.h"

namespace CmdProc
{
  bool tryParseFloat(char *s, float &outResult)
  {
    float res = 0, oldres = 0, q = 10;
    int pos = 0, len = strlen(s);
    char c;
    bool dec = false;
    if (len == 0)
      return false;

    bool neg = (s[0] == '-');
    if (neg)
      pos = 1;

    while (pos < len)
    {
      c = s[pos];
      if (c == '.')
      {
        if (dec)
          return false; // if we were in decimal already, this is syntax error
        else
          dec = true;
      }
      else
      {
        if (isDigit(c))
        {
          if (!dec)
            res = 10 * res + c - '0';
          else
          {
            res = res + (c - '0')/q;
            q *= 10;
            //Serial.print(F("  dec:"));
            //Serial.println(dec);
          }

          if (isinf(res) || (isnan(res)) || (oldres == res))
            return false; // if machine accuracy is reached, the result no longer changes
          else
          {
            oldres = res;
          }
        }
        else
          return false;
      }
      pos++;
    }
    outResult = res * (neg ? -1 : 1);
    return true;
  }

  bool tryParseInt(char *s, int &outResult, bool allowMinus = false)
  {
    int32_t i;
    bool res = tryParseInt32(s, i, allowMinus);
    outResult = i;
    return res;
  }

  bool tryParseInt32(char *s, int32_t &outResult, bool allowMinus = false)
  {
    int32_t r;
    //if ((!tryParseHex(s, outResult)) && (!tryParseDec(s, outResult, allowMinus)))
    if ((!tryParseHex32(s, r)) && (!tryParseDec32(s, r, allowMinus)))
    {
      outResult = 0;
      return false;
    }
    else
    {
      outResult = r;
      return true;
    }
  }

  bool tryParseDec(char *s, int &outResult, bool allowMinus = false)
  {
    int32_t i;
    bool res = tryParseDec32(s, i, allowMinus);
    outResult = i;
    return res;
  }

  bool tryParseDec32(char *s, int32_t &outResult, bool allowMinus = false)
  {
    outResult = 0;
    int n = strlen(s);
    if (n == 0)
      return false;
    int sign = 1;
    int a = 0;
    if ((allowMinus) && (s[0] == '-'))
    {
      sign = -1;
      a = 1;
    }
    for (int i = a; i < n; i++)
    {
      if (isDigit(s[i]))
      {
        outResult = 10 * outResult + sign * (s[i] - '0');
      }
      else
        return false;
    }
    return true;
  }

  bool tryParseHex32(char *s, int32_t &outResult)
  {
    // Expects '0xNN' formatted hex numbers, e.g. 0xA5 or 0x7. The length must be at least 3 characters.
    outResult = 0;
    int n = strlen(s);
    if ((n < 3) || (s[0] != '0') || (s[1] != 'x'))
      return false; // ako string ne počinje znacima "0x"

    for (int i = 2; i < n; i++)
    {
      if (isDigit(s[i]))
      {
        outResult = 16 * outResult + (s[i] - '0');
      }
      else if (isHexDigit(s[i]))
      {
        outResult = 16 * outResult + (s[i] - 'A' + 10); //
      }
      else
      {
        return false;
      }
    }
    return true;
  }

  bool isHexDigit(char c)
  {
    if (isDigit(c))
    {
      return true;
    }
    else
    {
      return (('A' <= c) && (c <= 'F'));
    }
  }

  bool tryParseTime(char *s, int &outHour, int &outMinute, int &outSecond)
  {
    char *p;
    p = strtok(s, ":");
    if ((p != NULL) && (tryParseDec(p, outHour)))
    {
      outHour = outHour % 24;
      p = strtok(NULL, ":");
      if ((p != NULL) && (tryParseDec(p, outMinute)))
      {
        outMinute = outMinute % 60;
        p = strtok(NULL, ":");
        if ((p != NULL) && (tryParseDec(p, outSecond)))
        {
          outSecond = outSecond % 60;
          return true;
        }
        else
        {
          outSecond = 0;
          return true;
        }
      }
    }
    return false;
  }

  bool tryParseDate(char *s, int &outDay, int &outMonth, int &outYear)
  {
    char *p;
    p = strtok(s, ".");
    if ((p != NULL) && (tryParseDec(p, outDay)))
    {
      p = strtok(NULL, ".");
      if ((p != NULL) && (tryParseDec(p, outMonth)))
      {
        p = strtok(NULL, ".");
        if ((p != NULL) && (tryParseDec(p, outYear)))
        {
          return isValidDate(outDay, outMonth, outYear);
        }
      }
    }
    return false;
  }

  bool isValidDate(int dd, int mm, int yy)
  {
    // preuzeto sa https://www.includehelp.com/c-programs/validate-date.aspx#:~:text=%20C%20program%20to%20validate%20date%20%28Check%20date,from%201%20to%2030%2C%201%20to...%20More%20
    if (yy >= 1900 && yy <= 9999 && mm >= 1 && mm <= 12 && dd >= 1)
    {
      if ((dd <= 31) && (mm == 1 || mm == 3 || mm == 5 || mm == 7 || mm == 8 || mm == 10 || mm == 12))
        return true;
      else if ((dd <= 30) && (mm == 4 || mm == 6 || mm == 9 || mm == 11))
        return true;
      else if ((dd <= 28) && (mm == 2))
        return true;
      else if (dd == 29 && mm == 2 && (yy % 400 == 0 || (yy % 4 == 0 && yy % 100 != 0)))
        return true;
      else
        return false;
    }
    return false;
  }

} // namespace CmdProc