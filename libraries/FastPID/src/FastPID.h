#ifndef FastPID_H
#define FastPID_H

#include <stdint.h>

#define INTEG_MAX    2000
#define INTEG_MIN    (INT32_MIN)
#define DERIV_MAX    2000
#define DERIV_MIN    (INT16_MIN)



/*
  A fixed point PID controller with a 32-bit internal calculation pipeline.
*/
class FastPID {

public:
  FastPID() 
  {
    clear();
  }

  FastPID(float kp, float ki, float kd, float hz)
  {
    setCoefficients(kp,ki,kd,hz);
  }

  ~FastPID();

  void setCoefficients(float kp, float ki, float kd, float hz);
  void setOutputRange(int16_t min, int16_t max);
  void clear();
  int16_t step(float sp, float fb);

private:

  // Configuration
  float _p, _i, _d;
  int16_t _outmax, _outmin; 
  
  // State
  float _sum;
  float _last_err;
};

#endif
