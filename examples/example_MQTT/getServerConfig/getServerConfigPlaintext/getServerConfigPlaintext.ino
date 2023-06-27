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
  //{1.}
  magel.getServerConfig([](String key, String value){
    Serial.print("# Config incoming\n# [Key]: ");
    Serial.println(key);
    Serial.print("# [Value]: ");
    Serial.println(value);
  });
  //or {2.} focus on key "DELAY"
  // magel.getServerConfig("DELAY", [](String value){ // focus only value form key "Delay"
  //   Serial.print("# Config incoming focus on [Key] Delay: ");
  //   Serial.println(value);
  // });
}

void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([](){
    magel.subscribe.serverConfig(PLAINTEXT); // subscribe server config content type PLAINTEXT
  });
  magel.interval(10,[](){ //time interval function inside every 10 sec
    magel.serverConfig.request("DELAY"); // request server config content type PLAINTEXT
    magel.serverConfig.request("INTERVAL"); // request server config content type PLAINTEXT
    magel.serverConfig.request("SSID"); // request server config content type PLAINTEXT
  });
}