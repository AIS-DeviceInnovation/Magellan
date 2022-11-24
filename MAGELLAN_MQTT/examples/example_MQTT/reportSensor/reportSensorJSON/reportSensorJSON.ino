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

  magel.getResponse(RESP_REPORT_JSON, [](EVENTS events){  // optional for make sure report success CODE = 20000
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
    magel.subscribe.report.response(); // optional register for get Resp report
  });
  magel.interval(10, [](){   
    //{1} 
    delay(50);
    String Temperature = String((float)random(24, 34));
    String Humidity = String((int)random(0, 100));
    magel.sensor.add("Temperature", Temperature.toFloat()); 
    magel.sensor.add("Humidity", Humidity.toInt());
    magel.sensor.report(); //send data sensor with auto json build
    //or {2}
    delay(50);
    magel.report.send("{\"Temperature_manual\":"+Temperature+",\"Humidity_manual\":"+Humidity+"}"); //send data sensor with manual json format
    //or {3}
    delay(50);
    magel.sensor.add("Temperature_buff", Temperature.toFloat()); 
    magel.sensor.add("Humidity_buff", Humidity.toInt());
    String buffer_data = magel.sensor.toJSONString(); //json build sensor to buffer
    magel.report.send(buffer_data); //send data sensor with buffer json format
  });
}

