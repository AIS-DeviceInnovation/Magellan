#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own function

#define LED_pin22 22 //LED pin 22
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

  pinMode(LED_pin22, OUTPUT);
  digitalWrite(LED_pin22, LOW);

  //{1} JSON String
  magel.getControlJSON([](String payload){
    Serial.print("# Control incoming JSON: ");
    Serial.println(payload);
    String control = magel.deserializeControl(payload);
    if(control == "{\"Lamp1\":1}")
    {
      digitalWrite(LED_pin22, HIGH);
      Serial.println("LED ON");
      magel.sensor.add("Lamp1", digitalRead(LED_pin22));
      magel.control.ACK(magel.sensor.toJSONString()); //ACKNOWLEDGE control to magellan
      magel.sensor.clear();
    }
    else if (control == "{\"Lamp1\":0}")
    {
      digitalWrite(LED_pin22, LOW);
      Serial.println("LED OFF");
      magel.sensor.add("Lamp1", digitalRead(LED_pin22));
      magel.control.ACK(magel.sensor.toJSONString()); //ACKNOWLEDGE control to magellan
      magel.sensor.clear();
    }  
    else
    {
      magel.control.ACK(control); //ACKNOWLEDGE control to magellan if another control
    }

  });
  //or {2} JSON Object
  // magel.getControlJSON([](JsonObject docObject){
  //   String Lamp1 = docObject["Lamp1"]; // buffer value form control key "Lamp1" if not found this key value is "null"
  //   if(Lamp1.indexOf("null") == -1)
  //   {
  //       Serial.print("# Control incoming JSON Object: ");
  //       Serial.print("# [Key] => Lamp1: ");
  //       Serial.println(Lamp1);
  //       if(Lamp1 == "1")
  //       {
  //         digitalWrite(LED_pin22, HIGH);
  //         Serial.println("LED ON");
  //         magel.sensor.add("Lamp1", digitalRead(LED_pin22));
  //         magel.control.ACK(magel.sensor.toJSONString()); //ACKNOWLEDGE control to magellan
  //         magel.sensor.clear();
  //       }
  //       else if (Lamp1 == "0")
  //       {
  //         digitalWrite(LED_pin22, LOW);
  //         Serial.println("LED OFF");
  //         magel.sensor.add("Lamp1", digitalRead(LED_pin22));
  //         magel.control.ACK(magel.sensor.toJSONString()); //ACKNOWLEDGE control to magellan
  //         magel.sensor.clear();
  //       }
  //   }
  // });

  // prepare sensor to magellan by report control key with inial value
  magel.report.send("Lamp1","0");
}

void loop() 
{
  magel.loop();
  reconnectWiFi(magel);
  magel.subscribes([](){
    magel.subscribe.control(); // subscribe server config content type JSON
  });
  magel.interval(10,[](){ //time interval function inside every 10000 millis

  });
}