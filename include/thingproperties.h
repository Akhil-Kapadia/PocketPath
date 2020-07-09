#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <arduino_secrets.h>

const char THING_ID[] = "d1d45b98-b89e-46fe-b072-492ce1c64c67";

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)


float latitude;
float longitude;
int date;
int time;

void initProperties(){

  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(latitude, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(longitude, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(date, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(time, READ, ON_CHANGE, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
