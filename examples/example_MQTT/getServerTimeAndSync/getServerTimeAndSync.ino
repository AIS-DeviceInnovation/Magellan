#include <Arduino.h>
#include <MAGELLAN_MQTT.h>
#include <MAGELLAN_WiFi_SETTING.h> //optional you can using wifi connect with your own function
#include "sys/time.h"

WiFiClient WiFi_client;
MAGELLAN_MQTT magel(WiFi_client);

String SSID = "set_your_ssid";
String PASS = "set_your_password";
String thingIdentifier = "set_your_thing_identifier";
String thingSecret = "set_your_thing_secret";

int unixTimeMG;

const char *timezone = "ICT-7"; // POSIX Timezone Strings "Asia/Bangkok" UTC+7
const char *days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
void setTimezone(const char *timezone)
{
    setenv("TZ", timezone, 1); // Set the timezone
    tzset();                   // Apply the timezone
}

void setup()
{
    Serial.begin(115200);
    WiFiSetting.SSID = SSID;  // optional wifi connection
    WiFiSetting.PASS = PASS;  // optional wifi connection
    connectWiFi(WiFiSetting); // optional wifi connection

    magel.getResponse(UNIXTIME, [](EVENTS events) { // for get unixTime from magellan
        unixTimeMG = events.Payload.toInt();
        Serial.print("[unixTimeMG from magellan]: ");
        Serial.println(unixTimeMG); // this unixTime to use!

        setTimezone(timezone);

        struct timeval tv;
        tv.tv_sec = unixTimeMG;  // Set seconds (Unix time)
        tv.tv_usec = 0;          // Microseconds (optional)
        settimeofday(&tv, NULL); // sync time to ESP32
    });
}

void loop()
{
    magel.loop();
    reconnectWiFi(magel);
    magel.subscribes([]()
                     {
                         magel.subscribe.getServerTime(PLAINTEXT);
                         magel.getServerTime(); // request time from magellan server
                     });
    magel.interval(1, []()
                   {  
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.print("Updated time: ");
        Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
        Serial.print("Today is: ");
        Serial.println(days[timeinfo.tm_wday]);
    } else {
        Serial.println("Failed to obtain updated time");
        magel.getServerTime(); // request time from magellan server
    } });
}
