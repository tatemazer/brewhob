/*
  This sketch demonstrates how to handle deferred OTA from Arduino IoT Cloud.

  Deferred OTA can be triggered using the arduino-cloud-cli with the following command:
  ./arduino-cloud-cli ota upload --device-id xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx --file filename.ino.bin --deferred
  The update file and the download link will be available to be used within one week.

  * always_deny callback will always postpone the OTA update
  * always_allow callback will immediately apply the OTA update
  * ask_user_via_serial callback will read user input from serial to apply or postpone OTA update

  This sketch is compatible with:
   - MKR WIFI 1010
   - Nano 33 IoT
   - Portenta
   - Nano RP2040
*/

#include "arduino_secrets.h"
#include "thingProperties.h"

#if defined(ESP32)
static int const LED_BUILTIN = 2;
#endif

bool always_deny() {
  return false;
}

bool always_allow() {
  return true;
}

static bool ask_user_via_serial_first_run = true;
bool ask_user_via_serial() {
  if (ask_user_via_serial_first_run) {
    Serial.println("Apply OTA? y / [n]");
    ask_user_via_serial_first_run = false;
  }
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'y' || c == 'Y') {
      return true;  
    }
  }
  return false;
}

bool onOTARequestCallback()
{
  /* Select the preferred behaviour changing the called function */
  //return always_deny();
  //return always_allow();
  return ask_user_via_serial();
}

void setup() {
  /* Initialize serial and wait up to 5 seconds for port to open */
  Serial.begin(9600);
  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }

  /* Configure LED pin as an output */
  pinMode(LED_BUILTIN, OUTPUT);

  /* This function takes care of connecting your sketch variables to the ArduinoIoTCloud object */
  initProperties();

  /* Initialize Arduino IoT Cloud library */
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /* Setup OTA callback */
  ArduinoCloud.onOTARequestCb(onOTARequestCallback);

  setDebugMessageLevel(DBG_INFO);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
}

/*
 * 'onLedChange' is called when the "led" property of your Thing changes
 */
void onLedChange() {
  Serial.print("LED set to ");
  Serial.println(led);
  digitalWrite(LED_BUILTIN, led);
}
