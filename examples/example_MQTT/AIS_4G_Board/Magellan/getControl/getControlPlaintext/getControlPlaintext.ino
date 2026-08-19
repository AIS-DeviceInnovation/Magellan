#include <Arduino.h>
#include <MAGELLAN_MQTT_4G_BOARD.h>

MAGELLAN_MQTT_4G_BOARD magel;

void setup()
{
  Serial.begin(115200);
  magel.begin();
  //{1.} control receives key and value from control
  magel.getControl([](String key, String value) {
    Serial.print("# Control incoming\n# [Key]: ");
    Serial.println(key);
    Serial.print("# [Value]: ");
    Serial.println(value);
    magel.control.ACK(key, value); // ACKNOWLEDGE control to magellan
  });
  //or {2.} control receives only value from a focused key "Lamp1"
  // magel.getControl("Lamp1", [](String value) { // focus only value from key "Lamp1"
  //   Serial.print("# Control incoming focus on [Key] Lamp1: ");
  //   Serial.println(value);
  //   magel.control.ACK("Lamp1", value);
  // });

  // prepare sensor to magellan by reporting control key with initial value
  magel.report.send("Lamp1", "0");
}

void loop()
{
  magel.loop();
  magel.subscribesHandler([]() {
  });
  magel.interval(10, []() { // time interval function inside every 10000 millis

  });
}
