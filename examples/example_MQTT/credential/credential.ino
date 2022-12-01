#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own functionn
#define REGEN_BY_CONDITION false
WiFiClient WiFi_client;
MAGELLAN_MQTT magel(WiFi_client);

String SSID = "set_your_ssid";
String PASS = "set_your_password";

int checkStatusUpdate = UNKNOWN;
void setup() 
{
  Serial.begin(115200);
  WiFiSetting.SSID = SSID; //optional wifi connection
  WiFiSetting.PASS = PASS; //optional wifi connection
  connectWiFi(WiFiSetting); //optional wifi connection
  magel.begin(setting); 
  
  Serial.println("# Read Credential 1st time");
  Serial.println("ThingIdentifier: "+ magel.credential.getThingIdentifier());
  Serial.println("ThingSecret: "+ magel.credential.getThingSecret());
  Serial.println("# Regenerate Credential");
  magel.credential.regenerate(REGEN_BY_CONDITION); // if set true == will regenerate only credential activated, false regenerate without condition
  Serial.println("# Read Credential 2nd time (After Regenerate)");
  Serial.println("ThingIdentifier: "+ magel.credential.getThingIdentifier());
  Serial.println("ThingSecret: "+ magel.credential.getThingSecret());
  Serial.println("PreviousThingIdentifier: "+ magel.credential.getPreviousThingIdentifier());
  Serial.println("PreviousThingSecret: "+ magel.credential.getPreviousThingSecret());
  Serial.println("# Recovery Credential(swarp credential old to current)");
  magel.credential.recovery();
  Serial.println("ThingIdentifier: "+ magel.credential.getThingIdentifier());
  Serial.println("ThingSecret: "+ magel.credential.getThingSecret());
  Serial.println("PreviousThingIdentifier: "+ magel.credential.getPreviousThingIdentifier());
  Serial.println("PreviousThingSecret: "+ magel.credential.getPreviousThingSecret());
}

void loop() 
{
    magel.loop();
}