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

#ifndef _GPRS_H_INCLUDED
#define _GPRS_H_INCLUDED

#include <IPAddress.h>
#include "NB.h"

#include "Modem.h"

class GPRS {

public:

  GPRS();
  virtual ~GPRS();

  /** Attach to GPRS/NB network
      @return connection status
   */
  NB_NetworkStatus_t networkAttach()
  {
      return attachGPRS();
  };

  /** Detach GPRS/NB network
      @return connection status
   */
  NB_NetworkStatus_t networkDetach(){ return detachGPRS(); };


  /** Returns 0 if last command is still executing
      @return 1 if success, >1 if error
   */
  int ready();

  /** Attach to GPRS service
      @param synchronous  Sync mode
      @return connection status
   */
  NB_NetworkStatus_t attachGPRS(bool synchronous = true);

  /** Detach GPRS service
      @param synchronous  Sync mode
      @return connection status
   */
  NB_NetworkStatus_t detachGPRS(bool synchronous = true);

  /** Get actual assigned IP address in IPAddress format
      @return IP address in IPAddress format
   */
  IPAddress getIPAddress();
  void setTimeout(unsigned long timeout);
  NB_NetworkStatus_t status();

private:
  int _state;
  NB_NetworkStatus_t _status;
  String _response;
  int _pingResult;
  unsigned long _timeout;
};

#endif
