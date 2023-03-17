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

int sampleRate = SAMPLE_FREQ_1000HZ;
int humFreq = NOTCH_FREQ_50HZ;
static int Threshold = 0;
unsigned long timeStamp;
unsigned long timeBudget;

const unsigned long RUN_TIME = 30000; //run time in milliseconds (30 seconds)
unsigned long startTime; // variable to store the start time of the program

void setup(){
    myFilter.init(sampleRate, humFreq, true, true, true);
    Serial.begin(9600);
    startTime = millis();
    timeBudget = 1e6 / sampleRate;

    //Chip initialization for accelerometer
    while(!acce.begin()){
      Serial.println("Initialization failed, please check the connection and I2C address settings");
      delay(1000);
    }
    //Get chip id
    Serial.print("chip id : ");
    Serial.println(acce.getID(),HEX);

    acce.setRange(/*Range = */DFRobot_LIS::eH3lis200dl_100g);

    acce.setAcquireRate(/*Rate = */DFRobot_LIS::eNormal_50HZ);
    delay(1000);
}

void loop(){
    long ax,ay,az;
    ax = acce.readAccX();//Get the acceleration in the x direction
    ay = acce.readAccY();//Get the acceleration in the y direction
    az = acce.readAccZ();//Get the acceleration in the z direction

    timeStamp = micros();
    if (millis() - startTime >= RUN_TIME) {
        //return;
        /*Having a set run time for data collection on the board requires the board to be plugged into power
        at a specific time after pairing.  I think it makes more sense to have the board continuously spit data
        and the app will take its sample when it is ready.*/
    }
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

    timeStamp = micros() - timeStamp;
    if (TIMING_DEBUG) {
        Serial.print(envelope1);
        Serial.print("\t\t");
        Serial.print(envelope2);
        Serial.print("\t\t");
        Serial.print(envelope3);
        Serial.print("\t\t");
        Serial.print(ax);
        Serial.print("\t\t");
        Serial.print(ay);
        Serial.print("\t\t");
        Serial.print(az);
        Serial.println();
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

