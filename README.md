# AVRmillis

This is a PlatformIO library to replace the Arduino `micros` and `millis` functions. 

It uses the Timer/counter 0 overflow interrupt in exactly the same manner as the Arduino does.

**WARNING:** At present, the code runs on an ATmega328 or ATmega168. I have not tested it for other AVR microcontrollers. It has been tested on an Uno, Duemilanove and a Nano with the ATmega328P.

## Structure

This is the structure of the PlatformIO Library as it was packaged. It will of course bear little resemblance to this when you install it into a project!

```
AVRmillis/
├── examples
│   └── main.cpp
├── include
│   ├── AVRmillis.h
│   └── defines.h
├── library.json
├── LICENSE
├── README.md
└── src
    └── AVRmillis.cpp
```

You can see that it is laid out exactly like a PlatformIO library. Which is exactly what it is!

The files you would need to utilise this library, live in the `include` and `src`directories.

## Installation into a Project

Once you have a new project created, edit the `platformio.ini` file and add:

```
lib_deps = normandunbar/AVRmillis@^1.0.2

```

See also [https://registry.platformio.org/libraries/normandunbar/AVRmillis/installation] for details.

When you build the project, PlatformIO will automatically download and install the library.



## The Files

* `examples/main.cpp` - the source file for the example. See below.
* `include/defines.h` - A few useful defines to convert your processor's CPU clock speed into something that allows the `millis` and `micros` functions to be accurate!
* `include/AVRmillis.h` - a header file defining the `AVR_millis` class---note the underscore.
* `src/AVRmillis.cpp` - a C++ source file which implements the `AVR_millis` class---note the underscore. It creates a standalone object named `AVRmillis`---note the lack of underscore. It is this object which handles your calls to `millis()` and `micros()`.
   Also in this file is the Timer/counter 0 Overflow Interrupt handler. It simply passes control back to the `AVRmillis` class' `interrupt()` function to do the hard work.

## The Class

The library code defines and implements a class `AVR_millis` (with an underscore) and creates a global object of this class named `AVRmillis` (without an underscore). Note the difference? There's an underscore in the class name, but not in the global object created for us, when executed. Don't get the two confused! The class publishes the following functions:

* `uint32_t AVR_millis::micros()`---called as `AVRmillis.micros()`;
* `uint32_t AVR_millis::millis()`---alled as `AVRmillis.millis()`;
* `void AVR_millis::interrupt()`---*don't* call this fucntion, you have been warned!;


### `AVR_millis::micros`

Exactly the same as the Arduino function `micros()`. This function returns an unsigned 32 bit integer representing the elapsed microseconds (millionths of a second) since the `AVRmillis` object was created.

This counter will rollover to zero after 71 minutes, 34 seconds and 967,295 microSeconds, plus 1 extra microsecond to cause the rollover.

If your code uses `micros()` and is left running for longer than this period, you need to consider the rollover in your calculations. (Hint: Don't use signed variables!)

```
#include "AVRmillis.h"
#include "avr/interrupt.h"

int main() {
    // Interrupts required!
    sei();

    // Grab micros() so far.
    uint32_t currentMicros = AVRmillis.micros();

    // Do something ...
    ...
}
```


### `AVR_millis::millis`

Exactly the same as the Arduino function `millis()`. This function returns an unsigned 32 bit integer representing the elapsed milliseconds (thousandths of a second) since the `AVRmillis` object was created.

This counter will rollover to zero after 49 days, 17 hours, 2 minutes, 47 seconds and 295 milliseconds, plus 1 extra milliSecond to cause the rollover.

If your code uses `millis()` and is left running for longer than this period, you need to consider the rollover in your calculations. See the example for details of how this can be done. (Don't use signed values!)

```
#include "AVRmillis.h"
#include "avr/interrupt.h"

int main() {
    // Interrupts required!
    sei();

    // Grab millis() so far.
    uint32_t currentMillis = AVRmillis.millis();

    // Do something ...
    ...
}
```
#### Handling Rollover

By using `unsigned` values the rollover effect is nullified:

```
...
    uint32_t timeNow = 0;
    uint32_t timeToggled = 0;
    uint16_t togglePeriod = 10000;
...
while (1) {
    // Grab current millis time.
    timeNow = AVRmillis.millis();

    // Has the flashPeriod expired? If so,
    // toggle the LED and save when we toggled.
    // Unsigned means never having to consider
    // rollover!
    if ((timeNow - timeToggled) >= togglePeriod) {
        toggleSomething();
        timeToggled = timeNow;
    }
...
```

### `AVR_millis::interrupt`

**DO NOT CALL THIS FUNCTION DIRECTLY**. Just saying!

This function exists purely to be called by the handler for the Timer/counter 0 Overflow Interrupt and is not designed to be called from your own code. So, please don't.

It has to be `public` as the interrupt handler cannot access the internals of the `AVRmillis` class in an extremely efficient manner, so this is the quickest and next best thing.

If interrupt handlers *could* be written as class member functions, life would be *interesting* to say the least! Imagine what would happen when an object went out of scope!


## The Example

The `src/main.cpp` file is the example, and simply blinks an LED in the usualy manner, except it doesn't use the Arduino `delay()` function so the loop *could* be doing other non-blocking work on each pass. In this example it just checks to see if the LED needs toggling.

The example code looks remarkably like this:

```
// Test sketch for the AVRmillis class. This one
// does the usual "flash the built in LED" every 1000
// milliseconds.
//
// Norman Dunbar.
// 4 January 2021.

#include "avr/interrupt.h"
#include "AVRmillis.h"

// Toggle the LED every flashPeriod milliseconds.
const uint16_t flashPeriod = 1000;

int main() {
    // Interrupts on! (I always forget!)
    sei();

    // Pseudo Globals.
    uint32_t timeToggled = 0;


    //--------------------------------------------------------
    // SETUP:
    //--------------------------------------------------------

    // D13/PB5 = OUTPUT.
    DDRB |= (1 << DDB5);

    //--------------------------------------------------------
    // LOOP:
    //--------------------------------------------------------
    while (1) {
        // Grab current millis time.
        uint32_t timeNow = AVRmillis.millis();

        // Has the flashPeriod expired? If so,
        // toggle the LED and save when we toggled.
        if ((timeNow - timeToggled) >= flashPeriod) {
            PINB |= (1 << PINB5);
            timeToggled = timeNow;
        }

        // You could be doing some real work here -- as
        // long as it doesn't take too long or block while
        // waiting for input etc.
    }

    return 0;
}
```

The code does the usual Arduino example of the "Blink" sketch, but without using or needing any of the Arduino stuff (technical term!) or the `delay()` function which is a busy delay and hangs up the board while it waits.



Have fun!
