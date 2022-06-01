/*

 This example enables you to change or remove the PIN number of
 a SIM card inserted into a MKR NB 1500 board.

 Circuit:
 * MKR NB 1500 board
 * Antenna
 * SIM card

 Created 12 Jun 2012
 by David del Peral
*/

// libraries
#include <MKRNB.h>

// pin manager object
NBPIN PINManager;

// save input in serial by user
String user_input = "";

// authenticated with PIN code
boolean auth = false;

// serial monitor result messages
String oktext = "OK";
String errortext = "ERROR";

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("Change PIN example\n");
  PINManager.begin();

  // check if the SIM have pin lock
  while (!auth) {
    int pin_query = PINManager.isPIN();
    if (pin_query == 1) {
      // if SIM is locked, enter PIN code
      Serial.print("Enter PIN code: ");
      user_input = readSerial();
      // check PIN code
      if (PINManager.checkPIN(user_input) == 0) {
        auth = true;
        PINManager.setPINUsed(true);
        Serial.println(oktext);
      } else {
        // if PIN code was incorrected
        Serial.println("Incorrect PIN. Remember that you have 3 opportunities.");
      }
    } else if (pin_query == -1) {
      // PIN code is locked, user must enter PUK code
      Serial.println("PIN locked. Enter PUK code: ");
      String puk = readSerial();
      Serial.print("Now, enter a new PIN code: ");
      user_input = readSerial();
      // check PUK code
      if (PINManager.checkPUK(puk, user_input) == 0) {
        auth = true;
        PINManager.setPINUsed(true);
        Serial.println(oktext);
      } else {
        // if PUK o the new PIN are incorrect
        Serial.println("Incorrect PUK or invalid new PIN. Try again!.");
      }
    } else if (pin_query == -2) {
      // the worst case, PIN and PUK are locked
      Serial.println("PIN and PUK locked. Use PIN2/PUK2 in a mobile phone.");
      while (true);
    } else {
      // SIM does not requires authentication
      Serial.println("No pin necessary.");
      auth = true;
    }
  }

  // start module
  Serial.print("Checking register in NB IoT / LTE Cat M1 network...");
  if (PINManager.checkReg() == 0) {
    Serial.println(oktext);
  }
  // if you are connect by roaming
  else if (PINManager.checkReg() == 1) {
    Serial.println("ROAMING " + oktext);
  } else {
    // error connection
    Serial.println(errortext);
    while (true);
  }
}

void loop() {
  // Function loop implements pin management user menu
  // You can only change PIN code if your SIM uses pin lock

  Serial.println("Choose an option:\n1 - On/Off PIN.");
  if (PINManager.getPINUsed()) {
    Serial.println("2 - Change PIN.");
  }
  // save user input to user_op variable
  String user_op = readSerial();
  if (user_op == "1") {
    Serial.println("Enter your PIN code:");
    user_input = readSerial();
    // activate/deactivate PIN lock
    PINManager.switchPIN(user_input);
  } else if (user_op == "2" && PINManager.getPINUsed()) {
    Serial.println("Enter your actual PIN code:");
    String oldPIN = readSerial();
    Serial.println("Now, enter your new PIN code:");
    String newPIN = readSerial();
    // change PIN
    PINManager.changePIN(oldPIN, newPIN);
  } else {
    Serial.println("Incorrect option. Try again!.");
  }
  delay(1000);
}

/*
  Read serial input
 */
String readSerial() {
  String text = "";
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        return text;
      }
      if (inChar != '\r') {
        text += inChar;
      }
    }
  }
}
