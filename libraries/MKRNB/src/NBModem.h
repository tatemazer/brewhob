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

#ifndef _NB_MODEM_H_INCLUDED
#define _NB_MODEM_H_INCLUDED

#include <Arduino.h>

class NBModem {

public:

  /** Constructor */
  NBModem();

  /** Check modem response and restart it
   */
  int begin();

  /** Obtain modem IMEI (command AT)
      @return modem IMEI number
   */
  String getIMEI();

  /** Obtain SIM card ICCID (command AT)
      @return SIM ICCID number
   */
  String getICCID();
};

#endif
