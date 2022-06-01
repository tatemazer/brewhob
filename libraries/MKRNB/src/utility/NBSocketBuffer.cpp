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

#include <stdlib.h>
#include <string.h>

#include "Modem.h"

#include "NBSocketBuffer.h"

#define NB_SOCKET_NUM_BUFFERS (sizeof(_buffers) / sizeof(_buffers[0]))

#define NB_SOCKET_BUFFER_SIZE 512

NBSocketBufferClass::NBSocketBufferClass()
{
  memset(&_buffers, 0x00, sizeof(_buffers));
}

NBSocketBufferClass::~NBSocketBufferClass()
{
  for (unsigned int i = 0; i < NB_SOCKET_NUM_BUFFERS; i++) {
    close(i);
  }
}

void NBSocketBufferClass::close(int socket)
{
  if (_buffers[socket].data) {
    free(_buffers[socket].data);
    _buffers[socket].data = _buffers[socket].head = NULL;
    _buffers[socket].length = 0;
  }
}

int NBSocketBufferClass::available(int socket)
{
  if (_buffers[socket].length == 0) {
    if (_buffers[socket].data == NULL) {
      _buffers[socket].data = _buffers[socket].head = (uint8_t*)malloc(NB_SOCKET_BUFFER_SIZE);
      _buffers[socket].length = 0;
    }

    String response;

    MODEM.sendf("AT+USORD=%d,%d", socket, NB_SOCKET_BUFFER_SIZE);
    int status = MODEM.waitForResponse(10000, &response);
    if (status != 1) {
      if (status == 2) {
        return -1;
      } else if (status == 4 && response.indexOf("Operation not allowed") != -1 ) {
        return -1;
      } else {
        return 0;
      }
    }

    if (!response.startsWith("+USORD: ")) {
      return 0;
    }

    int firstQuoteIndex = response.indexOf("\"");

    response.remove(0, firstQuoteIndex + 1);
    response.remove(response.length() - 1);

    size_t size = response.length() / 2;

    for (size_t i = 0; i < size; i++) {
      byte n1 = response[i * 2];
      byte n2 = response[i * 2 + 1];

      if (n1 > '9') {
        n1 = (n1 - 'A') + 10;
      } else {
        n1 = (n1 - '0');
      }

      if (n2 > '9') {
        n2 = (n2 - 'A') + 10;
      } else {
        n2 = (n2 - '0');
      }

      _buffers[socket].data[i] = (n1 << 4) | n2;
    }

    _buffers[socket].head = _buffers[socket].data;
    _buffers[socket].length = size;
  }

  return _buffers[socket].length;
}

int NBSocketBufferClass::peek(int socket)
{
  if (!available(socket)) {
    return -1;
  }

  return *_buffers[socket].head;
}

int NBSocketBufferClass::read(int socket, uint8_t* data, size_t length)
{
  int avail = available(socket);

  if (!avail) {
    return 0;
  }

  if (avail < (int)length) {
    length = avail;
  }

  memcpy(data, _buffers[socket].head, length);
  _buffers[socket].head += length;
  _buffers[socket].length -= length;

  return length;
}

NBSocketBufferClass NBSocketBuffer;
