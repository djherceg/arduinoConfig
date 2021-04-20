#include <ByteArray.h>
#include <string.h>
#include <stdlib.h>

ByteArray::ByteArray(uint8_t size)
{
    buffer = (uint8_t *)malloc(size);
    pos = 0;
    maxlen = size;
}

uint8_t ByteArray::write(uint8_t b)
{
    if (pos < maxlen)
    {
        buffer[pos] = b;
        pos++;
        return pos; // vraća sledeću poziciju u baferu
    }
    else
    {
        return 0; // vraća 0 ako upis nije uspeo
    }
}

void *ByteArray::copyfrom(void *src, uint8_t len, uint8_t destPos)
{
    if (destPos + len > maxlen)
    {
        return nullptr; // kopiranje bi premašilo granice bafera, pa se ne izvršava
    }
    else
    {
        return memcpy(buffer + destPos, src, len);
    }
}

void ByteArray::reset()
{
    pos = 0;
}

uint8_t ByteArray::setPos(uint8_t p)
{
    if (p >= maxlen) // ako je zadata pozicija izvan bafera
    {
        pos = maxlen; // postavi je na kraj bafera
    }
    else
    {
        pos = p;
    }
    return p; // vrati postavljenu poziciju
}

uint8_t ByteArray::getPos(uint8_t p)
{
    return pos;
}

uint8_t ByteArray::getLen()
{
    return len;
}

uint8_t ByteArray::setLen(uint8_t newlen)
{
    if (newlen <= maxlen)
    {
        len = newlen;
        pos = 0;
        return newlen;
    }
    return 0;       // zadavanje dužine sadržaja nije uspelo
}

uint8_t ByteArray::getMaxLen()
{
    return maxlen;
}

uint8_t ByteArray::operator[](uint8_t index)
{
    if (index >= len)
    {
        return (uint8_t)0;
    }
    else
    {
        return buffer[index];
    }
}