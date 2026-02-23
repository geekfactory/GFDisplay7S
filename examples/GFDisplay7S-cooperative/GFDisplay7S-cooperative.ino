/**
   GeekFactory - "INNOVATING TOGETHER"
   www.geekfactory.mx

   Example of how to use the GFDisplay7S library in a cooperative multitasking environment,
   where the display refreshing is performed in the main loop instead of a timer interrupt.
   
   This example is useful for platforms that don't support timer interrupts or when you want to
   keep the code simple without using interrupts.
*/
#include <GFDisplay7S.h>

// Array that defines the pins used for segments
// Order: {a, b, c, d, e, f, g, dp}
// Use GFDISPLAY7S_UNUSED_PIN if DP is not connected
const uint8_t segmentPins[] = { 5, 6, 7, 8, 9, 10, 11, GFDISPLAY7S_UNUSED_PIN };

// Array that defines the pins used to drive each digit’s common pin (left to right)
const uint8_t commonPins[] = { 12, 13, A0, A1, A2, A3 };

// Instantiate an object to drive the display array.
// First array = segment pins {a, b, c, d, e, f, g, dp}
// Second array = common pins for each digit (left to right)
// Third parameter = number of digits
GFDisplay7S display7s(segmentPins, commonPins, 6);

void setup() {
  // call begin() to prepare object for use
  display7s.begin();

  // call print() to display numeric data
  display7s.print("123456");

  // make the last digit (index 5, zero-based) blink
  display7s.blinkDigit(5);
}

void loop() {
  // do other processing in a non-blocking fashion here...

  // do_some_stuff();

  // then refresh the display array as needed
  static uint32_t lastDisplayScan = 0;
  uint32_t now = micros();
  // check if it's time to refresh the display array
  if ((uint32_t)(now - lastDisplayScan) >= 3000) {
    // call the process methond
    display7s.process();
    // schedule next execution
    lastDisplayScan += 3000;
  }
}