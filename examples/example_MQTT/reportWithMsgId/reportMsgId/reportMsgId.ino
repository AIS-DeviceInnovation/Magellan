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

  magel.getResponse(RESP_REPORT_JSON, [](EVENTS events){  // optional for make sure report success CODE = 20000
    Serial.println("\n =============== Callback ============== ");
    Serial.print("# [RESP REPORT] code: ");
    Serial.println(events.CODE); // follow status code on https://magellan.ais.co.th/api-document/3/0 {Error code topic}
    Serial.print("# [RESP REPORT] response message: ");
    Serial.println(events.RESP);
    Serial.print("# [MsgId] : ");
    Serial.println(events.MsgId);
    Serial.println(" =============== Callback ============== \n");
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

    //{1.} auto buildJSON but manual report
    int MsgId = magel.report.generateMsgId(); //generate message id
    magel.sensor.add("GPS", "13.777864,100.544068");
    magel.sensor.add("random", (int)random(0, 100));
    magel.sensor.add("Humidity", (int)random(0, 100));
    String payload = magel.sensor.toJSONString();
    magel.sensor.clear();
    magel.report.send(payload, MsgId);

    //{2.} manual report
    MsgId = magel.report.generateMsgId(); //generate message id
    magel.report.send("{\"hello\":\"world\"}", MsgId);
  });
}