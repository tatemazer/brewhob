#pragma once
#include "MegunoLinkProtocol.h"

class Plot : protected MegunoLinkProtocol
{
public:
  enum Colors 
  { 
    Red = 'r',
    Green = 'g', 
    Blue = 'b',
    Yellow = 'y',
    Black = 'k',
    Magenta = 'm',
    Cyan = 'c',
    White = 'w',
  };

  enum LineStyle
  {
    Solid = '_',
    Dashed = '-',
    Dotted = ':',
    DashDot = ';',
    DashDotDot = '%',
    NoLine = '#',
  };

  enum MarkerStyle 
  {
    Square = 's',
    Diamond = 'd',
    Triangle = '^',
    Circle = 'o',
    Cross = 'x',
    Plus = '+',
    Star = '*',
    DownwardTriangle = 'v',
    NoMarker = 'n',
    FilledSquare = 'S',
    FilledDiamond = 'D',
    FilledCircle = 'O',
  };

  enum Axis
  {
    DefaultAxis = ' ',
    LeftAxis = '<',
    RightAxis = '>',
  };

  void SetTitle(const char *title);
  void SetXLabel(const char *xlabel);
  void SetYLabel(const char *ylabel);
  void SetY2Label(const char *ylabel);
  void SetXlabel(const char *xlabel);
  void SetYlabel(const char *ylabel);
  void SetY2label(const char *ylabel);

  void SetTitle(const __FlashStringHelper *title);
  void SetXLabel(const __FlashStringHelper *xlabel);
  void SetYLabel(const __FlashStringHelper *ylabel);
  void SetY2Label(const __FlashStringHelper *ylabel);
  void SetXlabel(const __FlashStringHelper *xlabel);
  void SetYlabel(const __FlashStringHelper *ylabel);
  void SetY2label(const __FlashStringHelper *ylabel);

  void SetSeriesProperties(const char *SeriesName, Colors Color, LineStyle Line, uint8_t uLineWidth, MarkerStyle Marker, Axis ax = DefaultAxis);
  void SetSeriesProperties(const __FlashStringHelper *SeriesName, Colors Color, LineStyle Line, uint8_t uLineWidth, MarkerStyle Marker, Axis ax = DefaultAxis);

  void SetSeriesProperties(const char *SeriesName, const char *SeriesProperties);
  void SetSeriesProperties(const __FlashStringHelper *SeriesName, const char *SeriesProperties);
  void SetSeriesProperties(const __FlashStringHelper *SeriesName, const __FlashStringHelper *SeriesProperties);

  void Clear();
  void Clear(const char *SeriesName);
  void Clear(const __FlashStringHelper *SeriesName);

  void SetYRange(float fYLimLower, float fYLimUpper);
  void SetY2Range(float fYLimLower, float fYLimUpper);

  void SetYVisible(bool bVisible = true);
  void SetY2Visible(bool bVisible = true);

protected:
  Plot(const __FlashStringHelper *Context, Print &rDestination = Serial);
  Plot(const __FlashStringHelper *Context, const char *Channel, Print &rDestination = Serial);
  Plot(const __FlashStringHelper *Context, const __FlashStringHelper *Channel, Print &rDestination = Serial);

  void SetParameter(const __FlashStringHelper *pfstrParameter, const char * Value);
  void SetParameter(const __FlashStringHelper *pfstrParameter, const __FlashStringHelper * Value);
  void SendSeriesProperties(const char *SeriesProperties);
  void SendSeriesProperties(const __FlashStringHelper *SeriesProperties);
  void SendSeriesProperties(Colors Color, LineStyle Line, uint8_t uLineWidth, MarkerStyle Marker, Axis ax);

  void SendHeader_Data();
  void SendTimeSeparator();

private:
  void SendRangeDetail(float fYLimLower, float fYLimUpper);
};

