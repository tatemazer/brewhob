/*
 SMS receiver

 This sketch, for the MKR NB 1500 board, waits for a SMS message
 and displays it through the Serial port.

 Circuit:
 * MKR NB 1500 board
 * Antenna
 * SIM card that can receive SMS messages

 created 25 Feb 2012
 by Javier Zorzano / TD
*/

// include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h" 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instances
NB nbAccess;
NB_SMS sms;

// Array to hold the number an SMS is retrieved from
char senderNumber[20];

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Receiver");

  // connection state
  bool connected = false;

  // Start GSM connection
  while (!connected) {
    if (nbAccess.begin(PINNUMBER) == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");
  Serial.println("Waiting for messages");
}

void loop() {
  int c;

  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while ((c = sms.read()) != -1) {
      Serial.print((char)c);
    }

    Serial.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);

}
