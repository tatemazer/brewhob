/*
  This file is part of the MKR NB library.
  Copyright (c) 2019 Arduino SA. All rights reserved.

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

#ifndef _NB_SMS_H_INCLUDED
#define _NB_SMS_H_INCLUDED

#include <Stream.h>

#define NB_SMS_CLEAR_READ             (1)
#define NB_SMS_CLEAR_READ_SENT        (2)
#define NB_SMS_CLEAR_READ_SENT_UNSENT (3)
#define NB_SMS_CLEAR_ALL              (4)

class NB_SMS : public Stream {

public:
  /** Constructor
      @param synch    Determines sync mode
   */
  NB_SMS(bool synch = true);

  /** Write a character in SMS message
      @param c      Character
      @return size
    */
  size_t write(uint8_t c);

  /** Select SMS charset
      @param charset     Character set, one of "IRA" (default), "GSM", or "UCS2", reads from modem if null.
      @return returns first char of charset identifier on success and 0 on error
    */
  int setCharset(const char* charset = nullptr);

  /** Begin a SMS to send it
      @param to     Destination
      @return error command if it exists
    */
  int beginSMS(const char* to);

  /** Get last command status
      @return returns 0 if last command is still executing, 1 success, >1 error
   */
  int ready();

  /** End SMS
      @return error command if it exists
   */
  int endSMS();

  /** Check if SMS available and prepare it to be read
      @return number of bytes in a received SMS
   */
  int available();

  /** Read sender number phone
      @param number   Buffer for save number phone
      @param nlength    Buffer length
      @return 1 success, >1 error
   */
  int remoteNumber(char* number, int nlength); 

  /** Read one char for SMS buffer (advance circular buffer)
      @return byte
   */
  int read();

  /** Read a byte but do not advance the buffer header (circular buffer)
      @return byte
   */
  int peek();

  /** Delete the SMS from Modem memory and process answer
   */
  void flush();
  
  /** Delete all read and sent SMS from Modem memory and process answer
   */
  void clear(int flag = NB_SMS_CLEAR_READ_SENT);

private:
  bool _synch;
  int _state;
  String _incomingBuffer;
  int _smsDataIndex;
  int _smsDataEndIndex;
  bool _smsTxActive;
  int _charset;
  char _bufferUTF8[4];
  int _indexUTF8;
  const char* _ptrUTF8;
};

#endif
