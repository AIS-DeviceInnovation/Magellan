#include <Arduino.h>
#include <MAGELLAN_MQTT_4G_BOARD.h>

MAGELLAN_MQTT_4G_BOARD magel;

void setup()
{
  Serial.begin(115200);
  magel.begin();
  magel.getServerConfig([](String key, String value) { // content type PLAINTEXT
    Serial.print("# Config incoming\n# [Key]: ");
    Serial.println(key);
    Serial.print("# [Value]: ");
    Serial.println(value);
  });
  //or focus on a specific key
  // magel.getServerConfig("DELAY", [](String value) {
  //   Serial.print("# Config incoming focus on [Key] DELAY: ");
  //   Serial.println(value);
  // });
}

void loop()
{
  magel.loop();
  magel.subscribesHandler([]() {
    magel.subscribe.serverConfig(PLAINTEXT); // subscribe server config content type PLAINTEXT
  });
  magel.interval(10, []() { // time interval function inside every 10000 millis
    magel.serverConfig.request(PLAINTEXT); // request server config content type PLAINTEXT
  });
}
