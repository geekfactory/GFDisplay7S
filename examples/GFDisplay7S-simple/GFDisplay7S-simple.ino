/**
   GeekFactory - "INNOVATING TOGETHER"
   www.geekfactory.mx

   Basic example for GFDisplay7S, this example illustrates how to
   print numbers on the display array.
*/
#include <GFDisplay7S.h>

// instantiate an object to drive the display array, the segment and common pins numbers are passed as arrays
// the last parameter is the number of displays we're driving
GFDisplay7S display7s((const uint8_t[]){ 5, 6, 7, 8, 9, 10, 11, 0xFF }, (const uint8_t[]){ 12, 13, A0, A1, A2, A3 }, 6);

void setup()
{
  // call begin() to prepare object for use
  display7s.begin();
}

void loop()
{
	// call print() to display numeric data
	display7s.print("123456");

	// refresh the display array
	for(;;) {
		display7s.process();
		delay(3);
	}
}
