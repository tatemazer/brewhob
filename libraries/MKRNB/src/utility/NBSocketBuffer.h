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

#ifndef _NBSOCKET_BUFFER_H_INCLUDED
#define _NBSOCKET_BUFFER_H_INCLUDED

class NBSocketBufferClass {

public:
public:
  NBSocketBufferClass();
  virtual ~NBSocketBufferClass();

  void close(int socket);

  int available(int socket);
  int peek(int socket);
  int read(int socket, uint8_t* data, size_t length);

private:
  struct {
    uint8_t* data;
    uint8_t* head;
    int length;
  } _buffers[7];
};

extern NBSocketBufferClass NBSocketBuffer;

#endif
