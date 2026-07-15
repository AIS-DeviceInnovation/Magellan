#include <Arduino.h>
#include <MAGELLAN_MQTT_4G_BOARD.h>

MAGELLAN_MQTT_4G_BOARD magel;

int unixTimeMG;

void setup()
{
  Serial.begin(115200);
  magel.begin();

  magel.getResponse(UNIXTIME, [](EVENTS events) { // get unixTime from magellan
    unixTimeMG = events.Payload.toInt();
    Serial.print("[unixTimeMG from magellan]: ");
    Serial.println(unixTimeMG); // use this unixTime value
  });
}

void loop()
{
  magel.loop();
  magel.subscribesHandler([]() {
    magel.subscribe.getServerTime(PLAINTEXT);
  });
  magel.interval(5, []() {
    magel.getServerTime(); // request time from magellan server
  });
}
