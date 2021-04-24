# arduinoConfig
The library maintains runtime configuration parameters (called Pins) which are stored in variables. Parameters are registered with metadata.

Supported operations include:
  - Query pin metadata
  - Query pin value
  - Set pin value
  - Query device information

The library relies on the BytePacker library to pack and parse binary data.

Example
Let us demonstrate how the Configuration library is used. There are several steps:
	- Include the library header files
	- Declare state variables and their names
	- Instantiate the ConfigInfo object
	- Declare callback functions for pin value changes
	- Register state variables as pins with the library
	- Implement callbacks
	- Call the processCommand method to process configuration commands

Include the library.

	#include <ConfigInfo.h>

Declare state variables and provide initial values. Pin names are kept in PROGMEM to reduce RAM usage.

	bool sw1 = false;
	bool sw1old = false;
	int count1 = 42;
	int count1old = 42;
	float r = 37.2;
	float rold = 37.2;
	uint32_t lng = 1000;
	uint32_t lngold = 1000;
	char text1[] = {"this is a text._____"};      // pad the text to 20 chars
	char text1old[] = {"this is an old text."};   // this is also 20 chars
	const char sw1name[] PROGMEM{"Switch1"};
	const char count1name[] PROGMEM{"Counter1"};
	const char rname[] PROGMEM{"Temp1"};
	const char lngname[] PROGMEM{"LongCounter"};
	const char text1name[] PROGMEM{"Text1"};

Initialize the ConfigInfo object with the device name, manufacture date and time.

	ConfigInfo config(toDate16(2020, 11, 25), toTime16(20, 12, 00), 1, 0, deviceName);

Declare callback functions which will be invoked by the library when a pin value is changed.

	void callback(PinInfo *cfi);    // forward declaration for the general callback
	void pinCallback(PinInfo *cfi); // forward declaration for the individual pin callback

Register state variables with the ConfigInfo object and provide metadata.
	- Pin ID
	- Pin name (in PROGMEM)
	- Pin variable for the current value
	- Pin variable for the old value (optional)
	- Pin data type
	- Pin mode
	- Pin change callback (function pointer)

	  config.init(5);
	  config.addPin(201, sw1name, &sw1, &sw1old, PinDataType::pdtBool, PinMode::pmOutput, pinCallback);
	  config.addPin(202, count1name, &count1, &count1old, PinDataType::pdtInt16, PinMode::pmOutput, pinCallback);
	  config.addPin(203, rname, &r, &rold, PinDataType::pdtFloat, PinMode::pmOutput, pinCallback);
	  config.addPin(204, lngname, &lng, &lngold, PinDataType::pdtUInt32, PinMode::pmOutput, pinCallback);
	  config.addPin(205, text1name, text1, text1old, PinDataType::pdtString, PinMode::pmOutput, pinCallback);
	  config.setEventHandler(callback);
	
Implement the callbacks.

	void callback(PinInfo *pin)
	{
	  Serial.println(pin->id);
	}
	void pinCallback(PinInfo *cfi)
	{
	  Serial.println(cfi->id);
	  Serial.println(cfi->name);    // may not work because name is in PROGMEM
	  Serial.println(reinterpret_cast<const __FlashStringHelper *>(cfi->name));
	  Serial.print("Old val: ");
	  Serial.println(count1old);
	  Serial.print("New val: ");
	  Serial.println(count1);
	}

Note the __FlashStringHelper cast by the name field. This is because name points to an address in program memory, and not in RAM.

Processing incoming commands
To process incoming configuration commands, invoke the ConfigInfo::processCommand method.

	    switch (sbuf.buffer[0])
	    {
	    case '~': // config                         // Local Config command
	    {
	      uint8_t buflen = sbuf.getLength();
	      if (buflen > 2)
	      {
	        uint8_t payloadLen = sbuf.buffer[1]; // second byte is Payload size
	        if (payloadLen + 2 == buflen)        // verify that payload is as long
	        {
	          uint8_t rez = config.processCommand((uint8_t *)sbuf.buffer + 2, buflen - 2, dest.buffer, dest.getMaxLen());
	          dest.setLen(rez);
	          if (rez > 0)
	          {
	            Serial.write('~');              // Transmit the Local Config prefix,
	            Serial.write(rez);              // response length, and
	            Serial.write(dest.buffer, rez); // the actual response.
	          }
	        }
	      }
	      break;
	    }

Printing PROGMEM strings
A helper function to print C strings stored in PROGMEM. It copies the string to a temporary buffer in RAM, prints it, and finally deallocates the buffer. An alternative approach would be to read bytes in a loop directly from PROGMEM and send them to serial output.

	void printStr_P(const char *p)
	{
	  char *ptr = (char *)malloc(strlen_P(p) + 1);
	  if (ptr != NULL)
	  { // if memory allocation successful
	    strcpy_P(ptr, p);
	    Serial.print(ptr); // function overload
	    free(ptr);
	  }
	}

