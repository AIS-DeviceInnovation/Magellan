#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own function

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

  magel.clientConfig.add("Device_number", 142);
  magel.clientConfig.add("Location", "19.346641, 52.245513");
  magel.clientConfig.add("Installation_date", "2022-07-01");
  magel.clientConfig.add("Interval_setting", "10sec");
  magel.clientConfig.save();
}

void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
}