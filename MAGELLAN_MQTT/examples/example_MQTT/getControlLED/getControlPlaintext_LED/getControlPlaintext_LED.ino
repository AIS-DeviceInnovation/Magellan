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
  //{1} control get any key and value
  magel.getControl([](String key, String value){
    Serial.print("# Control incoming\n# [Key]: ");
    Serial.println(key);
    Serial.print("# [Value]: ");
    Serial.println(value);
    if(key == "Lamp1")
    {
      if(value == "1")
      {
        digitalWrite(LED_pin22, HIGH);
        Serial.println("LED ON");
        magel.control.ACK("Lamp1", String(digitalRead(LED_pin22))); //ACKNOWLEDGE control to magellan
      }
      else if (value == "0")
      {
        digitalWrite(LED_pin22, LOW);
        Serial.println("LED OFF");
        magel.control.ACK("Lamp1", String(digitalRead(LED_pin22))); //ACKNOWLEDGE control to magellan
      }   
    }
    else // ACKNOWLEDGE control to magellan form another control focus
    {
      magel.control.ACK(key, value); //ACKNOWLEDGE control to magellan  
    }
  });
  //or {2} get control by focus on 1 key optional
  // magel.getControl("Lamp1", [](String value){ // focus only value form key "Lamp1"
  //   Serial.print("# Control incoming focus on [Key] Lamp1: ");
  //   Serial.println(value);
  //   if(value == "1")
  //   {
  //     digitalWrite(LED_pin22, HIGH);
  //     Serial.println("LED ON");
  //     magel.control.ACK("Lamp1", String(digitalRead(LED_pin22))); //ACKNOWLEDGE control to magellan
  //   }
  //   else if (value == "0")
  //   {
  //     digitalWrite(LED_pin22, LOW);
  //     Serial.println("LED OFF");
  //     magel.control.ACK("Lamp1", String(digitalRead(LED_pin22))); //ACKNOWLEDGE control to magellan
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
    magel.subscribe.control(PLAINTEXT); // subscribe server config content type JSON
  });
  magel.interval(10,[](){ //time interval function inside every 10000 millis

  });
}