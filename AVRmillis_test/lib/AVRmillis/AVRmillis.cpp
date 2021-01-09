#include "AVRmillis.h"
#include "defines.h"
#include <avr/io.h>
#include <avr/interrupt.h>


AVR_millis::AVR_millis() {
    timer0_overflow_count = timer0_millis = timer0_fract = 0;

    // Timer in normal mode, Prescaler = 64.
    TCCR0A = TCCR0B = TIMSK0 = 0;
    TCCR0B |= ((1 << CS01) | (1 << CS00));

    // Clear flag & Enable overflow interrupt.
    TIFR0 &= ~(1 << TOV0);
    TIMSK0 |= (1 << TOIE0);
}


// How many milliseconds since we initialised this class
// object?
uint32_t AVR_millis::millis() {
    uint32_t m;

    // Save current interrupt state and stop
    // interrupts. (In case millis counter is
    // being updated. It's 32 bits wide and cannot
    // be done in one instruction.)
    uint8_t oldSREG = SREG;
    cli();

    // Grab the millis counter.
    m = timer0_millis;

    // Restore the interrupt state.
    SREG = oldSREG;

    // Done, send back the millis value.
    return m;
}


// How many milliseconds since we initialised this class
// object?
uint32_t AVR_millis::micros() {
    uint32_t m;

    // Save the current interrupt state and stop 
    // interrupts. In case the overflow counter is being 
    // updated. It's 32 bits wide and we have no 32 bit add
    // instruction that is uninterruptable.
    uint8_t oldSREG = SREG;
    cli();

    // Grab the overflow counter.
    m = timer0_overflow_count;

    // Has Timer/counter 0 *just* overflowed? If so accumulate
    // it into the micros count. 
    uint8_t t = TCNT0;
    if ((TIFR0 & (1 << TOV0)) && (t < 255))
        m++;

    // Restore interrupt state.
    SREG = oldSREG;

    // Calculate the micros count from the number of times
    // we have overflowed. We overflow every
    // 256 * (16,000,000 / 64) CPU clock cycles.
    // We need to consider the fractions of an overflow as 
    // well. The actual value is:
    //
    // (*Overflow_count * 256) + fraction) multiplied by
    // (prescaler / clock cycles per microsecond).
    return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}


// Do the overflow calculations. Called every 256
// Timer/counter 0 ticks, from the overflow interrupt
// handler.
void AVR_millis::interrupt() {
    // Grab the current values.
    uint32_t m = timer0_millis;
    uint8_t f = timer0_fract;

    // Increment millis counter by the number of
    // milliseconds per overflow period.

    // MUCH WEIRDNESS! 
    // Using MILLIS_INC didn't work. There was no code in
    // the compiled output that increments m by MILLIS_INC
    // so the value never incremented. WTH?
    m += MILLIS_INC;

    // I changed the above line to the following (m++). That
    // worked and millis() returned a value. However, when I
    // changed it back again, it kept on working! WTH?
    //m++;

    // Accumulate the fractions of a millisecond
    // per overflow period.
    f += FRACT_INC;

    // Do we have a "leap" milliseconds yet? If so,
    // add it in and adjust the fraction.
    if (f >= FRACT_MAX) {
        f -= FRACT_MAX;
        m += 1;
    } 

    // Copy the new values back to the class variables.
    timer0_fract = f;
    timer0_millis = m;

    // How many times have we overflowed since "time began"?
    timer0_overflow_count++;
}

// Define our one instance of the AVR_millis class.
AVR_millis AVRmillis;


// This is the ISR for the Timer/counter 0 overflow 
// interrupt. 
ISR(TIMER0_OVF_vect) {
    AVRmillis.interrupt();
}
