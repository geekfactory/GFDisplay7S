/**
   GeekFactory - "INNOVATING TOGETHER"
   www.geekfactory.mx

   Basic example for GFDisplay7S, this example illustrates how to print numbers on the display array.
*/
#include <GFDisplay7S.h>

// Array that defines the pins used for segments
// Order: {a, b, c, d, e, f, g, dp}
// Use GFDISPLAY7S_UNUSED_PIN if DP is not connected
const uint8_t segmentPins[] = {5, 6, 7, 8, 9, 10, 11, GFDISPLAY7S_UNUSED_PIN};

// Array that defines the pins used to drive each digit’s common pin (left to right)
const uint8_t commonPins[] = {12, 13, A0, A1, A2, A3};

// Instantiate an object to drive the display array.
// First array = segment pins {a, b, c, d, e, f, g, dp}
// Second array = common pins for each digit (left to right)
// Third parameter = number of digits
GFDisplay7S display7s(segmentPins, commonPins, 6);

void setup()
{
  // call begin() to prepare object for use
  display7s.begin();

  // call print() to display numeric data
  display7s.print("123456");

  // make the last digit (index 5, zero-based) blink
  display7s.blinkDigit(5);
}

void loop()
{
  // Refresh the display (call this frequently)
  display7s.process();

  // Small delay to control refresh rate
  // Smaller delays = smoother display, but more CPU usage
  // In practical use, replace this with a timer ISR for best performance
  delay(3);
}
