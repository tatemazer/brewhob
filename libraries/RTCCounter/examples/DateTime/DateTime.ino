/*
 * RTC Counter Date Time example
 * Created by Gabriel Notman
 *
 * This example demonstrates how to use the RTC Counter
 * library for date and time functions.
 *
 * This example code is in the public domain.
 * 
 * Created 05 November 2018
 */

#include <RTCCounter.h>
#include <time.h>

void setup()
{
  // Setup the RTCCounter
  rtcCounter.begin();

  // Set the alarm to generate an interrupt every 1s
  rtcCounter.setPeriodicAlarm(1);

  // Set the start date and time
  setDateTime(2018, 11, 5, 0, 0, 0);
}

void loop()
{
  // If the alarm interrupt has been triggered
  if (rtcCounter.getFlag()) {

    // Clear the interrupt flag
    rtcCounter.clearFlag();

    // Print the current date and time
    printDateTime();
  }
}

// Sets the clock based on date and time
void setDateTime(uint16_t year, uint8_t month, uint8_t day, 
  uint8_t hour, uint8_t minute, uint8_t second)
{
  // Use the tm struct to convert the parameters to and from an epoch value
  struct tm tm;

  tm.tm_isdst = -1;
  tm.tm_yday = 0;
  tm.tm_wday = 0;
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;

  rtcCounter.setEpoch(mktime(&tm));
}

// Prints out the date and time to SerialUSB
void printDateTime()
{
  // Get time as an epoch value and convert to tm struct
  time_t epoch = rtcCounter.getEpoch();
  struct tm* t = gmtime(&epoch);

  // Format and print the output
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%02d.%02d.%02d %02d:%02d:%02d", t->tm_year - 100, 
      t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

  SerialUSB.println(buffer);
}

