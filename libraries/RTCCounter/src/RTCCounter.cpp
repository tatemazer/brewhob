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

#include "RTCCounter.h"

#define EPOCH_TIME_OFF      946684800  // This is 1st January 2000, 00:00:00 in epoch time
#define EPOCH_TIME_YEAR_OFF 100        // years since 1900

RTCCounter::RTCCounter()
{
  _callBack = NULL;
  _intFlag = false;
  
  _configured = false;
  _periodic = false;
  _alarmPeriod = 0;
}

void RTCCounter::begin(bool resetTime)
{
  uint16_t tmp_reg = 0;
  
  PM->APBAMASK.reg |= PM_APBAMASK_RTC; // turn on digital interface clock
  config32kOSC();

  // If the RTC is in clock mode and the reset was
  // not due to POR or BOD, preserve the clock time
  // POR causes a reset anyway, BOD behaviour is?
  bool validTime = false;
  uint32_t oldTime;

  if ((!resetTime) && (PM->RCAUSE.reg & (PM_RCAUSE_SYST | PM_RCAUSE_WDT | PM_RCAUSE_EXT))) {
    if (RTC->MODE0.CTRL.reg & RTC_MODE0_CTRL_MODE_COUNT32) {
      validTime = true;
      oldTime = RTC->MODE0.COUNT.reg;
    }
  }

  // Setup clock GCLK2 with OSC32K divided by 32
  configureClock();

  RTCdisable();

  RTCreset();

  tmp_reg |= RTC_MODE0_CTRL_MODE_COUNT32; // set clock operating mode
  tmp_reg |= RTC_MODE0_CTRL_PRESCALER_DIV1024; // set prescaler to 1024 for MODE2
  tmp_reg &= ~RTC_MODE0_CTRL_MATCHCLR; // disable clear on match
  
  RTC->MODE0.READREQ.reg &= ~RTC_READREQ_RCONT; // disable continuously mode

  while (RTCisSyncing());
  RTC->MODE0.CTRL.reg = tmp_reg;
  
  NVIC_EnableIRQ(RTC_IRQn); // enable RTC interrupt 
  NVIC_SetPriority(RTC_IRQn, 0x03);

  RTCenable();

  // If desired and valid, restore the time value, else use first valid time value
  if ((!resetTime) && (validTime)) {
    while (RTCisSyncing());
    RTC->MODE0.COUNT.reg = oldTime;
  }
  
  _configured = true;
}

bool RTCCounter::isConfigured()
{
  return _configured;
}

bool RTCCounter::getFlag()
{
  return _intFlag;
}

void RTCCounter::clearFlag()
{
  _intFlag = false;
}

void RTCCounter::attachInterrupt(voidFuncPtr callback)
{
  _callBack = callback;
}

void RTCCounter::detachInterrupt()
{
  _callBack = NULL;
}

/*
 * Alarm Functions
 */
void RTCCounter::setAlarmEpoch(uint32_t epoch)
{
  if (_configured) {
    if (epoch > EPOCH_TIME_OFF) {
      setAlarmY2kEpoch(epoch - EPOCH_TIME_OFF);
    }
    else {
      setAlarmY2kEpoch(0);
    }
  }
}

void RTCCounter::setAlarmY2kEpoch(uint32_t y2kEpoch)
{
  if (_configured) {
    while (RTCisSyncing());
    RTC->MODE0.COMP[0].reg = y2kEpoch;

    RTC->MODE0.INTENSET.bit.CMP0 = 1;
  }
}

void RTCCounter::setPeriodicAlarm(uint32_t period, uint32_t offset)
{
  if (_configured) {
    uint32_t nextAlarm = getY2kEpoch() + period + offset;

    _periodic = true;
    _alarmPeriod = period;
    
    setAlarmY2kEpoch(nextAlarm);
  }
}

uint32_t RTCCounter::getAlarmEpoch()
{
  uint32_t alarm = 0;
    
  if (_configured) {
    alarm = getAlarmY2kEpoch() + EPOCH_TIME_OFF;
  }
  
  return alarm;
}

uint32_t RTCCounter::getAlarmY2kEpoch()
{
  uint32_t alarm = 0;
  
  if (_configured) {
    alarm = RTC->MODE0.COMP[0].reg;
  }
  
  return alarm;
}

uint32_t RTCCounter::getAlarmPeriod() 
{
  return _alarmPeriod;
}

bool RTCCounter::isAlarmPeriodic(){
  return _periodic;
}
  
void RTCCounter::disableAlarm()
{
  if (_configured) {
    RTC->MODE0.INTENCLR.bit.CMP0 = 1;

    _periodic = false;
    _alarmPeriod = 0;
  }
}

/*
 * Get Functions
 */
uint32_t RTCCounter::getEpoch()
{
  uint32_t epochTime = 0;
  
  if (_configured) {
    epochTime = getY2kEpoch() + EPOCH_TIME_OFF;
  }

  return epochTime;
}

uint32_t RTCCounter::getY2kEpoch()
{
  uint32_t y2kEpochTime = 0;
  
  if (_configured) {
    RTCreadRequest();

    while (RTCisSyncing());
    y2kEpochTime = RTC->MODE0.COUNT.reg;
  }

  return y2kEpochTime;
}

/*
 * Set Functions
 */
void RTCCounter::setEpoch(uint32_t epoch) 
{
  if (_configured) {
    if (epoch > EPOCH_TIME_OFF) {
      setY2kEpoch(epoch - EPOCH_TIME_OFF);
    }
    else {
      setY2kEpoch(0);
    }
  }
}

void RTCCounter::setY2kEpoch(uint32_t y2kEpoch)
{
  if (_configured) {
    while (RTCisSyncing());
    RTC->MODE0.COUNT.reg = y2kEpoch;
  }

  if (_periodic) {
    setPeriodicAlarm(_alarmPeriod, 0);
  }
}

/* Attach peripheral clock to 32k oscillator */
void RTCCounter::configureClock() {
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2)|GCLK_GENDIV_DIV(4);
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
  
  GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
  
  GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
  while (GCLK->STATUS.bit.SYNCBUSY);
}

/*
 * Private Utility Functions
 */

/* Configure the 32768Hz Oscillator */
void RTCCounter::config32kOSC() 
{
  SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_ONDEMAND |
                         SYSCTRL_XOSC32K_RUNSTDBY |
                         SYSCTRL_XOSC32K_EN32K |
                         SYSCTRL_XOSC32K_XTALEN |
                         SYSCTRL_XOSC32K_STARTUP(6) |
                         SYSCTRL_XOSC32K_ENABLE;
}

inline void RTCCounter::RTCreadRequest() {
  if (_configured) {
    while (RTCisSyncing());
    RTC->MODE0.READREQ.reg = RTC_READREQ_RREQ; // request a read update
  }
}

inline bool RTCCounter::RTCisSyncing()
{
  return (RTC->MODE0.STATUS.bit.SYNCBUSY); // wait for sync
}

void RTCCounter::RTCdisable()
{
  while (RTCisSyncing());
  RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE; // disable RTC
}

void RTCCounter::RTCenable()
{
  while (RTCisSyncing());
  RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_ENABLE; // enable RTC
}

void RTCCounter::RTCreset()
{
  while (RTCisSyncing());
  RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_SWRST; // software reset
}

void RTCCounter::IrqHandler()
{
  _intFlag = true;

  if (_periodic) {
    while (RTCisSyncing());
    RTC->MODE0.COMP[0].reg += _alarmPeriod;

    // For safety we will wait until the write sync
    // has completed, this takes 5-6ms
    while (RTCisSyncing());
  }

  RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;

  if (_callBack != NULL) {
    _callBack();
  }
}

/*
 * RTC Instance
 */
RTCCounter rtcCounter;

/*
 * RTC ISR
 */
void RTC_Handler2(void)
{
  rtcCounter.IrqHandler();
}
