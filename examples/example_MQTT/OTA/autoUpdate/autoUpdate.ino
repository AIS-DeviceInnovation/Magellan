#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own function

WiFiClient WiFi_client;
MAGELLAN_MQTT magel(WiFi_client);

String SSID = "set_your_ssid";
String PASS = "set_your_password";
String thingIdentifier = "set_your_thing_identifier";
String thingSecret = "set_your_thing_secret";

OTA_state checkStatusUpdate = OTA_state::UNKNOWN_STATE;
void setup() 
{
  Serial.begin(115200);
  WiFiSetting.SSID = SSID; //optional wifi connection
  WiFiSetting.PASS = PASS; //optional wifi connection
  connectWiFi(WiFiSetting); //optional wifi connection
  // magel.OTA.autoUpdate(/*args boolean*/); // this function ENABLED by default unless you set FALSE
  setting.ThingIdentifier = thingIdentifier;
  setting.ThingSecret = thingSecret;
  setting.clientBufferSize = defaultOTABuffer; // set buffer size compatible for OTA
  magel.begin(setting); 
}

void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([](){
    checkStatusUpdate = magel.OTA.checkUpdate();
    // subscribe function here!
  });
  magel.interval(10,[](){ //time interval function inside every 10000 millis
    // doing function something every 10 sec here!
    switch (checkStatusUpdate)
    {
    case OTA_state::UP_TO_DATE:
      Serial.print(F("checkStatusUpdate: "));
      Serial.println("# UP_TO_DATE");
      break;
    case OTA_state::OUT_OF_DATE:
      Serial.print(F("checkStatusUpdate: "));
      Serial.println(F("# OUT_OF_DATE"));
      break;   
    default:
      Serial.print(F("checkStatusUpdate: "));
      Serial.println("# UNKNOWN");
      break;
    }
  });
}