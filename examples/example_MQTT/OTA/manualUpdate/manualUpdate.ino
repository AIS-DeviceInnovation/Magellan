#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own function

WiFiClient WiFi_client;
MAGELLAN_MQTT magel(WiFi_client);

String SSID = "set_your_ssid";
String PASS = "set_your_password";
String thingIdentifier = "set_your_thing_identifier";
String thingSecret = "set_your_thing_secret";

int checkStatusUpdate = UNKNOWN;
void setup() 
{
  Serial.begin(115200);
  WiFiSetting.SSID = SSID; //optional wifi connection
  WiFiSetting.PASS = PASS; //optional wifi connection
  connectWiFi(WiFiSetting); //optional wifi connection
  magel.OTA.autoUpdate(false); // this function ENABLED by default unless you set FALSE
  setting.ThingIdentifier = thingIdentifier;
  setting.ThingSecret = thingSecret;
  setting.clientBufferSize = defaultOTABuffer; // set buffer size compatible for OTA
  magel.begin(setting); 

  magel.getServerConfig("autoUpdate", [](String resp){
    if(resp == "1")
    {
      magel.OTA.autoUpdate(true);
    }
    else{
      magel.OTA.autoUpdate(false);
    }
    magel.clientConfig.add("autoUpdateMode", ((magel.OTA.getAutoUpdate())? "ENABLE" : "DISABLE"));
    magel.clientConfig.save(); // update client config from device to thing optional
  });

  magel.getControl([](String key, String value){
    if(key == "executeUpdate")
    {
      magel.control.ACK("executeUpdate", value);
      if(value == "1")
      {
        magel.OTA.executeUpdate(); // executeUpdate OTA
      }
    }
    else // acknowledge other control
    {
      magel.control.ACK(key, value);
    }
  });

  // prepare sensor for add widget control
  magel.report.send("executeUpdate", "0"); 
}

void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([](){
    magel.subscribe.serverConfig(PLAINTEXT);
    magel.subscribe.control(PLAINTEXT);
    checkStatusUpdate = magel.OTA.checkUpdate(); // checkUpdate once time after connect and after reconnect
    // subscribe function here!
  });
  magel.interval(10,[](){ //time interval function inside every 10000 millis
    // doing function something every 10 sec here!
    switch (checkStatusUpdate)
    {
    case UP_TO_DATE:
      Serial.print(F("checkStatusUpdate: "));
      Serial.println("# UP_TO_DATE");
      break;
    case OUT_OF_DATE:
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