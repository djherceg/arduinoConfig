#ifndef __BYTEARRAY_
#define __BYTEARRAY_

#include <stdint.h>

class ByteArray {
    private:
        uint8_t maxlen;
        uint8_t pos;
        uint8_t len;

    public:
        uint8_t *buffer;

        ByteArray(uint8_t size);

        /** Vraća bajt iz bafera na poziciji index. Ako je index izvan opsega, vraća 0. */
        uint8_t operator [] (uint8_t index);

        /** Postavlja tekuću poziciju. Vraća postavljenu poziciju, koja može biti različita od zadate ako je van granica. */
        uint8_t setPos(uint8_t p);

        /** Vraća tekuću poziciju. */
        uint8_t getPos(uint8_t p);

        /** Vraća trenutnu dužinu bafera */
        uint8_t getLen();

        /** Zadaje dužinu sadržaja u baferu. Mora biti manja od maxlen. Resetuje pos na 0. Vraća dužinu ili 0 ako ne uspe. */
        uint8_t setLen(uint8_t newlen);

        /** Vraća maksimalnu dužinu bafera */
        uint8_t getMaxLen();

        /** Postavlja poziciju na 0 */
        void reset();

        /** Upisuje b na sledeću poziciju u baferu. Vraća sledeću poziciju ili 0 ako ne uspe. */
        uint8_t write(uint8_t b);

        /** Kopira len bajtova iz src na poziciju destPos u baferu. Vraća pointer na destination buffer ili nullptr ako ne uspe. */
        void* copyfrom(void* src, uint8_t len, uint8_t destPos);
        

};


#endif