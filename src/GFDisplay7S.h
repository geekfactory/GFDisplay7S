#ifndef GFDISPLAY7S_H
#define GFDISPLAY7S_H

#include <Arduino.h>
#include "Print.h"

/*-------------------------------------------------------------*
 *		Public constants for API			*
 *-------------------------------------------------------------*/
constexpr uint8_t GFDISPLAY7S_UNUSED_PIN = 0xFF;        // Use this value in the constructor to indicate a segment pin that is not used
constexpr uint8_t GFDISPLAY7S_DISPLAY_BUFFER_SIZE = 16; // size of the display buffer, maximum number of digits supported

constexpr uint8_t GFDISPLAY7S_DECIMAL_POINT_BIT = 0x80; // decimal point

constexpr uint8_t GFDISPLAY7S_FLAG_BLINK = 0x01; // blinking digit

/*-------------------------------------------------------------*
 *		Class declaration				*
 *-------------------------------------------------------------*/
class GFDisplay7S : public Print
{
public:
  /**
   * @brief Constructor for GFDisplay7S class.
   *
   * Configures a GFDisplay7S object to be used to drive multiplexed 7 segment displays.
   *
   * @param segmentPins An array containing the pin numbers that control individual segments on the
   * displays, MUST contain eight uint8_t elements.
   * @param commonDriverPins An array containing the pin numbers that control the common anodes or
   * cathodes of the display array.
   * @param displaysCount The total number of multiplexed displays, this number should match the
   * number of items stored in commonDriverPins array.
   * @param refreshPeriod If set to a non-zero value, the process() function is expected to be called
   * from a timer interrupt at this period in microseconds. If set to zero, the process() function
   * should be called from the main loop() function as often as possible.
   * @param segmentActiveLvl The active level on the pins that cause a segment to be lit, can be set
   * to HIGH or LOW.
   * @param commonActiveLvl The active level on the pins that causes the common anode or cathode
   * driver circuit to enable a display.
   */
  GFDisplay7S(const uint8_t *const segmentPins, const uint8_t *const commonDriverPins, const uint8_t displaysCount, uint32_t refreshPeriod = 0, const uint8_t segmentActiveLvl = HIGH, const uint8_t commonActiveLvl = HIGH);

  /**
   * @brief Prepares the object for use.
   *
   * Initializes the IO pins for use and prepares the object to be used. Should be called from the
   * Arduino setup() function.
   */
  void begin();

  /**
   * @brief Refresh the multiplexed displays.
   *
   * This function should be called periodically to refresh the displays. Inside this function we
   * send the current value to the active display and enable it using the common pin driver circuit.
   *
   * In every call to this function will change the currently enabled display.
   */
  void process();

  /**
   * @brief Clears all the displays.
   *
   * This function clears the displays by writing a space character in every position of the display
   * buffer.
   */
  void clear();

  /**
   * @brief Moves the cursor to the first position of the display.
   *
   * Moves the cursor to the begining of the display array. This function doesn't change the display
   * buffer contents.
   */
  void home();

  /**
   * @brief Enables the display array.
   *
   * Enables the display array showing the contents of the buffer on the 7 segment displays.
   */
  void display();

  /**
   * @brief Disables the display array.
   *
   * Disables the display array, turns off the displays and disables most of the processing that is
   * performed on the process() funcion.
   */
  void noDisplay();

  /**
   * @brief Enable blinking on the selected display digit.
   *
   * Enabled the blinking effect on the provided digit of the display. The parameter defines which
   * digit to blink.
   *
   * @param digit The digit to blink where 0 is the leftmost digit.
   */
  void blinkDigit(uint8_t digit);

  /**
   * @brief Disable blinking on the selected display digit.
   *
   * Disables the blinking effect on the provided digit of the display. The parameter defines which
   * digit to stop blinking.
   *
   * @param digit The digit to stop blinking where 0 is the leftmost digit.
   */
  void noBlinkDigit(uint8_t digit);

  /**
   * @brief Disable blinking on all display digits.
   *
   * Disables the blinking effect on all digits of the display.
   */
  void noBlink();

  /**
   * @brief Sets the blink interval for blinking digits.
   *
   * Sets the blink interval for blinking digits. The default interval is 300 milliseconds.
   *
   * @param interval The blink interval in milliseconds.
   */
  void setBlinkInterval(uint32_t interval);

  /**
   * @brief Sets the cursor to a specific position on the display.
   *
   * Sets the cursor to a specific position on the display. The next digit written to the display
   * will be written at this position.
   *
   * @param pos The position to set the cursor to, where 0 is the leftmost digit.
   */
  void setCursor(uint8_t pos);

  /**
   * @brief Sets or clears the decimal point on a specific digit.
   *
   * Sets or clears the decimal point on a specific digit of the display.
   *
   * @param digit The digit to set or clear the decimal point on, where 0 is the leftmost digit.
   * @param on If true, the decimal point is set, if false, the decimal point is cleared.
   *
   * @note This function only modifies the decimal point segment of the specified digit, it does not
   * affect the other segments of the digit.
   */
  void setDecimalPoint(uint8_t digit, bool on);

  /**
   * @brief Writes a single digit to the display and advance the cursor.
   *
   * Writes a digit to the current cursor position and moves the cursor to the next position.
   */
  virtual size_t write(uint8_t);

  // import other write forms from print class
  using Print::write;

protected:
  /**
  Constexpr defining the number of segments in a 7 segment display (including the dot segment)
   */
  static constexpr uint8_t NUM_SEGMENTS = 8;

  /**
  Pointer to an array of 8 elements containing the pins where the display segments are connected
  */
  const uint8_t *const _segmentPins;

  /**
  Pointer to array that stores the pins connected to the common anode or cathode driver circuits
  */
  const uint8_t *const _commonDriverPins;

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

  /**
  Variable that toggles at every blink interval to control blinking digits
  */
  bool _blinkVar = false;

  /**
  Interval in microseconds at which the process() function is called from a timer interrupt. It's used to compute
  the number of ISR ticks needed to reach the blink interval.
   */
  uint32_t _isrPeriod = 0;

  /**
  Stores the blink interval in milliseconds (default is 300 ms) or in ISR ticks if a refreshPeriod is given to
  the constructor.
  */
  uint32_t _blinkInterval = 300;

  /**
  Timer variable used to track the time for blinking digits. In millis() mode, it stores the last
  time the blink variable was toggled. In ISR mode, it counts the number of ISR cycles.
  */
  volatile uint32_t _blinkTimer = 0;

  /**
  Set to true if the process() function is called from a timer interrupt at a fixed frequency.
  */
  const bool _useISRTimebase = false;

  /**
  Cursor that controls where the next digit will be written
  */
  uint8_t _cursor = 0;

  /**
  Buffer that stores the current contents of the display, each byte contains the segments to be lit
  */
  uint8_t _displayBuffer[GFDISPLAY7S_DISPLAY_BUFFER_SIZE];

  /**
  Buffer that stores the effect flags for each digit (e.g., blinking).
  */
  uint8_t _effectBuffer[GFDISPLAY7S_DISPLAY_BUFFER_SIZE];

  /**
  Size in bytes for the display buffer.
  */
  const uint8_t _displayBufferSize = GFDISPLAY7S_DISPLAY_BUFFER_SIZE;

  /**
  For display multiplexing, selects the currently enabled display
  */
  uint8_t _currentDigit = 0;

  uint8_t charToSegments(uint8_t character);

  void updateBlinkState();

  void writeSegmentPins(uint8_t glyphCode, uint8_t effectFlags);

  void shutDownDisplays();

  inline void pinWrite(uint8_t pin, uint8_t val);
};

#endif