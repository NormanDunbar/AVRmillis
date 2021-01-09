#ifndef DEFINES_H
#define DEFINES_H

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

//------------------------------------------------------------
// These #defines are pretty much identical to the ones
// in wiring.c for the Arduino language.
// They help make sure that the speed of the CPU doesn't
// affect the timings for the millis() and micros() counters.
//------------------------------------------------------------

//------------------------------------------------------------
// How many clock ticks do we get in each microsecond. The 
// default (16 MHz clock speed) is 16.
//------------------------------------------------------------
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )

//------------------------------------------------------------
// Convert a number of clock cycles to microseconds based on
// the F_CPU frequency.
//------------------------------------------------------------
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )


//------------------------------------------------------------
// Timer 0 is an 8 bit timer/counter and overflows every 256 
// timer clock ticks. The timer must be set up to divide the
// system clock by 64. (I wonder if that could be #defined for
// other prescaler values?)
// Default is 1024.
//------------------------------------------------------------
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

//------------------------------------------------------------
// MILLIS_INC is the whole number of milliseconds that it takes
// for Timer 0 to overflow. By default 16 with a remainder of 
// 384 microseconds. 
//------------------------------------------------------------
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

//------------------------------------------------------------
// The fractions of a millisecond that we need to account for 
// to make sure that millis() always returns the correct 
// value. By default, this is 384/8 = 48. This fits a byte 
// sized variable so is quicker to work with as we don't
// have uninterruptable 16 bit arithmetic on the ATmega328.
//------------------------------------------------------------
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)


#endif // DEFINES_H