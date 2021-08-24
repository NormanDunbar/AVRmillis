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
    }

    return 0;
}

