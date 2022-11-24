#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own functionn

WiFiClient WiFi_client;
MAGELLAN_MQTT magel(WiFi_client);

String SSID = "set_your_ssid";
String PASS = "set_your_password";
String thingIdentifier = "set_your_thing_identifier";
String thingSecret = "set_your_thing_secret";

int unixTimeMG;
int timeZone = 7; //GMT +7
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
    Serial.print("[unixTimeMG from magellan GMT+0]: ");
    Serial.println(unixTimeMG);
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
    if(unixTimeMG > 0) //waiting until get Timestamp 
    {
      Serial.println("# Datetime now: "+ magel.utils.toDateTimeString(unixTimeMG, timeZone));
      Serial.println("# UniversalTime: "+ magel.utils.toUniversalTime(unixTimeMG, timeZone));

      tm DateTime = magel.utils.convertUnix(unixTimeMG, timeZone); //convert unix to struct tm
      Serial.println("Day: "+String(DateTime.tm_mday));
      Serial.println("Month: "+String(DateTime.tm_mon));
      Serial.println("Year: "+String(DateTime.tm_year + 1900));
      Serial.println("hour: "+String(DateTime.tm_hour));
      Serial.println("minute: "+String(DateTime.tm_min));
      Serial.println("second: "+String(DateTime.tm_sec));

      Serial.println("[Unix from convert GMT: "+String(timeZone)+"]: "+ String(magel.utils.toUnix(DateTime)));
      Serial.println("====================================================");
    }
  });
}
