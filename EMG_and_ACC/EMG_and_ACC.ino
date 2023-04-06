#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else 
#include "WProgram.h"
#endif

#include "EMGFilters.h" 
#include <DFRobot_LIS.h>

//When using I2C communication, use the following program to construct an object by DFRobot_H3LIS200DL_I2C
/*!
 * @brief Constructor 
 * @param pWire I2c controller
 * @param addr  I2C address(0x18/0x19)
 */
//DFRobot_H3LIS200DL_I2C acce(&Wire,0x18);
DFRobot_H3LIS200DL_I2C acce;

#define TIMING_DEBUG 1
#define SensorInputPin1 A0 // input pin number for 1st EMG
#define SensorInputPin2 A1 // input pin number for 2nd EMG
#define SensorInputPin3 A2 // input pin number for 3rd EMG

EMGFilters myFilter;

int sampleRate = 100;
int humFreq = NOTCH_FREQ_50HZ;
static int Threshold = 0;
unsigned long timeStamp;
unsigned long timeBudget;

void setup(){
    myFilter.init(sampleRate, humFreq, true, true, true);
    Serial.begin(9600);
    delay(1000);
}

void loop(){
    int Value1 = analogRead(SensorInputPin1); //unfiltered data for EMG 1
    int Value2 = analogRead(SensorInputPin2); //unfiltered data for EMG 2
    int Value3 = analogRead(SensorInputPin3); //unfiltered data for EMG 3

    //filter processing
    int DataAfterFilter1 = myFilter.update(Value1);
    int DataAfterFilter2 = myFilter.update(Value2);
    int DataAfterFilter3 = myFilter.update(Value3);

    int envelope1 = sq(DataAfterFilter1);
    int envelope2 = sq(DataAfterFilter2);
    int envelope3 = sq(DataAfterFilter3);
    //any value under threshold will be set to zero
    envelope1 = (envelope1 > Threshold) ? envelope1 : 0;
    envelope2 = (envelope2 > Threshold) ? envelope2 : 0;
    envelope3 = (envelope3 > Threshold) ? envelope3 : 0;

    if (TIMING_DEBUG) {
        Serial.print(envelope1);
        Serial.print("\t\t");
        Serial.print(envelope2);
        Serial.print("\t\t");
        Serial.print(envelope3);
    }
    delayMicroseconds(500);

}

/* if get EMG signal, return 1*/
int  getEMGCount(int gforce_envelope)
{
    static long integralData = 0;
    static long integralDataEve = 0;
    static bool remainFlag = false;
    static unsigned long timeMillis = 0;
    static unsigned long timeBeginzero = 0;
    static long fistNum = 0;
    static int TimeStandard = 200;
    /*
    The integral is processed to continuously add the signal value
    and compare the integral value of the previous sampling to determine whether the signal is continuous
    */
   if ((integralDataEve == integralData) && (integralDataEve !=0))
   {
    timeMillis = millis();
    if (remainFlag)
    {
        timeBeginzero = timeMillis;
        remainFlag = false;
        return 0;
    }
    /*If the integral value exceeds 200 ms, the integral value is clear 0, return that get EMG signal*/
    if ((timeMillis - timeBeginzero) >= TimeStandard)
    {
        integralDataEve = integralData = 0;
        return 1;
    }
    return 0;
   }
   else {
    remainFlag = true;
    return 0;
   }
}

