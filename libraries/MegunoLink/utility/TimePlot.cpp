#include "TimePlot.h"

TimePlot::TimePlot( const char *channelName /*= NULL*/, Print &rDestination )
  : Plot(F("TIMEPLOT"), channelName, rDestination)
{

}

TimePlot::TimePlot( const __FlashStringHelper *channelName, Print &rDestination )
  : Plot(F("TIMEPLOT"), channelName, rDestination)
{

}

void TimePlot::Run(bool bEnable)
{
  SendDataHeader(F("RUN"));
  m_rDestination.print(bEnable ? '1' : '0');
  SendDataTail();
}

void TimePlot::SetXRange(float fTimespanHours)
{
	SendDataHeader(F("xrange"));
	m_rDestination.print("T|");
	m_rDestination.print(fTimespanHours,5);
	SendDataTail();
}

void TimePlot::Stop()
{
  Run(false);
}

void TimePlot::SendFloatData(const char * seriesName, float yValue, int nDecimalPlaces, const char * seriesProperties/*=NULL*/)
{
  SendDataHeader(F("DATA"));
  m_rDestination.print(seriesName);
  SendSeriesProperties(seriesProperties);
  m_rDestination.print(F("T|"));
  m_rDestination.print(yValue, nDecimalPlaces);
  SendDataTail();
}

void TimePlot::SendFloatData(const char * seriesName, float yValue, int nDecimalPlaces, Colors Color, LineStyle Line, uint8_t uLineWidth, MarkerStyle Marker, Axis ax)
{
  SendDataHeader(F("DATA"));
  m_rDestination.print(seriesName);
  SendSeriesProperties(Color, Line, uLineWidth, Marker, ax);
  m_rDestination.print(F("T|"));
  m_rDestination.print(yValue, nDecimalPlaces);
  SendDataTail();
}

void TimePlot::SendFloatData(const __FlashStringHelper * seriesName, float yValue, int nDecimalPlaces, const char * seriesProperties/*=NULL*/)
{
  SendDataHeader(F("DATA"));
  m_rDestination.print(seriesName);
  SendSeriesProperties(seriesProperties);
  m_rDestination.print(F("T|"));
  m_rDestination.print(yValue, nDecimalPlaces);
  SendDataTail();
}

void TimePlot::SendFloatData(const __FlashStringHelper * seriesName, float yValue, int nDecimalPlaces, Colors Color, LineStyle Line, uint8_t uLineWidth, MarkerStyle Marker, Axis ax)
{
  SendDataHeader(F("DATA"));
  m_rDestination.print(seriesName);
  SendSeriesProperties(Color, Line, uLineWidth, Marker, ax);
  m_rDestination.print(F("T|"));
  m_rDestination.print(yValue, nDecimalPlaces);
  SendDataTail();
}