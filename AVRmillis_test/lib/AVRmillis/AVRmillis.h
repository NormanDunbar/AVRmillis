#ifndef AVRMILLIS_H
#define AVRMILLIS_H

//============================================================
// This code provides PlatdormIO projects which are AVR based
// specifically the ATmega328P, with the ability to use Arduino
// functions such as millis() and micros().
//
// Norman Dunbar
// 4 January 2021.
//============================================================
// WARNING: This code is pretty much stolen from the Arduino
// Language -- the joys of Open Source -- and it will take 
// over Timer/counter 0 and its overflow interrupt.
//
// The timer will be set to prescale the F_CPU by dividing by
// 64. No other functions of the timer/counter are used.
//
// If your code needs this feature, plus PWM, for example, 
// then please initialise the timer's registers by ORing the
// new values rather than setting the values directly.
//
// Assume that this code gets access to the timer's registers
// first, before any other code does and we should be ok!
//============================================================

#include <stdint.h>


class AVR_millis {

public:
    //--------------------------------------------------------
    // Constructor. There is no destructor because, we never
    // go out of scope -- not deliberately anyway!
    //--------------------------------------------------------
    AVR_millis();


    //--------------------------------------------------------
    // Return count of milliseconds since the code in the
    // cpp file created the AVRmillis object. Will overflow
    // every 49 days, 17 hours, 2 minutes and 47.29487424 
    // seconds. (Plus one for the rollover).
    //--------------------------------------------------------
    uint32_t millis();

    //--------------------------------------------------------
    // Return count of microseconds since the code in the
    // cpp file created the AVRmillis object. Will overflow 
    // every 71 minutes 34.967295 seconds. (Plus 1 for the 
    // rollover).
    //--------------------------------------------------------
    uint32_t micros();

    //--------------------------------------------------------
    // Do the actual interrupt handling. This is public as
    // it is called from the ISR(TIMER0_OVF_vect) which is
    // not ideal, really, but c'est la vie. If it's good 
    // enough for Arduino, I'm happy with it too. Mostly!
    //--------------------------------------------------------
    void interrupt();


    uint32_t getMillis(){ return timer0_millis;}
    uint8_t  getFract() { return timer0_fract;}
    void setMillis(uint32_t m) { timer0_millis = m;}
    void setFract(uint8_t f)   { timer0_fract = f;}



private:
    //--------------------------------------------------------
    // How many times have we overflowed Timer/counter 0 since
    // we initialised the AVRmillis object? Used by the 
    // micros() function.
    //--------------------------------------------------------
    volatile uint32_t timer0_overflow_count;

    //--------------------------------------------------------
    // How many complete milliseconds have we counted since 
    // we initialised the AVRmillis object?
    //--------------------------------------------------------
    volatile uint32_t timer0_millis;

    //--------------------------------------------------------
    // How many fractions of a millisecond have we accumulated
    // so far? Everytime we overflow and have accumulated one
    // extra (leap?) millisecond, we add it to the running 
    // total.
    //--------------------------------------------------------
    volatile uint8_t timer0_fract;
};

//There will be one of these in the cpp file for this class.
extern AVR_millis AVRmillis;

#endif // AVRMILLIS_H


