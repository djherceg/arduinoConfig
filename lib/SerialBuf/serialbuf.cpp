#include "serialbuf.h"
#include "Arduino.h"

void SerialBuf::init()
{
    init(100); // 0.1 seconds to enter command
}

void SerialBuf::init(uint32_t stopInterval)
{
    clear();
    this->stopInterval = stopInterval;
}

void SerialBuf::clear()
{
    overflow = false;
    finished = false;
    length = 0;
    position = 0;
    buffer[0] = 0; // null-terminate
    mils = millis();
}

void SerialBuf::loop()
{
    if (finished)
    {
        return;
    }

    if ((mode == SERIALBUF_BINARYMODE) && (length > 0) && (millis() - mils > stopInterval))
    {
        finished = true;
        return;
    }

    while (Serial.available())
    {
        mils = millis();

        int r = Serial.read();
        if (mode == SERIALBUF_TEXTMODE)
        {
            if (r == 13)
                continue;

            if (r == 10)
            {
                finished = true;
                return;
            }
        }

        if (length < maxbuf)
        {
            //int r = Serial.read();
            if (r > -1)
            {
                buffer[length++] = (char)r;
                buffer[length] = 0; // null-terminate
            }
        }
        else
        {
            overflow = true;
            clear();
        }
    }
}

void SerialBuf::textMode()
{
    mode = SERIALBUF_TEXTMODE;
    clear();
}

void SerialBuf::binaryMode()
{
    mode = SERIALBUF_BINARYMODE;
    clear();
}

int SerialBuf::getMode()
{
    return mode;
}

int SerialBuf::peek(uint8_t offset)
{
    if (position + offset < length)
    {
        return buffer[position + offset];
    }
    else
    {
        return -1;
    }
}

/* int SerialBuf::peeknext()
{
    if ((position + 1) < length)
    {
        return buffer[position + 1];
    }
    else
    {
        return -1;
    }
}*/

bool SerialBuf::iscurrent()
{
    return position < length;
}

bool SerialBuf::isnext()
{
    return (position + 1) < length;
}

bool SerialBuf::isnextn(int n)
{
    return (position + n) <= length;
}

/**
 * Čita karakter iz bafera ili vraća -1 ako ne postoji
 */
int SerialBuf::read()
{
    if (!isnext())
        return -1;

    return buffer[position++];
}

bool SerialBuf::isoverflow()
{
    return overflow;
}

bool SerialBuf::isavailable()
{
    return finished;
}

int SerialBuf::getLength()
{
    return length;
}

int SerialBuf::getPosition()
{
    return position;
}