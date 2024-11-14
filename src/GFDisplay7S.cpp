#include "GFDisplay7S.h"

GFDisplay7S::GFDisplay7S(const uint8_t* const segmentPins, const uint8_t* const commonDriverPins, const uint8_t displaysCount, uint8_t segmentActiveLvl, uint8_t commonActiveLvl)
  : _segmentPins(segmentPins), _commonDriverPins(commonDriverPins), _displaysCount(displaysCount), _segmentActiveLvl(segmentActiveLvl), _commonActiveLvl(commonActiveLvl) {
}

void GFDisplay7S::begin() {
  // initialize segment pins as outputs
  for (uint8_t i = 0; i < 9; i++)
    pinMode(_segmentPins[i], OUTPUT);
  // initialize common anode or cathode driver pins as outputs
  for (uint8_t i = 0; i < _displaysCount; i++)
    pinMode(_commonDriverPins[i], OUTPUT);

  // turn off segments and common pin drivers
  shutDownDisplays();
  // clear buffer and set cursor at the begining of the buffer
  clear();
  // turn on the display
  display();
}

void GFDisplay7S::process() {
  if (_enabled) {
    uint32_t now = millis();

    if (now - _blinkTime >= 300) {
      _blinkTime = now;
      _blinkVar = !_blinkVar;
    }
    // turn off the previous display
    digitalWrite(_commonDriverPins[(_currentDigit == 0) ? (_displaysCount - 1) : (_currentDigit - 1)], !_commonActiveLvl);
    // write the segment data to IO pins
    if (_effectBuffer[_currentDigit] == 0) {
      writeSegmentPins(_displayBuffer[_currentDigit]);
    } else {
      writeSegmentPins((_blinkVar) ? _displayBuffer[_currentDigit] : 0x10);
    }
    // turn on the current display
    digitalWrite(_commonDriverPins[_currentDigit], _commonActiveLvl);
    // check if we're on the last display and start over
    if (++_currentDigit >= _displaysCount)
      _currentDigit = 0;
  }
}

void GFDisplay7S::clear() {
  // reset cursor
  _cursor = 0;
  // fill buffer with spaces
  memset(_displayBuffer, 0x10, sizeof(_displayBuffer));
  // disable any display effects
  memset(_effectBuffer, 0, sizeof(_effectBuffer));
}

void GFDisplay7S::home() {
  _cursor = 0;
}

void GFDisplay7S::display() {
  _enabled = true;
}

void GFDisplay7S::noDisplay() {
  _enabled = false;
  _currentDigit = 0;
  shutDownDisplays();
}

void GFDisplay7S::blinkDigit(uint8_t digit) {
  if (digit >= _displayBufferSize)
    return;
  _effectBuffer[digit] = 1;
}

void GFDisplay7S::noBlinkDigit(uint8_t digit) {
  if (digit >= _displayBufferSize)
    return;
  _effectBuffer[digit] = 0;
}

void GFDisplay7S::noBlink() {
  memset(_effectBuffer, 0, sizeof(_effectBuffer));
}

void GFDisplay7S::setCursor(uint8_t pos) {
  _cursor = (pos < _displayBufferSize) ? pos : _displayBufferSize;
}

size_t GFDisplay7S::write(uint8_t value) {
  if (_cursor < _displayBufferSize) {
    _displayBuffer[_cursor] = mapCharToGliph(value);
    setCursor(_cursor + 1);
    return 1;  // assume success
  }
  return 0;
}

uint8_t GFDisplay7S::mapCharToGliph(uint8_t c) {
  // characters for digits from 0 to 9 map to 0x00 to 0x09
  if (c >= '0' && c <= '9') {
    c = c - '0';
    return c;
  }
  // map ASCII leters corresponding to hex digits to the corresponding gliph positions in array
  switch (c) {
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
      return 0x11;
  }
}

void GFDisplay7S::writeSegmentPins(uint8_t gliphCode) {
  for (uint8_t i = 0; i < 8; i++) {
    if (gliphSegments[gliphCode] & (1 << i)) {
      digitalWrite(_segmentPins[i], _segmentActiveLvl);
    } else {
      digitalWrite(_segmentPins[i], !_segmentActiveLvl);
    }
  }
}

void GFDisplay7S::shutDownDisplays() {
  // write inactive state to segment pins
  for (uint8_t i = 0; i < 9; i++) {
    pinMode(_segmentPins[i], OUTPUT);
    digitalWrite(_segmentPins[i], !_segmentActiveLvl);
  }
  // disable common pin drivers
  for (uint8_t i = 0; i < _displaysCount; i++) {
    pinMode(_commonDriverPins[i], OUTPUT);
    digitalWrite(_commonDriverPins[i], !_commonActiveLvl);
  }
}
