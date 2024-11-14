# GFDisplay7S - Geek Factory 7 Segment Display Library #

This library drives **7-segment display arrays** using a **multiplexing** technique. When using multiplexing, a single IO line of the microcontroller drives the same segment on every display in the array. The microcontroller also drives the common pin (anode or cathode) to turn on a single display at a given time. The displays are then turned on very fast in sequence, each showing the correct digit on its time slice to give the illusion that all the displays are ON simultaneously.

The library handles the display sequencing and turns on the appropriate pins to drive the segments. Any combination of digital pins can be used to connect the display segments, and the common display pins may also be driven with any digital pin of the microcontroller. Display arrays from 2 to 10 digits are supported by calling the constructor of the GFDisplay7S class with the correct parameters.

Standard Arduino print statements can show digits from **0 to 9 and A to F** in hexadecimal. The **space character and the dash symbol** are also supported. Because of the limitations of 7-segment displays, only the previously listed glyphs are supported; at the moment, there is no intention of supporting other characters.

The member function (method) **process() should run periodically** to perform the display time multiplexing; this should be called preferably from an interrupt service routine, for example, using a timer. The recommended period for the interrupt depends on the number of displays, but we recommend 4 ms per display or better. 

The interrupt code was deliberately left outside the library so that, depending on our processor architecture, we could use an appropriate library or independent code to set up the timer interrupt.

We designed and tested the library using independent displays and the display array. Get them from our online store (only if you are in Mexico):

* [7-segment display common anode or cathode](https://www.geekfactory.mx/producto/display-7-segmentos-anodo-catodo-comun/)
* [4 digits 7-segment display array](hhttps://www.geekfactory.mx/producto/display-7-segmentos-4-digitos-f5461ah/)

## Project Objectives ##

Our library should fulfill the following goals:

* Provide an API similar to the LiquidCrystal library.
* Should not directly rely on timers or other architecture-dependent features.
* The code should be very simple and have no dependencies other than the functions provided by the Arduino framework.
* We should be able to use this code in courses to demonstrate how to create an Arduino library.
* Use the code on platforms besides the AVR Arduino boards (i.e., ESP32, ATSAMD21, Raspberry Pi Pico).
* Provide simple, clean, usable code.

## Basic library usage ##

In order to use the library, we need to include the library header file and then instantiate an object from the class GFDisplay7S.

The constructor will accept two arrays and an uint8_t integer value as follows:

* An array of 8 integers that corresponds to the digital pin numbers where the A, B, C, D, E, F, G and decimal point segments are connected
* An array of lenght N containing the pins used to drive the common cathode or anode pin of the display array. The number of elements on this array corresponds to the number of digits.
* The final paramenter is an integer (uint8_t) that specifies the number of displays that we're driving.

```cpp
// include the library header file
#include <GFDisplay7S.h>

// instantiate an object to drive the display array the segment and common pins numbers are passed as arrays
// the last parameter is the number of displays we're driving
GFDisplay7S display7s((const uint8_t[]){2, 3, 4, 5, 6, 7, 8, 9 }, (const uint8_t[]){ A0, A1, A2, A3, A4, A5 }, 6);
```

We also need to call the begin() function member in order to initialize the pins used to drive the displays and prepare the object for use.

```cpp
void setup()
{
  // call begin() to prepare object for use
  display7s.begin();
}
```

To show data on the displays we can use the typical print() function:

```cpp
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
```

The display needs to be updated every 2 - 3 milliseconds by calling process(); this should be done preferably inside an interrupt service routine.

## Supported devices ##

This library was developed/tested on the following boards:

* Arduino UNO R3
* Arduino Mega 2560 R3
* Raspberry Pi Pico
* ESP32 based devices

## Contact me ##

* Feel free to write for any inquiry: ruben at geekfactory.mx
* Check our website: https://www.geekfactory.mx
