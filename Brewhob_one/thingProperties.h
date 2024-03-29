// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "Brewhob.h"

const char SSID[]     = "dank";          // Network SSID (name)
const char PASS[]     = "reallydank";    // Network password (use for WPA, or use as key for WEP)

void onShotSizeChange();
void onBrewBoilerSPChange();
void onDelayPumpStartChange();
void onDwellChange();
void onPrewetChange();
void onSteamBoilerSPChange();
void onScheduleChange();
void onBrewBoilerOnChange();
void onSteamBoilerOnChange();

String _LastShot;
String _Log;
String _State;
CloudCounter _FlowMeter;
CloudCounter _ShotSize;
float _PowerConsumption_kWh;
CloudTemperatureSensor _BrewBoilerTemp;
CloudTemperatureSensor _SteamBoilerTemp;
int _BrewBoilerSP;
int _DelayPumpStart;
int _Dwell;
int _Prewet;
int _SteamBoilerSP;
CloudSchedule _Schedule;
bool _BrewBoilerOn;
bool _SteamBoilerOn;
CloudTime _ShotTimer;
int _FlowRate;


void initProperties(){

  ArduinoCloud.addProperty(_LastShot,             READ,         ON_CHANGE, NULL);
  ArduinoCloud.addProperty(_Log,                  READ,       1 * SECONDS, NULL);
  ArduinoCloud.addProperty(_State,                READ,       1 * SECONDS, NULL);
  ArduinoCloud.addProperty(_FlowMeter,            READ,       1 * SECONDS, NULL);
  ArduinoCloud.addProperty(_ShotSize,             READWRITE,  1 * SECONDS, onShotSizeChange);
  ArduinoCloud.addProperty(_PowerConsumption_kWh, READ,       1 * SECONDS, NULL);
  ArduinoCloud.addProperty(_BrewBoilerTemp,       READ,       1 * SECONDS, NULL);
  ArduinoCloud.addProperty(_SteamBoilerTemp,      READ,       1 * SECONDS, NULL);
  ArduinoCloud.addProperty(_BrewBoilerSP,         READWRITE,  1 * SECONDS, onBrewBoilerSPChange);
  ArduinoCloud.addProperty(_DelayPumpStart,       READWRITE,  1 * SECONDS, onDelayPumpStartChange);
  ArduinoCloud.addProperty(_Dwell,                READWRITE,  1 * SECONDS, onDwellChange);
  ArduinoCloud.addProperty(_Prewet,               READWRITE,  1 * SECONDS, onPrewetChange);
  ArduinoCloud.addProperty(_SteamBoilerSP,        READWRITE,  1 * SECONDS, onSteamBoilerSPChange);
  ArduinoCloud.addProperty(_Schedule,             READWRITE,  1 * SECONDS, onScheduleChange);
  ArduinoCloud.addProperty(_BrewBoilerOn,         READWRITE,  1 * SECONDS, onBrewBoilerOnChange);
  ArduinoCloud.addProperty(_SteamBoilerOn,        READWRITE,  1 * SECONDS, onSteamBoilerOnChange);
  ArduinoCloud.addProperty(_ShotTimer,            READ,       1 * SECONDS, NULL);
  ArduinoCloud.addProperty(_FlowRate,             READ,       1 * SECONDS, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);