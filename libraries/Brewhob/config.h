#ifndef CONFIG_H
#define CONFIG_H


#define FLOW_PIN  2
#define SW1_PIN   3
#define SW2_PIN   9
#define SW3_PIN   10
#define PUMP_PIN  6
#define SOL1_PIN  A3 //Fill
#define SOL2_PIN  A2 //Brew
#define SOL3_PIN  A1 //Tea
#define FILL_IN_PIN A6
#define HEAT1_PIN 5
#define HEAT2_PIN 4
#define RTD1_PIN  8
#define RTD2_PIN  7
#define POT_PIN     A4
#define POT_POW_PIN A5



#define BREW_VALVE_EN 1
#define TEA_VALVE_EN 1
#define BREW_BOILER_EN 1
#define STEAM_BOILER_EN 1
#define BREW_CNTRL_ANALOG 0 
#define AC_PUMP 1

#define WIFI_ENABLED 1
#define TIMEZONE_OFFSET -8
#define TIME_ON_HOUR 8
#define TIME_ON_MINUTE 0

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      4300.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  1000.0

#define FILL_DELAY 1
#define FILL_TALLY_LIM 3 //number of consecutive "low water" reads to cause a fill event 
#define SHOT_SIZE 350
#define CC_PER_PULSE 0.20

#define Kp 200 //14.4
#define Ki 1 //0.27
#define Kd 60 //18
#define SETPOINT1 212
#define SETPOINT2 260

#define DOLLARS_PER_KWH 0.1559
#define BREW_BOILER_WATTAGE_KW 0.5
#define STEAM_BOILER_WATTAGE_KW 1

#endif