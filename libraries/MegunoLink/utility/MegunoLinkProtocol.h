#pragma once
#define __PROG_TYPES_COMPAT__
#include <Arduino.h>

class MegunoLinkProtocol
{
  const void *m_ChannelName;
  const bool m_bFlashString;

  const char * const m_pfchContext;

protected:
  Print &m_rDestination;

protected:
  MegunoLinkProtocol(const __FlashStringHelper *Context);
  MegunoLinkProtocol(const __FlashStringHelper *Context, const char *Channel);
  MegunoLinkProtocol(const __FlashStringHelper *Context, const __FlashStringHelper *Channel);

  MegunoLinkProtocol(const __FlashStringHelper *Context, Print &rDestination);
  MegunoLinkProtocol(const __FlashStringHelper *Context, const char *Channel, Print &rDestination);
  MegunoLinkProtocol(const __FlashStringHelper *Context, const __FlashStringHelper *Channel, Print &rDestination);

  void SendDataHeader(const __FlashStringHelper *pfstrCommand);
  void SendDataTail();

};

