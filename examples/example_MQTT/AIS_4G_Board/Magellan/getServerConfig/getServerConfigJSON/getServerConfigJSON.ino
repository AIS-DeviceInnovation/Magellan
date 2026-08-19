#include <Arduino.h>
#include <MAGELLAN_MQTT_4G_BOARD.h>

MAGELLAN_MQTT_4G_BOARD magel;

void setup()
{
  Serial.begin(115200);
  magel.begin();
  //{1.} json string
  magel.getServerConfigJSON([](String payload) {
    Serial.print("# Config incoming JSON: ");
    Serial.println(payload);
  });
  //or {2.} json object
  // magel.getServerConfigJSON([](JsonObject docObject) {
  //   String buffer = docObject["DELAY"]; // "null" if key not found
  //   if (buffer.indexOf("null") == -1)
  //   {
  //     Serial.print("# Config incoming JSON Object: ");
  //     Serial.print("# [Key] => Delay: ");
  //     Serial.println(buffer);
  //   }
  // });
}

void loop()
{
  magel.loop();
  magel.subscribesHandler([]() {
  });
  magel.interval(10, []() { // time interval function inside every 10000 millis
    magel.serverConfig.request(); // request server config content type JSON
  });
}
