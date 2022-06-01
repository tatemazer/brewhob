/*
 * RTC Counter Periodic Blink example
 * Created by Gabriel Notman
 *
 * This example demonstrates how to use the RTC Counter
 * library for simple periodic events.
 *
 * This example code is in the public domain.
 * 
 * Created 05 November 2018
 */

#include <RTCCounter.h>

void setup()
{
  // Setup the RTCCounter
  rtcCounter.begin();

  // Set the alarm to generate an interrupt every 5s
  rtcCounter.setPeriodicAlarm(5);

  // Setup the LED pin
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // If the alarm interrupt has been triggered
  if (rtcCounter.getFlag()) {

    // Clear the interrupt flag
    rtcCounter.clearFlag();

    // Blink the LED
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

