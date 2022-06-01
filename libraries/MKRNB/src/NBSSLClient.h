/*
  This file is part of the MKR NB IoT library.
  Copyright (C) 2018 Arduino SA (http://www.arduino.cc/)

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

#ifndef _NB_SSL_CLIENT_H_INCLUDED
#define _NB_SSL_CLIENT_H_INCLUDED

#include "NBClient.h"
#include "utility/NBRootCerts.h"

class NBSSLClient : public NBClient {

public:
  NBSSLClient(bool synch = true);
  NBSSLClient(const NBRootCert* myRCs, int myNumRCs, bool synch = true);
  virtual ~NBSSLClient();

  virtual int ready();
  virtual int iterateCerts();

  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char* host, uint16_t port);

  private:
    const NBRootCert* _RCs;
    int _numRCs;
    static bool _defaultRootCertsLoaded;
    bool _customRootCerts;
    bool _customRootCertsLoaded;
    int _certIndex;
    int _state;
};

#endif
