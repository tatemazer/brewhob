/*
  Brewhob.h - Library for flashing Brewhob code.
  Created by Tate Mazer, 4/5/2020.
*/
#ifndef Brewhob_h
#define Brewhob_h

#include <Arduino.h>
#include <Adafruit_MAX31865.h>
#include <Filter.h>
#include <FreeRTOS_SAMD21.h> //samd21
#include <FastPID.h>
#include "config.h"
#include <RTCZero.h>
#include <RTCCounter.h>


//"get" functions return the current stored values. 
//"read" functions read sensors, then store and return the new value
//"set" functions set device states and store values. return 0 if successful.
class Brewhob
{
  public:
    Brewhob();
    ~Brewhob() {};

    enum State {STANDBY,BREW,FILL,FILLDELAY,OFF,TEA};

    void printAll();
    void printHeader();

    void enableRTD();


    //done, untested
    int readFill(); //returns true if sensor is touching water
    int readSwitch(int swNum);
    int setState();
    int setSolenoid(int solNum, int val);
    int setPump(int pwm); //0-255
    int setManualMode(bool val);
    int setPower(bool val);
    void resetFM();
    void incrementFM();
    float readRTD(int sensorNum);
    void enableRTD(int sensorNum);
    String getState();
    String getLastShot();
    float getRTD(int sensorNum);
    float getShotTimer();
    float getFlowCount();
    int getFlowRate();
    int getPump();
    int setShotSize(int shotSize);
    int getShotSize();
    int getPID(int sensorNum);
    int readPID(int sensorNum);
    void readPot();
    //void setTime(int time);
    void print2digits(int number); 
    void setTea(bool in);
    bool getTea();
    void setTemp(int sensorNum, float val);

    String                  log_;
    String                  lastShotSpecs_;
    int                     dwell_;
    int                     prewet_;
    bool                    dwellOn_;
    volatile unsigned long  shotTimerStart_;
    int                     delayPumpStart_;
    bool                    pumpDisabled_;


  private:
    volatile int            sw1State_;
    volatile int            sw2State_;
    volatile int            sw3State_;
    volatile int            sol1State_;
    volatile int            sol2State_;
    volatile int            sol3State_;
    volatile int            pumpState_; //0-255 (0 OR 255 for AC pump)
    volatile unsigned long  sw1Time_;
    volatile unsigned long  sw2Time_;
    volatile unsigned long  sw3Time_;
    float                   temp1_;
    float                   temp2_;
    float                   setpoint1_;
    float                   setpoint2_;
    bool                    manualMode_;
    bool                    power_;
    bool                    tea_;
    volatile State          state_; //standby,brew,fill,filldelay,off


    Adafruit_MAX31865*      rtd1_;
    Adafruit_MAX31865*      rtd2_;

    FastPID*                PID1_; 
    FastPID*                PID2_; 
    int64_t                 PID1Val_ = 0;
    int64_t                 PID2Val_ = 0;


    int                     fillProbeState_; //touching water = 1=
    unsigned long           fillDelayCounter_;
    uint16_t                fillTally_ = 0;

    volatile uint16_t       potVal_;

    volatile int            shotTimer_;
    //flowmeter data
    volatile int            flowCount_;
    volatile int            flowRate_;
    volatile unsigned long  lastPulseTime_;
    volatile int            shotSize_;
    ExponentialFilter<float> ADCFilter1 = ExponentialFilter<float>(10, 72);
    ExponentialFilter<float> ADCFilter2 = ExponentialFilter<float>(10, 72);
    ExponentialFilter<float> ADCFilterPot = ExponentialFilter<float>(10, 0);
    //RTCZero rtc;
};

#endif