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

  magel.getResponse(RESP_REPORT_JSON, [](EVENTS events) { // optional for make sure report success CODE = 20000
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
  magel.interval(10, []()
                 {
                   ResultReport result;              // decleare struct ResultReport for buffer result report with advance setting to report(optional)
                   RetransmitSetting settingReport; // decleare struct RetransmitSetting for advance setting to report

                   //{1.} auto buildJSON report with msgId by using struct "RetransmitSetting" 
                   Serial.println("\n############## [Example Report] {1.} ##");
                   magel.sensor.add("temp", 24.551);
                   magel.sensor.add("humid", 30);
                   result = magel.sensor.report(settingReport); // report sensor with setting report(default setting from struct)
                   Serial.print("[MsgId report]: ");
                   Serial.println(result.msgId);
                   Serial.print("[Status report]: ");
                   Serial.println((result.statusReport)? "SUCCESS" : "FAIL");

                   //{2.} manual report with msgId by using struct "RetransmitSetting" and set manual MsgId
                   Serial.println("\n############## [Example Report] {2.} ##");
                   int MsgId = magel.report.generateMsgId(); // generate message id
                   settingReport.setMsgId(MsgId);           // set manual message id to setting
                   result = magel.report.send("{\"hello\":\"magellan\"}", settingReport);
                   Serial.print("[MsgId report]: ");
                   Serial.println(result.msgId);
                   Serial.print("[Status report]: ");
                   Serial.println((result.statusReport)? "SUCCESS" : "FAIL");

                   //{3.} manual report with msgId by using struct "RetransmitSetting" and set manual MsgId
                   Serial.println("\n############## [Example Report] {3.} ##");
                   settingReport.generateMsgId();          // generate new message id to setting
                   result = magel.report.send("{\"hello\":\"world\"}", settingReport);
                   Serial.print("[MsgId report]: ");
                   Serial.println(result.msgId);
                   Serial.print("[Status report]: ");
                   Serial.println((result.statusReport)? "SUCCESS" : "FAIL"); });
}