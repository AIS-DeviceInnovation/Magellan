#include <Arduino.h>
#include <MAGELLAN_MQTT_4G_BOARD.h>

MAGELLAN_MQTT_4G_BOARD magel;

void setup()
{
  Serial.begin(115200);
  magel.begin();
  //{1.} json string
  magel.getControlJSON([](String payload) {
    Serial.print("# Control incoming JSON: ");
    Serial.println(payload);
    String control = magel.deserializeControl(payload);
    magel.control.ACK(control); // ACKNOWLEDGE control to magellan
  });
  //or {2.} json object
  // magel.getControlJSON([](JsonObject docObject) {
  //   String Lamp1 = docObject["Lamp1"]; // buffer value from control key "Lamp1"; "null" if not found
  //   if (Lamp1.indexOf("null") == -1)
  //   {
  //     Serial.print("# Control incoming JSON Object: ");
  //     Serial.print("# [Key] => Lamp1: ");
  //     Serial.println(Lamp1);
  //     magel.sensor.add("Lamp1", Lamp1);
  //     magel.control.ACK(magel.sensor.toJSONString());
  //     magel.sensor.clear();
  //   }
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
