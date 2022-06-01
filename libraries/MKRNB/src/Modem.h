/*
  This file is part of the MKR NB library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _MODEM_INCLUDED_H
#define _MODEM_INCLUDED_H

#include <stdarg.h>
#include <stdio.h>

#include <Arduino.h>

/* The NB 1500 does not connect the SARA V_INT pin so that it can be monitored.
   The below constants and associated code enables connecting SARA_VINT to a digital input
   using a level shifter (1.8V to 5V) or simply a MOS transistor, say a 2N7000. 
   The code does rudimentary tracking of the on/off state if this is not available.
*/ 
#define SARA_VINT_OFF (-1)
#define SARA_VINT_ON  (-2)

#ifndef SARA_VINT
#define SARA_VINT SARA_VINT_OFF
#endif

class ModemUrcHandler {
public:
  virtual void handleUrc(const String& urc) = 0;
};

class ModemClass {
public:
  ModemClass(Uart& uart, unsigned long baud, int resetPin, int powerOnPin, int vIntPin=SARA_VINT);

  int begin(bool restart = false);
  void end();

  void debug();
  void debug(Print& p);
  void noDebug();

  int autosense(unsigned long timeout = 10000);

  int noop();
  int reset();
  int shutdown();
  void hardReset(); // Hardware pin reset, only use in EMERGENCY
  int isPowerOn();
  void setVIntPin(int vIntPin);

  size_t write(uint8_t c);
  size_t write(const uint8_t*, size_t);

  void send(const char* command);
  void send(const String& command) { send(command.c_str()); }
  void sendf(const char *fmt, ...);

  int waitForPrompt(unsigned long timeout = 500);
  int waitForResponse(unsigned long timeout = 200, String* responseDataStorage = NULL);
  int ready();
  void poll();
  void setResponseDataStorage(String* responseDataStorage);

  void addUrcHandler(ModemUrcHandler* handler);
  void removeUrcHandler(ModemUrcHandler* handler);

  void setBaudRate(unsigned long baud);

private:
  Uart* _uart;
  unsigned long _baud;
  int _resetPin;
  int _powerOnPin;
  int _vIntPin;
  unsigned long _lastResponseOrUrcMillis;

  enum {
    AT_COMMAND_IDLE,
    AT_RECEIVING_RESPONSE
  } _atCommandState;
  int _ready;
  String _buffer;
  String* _responseDataStorage;

  #define MAX_URC_HANDLERS 8 // 7 sockets + GPRS
  static ModemUrcHandler* _urcHandlers[MAX_URC_HANDLERS];
  static Print* _debugPrint;
};

extern ModemClass MODEM;

#endif
