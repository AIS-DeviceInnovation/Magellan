#ifndef MAGELLAN_WIFI_SETTING_H
#define MAGELLAN_WIFI_SETTING_H

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#endif


#include "./MAGELLAN_MQTT.h"

struct WIFI_SETTING{
    String SSID = "UNKNOWN";
    String PASS = "UNKNOWN";
}WiFiSetting;

unsigned long prv_millis = 0;
unsigned long intervalConnect = 5000;
unsigned long intervalReconnect = 10000;
boolean wifiDisconnect = false;
extern WIFI_SETTING WiFiSetting;

void connectWiFi(WIFI_SETTING &sWiFi_setting) 
{

  #ifdef ESP32
  WiFi.mode(WIFI_STA);
  WiFi.begin(sWiFi_setting.SSID.c_str(), sWiFi_setting.PASS.c_str());
  Serial.print(F("# Connecting to WIFI network"));
  Serial.print(" SSID: "+ WiFiSetting.SSID);
  wifiDisconnect = (WiFi.status() != WL_CONNECTED? true : false);
  while(wifiDisconnect)
  {
    wifiDisconnect = (WiFi.status() != WL_CONNECTED? true : false);
    if((millis() - prv_millis >= intervalConnect))
    {
      Serial.print(F("."));

      // #ifdef ESP32
      WiFi.disconnect();
      WiFi.reconnect();


      prv_millis = millis();
    }
    if(!wifiDisconnect)
    {
      break;
    }
  }
  #elif defined ESP8266
  WiFi.begin(sWiFi_setting.SSID.c_str(), sWiFi_setting.PASS.c_str());
  Serial.print(F("# Connecting to WIFI network"));
  Serial.print(" SSID: "+ WiFiSetting.SSID);
   while (WiFi.status() != WL_CONNECTED) 
   {
      delay(250);
      Serial.print(".");
   }
  #endif
  Serial.println(F("\n# Wifi Connected!"));
  Serial.print(F("# Connected to WiFi network with IP Address: "));
  Serial.println(WiFi.localIP());
}

String getSSID()
{
  return WiFiSetting.SSID;
}

void connectWiFi(String SSID, String PASS) 
{
  #ifdef ESP32
  WiFiSetting.SSID = SSID;
  WiFiSetting.PASS = PASS;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFiSetting.SSID.c_str(), WiFiSetting.PASS.c_str());
  Serial.print(F("# Connecting to WIFI network"));
  Serial.print(" SSID: "+ WiFiSetting.SSID);
  wifiDisconnect = (WiFi.status() != WL_CONNECTED? true : false);
  while(wifiDisconnect)
  {
    wifiDisconnect = (WiFi.status() != WL_CONNECTED? true : false);
    if((millis() - prv_millis >= intervalConnect))
    {
      Serial.print(F("."));
      WiFi.disconnect();
      WiFi.reconnect();
      prv_millis = millis();
    }
    if(!wifiDisconnect)
    {
      break;
    }
  }
  #elif defined ESP8266
  WiFiSetting.SSID = SSID;
  WiFiSetting.PASS = PASS;
  WiFi.begin(WiFiSetting.SSID.c_str(), WiFiSetting.PASS.c_str());
  Serial.print(F("# Connecting to WIFI network"));
  Serial.print(" SSID: "+ WiFiSetting.SSID);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(250);
     Serial.print(".");
  }
  #endif
  Serial.println(F("\n# Wifi Connected!"));
  Serial.print(F("# Connected to WiFi network with IP Address: "));
  Serial.println(WiFi.localIP());
}

void reconnectWiFi(MAGELLAN_MQTT &mqttClient)
{
  wifiDisconnect = (WiFi.status() != WL_CONNECTED? true : false);
  if(wifiDisconnect){Serial.print(F("# Reconnecting to Wifi..."));}
  while(wifiDisconnect)
  {
    wifiDisconnect = (WiFi.status() != WL_CONNECTED? true : false);
    if((millis() - prv_millis >= intervalReconnect))
    {
      Serial.print(F("."));
      WiFi.disconnect();
      WiFi.reconnect();
      prv_millis = millis();
    }
    if(!wifiDisconnect)
    {
      break;
    }
  }

  if((!wifiDisconnect) && (!mqttClient.isConnected()))
  {
    mqttClient.reconnect();
  }
}    
#endif
