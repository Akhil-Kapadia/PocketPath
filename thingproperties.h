#include "allincludes.h"

const char THING_ID[] = "757347ff-655e-4299-bc92-6f7c2ea1ccb7";

const char SSID[] = SECRET_SSID; // Network SSID (name)
const char PASS[] = SECRET_PASS; // Network password (use for WPA, or use as key for WEP)

extern String lineCSV;
extern float latitude;
extern float longitude;

void initProperties()
{

    ArduinoCloud.setThingId(THING_ID);
    ArduinoCloud.addProperty(lineCSV, READ, ON_CHANGE, NULL);
    ArduinoCloud.addProperty(latitude, READ, ON_CHANGE, NULL);
    ArduinoCloud.addProperty(longitude, READ, ON_CHANGE, NULL);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
