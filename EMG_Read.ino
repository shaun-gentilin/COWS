/* This file sets up the sampling frequency and data collection runtime for the EMG sensor.
At this point the program uses an example filter where it simply squares the unfiltered value.
Actual filter operations and export of data to Bluno beetle will be added in the future */

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else 
#include "WProgram.h"
#endif

#include "EMGFilters.h" 

#define TIMING_DEBUG 1
#define SensorInputPin1 A0 // input pin number for 1st EMG
#define SensorInputPin2 A1 // input pin number for 2nd EMG
#define SensorInputPin3 A2 // input pin number for 3rd EMG

EMGFilters myFilter;
//discrete filters must work with fixed sample frequency
// our emg filter only supports "SAMPLE_FREQ_500HZ" or "SAMPLE_FREQ_1000HZ"
// other sample frequencies will bypass the filter

int sampleRate = SAMPLE_FREQ_1000HZ;

int humFreq = NOTCH_FREQ_50HZ;

//Calibration
// put on sensors, relax your muscles
//wait a few seconds, and select the max value as the threshold
// any value under threshold will be set to 0

static int Threshold = 0;

unsigned long timeStamp;
unsigned long timeBudget;

const unsigned long RUN_TIME = 30000; //run time in milliseconds (30 seconds)
unsigned long startTime; // variable to store the start time of the program


void setup(){
    /*add setup code here*/
    myFilter.init(sampleRate, humFreq, true, true, true);

    //open serial
    Serial.begin(9600);

    //record the start time of the program
    startTime = millis();

    //setup for time cost measure
    //using micros()
    timeBudget = 1e6 / sampleRate;
    //micros will overflow and auto return to zero every 70 minutes
   
}

void loop() {
    /*add main program code here*/
    //In order to make sure the ADC sample frequency on arduino, 
    // the time cost should be measured each loop
    /*------start here------*/
    
    timeStamp = micros();

    //check if the elapsed time is greater than or equal to the run time
    if (millis() - startTime >= RUN_TIME) {
      // if the run time has elapsed, stop the program
      return;
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

/*void draw() {
  if (mySerial.available() > 0 ){
    String envelope1 = mySerial.readString();
    if ( envelope1 !=null ) {
      output.println( envelope1, envelope2, envelope3 );
    }
  }
}

void keyPressed() {
  output.flush(); //Writes the remaining data to the file
  output.close(); // Finishes the file
  exit(); // Stops the program
}*/