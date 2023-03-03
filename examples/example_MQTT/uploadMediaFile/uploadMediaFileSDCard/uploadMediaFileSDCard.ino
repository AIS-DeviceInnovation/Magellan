#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h>
#include <MAGELLAN_MEDIA_FILE.h>

WiFiClient WiFi_client;
MAGELLAN_MQTT magel(WiFi_client);
MAGELLAN_MEDIA_FILE media;

void setup()
{
    Serial.begin(115200);
    if (!SD.begin(SS, SPI, 40000U, "/sdcard"))
    {
      Serial.println("Card Mount Failed");
      return;
    }
    WiFiSetting.SSID = "SSID";
    WiFiSetting.PASS = "Password";
    connectWiFi(WiFiSetting);
    magel.begin();

    magel.getResponse(RESP_REPORT_JSON, [](EVENTS resp)
    { 
      Serial.println("# Code: " + String(resp.CODE)); 
    });
}

void loop()
{
    magel.loop();
    reconnectWiFi(magel);
    magel.subscribes([]()
                     {
                       magel.subscribe.report.response();
                     });
    magel.interval(15, []()
                   {
                    media.uploadFromSDCard("key","/FilePath.mp3",Audio,"mpeg",SD);
                   });
}
