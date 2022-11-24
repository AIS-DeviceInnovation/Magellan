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
  magel.interval(10,[]() 
  {
    String payload;
    unsigned int Positive_Number = 1234;
    payload ="{\"Positive_Number\":"+String(Positive_Number)+"}";
    magel.report.send(payload);  //Report data positive number type
    delay(50);           

    //Negative Number Type
    int Negative_Number = -1234;
    payload ="{\"Negative_Number\":"+String(Negative_Number)+"}";
    magel.report.send(payload);  //Report data positive number type
    delay(50);         

    //Floating Point Number Type
    float Floating_Point_Number = 12.34;
    payload ="{\"Floating_Point_Number\":"+String(Floating_Point_Number)+"}";
    magel.report.send(payload);  //Report data positive number type
    delay(50);           

    //Text Type
    String Text = "\"Hello World\"";
    payload ="{\"Text\":"+String(Text)+"}";
    magel.report.send(payload);  //Report data positive number type
    delay(50);           

    //Location Type
    String Location = "\"13.764980,100.538335\"";
    payload ="{\"Location\":"+String(Location)+"}";
    magel.report.send(payload);  //Report data positive number type
    delay(50);            

    //Boolean Type
    bool Boolean = 1;
    payload ="{\"Boolean\":"+String(Boolean)+"}";
    magel.report.send(payload);  //Report data positive number type
    delay(50);         

    //Number Array type
    String NumberArray = "\"[1,2,3]\"";
    payload ="{\"numberArray\":"+ String(NumberArray) +"}";
    magel.report.send(payload);  //Report data positive number type
    delay(50);  
  });
}