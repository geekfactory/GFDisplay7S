/**
   GeekFactory - "INNOVATING TOGETHER"
   www.geekfactory.mx

   This example shows how to use the library with a timer interrupt to keep the 
   refresh routine outside of the main loop.

   This example requires the TimerOne library, available in the Arduino Library Manager
   (search for "TimerOne" by Paul Stoffregen).
*/
#include <GFDisplay7S.h>
#include <TimerOne.h>

// Array that defines the pins used for segments
// Order: {a, b, c, d, e, f, g, dp}
// Use GFDISPLAY7S_UNUSED_PIN if DP is not connected
const uint8_t segmentPins[] = { 5, 6, 7, 8, 9, 10, 11, GFDISPLAY7S_UNUSED_PIN };

// Array that defines the pins used to drive each digit’s common pin (left to right)
const uint8_t commonPins[] = { 12, 13, A0, A1, A2, A3 };

// Instantiate an object to drive the display array
GFDisplay7S display7s(segmentPins, commonPins, 6, 3000);

void setup() {
  // Prepare object for use
  display7s.begin();

  // Setup Timer1 to call the display refresh interrupt routine every 3 ms
  // Adjust this value for smoother display (smaller = brighter, but higher CPU usage)
  Timer1.initialize(3000);
  Timer1.attachInterrupt(refreshDisplayISR);

  // Print initial data
  display7s.print("12.3456");

  // Make the last digit (index 5, zero-based) blink
  display7s.blinkDigit(5);
}

void loop() {
  // we can call delay or do other things in the main loop and
  // the display will be updated anyway
  display7s.setBlinkInterval(100);
  delay(5000);
  display7s.setBlinkInterval(300);
  delay(5000);
}

/**
 * This ISR is called by Timer1 to refresh the display.
 * Note: process() is lightweight and safe to call from an ISR.
 */
void refreshDisplayISR() {
  display7s.process();
}
