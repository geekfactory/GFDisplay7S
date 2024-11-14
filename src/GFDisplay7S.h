#ifndef GFDISPLAY7S_H
#define GFDISPLAY7S_H

#include <Arduino.h>
#include "Print.h"

// stores the segments needed to form the 0 to 9 digits
const uint8_t gliphSegments[18] = { 0x3F, 0x06, 0x5b, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x00, 0x40 };

class GFDisplay7S : public Print {
public:
  /**
  * @brief Constructor for GFDisplay7S class.
  *
  * Configures a GFDisplay7S object to be used to drive multiplexed 7 segment displays.
  *
  * @param segmentPins An array containing the pin numbers that control individual segments on the
  * displays, must contain eight uint8_t elements.
  * @param commonDriverPins An array containing the pin numbers that control the common anodes or
  * cathodes of the display array.
  * @param displaysCount The total number of multiplexed displays, this number should match the
  * number of items stored in commonDriverPins array.
  * @param segmentActiveLvl The active level on the pins that cause a segment to be lit, can be set
  * to HIGH or LOW.
  * @param commonActiveLvl The active level on the pins that causes the common anode or cathode
  * driver circuit to enable a display.
  */
  GFDisplay7S(const uint8_t* const segmentPins, const uint8_t* const commonDriverPins, const uint8_t displaysCount, const uint8_t segmentActiveLvl = HIGH, const uint8_t commonActiveLvl = HIGH);

  /**
  * @brief Prepares the object for use
  *
  * Initializes the IO pins for use and prepares the object to be used. Should be called from the
  * Arduino setup() function.
  */
  void begin();

  /**
  * @brief Refresh the multiplexed displays
  *
  * This function should be called periodically to refresh the displays. Inside this function we
  * send the current value to the active display and enable it using the common pin driver circuit.
  *
  * In every call to this function will change the currently enabled display.
  */
  void process();

  /**
  * @brief Clears all the displays
  *
  * This function clears the displays by writing a space character in every position of the display
  * buffer.
  */
  void clear();

  /**
  * @brief Moves the cursor to the first position of the display
  *
  * Moves the cursor to the begining of the display array. This function doesn't change the display
  * buffer contents.
  */
  void home();

  /**
  * @brief Enables the display array
  *
  * Enables the display array showing the contents of the buffer on the 7 segment displays.
  */
  void display();

  /**
  * @brief Disables the display array
  *
  * Disables the display array, turns off the displays and disables most of the processing that is
  * performed on the process() funcion.
  */
  void noDisplay();

  /**
  * @brief Enable blinking on the selected display digit
  *
  * Enabled the blinking effect on the provided digit of the display. The parameter defines which
  * digit to blink.
  *
  * @param digit The digit to blink where 0 is the leftmost digit.
  */
  void blinkDigit(uint8_t digit);

  void noBlinkDigit(uint8_t digit);

  void noBlink();

  void setCursor(uint8_t pos);

  /**
  * @brief Writes a single digit to the display and advance the cursor
  *
  * Writes a digit to the current cursor position and moves the cursor to the next position.
  */
  virtual size_t write(uint8_t);

  // import other write forms from print class
  using Print::write;
protected:
  /**
  Pointer to an array of 8 elements containing the pins where the display segments are connected
  */
  const uint8_t* const _segmentPins;

  /**
  Pointer to array that stores the pins connected to the common anode or cathode driver circuits
  */
  const uint8_t* const _commonDriverPins;

  /**
  Total number of multiplexed displays
  */
  const uint8_t _displaysCount;

  /**
  Active level for the display segments
  */
  const uint8_t _segmentActiveLvl;

  /**
  Common anode / cathode driver circuit active level
  */
  const uint8_t _commonActiveLvl;

  /**
  Controls if the display is enabled or disabled
  */
  bool _enabled = false;

  bool _blinkVar = false;

  uint32_t _blinkTime = 0;

  /**
  Cursor that controls where the next digit will be written
  */
  uint8_t _cursor;

  /**
  Buffer that stores the gliph's codes that are shown in the displays
  */
  uint8_t _displayBuffer[16];

  uint8_t _effectBuffer[16];

  /**
  Size in bytes for the display buffer.
  */
  const uint8_t _displayBufferSize = sizeof(_displayBuffer);

  /**
  For display multiplexing, selects the currently enabled display
  */
  uint8_t _currentDigit = 0;

  uint8_t mapCharToGliph(uint8_t c);

  void writeSegmentPins(uint8_t digit);

  void shutDownDisplays();
};

#endif