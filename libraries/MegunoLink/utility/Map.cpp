#include "Map.h"


Map::Map(Print &rDestination )
  : MegunoLinkProtocol(F("MAP"), rDestination)
{
}


void Map::SendData(const char *Placename, const char *Latitude, const char *Longitude)
{
  SendDataHeader(F("SET"));
  m_rDestination.print(Placename);
  m_rDestination.print('|');
  m_rDestination.print(Latitude);
  m_rDestination.print('|');
  m_rDestination.print(Longitude);
  SendDataTail();
}

void Map::SendData(const __FlashStringHelper *Placename, const char *Latitude, const char *Longitude)
{
  SendDataHeader(F("SET"));
  m_rDestination.print(Placename);
  m_rDestination.print('|');
  m_rDestination.print(Latitude);
  m_rDestination.print('|');
  m_rDestination.print(Longitude);
  SendDataTail();
}

void Map::SendData(const char *Placename, float Latitude, float Longitude)
{
  SendDataHeader(F("SET"));
  m_rDestination.print(Placename);
  m_rDestination.print('|');
  m_rDestination.print(Latitude, 7);
  m_rDestination.print('|');
  m_rDestination.print(Longitude, 7);
  SendDataTail();
}

void Map::SendData(const __FlashStringHelper *Placename, float Latitude, float Longitude)
{
  SendDataHeader(F("SET"));
  m_rDestination.print(Placename);
  m_rDestination.print('|');
  m_rDestination.print(Latitude, 7);
  m_rDestination.print('|');
  m_rDestination.print(Longitude, 7);
  SendDataTail();
}

