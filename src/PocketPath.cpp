/* 
  Sketch generated by the Arduino IoT Cloud Thing "PocketPath"
  https://create.arduino.cc/cloud/things/d1d45b98-b89e-46fe-b072-492ce1c64c67 

  Arduino IoT Cloud Properties description

  The following variables are automatically generated and updated when changes are made to the Thing properties

  float latitude;
  float longitude;
  int date;
  int time;

  Properties which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "transmit.h"

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();
}

void loop() {
  int State;
  // Your code here 
}

void tracking() {

}

void sleeping() {

}

void transmit() {

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();


}


void onDateChange() {
  // Do something
}





