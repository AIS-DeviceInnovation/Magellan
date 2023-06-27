#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own function

WiFiClient WiFi_client;
MAGELLAN_MQTT magel(WiFi_client);

String SSID = "set_your_ssid";
String PASS = "set_your_password";
String thingIdentifier = "set_your_thing_identifier";
String thingSecret = "set_your_thing_secret";

int unixTimeMG;

void setup()
{
  Serial.begin(115200);
  WiFiSetting.SSID = SSID; //optional wifi connection
  WiFiSetting.PASS = PASS; //optional wifi connection
  connectWiFi(WiFiSetting); //optional wifi connection

  setting.ThingIdentifier = thingIdentifier;
  setting.ThingSecret = thingSecret;
  magel.begin(setting); 
    
  magel.getResponse(UNIXTIME, [](EVENTS events){  // for get unixTime from magellan
    unixTimeMG = events.Payload.toInt();
    Serial.print("[unixTimeMG from magellan]: ");
    Serial.println(unixTimeMG);// this unixTime to use!
  });
}


void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([]()
  {
    magel.subscribe.getServerTime(PLAINTEXT);
  });
  magel.interval(5, [](){  
    magel.getServerTime(); // request time from magellan server
  });
}
