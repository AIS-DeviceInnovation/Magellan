#include <Arduino.h>
#include <MAGELLAN_MQTT_4G_BOARD.h>

MAGELLAN_MQTT_4G_BOARD magel;

void setup()
{
  Serial.begin(115200);
  magel.begin();

  magel.clientConfig.add("SSID", "AIS_IoT");       // define key and value
  magel.clientConfig.add("location", "Bangkok");
  magel.clientConfig.add("version", "1.0.0");
  magel.clientConfig.save(); // save client config to magellan (call once per change)
}

void loop()
{
  magel.loop();
  magel.subscribesHandler([]() {
    // subscribe function here!
  });
  magel.interval(10, []() { // time interval function inside every 10000 millis

  });
}
