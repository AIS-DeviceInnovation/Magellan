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

  magel.getResponse(RESP_HEARTBEAT_JSON, [](EVENTS events){ // focus only Event RESP_HEARTBEAT_JSON
    Serial.print("# Response incoming focus on [HEARTBEAT] Code: ");
    Serial.println(events.CODE);// follow status code on https://magellan.ais.co.th/api-document/3/0 {Error code topic}  
    Serial.print("# [HEARTBEAT] response message: ");
    Serial.println(events.RESP);
  });
}

void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([](){
    magel.subscribe.heartbeat.response(); // subscribe server config content type JSON
  });
  magel.interval(10,[](){ //time interval function inside every 10 sec

  });
  magel.heartbeat(10); // tringger heartbeat to magellan every 10 sec
}