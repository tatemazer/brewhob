#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

#if defined(BOARD_HAS_WIFI)
#elif defined(BOARD_HAS_GSM)
#elif defined(BOARD_HAS_LORA)
#elif defined(BOARD_HAS_NB)
#else
  #error "Arduino IoT Cloud currently only supports MKR1000, MKR WiFi 1010, MKR WAN 1300/1310, MKR NB 1500 and MKR GSM 1400"
#endif

#define THING_ID "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define BOARD_ID "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"

void onLedChange();

bool led;
int potentiometer;
int seconds;

void initProperties() {
#if defined(BOARD_ESP8266)
  ArduinoCloud.setBoardId(BOARD_ID);
  ArduinoCloud.setSecretDeviceKey(SECRET_DEVICE_KEY);
#endif
  ArduinoCloud.setThingId(THING_ID);
#if defined(BOARD_HAS_WIFI) || defined(BOARD_HAS_GSM) || defined(BOARD_HAS_NB)
  ArduinoCloud.addProperty(led, Permission::Write).onUpdate(onLedChange);
  ArduinoCloud.addProperty(potentiometer, Permission::Read).publishOnChange(10);
  ArduinoCloud.addProperty(seconds, Permission::Read).publishOnChange(1);
#elif defined(BOARD_HAS_LORA)
  ArduinoCloud.addProperty(led, 1, READWRITE, ON_CHANGE, onLedChange);
  ArduinoCloud.addProperty(potentiometer, 2, READ, ON_CHANGE);
  ArduinoCloud.addProperty(seconds, 3, READ, 5 * MINUTES);
#endif
}

#if defined(BOARD_HAS_WIFI)
  WiFiConnectionHandler ArduinoIoTPreferredConnection(SECRET_SSID, SECRET_PASS);
#elif defined(BOARD_HAS_GSM)
  GSMConnectionHandler ArduinoIoTPreferredConnection(SECRET_PIN, SECRET_APN, SECRET_LOGIN, SECRET_PASS);
#elif defined(BOARD_HAS_LORA)
  LoRaConnectionHandler ArduinoIoTPreferredConnection(SECRET_APP_EUI, SECRET_APP_KEY, _lora_band::EU868, NULL, _lora_class::CLASS_A);
#elif defined(BOARD_HAS_NB)
  NBConnectionHandler ArduinoIoTPreferredConnection(SECRET_PIN, SECRET_APN, SECRET_LOGIN, SECRET_PASS);
#endif
