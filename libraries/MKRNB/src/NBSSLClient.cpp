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

#include "utility/NBRootCerts.h"

#include "Modem.h"

#include "NBSSLClient.h"

enum {
  SSL_CLIENT_STATE_LOAD_ROOT_CERT,
  SSL_CLIENT_STATE_WAIT_LOAD_ROOT_CERT_RESPONSE,
  SSL_CLIENT_STATE_WAIT_DELETE_ROOT_CERT_RESPONSE
};

bool NBSSLClient::_defaultRootCertsLoaded = false;

NBSSLClient::NBSSLClient(bool synch) :
  NBClient(synch),
  _RCs(NB_ROOT_CERTS),
  _numRCs(NB_NUM_ROOT_CERTS),
  _customRootCerts(false)
{
}

NBSSLClient::NBSSLClient(const NBRootCert* myRCs, int myNumRCs, bool synch) :
  NBClient(synch),
  _RCs(myRCs),
  _numRCs(myNumRCs),
  _customRootCerts(true),
  _customRootCertsLoaded(false)
{
}

NBSSLClient::~NBSSLClient()
{
}

int NBSSLClient::ready()
{
  if ((!_customRootCerts && _defaultRootCertsLoaded) ||
      (_customRootCerts && (_numRCs == 0 || _customRootCertsLoaded))) {
    // root certs loaded already, continue to regular NBClient
    return NBClient::ready();
  }

  int ready = MODEM.ready();
  if (ready == 0) {
    // a command is still running
    return 0;
  }

  switch (_state) {
    case SSL_CLIENT_STATE_LOAD_ROOT_CERT: {
      if (_RCs[_certIndex].size) {
        // load the next root cert
        MODEM.sendf("AT+USECMNG=0,0,\"%s\",%d", _RCs[_certIndex].name, _RCs[_certIndex].size);
        if (MODEM.waitForPrompt() != 1) {
          // failure
          ready = -1;
        } else {
          // send the cert contents
          MODEM.write(_RCs[_certIndex].data, _RCs[_certIndex].size);
          _state = SSL_CLIENT_STATE_WAIT_LOAD_ROOT_CERT_RESPONSE;
          ready = 0;
        }
      } else {
        // remove the next root cert name
        MODEM.sendf("AT+USECMNG=2,0,\"%s\"", _RCs[_certIndex].name);

        _state = SSL_CLIENT_STATE_WAIT_DELETE_ROOT_CERT_RESPONSE;
        ready = 0;
      }
      break;
    }

    case SSL_CLIENT_STATE_WAIT_LOAD_ROOT_CERT_RESPONSE: {
      if (ready > 1) {
        // error
      } else {
        ready = iterateCerts();
      }
      break;
    }

    case SSL_CLIENT_STATE_WAIT_DELETE_ROOT_CERT_RESPONSE: {
      // ignore ready response, root cert might not exist
      ready = iterateCerts();
      break;
    }
  }

  return ready;
}

int NBSSLClient::connect(IPAddress ip, uint16_t port)
{
  _certIndex = 0;
  _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;

  return connectSSL(ip, port);
}

int NBSSLClient::connect(const char* host, uint16_t port)
{
  _certIndex = 0;
  _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;

  return connectSSL(host, port);
}

int NBSSLClient::iterateCerts()
{
  _certIndex++;
  if (_certIndex == _numRCs) {
    // all certs loaded
    if (_customRootCerts) {
      _customRootCertsLoaded = true;
    } else {
      _defaultRootCertsLoaded = true;
    }
  } else {
    // load next
    _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;
  }
  return 0;
}
