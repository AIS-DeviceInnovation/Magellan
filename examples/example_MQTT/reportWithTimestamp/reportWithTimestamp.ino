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
    Serial.println(unixTimeMG);
    });

    magel.getResponse(RESP_REPORT_TIMESTAMP, [](EVENTS events){  // optional for make sure report with timestamp success CODE = 20000
    Serial.print("[ReportWithTimestamp success code]: ");
    Serial.println(events.CODE);// follow status code on https://magellan.ais.co.th/api-document/3/0 {Error code topic}
    });
}


void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([]()
  {
    magel.subscribe.getServerTime(PLAINTEXT);
    magel.subscribe.reportWithTimestamp.response();
  });
  magel.interval(5, [](){  
    magel.getServerTime(); // request time from magellan server
    if(unixTimeMG > 0) //if get Timestamp from magellan send data
    {
      magel.sensor.add("Temperature", (float)random(24, 34)); 
      magel.sensor.add("Humidity", (int)random(0, 100)); 
      String payload = magel.sensor.toJSONString(); // build sensor value to json string
      //report send data sensor to magellan with timestamp.
      magel.report.send(unixTimeMG, payload);
      magel.sensor.clear(); unixTimeMG = 0; // clear buffer sensor and reset unixTimeMG
    }
  });
}
