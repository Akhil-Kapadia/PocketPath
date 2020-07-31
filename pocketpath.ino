#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino_MKRGPS.h>
#include <ArduinoLowPower.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

#define SLEEPING 0
#define TRACKING 1
#define TRANSMITTING 2
//LEDs
#define LED_TRACKING 2
#define LED_SLEEPING 3
#define LED_TRANSMITTING 4
//State pins
#define BMS_0 0
#define BMS_1 1
//Chip Select for SD card
#define CS 7

//Max number of files in SD card
const int LIMIT = 100;

//GPS variables
float latitude;
float longitude;
//Local variables
int state = 0;          //3 states, sleeping, tracking, transmitting.
boolean paused = false; //If pause button was pressed.
String name;

//Objs
File sensorData;
WiFiServer server(80);

//function declartions
void interruptGPS();
void interruptPAUSED();
void interruptTRANSMIT();
void sleepRoutine();
String aquireGPSdata();
void saveData(String data);
void wifiSetup();
void printWifiStatus();
void webpage();

void setup()
{
  char fileName[13];

  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  //Initalize GPS
  if (!GPS.begin(GPS_MODE_SHIELD))
    Serial.println("Failed to initialize GPS!");

  //initalize SD card
  if (!SD.begin(CS))
  {
    pinMode(CS, OUTPUT);
    Serial.println("initialization failed!");
    return;
  }

  //create new CSV file numerical named per session.
  //create new file
  for (int n = 0; n < LIMIT; n++)
  {
    sprintf(fileName, "DLOG%.3d.CSV", n);
    if (SD.exists(fileName))
      continue;
    sensorData = SD.open(fileName, FILE_WRITE);
    Serial.println("File: " + String(fileName));
    break;
  }
  sensorData.close();

  //Interrupt subroutines.
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, interruptGPS, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BMS_0), interruptPAUSED, RISING);
  attachInterrupt(digitalPinToInterrupt(BMS_1), interruptTRANSMIT, RISING);

  //Configure pins
  pinMode(LED_TRACKING, OUTPUT);
  pinMode(LED_SLEEPING, OUTPUT);
  pinMode(LED_TRANSMITTING, OUTPUT);

  void loop()
  {
    //Change states here according to buttons or whatever.

    switch (state)
    {
    case SLEEPING:
    }
    Serial.println("In Sleep mode...");
    //Set LEDS.
    analogWrite(LED_SLEEPING, 128); //Use PWM to save current consumption.
    digitalWrite(LED_TRACKING, LOW);
    digitalWrite(LED_TRANSMITTING, LOW);
    //put things into low power and starts 30s sleep timer.
    sleepRoutine();
    break;

  case TRACKING:
    Serial.println("In tracking state ...\n Attempting to retrieve GPS locational data...");
    GPS.wakeup();
    //Attemps to retrieve location data. Timeout in 5 seconds.
    //Try to get gps data.
    if (GPS.available())
    {
      saveData(aquireGPSdata());
      state = SLEEPING;
    }
    break;

  case TRANSMITTING:
    Serial.println("In transmit state.");
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
void interruptGPS()
{
  state = (paused) ? SLEEPING : TRACKING;
}

//Interrupt service routine if paused button is pressed.
void interruptPAUSED()
{
  paused = true;
}

//Interrupts service routing is transmit button is pressed. Changes state.
void interruptTRANSMIT()
{
  state = TRANSMITTING;
}

//Sleep routine puts all devices into a low power state.
void sleepRoutine()
{
  //put things into low power
  GPS.standby();
  WiFi.lowPowerMode();
  WiFi.end();
  Serial.println("Peripheals put to sleep");
  LowPower.sleep(30000);
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

  // print GPS values
  Serial.print("Location: ");
  Serial.print(latitude, 7);
  Serial.print(", ");
  Serial.println(longitude, 7);
  Serial.println();

  return String(latitude, 6) + "," + String(longitude, 6); // convert to CSV
}

//Writes data to SD card
void saveData(String data)
{
  sensorData = SD.open(name + ".csv", FILE_WRITE);
  if (sensorData)
  {
    Serial.print("Writing data to sd card..");
    sensorData.println(data);
    sensorData.close(); // close the file
    Serial.println("done.");
  }
  else
  {
    data = "";
    Serial.println("Error writing to file !");
  }
}

/**  TRANSMITTING STATE CODE SECTION
 * 
 * DEPENDENCIES : WiFiNINA, SD, SPI.
 * 
 * This section of code handles the wireless communication of data over wifi. 
 * A simple web server will display the GPS data information from CSV files stored in the SD card, 
 * and can display past sessions by interactin with the webpage created by the arduino.
 */

//Initizes connection to wifi network.
void wifiSetup()
{
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  //Connected now, so print out the status.
  printWifiStatus();
}

//prints wifi connection status.
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


//Begin displaying webpage to client connected to webserver.
void webpage()
{
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close"); // the connection will be closed after completion of the response
          client.println("Refresh: 5");        // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++)
          {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
}