/*

 NB Scan Networks

 This example prints out the IMEI number of the modem,
 then checks to see if it's connected to a carrier.
 Then it scans for nearby networks and prints out their signal strengths.

 Circuit:
 * MKR NB 1500 board
 * Antenna
 * SIM card

 Created 8 Mar 2012
 by Tom Igoe, implemented by Javier Carazo
 Modified 4 Feb 2013
 by Scott Fitzgerald
*/

// libraries
#include <MKRNB.h>

#include "arduino_secrets.h" 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instance
NB nbAccess;     // include a 'true' parameter to enable debugging
NBScanner scannerNetworks;
NBModem modemTest;

// Save data variables
String IMEI = "";

// serial monitor result messages
String errortext = "ERROR";

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("NB IoT/LTE Cat M1 networks scanner");
  scannerNetworks.begin();

  // connection state
  boolean connected = false;

  // Start module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
    if (nbAccess.begin(PINNUMBER) == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  // get modem parameters
  // IMEI, modem unique identifier
  Serial.print("Modem IMEI: ");
  IMEI = modemTest.getIMEI();
  IMEI.replace("\n", "");
  if (IMEI != NULL) {
    Serial.println(IMEI);
  }
}

void loop() {
  // currently connected carrier
  Serial.print("Current carrier: ");
  Serial.println(scannerNetworks.getCurrentCarrier());

  // returns strength and BER
  // signal strength in 0-31 scale. 31 means power > 51dBm
  // BER is the Bit Error Rate. 0-7 scale. 99=not detectable
  Serial.print("Signal Strength: ");
  Serial.print(scannerNetworks.getSignalStrength());
  Serial.println(" [0-31]");

  // scan for existing networks, displays a list of networks
  Serial.println("Scanning available networks. May take some seconds.");
  Serial.println(scannerNetworks.readNetworks());
  // wait ten seconds before scanning again
  delay(10000);
}
