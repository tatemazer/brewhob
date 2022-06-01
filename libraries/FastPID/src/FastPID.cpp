#include "FastPID.h"

#include <Arduino.h>

FastPID::~FastPID() {
}

void FastPID::clear() { 
  _sum = 0; 
  _last_err = 0;
} 

void FastPID::setCoefficients(float kp, float ki, float kd, float hz) {
  _p = kp;
  _i = ki / hz;
  _d = kd * hz;
}

void FastPID::setOutputRange(int16_t min, int16_t max)
{
  _outmin = min;
  _outmax = max;
}

int16_t FastPID::step(float sp, float fb) {

  // int16 + int16 = int17
  float err = sp - fb;
  int32_t P = 0, I = 0;
  int32_t D = 0;

  if (_p) {
    // uint16 * int16 = int32
    P = _p * err;
  }

  //discourage wind-up
  if (_i) {
    // int17 * int16 = int33
    _sum += err * _i;

    // Limit sum to 32-bit signed value so that it saturates, never overflows.
    if (_sum > INTEG_MAX)
      _sum = INTEG_MAX;
    else if (_sum < INTEG_MIN)
      _sum = INTEG_MIN;

    // int32
    I = _sum;
  }

  if (_d) {
    // (int17 - int16) - (int16 - int16) = int19
    int32_t deriv = (err - _last_err);
    _last_err = err; 

    // Limit the derivative to 16-bit signed value.
    if (deriv > DERIV_MAX)
      deriv = DERIV_MAX;
    else if (deriv < DERIV_MIN)
      deriv = DERIV_MIN;

    // int16 * int16 = int32
    D = _d* deriv;
  }

  // int32 (P) + int32 (I) + int32 (D) = int34
  int64_t out = int64_t(P) + int64_t(I) + int64_t(D);

  
 /* Serial.print(" P:");
  Serial.print(P);
  Serial.print(" I:");
  Serial.print(I);
  Serial.print(" D:");
  Serial.print(D);
*/

  // Make the output saturate
  if (out > _outmax) 
    out = _outmax;
  else if (out < _outmin) 
    out = _outmin;

  return out;
}
