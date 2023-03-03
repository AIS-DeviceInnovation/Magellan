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
    magel.interval(15, [&]()
                   {
                     File file = SPIFFS.open("/filePath", "r");
                     if (!file)
                     {
                       Serial.println("Failed to open image file in SPIFFS");
                       return;
                     }
                     size_t size = file.size();
                     uint8_t *buffer = new uint8_t[size];
                     file.read(buffer, size);
                     media.uploadFromBinary("key", buffer, size, Image, "png");
                     file.close();
                     delete[] buffer;
                   });
}
