/****************************************************************************************************************************
  RepeatedAttachInterrupt_uS.ino
  For SAMD boards
  Written by Khoi Hoang
  
  Built by Khoi Hoang https://github.com/khoih-prog/SAMD_TimerInterrupt
  Licensed under MIT license
  
  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one SAMD timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
  
  Based on SimpleTimer - A timer library for Arduino.
  Author: mromani@ottotecnica.com
  Copyright (c) 2010 OTTOTECNICA Italy
  
  Based on BlynkTimer.h
  Author: Volodymyr Shymanskyy
  
  Version: 1.5.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      30/10/2020 Initial coding
  1.0.1   K Hoang      06/11/2020 Add complicated example ISR_16_Timers_Array using all 16 independent ISR Timers.
  1.1.1   K.Hoang      06/12/2020 Add Change_Interval example. Bump up version to sync with other TimerInterrupt Libraries
  1.2.0   K.Hoang      08/01/2021 Add better debug feature. Optimize code and examples to reduce RAM usage
  1.3.0   K.Hoang      02/04/2021 Add support to Sparkfun SAMD21 and SAMD51 boards
  1.3.1   K.Hoang      09/05/2021 Fix compile error to some SAMD21-based boards
  1.4.0   K.Hoang      02/06/2021 Fix SAMD21 rare bug caused by not fully init Prescaler
  1.5.0   K.Hoang      08/10/2021 Improve frequency precision by using float instead of ulong
*****************************************************************************************************************************/

/*
  Notes:
  Special design is necessary to share data between interrupt code and the rest of your program.
  Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
  variable can not spontaneously change. Because your function may change variables while your program is using them,
  the compiler needs this hint. But volatile alone is often not enough.
  When accessing shared variables, usually interrupts must be disabled. Even with volatile,
  if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
  If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
  or the entire sequence of your code which accesses the data.

  Based on the sketch of (https://github.com/thiagothimotti) posted in (https://github.com/khoih-prog/SAMD_TimerInterrupt/issues/3)
  to report the issue: "Bug when going from a >20000us period to a <20000us period. The timer period become 4 times greater. #3"
*/

#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
    || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
    || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21E18A__) || defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) \
    || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) || defined(__SAMD21G18A__) )
#error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "SAMDTimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
// Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     4

#include "SAMDTimerInterrupt.h"

// Depending on the board, you can select SAMD21 Hardware Timer from TC3-TCC
// SAMD21 Hardware Timer from TC3 or TCC
// SAMD51 Hardware Timer only TC3
SAMDTimer ITimer(TIMER_TC3);
//SAMDTimer ITimer(TIMER_TCC);

volatile uint32_t myClockTimer = 0, lastMicros = 0;

void clock(void)
{
  myClockTimer = micros() - lastMicros; //2us come from here
  lastMicros = micros();
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  delay(100);

  Serial.print(F("\nStarting RepeatedAttachInterrupt_uS on ")); Serial.println(BOARD_NAME);
  Serial.println(SAMD_TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));
}

void loop()
{
  ITimer.attachInterruptInterval(19995, clock);
  delay(5000);
  Serial.print(F("myClockTimer (19995) = ")); Serial.println(myClockTimer);
  ITimer.attachInterruptInterval(19995, clock);
  delay(5000);
  Serial.print(F("myClockTimer (19995) = ")); Serial.println(myClockTimer);
  ITimer.attachInterruptInterval(19995, clock);
  delay(5000);
  Serial.print(F("myClockTimer (19995) = ")); Serial.println(myClockTimer);
  ITimer.attachInterruptInterval(19000, clock);
  delay(5000);
  Serial.print(F("myClockTimer (19000) = ")); Serial.println(myClockTimer);
  ITimer.attachInterruptInterval(20005, clock);
  delay(5000);
  Serial.print(F("myClockTimer (20005) = ")); Serial.println(myClockTimer);
  ITimer.attachInterruptInterval(30000, clock);
  delay(5000);
  Serial.print(F("myClockTimer (30000) = ")); Serial.println(myClockTimer);
  ITimer.attachInterruptInterval(19995, clock);
  delay(5000);
  Serial.print(F("myClockTimer (19995) = ")); Serial.println(myClockTimer);
  ITimer.attachInterruptInterval(30000, clock);
  delay(5000);
  Serial.print(F("myClockTimer (30000) = ")); Serial.println(myClockTimer);
}
