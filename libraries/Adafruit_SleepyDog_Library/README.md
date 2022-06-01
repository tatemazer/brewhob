# Adafruit SleepyDog Arduino Library [![Build Status](https://github.com/adafruit/Adafruit_SleepyDog/workflows/Arduino%20Library%20CI/badge.svg)](https://github.com/adafruit/Adafruit_SleepyDog/actions)[![Documentation](https://github.com/adafruit/ci-arduino/blob/master/assets/doxygen_badge.svg)](http://adafruit.github.io/Adafruit_SleepyDog/html/index.html)

Arduino library to use the watchdog timer for system reset and low power sleep.

Currently supports the following hardware:

*  Arduino Uno or other ATmega328P-based boards.
*  Arduino Mega or other ATmega2560- or 1280-based boards.
*  Arduino Zero, Adafruit Feather M0 (ATSAMD21).
*  Adafruit Feather M4 (ATSAMD51).
*  Arduino Leonardo or other 32u4-based boards (e.g. Adafruit Feather) WITH CAVEAT: USB Serial connection is clobbered on sleep; if sketch does not require Serial comms, this is not a concern. The example sketches all print to Serial and appear frozen, but the logic does otherwise continue to run. You can restore the USB serial connection after waking up using `USBDevice.attach();` and then reconnect to USB serial from the host machine.
*  Partial support for Teensy 3.X and LC (watchdog, no sleep).
*  ATtiny 24/44/84 and 25/45/85
*  ESP32, ESP32-S2
*  ESP8266 WITH CAVEAT: The software and hardware watchdog timers are fixed to specific
intervals and not programmable. Notes about this are within the `utility/WatchdogESP8266.cpp` file.
