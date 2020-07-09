

#include "thingproperties.h"
#include <SD.h>
#include <SPI.h>

String line;

void cloudConnect()
{
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
