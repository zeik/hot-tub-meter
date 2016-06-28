// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature/spark-dallas-temperature.h"


/*
This software is used to measure temperature from my hot tub, and send result to cloud.
I use my Android app to monitor heating status of hot tub.

I/O setup:
I have 2 DS18B20 connected to my device (top and bottom temperature )

1-wire GND connected to GND
1-wire power, Connected to 3V3 (active power mode).
D2 - 1-wire signal, 2K-10K resistor to D5 (3v3)

*/

// Only include One of the following depending on your environment!
#include "OneWire/OneWire.h"  // Use this include for the Web IDE:
// #include "OneWire.h" // Use this include for Particle Dev where everything is in one directory.


// Comment this out for normal operation
//SYSTEM_MODE(SEMI_AUTOMATIC);  // skip connecting to the cloud for (Electron) testing

unsigned long lastUpdate = 0;
unsigned int Metric_Publish_Rate = 60000;
unsigned int MetricnextPublishTime;
int DS18B20nextSampleTime;
int DS18B20_SAMPLE_INTERVAL = 60000;
float celsius[3];
char szInfo[64];


DallasTemperature dallas(new OneWire(D2));

void setup() {
  Time.zone(+3);
  Particle.syncTime();
  celsius[0] = -1;
  celsius[1] = -1;
  celsius[2] = -1;
  celsius[3] = -1;
  Serial.begin(115200);
  dallas.begin();
}

void loop() {
  delay (5000);
    if (dallas.isParasitePowerMode()) {
      Serial.println("parasitic power\n");
      Serial.printf("device count: %u \n",dallas.getDeviceCount());
  }
  if (millis() > DS18B20nextSampleTime){
    //Serial.println("reading temps\n");
    getTemps();
    }

  if (millis() > MetricnextPublishTime){
    Serial.println("Publishing now.");
    publishData();
  }
}


void publishData() {
      unsigned int sensor;
      sprintf(szInfo, "%2.1f;%2.1f", celsius[0],celsius[1]);
      Serial.printf(szInfo);
      Particle.publish("dsTmps", szInfo, PRIVATE);
      MetricnextPublishTime = millis() + Metric_Publish_Rate;
}

void getTemps() {
    dallas.requestTemperatures();
    celsius[0] = dallas.getTempCByIndex( 0 );
    delay (1000);
    celsius[1] = dallas.getTempCByIndex( 1 );
    
    // retry if there are read fails
    for (int i=0; (i<10 && ( celsius[0] == -127 || celsius[1] == -127) );i++ ) {
        delay (1000);
        celsius[0] = dallas.getTempCByIndex( 0 );
        delay (1000);
        celsius[1] = dallas.getTempCByIndex( 1 );
    }
    

    Serial.printf("Temperatures: %2.1f, %2.1ff\n",celsius[0], celsius[1]); 
}
