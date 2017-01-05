#include "rgbTools.h"



int REDPIN, GREENPIN, BLUEPIN;

/* maximum brightness for each color, use to
modify final colours depending on LED choice*/
double _brightness = 1; //the overall brightness target 0(off) --> 1(max brightness)

//globals
rgb _currentRGB;
int _lastUpdate = 0;
int _iterationDelay = 10; //ms between step colour changes
int _fadeSteps = 0;

/* RGB Fading variables*/
bool _fadingRGB = false;
rgb _initialColour;
double _rgbFadeAmounts[3];

/* Kelvin Fading variables*/
bool _fadingKelvin = false;
int _currentKelvin = 0;
double _kelvinFadeAmount;

void setupRGB (int r_pin, int g_pin, int b_pin, double initialBrightness) {

  REDPIN = r_pin;
  GREENPIN = g_pin;
  BLUEPIN = b_pin;
  _brightness = initialBrightness;

  analogWriteRange(255); //use 0 --> 255 as pwm range
  analogWriteFreq(300); //set PWM freq. 
  delay(50);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);

}
void fadeRGB(rgb fadeRGB, int timespan) {
  timespan = max(timespan,_iterationDelay);
  _fadingRGB = true;
  _fadeSteps = timespan/_iterationDelay;
  _rgbFadeAmounts[0] =  (_currentRGB.r - fadeRGB.r) / _fadeSteps;
  _rgbFadeAmounts[1] = (_currentRGB.g - fadeRGB.g) / _fadeSteps;
  _rgbFadeAmounts[2] =  (_currentRGB.b - fadeRGB.b) / _fadeSteps;
}

void rgbLoop()
{
  unsigned long current_millis = millis();
  if (_fadingRGB || _fadingKelvin) {
    // Enough time since the last step ?
    if (current_millis - _lastUpdate >= _iterationDelay) {
      if (_fadeSteps > 0) {
        //still fading steps left
        if(_fadingRGB){
          _currentRGB.r = _currentRGB.r - _rgbFadeAmounts[0];
          _currentRGB.g = _currentRGB.g - _rgbFadeAmounts[1];
          _currentRGB.b = _currentRGB.b - _rgbFadeAmounts[2];
        }
        if(_fadingKelvin){
          _currentKelvin = _currentKelvin-_kelvinFadeAmount;
          _currentRGB = (kelvinToRGB(_currentKelvin));
        }
        _fadeSteps--;
      } else {
        _fadingRGB = false;
        _fadingKelvin = false;
      }
      _lastUpdate = current_millis;
    }
  }
  //always set colours
  analogWrite(REDPIN, _currentRGB.r * _brightness);
  analogWrite(GREENPIN, _currentRGB.g * _brightness);
  analogWrite(BLUEPIN, _currentRGB.b * _brightness);
}

void fadeKelvin(int toKelvin, int timespan)
{
  timespan = max(timespan,_iterationDelay); //limit smallest timespan to be iteration delay
  if(_currentKelvin != 0)
  {
    //fade kelvin
    _fadingKelvin = true;
    _fadeSteps = timespan/_iterationDelay; //number of iterations between moves (30ms delay
    _kelvinFadeAmount =  (_currentKelvin - toKelvin) / _fadeSteps;
  } else {
    //fade RGB
    fadeRGB(kelvinToRGB(toKelvin),timespan);
  }
}

void setRGB(rgb setRGB)
{
   //will reset kelvin value as well, so that proper fading can occur
   _fadingRGB = false;
   _fadingKelvin = false;
   _currentKelvin = 0;

   setRGB.r = constrain(setRGB.r, 0, 255);
   setRGB.g = constrain(setRGB.g, 0, 255);
   setRGB.b = constrain(setRGB.b, 0, 255);
   _currentRGB = setRGB; //store latest values for fading purposes

}

void setBrightness(double brightness)
{
  //change the overall brightness of the lights
  _brightness = brightness;
}

void off()
{
  _currentRGB.r = 0;
  _currentRGB.g = 0;
  _currentRGB.b = 0;
}

void setKelvin(int kelvin)
{
    _currentKelvin = kelvin;
    setRGB(kelvinToRGB(kelvin));
}