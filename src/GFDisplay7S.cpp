#include "GFDisplay7S.h"

// stores the segments needed to form the 0 to 9 digits
static const uint8_t glyphSegments[18] = {0x3F, 0x06, 0x5b, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x00, 0x40};

GFDisplay7S::GFDisplay7S(const uint8_t *const segmentPins, const uint8_t *const commonDriverPins, const uint8_t displaysCount, uint32_t refreshPeriod, uint8_t segmentActiveLvl, uint8_t commonActiveLvl)
    : _segmentPins(segmentPins), _commonDriverPins(commonDriverPins), _displaysCount(displaysCount), _useISRTimebase(refreshPeriod > 0), _segmentActiveLvl(segmentActiveLvl), _commonActiveLvl(commonActiveLvl)
{
  _isrPeriod = refreshPeriod;
}

void GFDisplay7S::begin()
{
  // initialize segment pins as outputs
  for (uint8_t i = 0; i < NUM_SEGMENTS; i++)
    if (_segmentPins[i] != GFDISPLAY7S_UNUSED_PIN)
      pinMode(_segmentPins[i], OUTPUT);

  // initialize common anode or cathode driver pins as outputs
  for (uint8_t i = 0; i < _displaysCount; i++)
    if (_commonDriverPins[i] != GFDISPLAY7S_UNUSED_PIN)
      pinMode(_commonDriverPins[i], OUTPUT);

  // if using ISR timebase, compute the number of ISR ticks needed to reach the blink interval
  if (_useISRTimebase)
  {
    if (_isrPeriod == 0)
      _isrPeriod = 5000; // default to 5 us if not set (should not happen)
    _blinkTimer = 0;
    setBlinkInterval(_blinkInterval);
  }
  else
  {
    _blinkTimer = millis();
  }

  // turn off segments and common pin drivers
  shutDownDisplays();
  // clear buffer and set cursor at the begining of the buffer
  clear();
  // turn on the display
  display();
}

void GFDisplay7S::process()
{
  // if display is disabled, do nothing
  if (!_enabled)
    return;

  // update blink variable if needed
  updateBlinkState();

  // turn off the previous display
  digitalWrite(_commonDriverPins[(_currentDigit == 0) ? (_displaysCount - 1) : (_currentDigit - 1)], !_commonActiveLvl);
  // write the segment data to IO pins
  writeSegmentPins(_displayBuffer[_currentDigit], _effectBuffer[_currentDigit]);
  // turn on the current display
  digitalWrite(_commonDriverPins[_currentDigit], _commonActiveLvl);
  // check if we're on the last display and start over
  if (++_currentDigit >= _displaysCount)
    _currentDigit = 0;
}

void GFDisplay7S::clear()
{
  _cursor = 0;
  for (uint8_t i = 0; i < _displayBufferSize; i++)
  {
    _displayBuffer[i] = charToSegments(' ');
    _effectBuffer[i] = 0;
  }
}

void GFDisplay7S::home()
{
  _cursor = 0;
}

void GFDisplay7S::display()
{
  _enabled = true;
}

void GFDisplay7S::noDisplay()
{
  _enabled = false;
  _currentDigit = 0;
  shutDownDisplays();
}

void GFDisplay7S::blinkDigit(uint8_t digit)
{
  if (digit >= _displayBufferSize)
    return;
  _effectBuffer[digit] |= GFDISPLAY7S_FLAG_BLINK;
}

void GFDisplay7S::noBlinkDigit(uint8_t digit)
{
  if (digit >= _displayBufferSize)
    return;
  _effectBuffer[digit] &= ~GFDISPLAY7S_FLAG_BLINK;
}

void GFDisplay7S::noBlink()
{
  for (uint8_t i = 0; i < _displayBufferSize; i++)
    _effectBuffer[i] &= ~GFDISPLAY7S_FLAG_BLINK;
}

void GFDisplay7S::setBlinkInterval(uint16_t interval)
{
  if (_useISRTimebase)
    _blinkInterval = (interval * 1000UL) / _isrPeriod;
  else
    _blinkInterval = interval;
}

void GFDisplay7S::setCursor(uint8_t pos)
{
  _cursor = (pos < _displayBufferSize) ? pos : _displayBufferSize;
}

void GFDisplay7S::setDecimalPoint(uint8_t digit, bool on)
{
  if (digit >= _displayBufferSize)
    return;
  if (on)
    _displayBuffer[digit] |= GFDISPLAY7S_DECIMAL_POINT_BIT;
  else
    _displayBuffer[digit] &= ~GFDISPLAY7S_DECIMAL_POINT_BIT;
}

size_t GFDisplay7S::write(uint8_t character)
{
  if (_cursor >= _displayBufferSize)
    return 0;

  if (character == '.')
  {
    if (_cursor == 0)
      return 0; // nothing to attach dot to
    _displayBuffer[_cursor - 1] |= GFDISPLAY7S_DECIMAL_POINT_BIT;
    return 1;
  }

  _displayBuffer[_cursor] = charToSegments(character);
  _cursor++;
  return 1;
}

uint8_t GFDisplay7S::charToSegments(uint8_t c)
{
  if (c >= '0' && c <= '9')
    return glyphSegments[c - '0'];
  if (c >= 'A' && c <= 'F')
    return glyphSegments[10 + (c - 'A')];
  if (c >= 'a' && c <= 'f')
    return glyphSegments[10 + (c - 'a')];
  if (c == ' ')
    return glyphSegments[16];
  if (c == '-')
    return glyphSegments[17];

  return glyphSegments[17];
}

void GFDisplay7S::writeSegmentPins(uint8_t segmentValues, uint8_t effectFlags)
{
  // if blinking is enabled and the blink variable is set, turn off the digit
  if ((effectFlags & GFDISPLAY7S_FLAG_BLINK) && _blinkVar)
    segmentValues = charToSegments(' ');

  // iterate over segment pins and set them according to the glyph code
  for (uint8_t i = 0; i < NUM_SEGMENTS; i++)
  {
    if (segmentValues & (1 << i))
      pinWrite(_segmentPins[i], _segmentActiveLvl);
    else
      pinWrite(_segmentPins[i], !_segmentActiveLvl);
  }
}

void GFDisplay7S::shutDownDisplays()
{
  // write inactive state to segment pins
  for (uint8_t i = 0; i < NUM_SEGMENTS; i++)
    pinWrite(_segmentPins[i], !_segmentActiveLvl);
  // disable common pin drivers
  for (uint8_t i = 0; i < _displaysCount; i++)
    pinWrite(_commonDriverPins[i], !_commonActiveLvl);
}

void GFDisplay7S::updateBlinkState()
{
  if (!_useISRTimebase)
  {
    uint32_t now = millis();
    if (now - _blinkTimer >= _blinkInterval)
    {
      _blinkTimer = now;
      _blinkVar = !_blinkVar;
    }
  }
  else
  {
    // ISR cycle-based mode
    if (++_blinkTimer >= _blinkInterval)
    {
      _blinkTimer = 0;
      _blinkVar = !_blinkVar;
    }
  }
}

inline void GFDisplay7S::pinWrite(uint8_t pin, uint8_t val)
{
  if (pin != GFDISPLAY7S_UNUSED_PIN)
    digitalWrite(pin, val);
}
