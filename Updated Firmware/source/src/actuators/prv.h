#ifndef MY_SERVO_LIB_H__
#define  MY_SERVO_LIB_H__

#include <Servo.h>
#include <Arduino.h>

class PrvServoLib
{
public:

  PrvServoLib()
    : _servoPin(9)
    , _attached(false)
  {}

  void attach()
  {
    if (_attached)
      return;      

    _attached = true;
    _servo.attach(_servoPin);
  }

  void detach()
  {
      if (! _attached)
          return;

      _attached = false;
      _servo.detach();
  }    

  void setPin(int pin){
    _servoPin = pin;
  }


private:
  int _servoPin;
  bool _attached;
  Servo _servo;

};


#endif