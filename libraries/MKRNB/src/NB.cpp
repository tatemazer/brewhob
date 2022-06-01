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
#define _XOPEN_SOURCE
#include <time.h>

#include "Modem.h"

#include "NB.h"

enum {
  READY_STATE_SET_ERROR_DISABLED,
  READY_STATE_WAIT_SET_ERROR_DISABLED,
  READY_STATE_SET_MINIMUM_FUNCTIONALITY_MODE,
  READY_STATE_WAIT_SET_MINIMUM_FUNCTIONALITY_MODE,
  READY_STATE_CHECK_SIM,
  READY_STATE_WAIT_CHECK_SIM_RESPONSE,
  READY_STATE_UNLOCK_SIM,
  READY_STATE_WAIT_UNLOCK_SIM_RESPONSE,
  READY_STATE_DETACH_DATA,
  READY_STATE_WAIT_DETACH_DATA,
  READY_STATE_SET_PREFERRED_MESSAGE_FORMAT,
  READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE,
  READY_STATE_SET_HEX_MODE,
  READY_STATE_WAIT_SET_HEX_MODE_RESPONSE,
  READY_STATE_SET_AUTOMATIC_TIME_ZONE,
  READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE,
  READY_STATE_SET_APN,
  READY_STATE_WAIT_SET_APN,
  READY_STATE_SET_APN_AUTH,
  READY_STATE_WAIT_SET_APN_AUTH,
  READY_STATE_SET_FULL_FUNCTIONALITY_MODE,
  READY_STATE_WAIT_SET_FULL_FUNCTIONALITY_MODE,
  READY_STATE_CHECK_REGISTRATION,
  READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE,
  READY_STATE_DONE
};

NB::NB(bool debug) :
  _state(ERROR),
  _readyState(0),
  _pin(NULL),
  _apn(""),
  _username(""),
  _password(""),
  _timeout(0)
{
  if (debug) {
    MODEM.debug();
  }
}

NB_NetworkStatus_t NB::begin(const char* pin, bool restart, bool synchronous)
{
  return begin(pin, "", restart, synchronous);
}

NB_NetworkStatus_t NB::begin(const char* pin, const char* apn, bool restart, bool synchronous)
{
  return begin(pin, apn, "", "", restart, synchronous);
}

NB_NetworkStatus_t NB::begin(const char* pin, const char* apn, const char* username, const char* password, bool restart, bool synchronous)
{
  if (!MODEM.begin(restart)) {
    _state = ERROR;
  } else {
    _pin = pin;
    _apn = apn;
    _username = username,
    _password = password;
    _state = IDLE;
    _readyState = READY_STATE_SET_ERROR_DISABLED;

    if (synchronous) {
      unsigned long start = millis();

      while (ready() == 0) {
        if (_timeout && !((millis() - start) < _timeout)) {
          _state = ERROR;
          break;
        }

        delay(100);
      }
    } else {
      return (NB_NetworkStatus_t)0;
    }
  }

  return _state;
}

int NB::isAccessAlive()
{
  String response;

  MODEM.send("AT+CEREG?");
  if (MODEM.waitForResponse(100, &response) == 1) {
    int status = response.charAt(response.length() - 1) - '0';

    if (status == 1 || status == 5 || status == 8) {
      return 1;
    }
  }

  return 0;
}

bool NB::shutdown()
{
  // Attempt AT command shutdown
  if (_state == NB_READY && MODEM.shutdown()) {
    _state = NB_OFF;
    return true;
  }
  return false;
}

bool NB::secureShutdown()
{
  // Hardware power off
  MODEM.end();
  _state = NB_OFF;
  return true;
}

int NB::ready()
{
  if (_state == ERROR) {
    return 2;
  }

  int ready = MODEM.ready();

  if (ready == 0) {
    return 0;
  }

  switch (_readyState) {
    case READY_STATE_SET_ERROR_DISABLED: {
      MODEM.send("AT+CMEE=0");
      _readyState = READY_STATE_WAIT_SET_ERROR_DISABLED;
      ready = 0;
      break;
    }
  
    case READY_STATE_WAIT_SET_ERROR_DISABLED: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_MINIMUM_FUNCTIONALITY_MODE;
        ready = 0;
      }
      
      break;
    }

    case READY_STATE_SET_MINIMUM_FUNCTIONALITY_MODE: {
      MODEM.send("AT+CFUN=0");
      _readyState = READY_STATE_WAIT_SET_MINIMUM_FUNCTIONALITY_MODE;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_SET_MINIMUM_FUNCTIONALITY_MODE:{
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_CHECK_SIM;
        ready = 0;
      }

      break;
    }

    case READY_STATE_CHECK_SIM: {
      MODEM.setResponseDataStorage(&_response);
      MODEM.send("AT+CPIN?");
      _readyState = READY_STATE_WAIT_CHECK_SIM_RESPONSE;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_CHECK_SIM_RESPONSE: {
      if (ready > 1) {
        // error => retry
        _readyState = READY_STATE_CHECK_SIM;
        ready = 0;
      } else {
        if (_response.endsWith("READY")) {
          _readyState = READY_STATE_SET_PREFERRED_MESSAGE_FORMAT;
          ready = 0;
        } else if (_response.endsWith("SIM PIN")) {
          _readyState = READY_STATE_UNLOCK_SIM;
          ready = 0;
        } else {
          _state = ERROR;
          ready = 2;
        }
      }

      break;
    }

    case READY_STATE_UNLOCK_SIM: {
      if (_pin != NULL) {
        MODEM.setResponseDataStorage(&_response);
        MODEM.sendf("AT+CPIN=\"%s\"", _pin);

        _readyState = READY_STATE_WAIT_UNLOCK_SIM_RESPONSE;
        ready = 0;
      } else {
        _state = ERROR;
        ready = 2;
      }
      break;
    }

    case READY_STATE_WAIT_UNLOCK_SIM_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_DETACH_DATA;
        ready = 0;
      }

      break;
    }

    case READY_STATE_DETACH_DATA: {
      MODEM.send("AT+CGATT=0");
      _readyState = READY_STATE_WAIT_DETACH_DATA;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_DETACH_DATA:{
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_PREFERRED_MESSAGE_FORMAT;
        ready = 0;
      }

      break;
    }

    case READY_STATE_SET_PREFERRED_MESSAGE_FORMAT: {
      MODEM.send("AT+CMGF=1");
      _readyState = READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_HEX_MODE;
        ready = 0;
      }

      break;
    }

    case READY_STATE_SET_HEX_MODE: {
      MODEM.send("AT+UDCONF=1,1");
      _readyState = READY_STATE_WAIT_SET_HEX_MODE_RESPONSE;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_SET_HEX_MODE_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_AUTOMATIC_TIME_ZONE;
        ready = 0;
      }

      break;
    }

    case READY_STATE_SET_AUTOMATIC_TIME_ZONE: {
      MODEM.send("AT+CTZU=1");
      _readyState = READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_APN;
        ready = 0;
      }
      break;
    }

    case READY_STATE_SET_APN: {
      MODEM.sendf("AT+CGDCONT=1,\"IP\",\"%s\"", _apn);
      _readyState = READY_STATE_WAIT_SET_APN;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_SET_APN: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_APN_AUTH;
        ready = 0;
      }
      break;
    }

    case READY_STATE_SET_APN_AUTH: {
      if (strlen(_username) > 0 || strlen(_password) > 0) {
        // CHAP
        MODEM.sendf("AT+UAUTHREQ=1,2,\"%s\",\"%s\"", _password, _username);
      } else {
        // no auth
        MODEM.sendf("AT+UAUTHREQ=1,%d", 0);
      }

      _readyState = READY_STATE_WAIT_SET_APN_AUTH;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_SET_APN_AUTH: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_SET_FULL_FUNCTIONALITY_MODE;
        ready = 0;
      }
      break;
    }

    case READY_STATE_SET_FULL_FUNCTIONALITY_MODE: {
      MODEM.send("AT+CFUN=1");
      _readyState = READY_STATE_WAIT_SET_FULL_FUNCTIONALITY_MODE;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_SET_FULL_FUNCTIONALITY_MODE:{
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        _readyState = READY_STATE_CHECK_REGISTRATION;
        ready = 0;
      }

      break;
    }
  
    case READY_STATE_CHECK_REGISTRATION: {
      MODEM.setResponseDataStorage(&_response);
      MODEM.send("AT+CEREG?");
      _readyState = READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE;
      ready = 0;
      break;
    }

    case READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE: {
      if (ready > 1) {
        _state = ERROR;
        ready = 2;
      } else {
        int status = _response.charAt(_response.length() - 1) - '0';

        if (status == 0 || status == 4) {
          _readyState = READY_STATE_CHECK_REGISTRATION;
          ready = 0;
        } else if (status == 1 || status == 5 || status == 8) {
          _readyState = READY_STATE_DONE;
          _state = NB_READY;
          ready = 1;
        } else if (status == 2) {
          _readyState = READY_STATE_CHECK_REGISTRATION;
          _state = CONNECTING;
          ready = 0;
        } else if (status == 3) {
          _state = ERROR;
          ready = 2;
        } 
      }

      break;
    }

    case READY_STATE_DONE:
      break;
  }

  return ready;
}

void NB::setTimeout(unsigned long timeout)
{
  _timeout = timeout;
}

unsigned long NB::getTime()
{
  String response;

  MODEM.send("AT+CCLK?");
  if (MODEM.waitForResponse(100, &response) != 1) {
    return 0;
  }

  struct tm now;

  int dashIndex = response.lastIndexOf('-');
  if (dashIndex != -1) {
    response.remove(dashIndex);
  }

  if (strptime(response.c_str(), "+CCLK: \"%y/%m/%d,%H:%M:%S", &now) != NULL) {
    // adjust for timezone offset which is +/- in 15 minute increments

    time_t result = mktime(&now);
    time_t delta = ((response.charAt(26) - '0') * 10 + (response.charAt(27) - '0')) * (15 * 60);

    if (response.charAt(25) == '-') {
      result += delta;
    } else if (response.charAt(25) == '+') {
      result -= delta;
    }

    return result;
  }

  return 0;
}

unsigned long NB::getLocalTime()
{
  String response;

  MODEM.send("AT+CCLK?");
  if (MODEM.waitForResponse(100, &response) != 1) {
    return 0;
  }

  struct tm now;

  if (strptime(response.c_str(), "+CCLK: \"%y/%m/%d,%H:%M:%S", &now) != NULL) {

    time_t result = mktime(&now);
    return result;
  }

  return 0;
}

bool NB::setTime(unsigned long const epoch, int const timezone)
{
  String hours, date;
  const uint8_t daysInMonth [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 };
  unsigned long unix_time = epoch - 946684800UL; /* Subtract seconds from 1970 to 2000 */

  if (((unix_time  % 86400L) / 3600) < 10 ) {
    hours = "0";
  }

  hours += String((unix_time  % 86400L) / 3600) + ":";
  if ( ((unix_time  % 3600) / 60) < 10 ) {
    hours = "0";
  }

  hours += String((unix_time  % 3600) / 60) + ":";
   if ((unix_time % 60) < 10 ) {
    hours += "0";
  }

  hours += String(unix_time % 60)+ "+";
  if (timezone < 10) {
    hours += "0";
  }

  hours += String(timezone);
  /* Convert unix_time from seconds to days */
  int days = unix_time / (24 * 3600);
  int leap;
  int year = 0;
  while (1) {
    leap = year % 4 == 0;
    if (days < 365 + leap) {
      if (year < 10) {
        date += "0";
      }
    break;
    }
    days -= 365 + leap;
    year++;
  }

  date += String(year) + "/";
  int month;
  for (month = 1; month < 12; month++) {
    uint8_t daysPerMonth = daysInMonth[month - 1];
    if (leap && month == 2)
      daysPerMonth++;
    if (days < daysPerMonth) {
      if (month < 10) {
        date += "0";
      }
      break;
    }
    days -= daysPerMonth;
  }
  date += String(month) + "/";

  if ((days + 1) < 10) {
    date += "0";
  }
  date +=  String(days + 1) + ",";

  MODEM.send("AT+CCLK=\"" + date + hours + "\"");
  if (MODEM.waitForResponse(100) != 1) {
    return false;
  }
  return true;
}

NB_NetworkStatus_t NB::status()
{
  return _state;
}
