#include "GFDisplay7S.h"

static constexpr uint8_t GFDISPLAY7S_GLYPH_CODE_MASK = 0x7F;
// stores the segments needed to form the 0 to 9 digits
static const uint8_t glyphSegments[18] = {0x3F, 0x06, 0x5b, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x00, 0x40};

GFDisplay7S::GFDisplay7S(const uint8_t *const segmentPins, const uint8_t *const commonDriverPins, const uint8_t displaysCount, uint8_t segmentActiveLvl, uint8_t commonActiveLvl)
    : _segmentPins(segmentPins), _commonDriverPins(commonDriverPins), _displaysCount(displaysCount), _segmentActiveLvl(segmentActiveLvl), _commonActiveLvl(commonActiveLvl)
{
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
  uint32_t now = millis();
  if (now - _blinkTime >= _blinkInterval)
  {
    _blinkTime = now;
    _blinkVar = !_blinkVar;
  }

  uint8_t glyphCode = _displayBuffer[_currentDigit] & GFDISPLAY7S_GLYPH_CODE_MASK;
  bool dp = (_displayBuffer[_currentDigit] & GFDISPLAY7S_FLAG_DP) != 0;

  // turn off the previous display
  digitalWrite(_commonDriverPins[(_currentDigit == 0) ? (_displaysCount - 1) : (_currentDigit - 1)], !_commonActiveLvl);
  // write the segment data to IO pins
  writeSegmentPins(glyphCode, _effectBuffer[_currentDigit], dp);
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
    _displayBuffer[i] = mapCharToGlyph(' ');
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
    _displayBuffer[digit] |= GFDISPLAY7S_FLAG_DP;
  else
    _displayBuffer[digit] &= ~GFDISPLAY7S_FLAG_DP;
}

size_t GFDisplay7S::write(uint8_t character)
{
  if (_cursor >= _displayBufferSize)
    return 0;

  uint8_t glyph = mapCharToGlyph(character);
  uint8_t dp = _displayBuffer[_cursor] & GFDISPLAY7S_FLAG_DP;
  _displayBuffer[_cursor] = glyph | (dp ? GFDISPLAY7S_FLAG_DP : 0);

  _cursor++;
  return 1;
}

uint8_t GFDisplay7S::mapCharToGlyph(uint8_t character)
{
  // characters for digits from 0 to 9 map to 0x00 to 0x09
  if (character >= '0' && character <= '9')
    return character - '0';
  // map ASCII leters corresponding to hex digits to the corresponding glyph positions in array
  switch (character)
  {
  case 'A':
  case 'a':
    return 0x0A;
  case 'B':
  case 'b':
    return 0x0B;
  case 'C':
  case 'c':
    return 0x0C;
  case 'D':
  case 'd':
    return 0x0D;
  case 'E':
  case 'e':
    return 0x0E;
  case 'F':
  case 'f':
    return 0x0F;
  case ' ':
    return 0x10;
  case '-':
    return 0x11;
  default:
    return 0x11; // unknown characters are mapped to the '-' glyph
  }
}

void GFDisplay7S::writeSegmentPins(uint8_t glyphCode, uint8_t effectFlags, bool dp)
{
  // if blinking is enabled and the blink variable is set, turn off the digit
  if ((effectFlags & GFDISPLAY7S_FLAG_BLINK) && _blinkVar)
    glyphCode = mapCharToGlyph(' ');

  // iterate over segment pins and set them according to the glyph code
  for (uint8_t i = 0; i < NUM_SEGMENTS; i++)
  {
    if (i == 7)
    {
      pinWrite(_segmentPins[i], dp ? _segmentActiveLvl : !_segmentActiveLvl);
      continue;
    }

    if (glyphSegments[glyphCode] & (1 << i))
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

void GFDisplay7S::pinWrite(uint8_t pin, uint8_t val)
{
  if (pin != GFDISPLAY7S_UNUSED_PIN)
    digitalWrite(pin, val);
}
