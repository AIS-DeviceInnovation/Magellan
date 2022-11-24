#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own functionn

WiFiClient WiFi_client;
MAGELLAN_MQTT magel(WiFi_client);

String SSID = "set_your_ssid";
String PASS = "set_your_password";
String thingIdentifier = "set_your_thing_identifier";
String thingSecret = "set_your_thing_secret";

void setup() 
{
  Serial.begin(115200);
  WiFiSetting.SSID = SSID; //optional wifi connection
  WiFiSetting.PASS = PASS; //optional wifi connection
  connectWiFi(WiFiSetting); //optional wifi connection

  setting.ThingIdentifier = thingIdentifier;
  setting.ThingSecret = thingSecret;
  magel.begin(setting); 
  //{1.} json string
  magel.getControlJSON([](String payload){
    Serial.print("# Control incoming JSON: ");
    Serial.println(payload);
    String control = magel.deserializeControl(payload);
    magel.control.ACK(control); //ACKNOWLEDGE control to magellan
  });
  //or {2.} json object
  // magel.getControlJSON([](JsonObject docObject){
  //   String Lamp1 = docObject["Lamp1"]; // buffer value form control key "Lamp1" if not found this key value is "null"
  //   if(Lamp1.indexOf("null") == -1)
  //   {
  //       Serial.print("# Control incoming JSON Object: ");
  //       Serial.print("# [Key] => Lamp1: ");
  //       Serial.println(Lamp1);
  //       magel.sensor.add("Lamp1", Lamp1);
  //       magel.control.ACK(magel.sensor.toJSONString()); //ACKNOWLEDGE control to magellan
  //       magel.sensor.clear();
  //   }
  // });

  // prepare sensor to magellan by report control key with inial value
  magel.report.send("Lamp1","0");
}

void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([](){
    magel.subscribe.control(); // subscribe server control content type JSON
  });
  magel.interval(10,[](){ //time interval function inside every 10000 millis

  });
}