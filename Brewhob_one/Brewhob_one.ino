/*
  Brewhob_one.ino

  Runs Brewhob one board applied on an espresso machine
  
  modified 03/21/2022
  by Tate Mazer
  
*/
#include <Adafruit_MAX31865.h>
#include "thingProperties.h"
#include "Brewhob.h"

Brewhob* brewhob;
unsigned long last2s_ms;
unsigned long last1s_ms;
unsigned long last10s_ms;


/************* INTERRUPTS ***********/

void SW1_ISR(){ //brew switch ISR
  brewhob->readSwitch(1);
  brewhob->setState();
}
void SW2_ISR(){
    brewhob->readSwitch(2);
    brewhob->setState();
}

void FLOW_ISR(){
  //Only measure flow after full infusion has started
  if(brewhob->getState() == "BREW" &&
    millis() - brewhob->shotTimerStart_ >= ( brewhob->prewet_ + brewhob->dwell_ + brewhob->delayPumpStart_ ) * 1000 ){
    brewhob->incrementFM();
    brewhob->setState();
  }
}

/************* END INTERRUPTS ***********/


void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  brewhob = new Brewhob();

  if(WIFI_ENABLED){
    // Defined in thingProperties.h
    initProperties();

    // Connect to Arduino IoT Cloud, disable watch dog, 
    // this can trigger during the initial connection to wifi
    ArduinoCloud.begin(ArduinoIoTPreferredConnection,false);
    
    //0-4
    setDebugMessageLevel(4);
    ArduinoCloud.printDebugInfo();
  }

  brewhob->enableRTD();

  attachInterrupt(digitalPinToInterrupt(SW1_PIN), SW1_ISR, RISING); //Brew Switch ISR
  attachInterrupt(digitalPinToInterrupt(SW2_PIN), SW2_ISR, RISING); //Brew Switch ISR
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), FLOW_ISR,CHANGE); //flowmeter ISR
}

void loop() {
  if(millis() - last1s_ms >=1000){ //every second
    last1s_ms = millis();
    if(brewhob->getState() != "OFF" && STEAM_BOILER_EN) 
      brewhob->readFill();

    brewhob->readRTD(1);
    brewhob->readRTD(2); 

    if(WIFI_ENABLED){
      brewhob->setPower(_Schedule.isActive()); //turn off/on based on iot schedule
      //Send new data to cloud
      _BrewBoilerTemp   = (int)brewhob->getRTD(1);
      _SteamBoilerTemp  = (int)brewhob->getRTD(2); 
      _ShotTimer        = brewhob->getShotTimer();
      _FlowMeter        = brewhob->getFlowCount();
      _State            = brewhob->getState();
      _Log              = brewhob->log_;
      _FlowRate         = brewhob->getFlowRate();

      ArduinoCloud.update();

      //update local data from remotely saved data
      brewhob->setShotSize(_ShotSize);
      brewhob->setTemp(1,_BrewBoilerSP);
      brewhob->setTemp(2,_SteamBoilerSP);
      brewhob->prewet_ = _Prewet;
      brewhob->dwell_  = _Dwell;
      brewhob->delayPumpStart_ = _DelayPumpStart;
    
    }
    brewhob->setState();
    brewhob->printAll(); 
  }

  if(millis() - last2s_ms >=2000){ //every 2 seconds
    last2s_ms = millis();

    if(brewhob->getState() != "OFF"){
      //turn on brew heater if pwm is nonzero, update pid value
      digitalWrite(
        HEAT1_PIN,brewhob->readPID(1)!=0 
        && BREW_BOILER_EN 
        && _BrewBoilerOn);

      brewhob->readPID(2);
    }
    digitalWrite(HEAT2_PIN,LOW);
    //digitalWrite(HEAT2_PIN,brewhob.readPID(2)!=0 && STEAM_BOILER_EN);
  }

  if(millis() - last10s_ms >=10000){ //every 10 seconds
    last10s_ms = millis();
    brewhob->printHeader();
  }
  //turn off brew boiler if the end of the PWM pulse has been reached
  if(millis() - last2s_ms>=brewhob->getPID(1)) digitalWrite(HEAT1_PIN,LOW);

  //turn on the steam boiler if the brew boiler is off, and its time to turn it on, and the heaters are enabled
  //if(millis() - last2s_ms>=brewhob.getPID(2)) digitalWrite(HEAT2_PIN,LOW);
  
  if( (millis() - last2s_ms >= brewhob->getPID(1) || !_BrewBoilerOn)
    && millis() >= last2s_ms + 2000 - brewhob->getPID(2) 
    && brewhob->getState() != "OFF"
    && STEAM_BOILER_EN
    && _SteamBoilerOn
    ) digitalWrite(HEAT2_PIN,HIGH);


  /* PREWETTING -> DWELL: Pump and valve on -> pump on and valve off */

  if( brewhob->getState() == "BREW"
    && millis() - brewhob->shotTimerStart_ >= ( brewhob->prewet_ * 1000 ) 
    && millis() - brewhob->shotTimerStart_ < ( brewhob->prewet_ + brewhob->dwell_ ) * 1000 )
  {
    //Enter Dwell, disable brew solenoid
    brewhob->setSolenoid(2,0);
    brewhob->dwellOn_ = true;
  }
  else{
    brewhob->dwellOn_ = false;
  }

  if( brewhob->getState() == "BREW"
    && millis() - brewhob->shotTimerStart_ >= ( brewhob->prewet_ + brewhob->dwell_ ) * 1000 )
  {
    //Exit dwell, enable brew valve
    brewhob->setSolenoid(2,1);
  }

  /* PREWETTING -> PUMP DELAY: Pump and Valve on -> Pump off and valve on */

  if( brewhob->getState() == "BREW"
    && millis() - brewhob->shotTimerStart_ >= ( brewhob->prewet_ * 1000 ) 
    && millis() - brewhob->shotTimerStart_ < ( brewhob->prewet_ + brewhob->delayPumpStart_ ) * 1000 )
  {
    //Enter Pump Delay, disable pump
    brewhob->setPump(0);
    brewhob->pumpDisabled_ = true;
  }
  else{
    brewhob->pumpDisabled_ = false;
  }

  if( brewhob->getState() == "BREW"
    && millis() - brewhob->shotTimerStart_ >= (brewhob->prewet_ + brewhob->delayPumpStart_ ) * 1000 )
  {
    brewhob->setPump(255);
  }
}


void onBrewBoilerOnChange(){}
void onSteamBoilerOnChange(){}
void onShotSizeChange()  {
  brewhob->setShotSize(_ShotSize);
}
void onScheduleChange(){}
void onBrewBoilerSPChange(){}
void onSteamBoilerSPChange(){}
void onPrewetChange()  {}
void onDwellChange()  {
  //Dwell and Pump Delay cannot both be enabled
  if(_Dwell){
    brewhob->delayPumpStart_ = 0;
    _DelayPumpStart = 0;
  }
}
void onDelayPumpStartChange() {
  //Dwell and Pump Delay cannot both be enabled
  if(_DelayPumpStart){
    brewhob->dwell_ = 0;
    _Dwell = 0;
  }
}