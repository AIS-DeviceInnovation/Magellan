#include <Arduino.h>
#include <MAGELLAN_MQTT_4G_BOARD.h>

MAGELLAN_MQTT_4G_BOARD magel;

void setup()
{
  Serial.begin(115200);
  magel.begin();
  magel.getResponse(RESP_HEARTBEAT_JSON, [](EVENTS events) { // focus only Event RESP_HEARTBEAT_JSON
    Serial.print("# Response incoming focus on [HEARTBEAT] Code: ");
    Serial.println(events.CODE); // follow status code on https://magellan.ais.co.th/api-document/3/0 {Error code topic}
    Serial.print("# [HEARTBEAT] response message: ");
    Serial.println(events.RESP);
  });
}

void loop()
{
  magel.loop();
  magel.subscribesHandler([]() {
    magel.subscribe.heartbeat.response(); // subscribe server heartbeat response
  });
  magel.interval(10, []() { // time interval function inside every 10 sec

  });
  magel.heartbeat(10); // trigger heartbeat to magellan every 10 sec
}
