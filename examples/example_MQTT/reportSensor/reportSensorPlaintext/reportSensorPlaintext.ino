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

  magel.getResponse(RESP_REPORT_PLAINTEXT, [](EVENTS events){  // optional for make sure report success CODE = 20000
    Serial.print("[RESP REPORT] code: ");
    Serial.println(events.CODE);// follow status code on https://magellan.ais.co.th/api-document/3/0 {Error code topic}
    Serial.print("# [RESP REPORT] response message: ");
    Serial.println(events.RESP);
  });
}

void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([]()
  {
    magel.subscribe.report.response(PLAINTEXT); // optional register for get Resp report
  });
  magel.interval(10, [](){   
    delay(50);
    String Temperature = String((float)random(24, 34));
    String Humidity = String((int)random(0, 100));
    magel.report.send("Temperature", Temperature); 
    magel.report.send("Humidity", Humidity);
  });
}

