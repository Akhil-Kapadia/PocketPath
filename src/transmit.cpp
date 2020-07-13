

#include "allincludes.h"
#include "thingproperties.h"

 String lineCSV;
 float latitude;
 float longitude;

void cloudConnect()
{
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

}

void updateCloud(File dataFile, String name)
{
    dataFile = SD.open(name, FILE_READ);
    if (dataFile)
    {
        Serial.println(name);
        while (dataFile.available())
        {
            Serial.write(dataFile.read());
            lineCSV = dataFile.read();
            ArduinoCloud.update();
        }
        dataFile.close();
    }
    else
    {
        Serial.println("Error opening file: " + name);
    }
}

//Simple method check if connected to cloud and wifi.
boolean WiFiconnected() { 
    return ArduinoCloud.connected();
}

//Simple method to disconnect from wifi and cloud.
void wifiDisconnect() {
    ArduinoCloud.disconnect();
}

//Simple method to connect to wifi and cloud.
void wifiConnect() {
    ArduinoCloud.connect();
}

//simple method prints wifi/cloud debug information
void cloudDebug() {
    ArduinoCloud.printDebugInfo();
}

void testupdate(){
    ArduinoCloud.update();
}