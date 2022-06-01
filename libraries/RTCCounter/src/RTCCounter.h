/*
  RTC Counter library for Arduino SAMD21 boards.
  Copyright (c) 2018 Gabriel Notman. All rights reserved.

  Based in part on the RTC library for Arduino Zero.
  Copyright (c) 2015 Arduino LLC. All rights reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef RTC_COUNTER_H
#define RTC_COUNTER_H

#include "Arduino.h"

typedef void(*voidFuncPtr)(void);

class RTCCounter {
public:
  RTCCounter();
  void begin(bool resetTime = false);
  bool isConfigured();

  bool getFlag();
  void clearFlag();

  void attachInterrupt(voidFuncPtr callback);
  void detachInterrupt();
  
  /* Alarm functions */
  void setAlarmEpoch(uint32_t epoch);
  void setAlarmY2kEpoch(uint32_t y2kEpoch);
  void setPeriodicAlarm(uint32_t period, uint32_t offset=0);

  uint32_t getAlarmEpoch();
  uint32_t getAlarmY2kEpoch();

  uint32_t getAlarmPeriod();
  bool isAlarmPeriodic();
  
  void disableAlarm();
  
  /* Get Functions */
  uint32_t getEpoch();
  uint32_t getY2kEpoch();

  /* Set Functions */
  void setEpoch(uint32_t epoch);
  void setY2kEpoch(uint32_t y2kEpoch);

  /* Other */
  void IrqHandler();

private:
  voidFuncPtr _callBack = NULL;
  volatile bool _intFlag = false;

  bool _configured;
  bool _periodic;
  uint32_t _alarmPeriod;

  void config32kOSC(void);
  void configureClock(void);
  void RTCreadRequest();
  bool RTCisSyncing(void);
  void RTCdisable();
  void RTCenable();
  void RTCreset();
  void RTCresetRemove();
};

extern RTCCounter rtcCounter;

#endif // RTC_COUNTER_H
