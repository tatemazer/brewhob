/*
  Brewhob.cpp - Library for flashing Brewhob code.
  Created by Tate Mazer, April 10, 2020.
*/

#include <Arduino.h>
#include <Brewhob.h>
#include <Adafruit_MAX31865.h>

Brewhob::Brewhob()
  : sw1State_(0), //Buttons are active low, but we are storing "pressed" == 1
    sw2State_(0),
    sw3State_(0),
    sw1Time_(0),
    sw2Time_(0),
    sw3Time_(0),
    temp1_(SETPOINT1),
    temp2_(SETPOINT2),
    state_(State::STANDBY),
    pumpState_(0), //0-255 (0 OR 255 for AC pump)
    manualMode_(0),
    pumpDisabled_(0),
    power_(1),
    tea_(0),
    sol1State_(0),
    sol2State_(0),
    sol3State_(0),
    flowCount_(0),
    flowRate_(0),
    lastPulseTime_(0),
    shotTimer_(0),
    fillDelayCounter_(0),
    shotSize_(SHOT_SIZE),
    fillProbeState_(1) //initialize probe to "touching water" so as to avoid prematurely entering fill state upon startup
{
  //Initialize Pins
  pinMode(FLOW_PIN,   INPUT);
  pinMode(SW1_PIN,    INPUT);
  pinMode(SW2_PIN,    INPUT);
  pinMode(SW3_PIN,    INPUT);
  pinMode(FILL_IN_PIN,INPUT);
  pinMode(PUMP_PIN,   OUTPUT);
  pinMode(SOL1_PIN,   OUTPUT);
  pinMode(SOL2_PIN,   OUTPUT);
  pinMode(SOL3_PIN,   OUTPUT);
  pinMode(HEAT1_PIN,  OUTPUT);
  pinMode(HEAT2_PIN,  OUTPUT);
  digitalWrite(POT_POW_PIN,HIGH);

  analogReadResolution(12);

  rtd1_ = new Adafruit_MAX31865(RTD1_PIN);
  rtd2_ = new Adafruit_MAX31865(RTD2_PIN);

  PID1_ = new FastPID(Kp, Ki, Kd, 0.5);
  PID2_ = new FastPID(Kp, Ki, Kd, 0.5);

  PID1_->setOutputRange(0,2000);
  PID2_->setOutputRange(0,2000);

  Serial.begin(9600);
}


void Brewhob::printHeader(){
  Serial.println("State\t Sw Sol RTD Setpoint FM\tTmr\tFillV Pre Dwell");
}
void Brewhob::printAll(){

  //print states
  switch(state_)
  {
    case State::STANDBY : 
      log_ = "STANDBY  ";
      break;
    case State::BREW : 
      log_ = "BREW     ";
      break;
    case State::FILL : 
      log_ = "FILL     ";
      break;
    case State::FILLDELAY :
      log_ = "FILLDELAY";
      break;
    case State::OFF :
      log_ = "OFF      ";
      break;
  }

  log_ = log_ 
    + sw1State_ 
    + sw2State_
    + sw3State_ + " "
    + sol1State_ 
    + sol2State_
    + sol3State_ + " "
    + (int)round(getRTD(1)) +  " "
    + (int)round(getRTD(2)) +  " "
    + setpoint1_+ " "
    + setpoint2_+ " "
    + flowCount_ + " "
    + shotTimer_ + " "
    + analogRead(FILL_IN_PIN) + " "
    + prewet_ + " "
    + dwell_;

  Serial.println(log_);
}

//returns true if sensor is touching water
int Brewhob::readFill(){

  int val = analogRead(FILL_IN_PIN);
  if(val>2000) fillTally_++; //not touching water,but may want to wait to confirm this isnt noise
  else fillTally_ = 0;

  // only enable fill state after a set number of "no water" reads
  fillProbeState_ = (fillTally_ <= FILL_TALLY_LIM);
  return fillProbeState_;
}

//if any switches are considered momentary, their state will "latch"
int Brewhob::readSwitch(int swNum){
  switch(swNum){
    case 1:
      //Debouncing: refuse to read switch again if we just recently did.
      if(millis()-sw1Time_>100){
        sw1Time_ = millis();

        //toggle state
        if(BREW_VALVE_EN)
          sw1State_ = !sw1State_;
        else
          sw1State_ = !digitalRead(SW1_PIN);
        return sw1State_;
      }
      break;
    case 2:
      //Debouncing: refuse to read switch again if we just recently did.
      if(millis()-sw2Time_>100){
        sw2Time_ = millis();

        //toggle state
        if(TEA_VALVE_EN)
          sw2State_ = !sw2State_;
        else
          sw2State_ = !digitalRead(SW2_PIN);
        return sw2State_;
      }
      break;
    case 3:
      //Debouncing: refuse to read switch again if we just recently did.
      if(millis()-sw3Time_>100){
        sw3Time_ = millis();
        sw3State_ = !digitalRead(SW3_PIN);
        return sw3State_;
      }
      break;
  }
  return -1;
}

int Brewhob::setState(){
  //set new state_ value based on current device status's
  switch(state_){
    case State::OFF :
      //only exit OFF if told to do so via web
      if(power_){
        //Go straight to standby
        state_ = State::STANDBY;
      }
      break;
    case State::STANDBY : 
      if(!power_){
        //Only enter off state once the machine reaches standby
        state_ = State::OFF;
      }
      else if((BREW_CNTRL_ANALOG && potVal_ > 755 && flowCount_ < shotSize_) 
      || (!BREW_CNTRL_ANALOG && sw1State_ && flowCount_ < shotSize_)){ //ensure that the lever has been lowered since the last shot
        state_= State::BREW;
        shotTimerStart_ = millis();
        resetFM();
      }

      else if(fillProbeState_ == 0 && STEAM_BOILER_EN){
        state_ = State::FILL;
      }

      if(BREW_CNTRL_ANALOG && potVal_ < 755 || !BREW_CNTRL_ANALOG && !sw1State_) resetFM();

      break;
    case State::BREW : 

      if(BREW_CNTRL_ANALOG && potVal_ < 755 || 
        !BREW_CNTRL_ANALOG && !sw1State_ || 
        flowCount_ >= shotSize_){

        state_ = State::STANDBY;

        if(shotTimer_ > 20){
          //lastShotSpecs Here
          recordShotSpecs();
        }
      }
      shotTimer_ = ( millis() - shotTimerStart_ ) / 1000;


      break;
    case State::FILL :

      if(BREW_CNTRL_ANALOG && potVal_ > 755 && flowCount_ < shotSize_)
      {
        state_= State::BREW;
        shotTimerStart_ = millis();
        resetFM();
      }

      else if(fillProbeState_) // now touching water, transition to fill-delay state
      {
        state_ = State::FILLDELAY;
        fillDelayCounter_ = millis();
      }
      break;
    case State::FILLDELAY :
      if(sw1State_ && flowCount_ < shotSize_)
      {
        state_= State::BREW;
        shotTimerStart_ = millis();
        resetFM();
      }
      else if(millis() - fillDelayCounter_ > 1000*FILL_DELAY)
      {
        state_= State::STANDBY;
      }
      break;  
  }


  //set devices according to current state
  Serial.println(getState());
  switch(state_){
    case State::STANDBY : 
      setSolenoid(1,0);
      setSolenoid(2,0);
      setSolenoid(3,0);
      setPump(0);

      if(sw2State_){
        setSolenoid(3,1);
        setPump(180);
      }
      break;

    case State::BREW :
      //Set the initial pump pressure only if the pump was previously off
      if(pumpState_ == 0 & !BREW_CNTRL_ANALOG){
        if(manualMode_) setPump(50);
        else            setPump(180); 
      }

      //close fill and tea valve
      setSolenoid(1,0);
      setSolenoid(3,0);


      //Open Brew Solenoid Valve
      if(BREW_VALVE_EN && !dwellOn_)
        setSolenoid(2,1);

      if(BREW_CNTRL_ANALOG)
        setPump(map(potVal_,755,900,50,180));

      if(sw2State_){
        setSolenoid(3,1);
        setPump(180);
      }
      break;

    case State::FILL : 

      //Open Fill Solenoid, close others
      setSolenoid(1,1);
      setSolenoid(2,0);
      setSolenoid(3,0);
      setPump(100);

      if(sw2State_){
        setSolenoid(3,1);
        setPump(180);
      }
      break;
    case State::FILLDELAY :

      if(sw2State_){
        setSolenoid(3,1);
        setPump(180);
      }

      break;
  }  
  return 0;
}

int Brewhob::setSolenoid(int solNum, int val){
   switch(solNum){
    case 1:
      digitalWrite(SOL1_PIN,val);
      sol1State_ = val;
      break;
    case 2:
      digitalWrite(SOL2_PIN,val);
      sol2State_ = val;
      break;
    case 3:
      digitalWrite(SOL3_PIN,val);
      sol3State_ = val;
      break;
  }
  return 0;
}

//if pwm is nonzero and AC_PUMP, set duty cycle to 100%
int Brewhob::setPump(int pwm){//0-255
  if(pumpDisabled_)          pwm = 0;
  else if (AC_PUMP && pwm>0) pwm =255;
  analogWrite(PUMP_PIN,pwm);
  pumpState_ = pwm;
  return 0;
}
int Brewhob::setManualMode(bool val){manualMode_ = val;}
int Brewhob::setPower(bool val){power_ = val;}
void Brewhob::resetFM(){
  flowCount_=0;
  lastPulseTime_ = 0;
}
void Brewhob::incrementFM(){
  flowCount_+=1;

  //Always keep flowrate at 0 when not brewing
  if(state_ != State::BREW){
    lastPulseTime_ = 0;
    flowRate_ = 0;
  }
  else{
    //Not the first pulse measured
    if( lastPulseTime_ != 0){
      //Pulses per second
      flowRate_ = 1000 / (millis() - lastPulseTime_) ;
    }
    lastPulseTime_ = millis();
  }
  
}
float Brewhob::readRTD(int sensorNum){

  if(sensorNum == 1){
    rtd1_->clearFault();
    ADCFilter1.Filter((rtd1_->temperature(RNOMINAL, RREF))*9/5+32);
    temp1_ = ADCFilter1.Current();
    return temp1_;
  }
  if(sensorNum == 2){
    rtd2_->clearFault();
    ADCFilter2.Filter((rtd2_->temperature(RNOMINAL, RREF))*9/5+32);
    temp2_ = ADCFilter2.Current();
    return temp2_;
  }
}
float Brewhob::getRTD(int sensorNum){
  if(sensorNum == 1){
    return ADCFilter1.Current();
  }
  if(sensorNum == 2){
    return ADCFilter2.Current();
  }
  else
    return -1;
}
String Brewhob::getState(){
  switch(state_){
    case State::STANDBY : 
      return "STANDBY";
    case State::BREW : 
      return "BREW";
    case State::FILL : 
      return "FILL";
    case State::FILLDELAY :
      return "FILLDELAY";
    case State::OFF :
      return "OFF";
  }
}
float Brewhob::getShotTimer(){return shotTimer_;}
float Brewhob::getFlowCount(){return flowCount_;}
int Brewhob::getFlowRate() {return flowRate_;}
int Brewhob::getPump(){return pumpState_;}
int Brewhob::setShotSize(int shotSize){
  shotSize_ = shotSize;
  return 0;
}
int Brewhob::getShotSize(){return shotSize_;}
int Brewhob::getPID(int sensorNum){
  return sensorNum==1?PID1Val_:PID2Val_;
}
int Brewhob::readPID(int sensorNum){
  if(sensorNum==1){
    PID1Val_ = PID1_->step(setpoint1_, temp1_);
    return PID1Val_;
  }
  else{
    PID2Val_ = PID2_->step(setpoint2_, temp2_);
    return PID2Val_;
  }
}
void Brewhob::readPot(){
  int val = analogRead(POT_PIN);
  ADCFilterPot.Filter(val);
  if(val<755) potVal_ = val;
  else potVal_ = ADCFilterPot.Current();
}
/*
void Brewhob::setTime(int time)
{
  rtc.setEpoch(time);
}
*/
void Brewhob::print2digits(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}
void Brewhob::setTea(bool in){ tea_ = in; }
bool Brewhob::getTea(){ return tea_; }
void Brewhob::enableRTD(){
  rtd1_->begin(MAX31865_2WIRE);
  rtd2_->begin(MAX31865_2WIRE);
}
void Brewhob::setTemp(int sensorNum, float val){
  if(sensorNum==1) setpoint1_ = val;
  if(sensorNum==2) setpoint2_ = val;
}
void Brewhob::recordShotSpecs(){
  lastShotSpecs_ = "";
  lastShotSpecs_ = lastShotSpecs_
                 + shotTimer_ 
                 + "s, "
                 + flowCount_
                 + " Pulses, ";


  if(prewet_){
    lastShotSpecs_ = lastShotSpecs_
                   + "Prewet "
                   + prewet_
                   + "s, ";
  }

  if(dwell_){
    lastShotSpecs_ = lastShotSpecs_
                   + "Dwell "
                   + dwell_
                   + "s, ";
  }
  if(delayPumpStart_){
    lastShotSpecs_ = lastShotSpecs_
                   + "Bloom "
                   + delayPumpStart_
                   + "s, ";
  }
  if(BREW_BOILER_EN){
    lastShotSpecs_ = lastShotSpecs_
                   + (int)round(getRTD(1)) 
                   + "F, ";
  }
}
