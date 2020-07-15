
#include "allincludes.h"
#include "thingproperties.h"

//States
#define SLEEPING 0
#define TRACKING 1
#define TRANSMITTING 2
//LEDs
#define LED_TRACKING 3
#define LED_SLEEPING 4
#define LED_TRANSMITTING 5
//State pins
#define BMS_0 0
#define BMS_1 1
//Chip Select for SD card
#define CS 7

//Variables to be sent to cloud
String lineCSV = "testing : ";
float latitude;
float longitude;
//Local variables
int state;        //3 states, sleeping, tracking, transmitting.
boolean SLEEPING; //If pause button was pressed.

void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  while (!Serial)
    ;

  Serial.println("Begining Cloud Connection.");
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  Serial.println("Intializing cloud propeties.");
  // Defined in thingProperties.h
  initProperties();

  // The following function allows you to obtain more information
  // related to the state of network and IoT Cloud connection and errors
  // the higher number the more granular information you’ll get.
  // The default is 0 (only errors).
  // Maximum is 4
  Serial.println("Printing Cloud debug info:");
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  //Initalize GPS
  if (!GPS.begin(GPS_MODE_SHIELD))
    Serial.println("Failed to initialize GPS!");

  //Interrupt subroutine for sleep/gps timer.
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, GPSinterrupt, CHANGE);

  //Configure pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_TRACKING, OUTPUT);
  pinMode(LED_SLEEPING, OUTPUT);
  pinMode(LED_TRANSMITTING, OUTPUT);
}

void loop()
{
  File sensorData;
  String fileName;
  //Change states here according to buttons or whatever.

  switch (state)
  {
  case SLEEPING:
    Serial.println("In Sleep mode...");
    sleepRoutine();
    //Set LEDS.
    analogWrite(LED_SLEEPING, 128); //Use PWM to save current consumption.
    digitalWrite(LED_TRACKING, LOW);
    digitalWrite(LED_TRANSMITTING, LOW);
    break;

  case TRACKING: //Tracking state. References Tracking.cpp
    //Attemps to retrieve location data. Timeout in 5 seconds.
    if (GPS.available())
    {
      aquireGPSdata()
          state = SLEEPING;
    }

    break;

  case TRANSMITTING:
    Serial.println("Begin Transmit state.");

    lineCSV = +"x";
    ArduinoCloud.update();
    delay(1000);
    state = SLEEPING;
    break;

  default:
    state = SLEEPING; //Put everything into sleep mode.
    break;
  }
}

/** SLEEPING state CODE SECTION
 * 
 * DEPENDENCIES : ArduinoLowPower.h, Arduino_MKRGPS, ArduinoCloudIoT.h, WiFiNINA.h
 * 
 * This section of code facilitaces the sleeping state of PocketPath. Included 
 * in this section are the interrupts and power saving routines needed to optimize power consumption.  
 * A 30 sec interrupt based on the RTC will change the "state" variable into TRACKING.
 * If the pause button is pressed, the state will change to SLEEPING and the interrupt routine will be disabled.
 */

//Interrupt service routine triggered every 30s. Changes the state.
void GPSinterrupt()
{
  state = (SLEEPING) ? SLEEPING : TRACKING;
}

//Sleep routine puts all devices into a low power state.
void sleepRoutine()
{
  //put things into low power
  LowPower.sleep(30000);
  GPS.standby();
  ArduinoCloud.disconnect();
  WiFi.lowPowerMode();
}

/** TRACKING STATE CODE SECTION
 * 
 * DEPENDENCIES : Arduino_MKRGPS.h, SD.h, SPI.h
 * 
 * This section of code is responsible for the tracking portion of PocketPath. Here
 * the SAM-M8Q (GPS chip) will aquire a fix and retrieve location data such as latitude
 * longitude and time. These will then be stored in CSV format and written into a SD card.
 * 
 * The name of the CSV file should be the date stamp of the current session. 
 */

//As soon as the GPS is available returns a string of GPS location data.
String aquireGPSdata()
{
  // read GPS values
  float latitude = GPS.latitude();
  float longitude = GPS.longitude();
  float altitude = GPS.altitude();
  int satellites = GPS.satellites();

  // print GPS values
  Serial.println();
  Serial.print("Location: ");
  Serial.print(latitude, 7);
  Serial.print(", ");
  Serial.println(longitude, 7);

  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println("m");

  Serial.print("Number of satellites: ");
  Serial.println(satellites);

  Serial.println();

  
  return "";
}