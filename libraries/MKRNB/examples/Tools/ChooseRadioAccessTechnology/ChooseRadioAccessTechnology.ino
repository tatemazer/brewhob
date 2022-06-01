/*
  Radio Access Technology selection for Arduino MKR NB 1500

  This sketch allows you to select your preferred 4G
  Narrowband Radio Access Technology (RAT).

  You can choose among CAT-M1, NB-IoT or a combination of both.

  Selecting JUST ONE technology will speed up the modem's network
  registration A LOT!

  The chosen configuration will be saved to modem's internal memory
  and will be preserved through MKR NB 1500 sketch uploads.

  In order to change the RAT, you will need to run this sketch again.

  Circuit:
  - MKR NB 1500 board
  - Antenna
  - SIM card

  Created 26 November 2018
  by Giampaolo Mancini

*/

#include <MKRNB.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  MODEM.begin();
  while (!MODEM.noop());

  for (int i = 0; i < 80; i++) Serial.print("*");
  Serial.println();
  Serial.println("This sketch allows you to select your preferred");
  Serial.println("4G Narrowband Radio Access Technology (RAT).");
  Serial.println();
  Serial.println("You can choose among CAT-M1, NB-IoT or a combination of both.");
  Serial.println();
  Serial.println("Selecting JUST ONE technology will speed up the modem's network registration A LOT!");
  Serial.println();
  Serial.println("The chosen configuration will be saved to modem's internal memory");
  Serial.println("and will be preserved through MKR NB 1500 sketch uploads.");
  Serial.println();
  Serial.println("In order to change the RAT, you will need to run this sketch again.");
  for (int i = 0; i < 80; i++) Serial.print("*");

  Serial.println();
  Serial.println();
  Serial.println("Please choose your Radio Access Technology:");
  Serial.println();
  Serial.println("    0 - CAT M1 only");
  Serial.println("    1 - NB IoT only");
  Serial.println("    2 - CAT M1 preferred, NB IoT as failover (default)");
  Serial.println("    3 - NB IoT preferred, CAT M1 as failover");
  Serial.println();
}

void loop() {
  String uratChoice;

  Serial.print("> ");

  Serial.setTimeout(-1);
  while (Serial.available() == 0);
  String uratInput = Serial.readStringUntil('\n');
  uratInput.trim();
  int urat = uratInput.toInt();
  Serial.println(urat);

  switch (urat) {
    case 0:
      uratChoice = "7";
      break;
    case 1:
      uratChoice = "8";
      break;
    case 2:
      uratChoice = "7,8";
      break;
    case 3:
      uratChoice = "8,7";
      break;
    default:
      Serial.println("Invalid input. Please, retry.");
      return;
  }

  setRAT(uratChoice);
  apply();

  Serial.println();
  Serial.println("Radio Access Technology selected.");
  Serial.println("Now you can upload your 4G application sketch.");
  while (true);
}

bool setRAT(String choice)
{
  String response;

  Serial.print("Disconnecting from network: ");
  MODEM.sendf("AT+COPS=2");
  MODEM.waitForResponse(2000);
  Serial.println("done.");

  Serial.print("Setting Radio Access Technology: ");
  MODEM.sendf("AT+URAT=%s", choice.c_str());
  MODEM.waitForResponse(2000, &response);
  Serial.println("done.");

  return true;
}

bool apply()
{
  Serial.print("Applying changes and saving configuration: ");
  MODEM.send("AT+CFUN=15");
  MODEM.waitForResponse(5000);
  delay(5000);

  do {
    delay(1000);
    MODEM.noop();
  } while (MODEM.waitForResponse(1000) != 1);

  Serial.println("done.");
  
  return true;
}
