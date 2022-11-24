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
  magel.getServerConfigJSON([](String payload){
    Serial.print("# Config incoming JSON: ");
    Serial.println(payload);
  });
  //or {2.} json object
  // magel.getServerConfigJSON([](JsonObject docObject){
  //   String buffer = docObject["DELAY"]; // buffer value form config key "DELAY" if not found this key value is "null"
  //   if(buffer.indexOf("null") == -1)
  //   {
  //       Serial.print("# Config incoming JSON Object: ");
  //       Serial.print("# [Key] => Delay: ");
  //       Serial.println(buffer);
  //   }
  // });
}


void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([](){
    magel.subscribe.serverConfig(); // subscribe server config content type JSON
  });
  magel.interval(10,[](){ //time interval function inside every 10000 millis
    magel.serverConfig.request(); // request server config content type JSON
  });
}