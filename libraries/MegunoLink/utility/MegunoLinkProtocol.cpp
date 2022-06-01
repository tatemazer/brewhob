#include "MegunoLinkProtocol.h"



MegunoLinkProtocol::MegunoLinkProtocol( const __FlashStringHelper *Context )
  : m_pfchContext((PROGMEM const char*)Context), m_bFlashString(false), m_ChannelName(NULL), m_rDestination(Serial)
{
}

MegunoLinkProtocol::MegunoLinkProtocol( const __FlashStringHelper *Context, const char *Channel )
  : m_pfchContext((PROGMEM const char*)Context), m_bFlashString(false), m_ChannelName(Channel), m_rDestination(Serial)
{

}

MegunoLinkProtocol::MegunoLinkProtocol( const __FlashStringHelper *Context, const __FlashStringHelper *Channel )
  : m_pfchContext((PROGMEM const char*)Context), m_bFlashString(true), m_ChannelName((PROGMEM const char*)Channel), m_rDestination(Serial)
{
}

MegunoLinkProtocol::MegunoLinkProtocol( const __FlashStringHelper *Context, Print &rDestination )
  : m_pfchContext((PROGMEM const char*)Context), m_bFlashString(false), m_ChannelName(NULL), m_rDestination(rDestination)
{
}

MegunoLinkProtocol::MegunoLinkProtocol( const __FlashStringHelper *Context, const char *Channel, Print &rDestination )
  : m_pfchContext((PROGMEM const char*)Context), m_bFlashString(false), m_ChannelName(Channel), m_rDestination(rDestination)
{

}

MegunoLinkProtocol::MegunoLinkProtocol( const __FlashStringHelper *Context, const __FlashStringHelper *Channel, Print &rDestination )
  : m_pfchContext((PROGMEM const char*)Context), m_bFlashString(true), m_ChannelName((PROGMEM const char*)Channel), m_rDestination(rDestination)
{
}

void MegunoLinkProtocol::SendDataHeader(const __FlashStringHelper *pfstrCommand)
{
  m_rDestination.print('{');
  m_rDestination.print((const __FlashStringHelper *) m_pfchContext);
  if (m_ChannelName != NULL)
  {
    m_rDestination.print(':');
    if (m_bFlashString)
    {
      m_rDestination.print((const __FlashStringHelper *)m_ChannelName);
    }
    else
    {
      m_rDestination.print((const char *)m_ChannelName);
    }
  }
  m_rDestination.print('|');
  m_rDestination.print(pfstrCommand);
  m_rDestination.print('|');
}

void MegunoLinkProtocol::SendDataTail()
{
  m_rDestination.println('}');
}
