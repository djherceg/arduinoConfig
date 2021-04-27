Ovde ce biti uputstva

Verzije
=======
25.4.2021. v1.4.1
- Start developing support for ESP32. 
  The first issue is to remove the dependency avr/pgmspace.h and to read pin names from RAM instead of PROGMEM.
  Arduino version still requires pin names in PROGMEM

TODO:
- Enable pin names to be stored either in PROGMEM or RAM on Arduino