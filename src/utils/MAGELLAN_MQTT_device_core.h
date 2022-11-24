/*
Copyright (c) 2020, Advanced Wireless Network
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
support esp8266, esp32
 
Author:(POC Device Magellan team)      
Create Date: 25 April 2022. 
Modified: 21 september 2022.
Released for private usage.
*/
#ifndef MAGELLAN_MQTT_DEVICE_CORE_h
#define MAGELLAN_MQTT_DEVICE_CORE_h

#include <Arduino.h> 
#include "./PubSubClient.h"
#include "./ArduinoJson-v6.18.3.h"

#include <Arduino.h>
#ifdef ESP32
#include <Update.h>
#define defaultOTABuffer 8192
#define _default_OverBufferSize defaultOTABuffer
#elif defined ESP8266
#define defaultOTABuffer 4096
#define _default_OverBufferSize defaultOTABuffer
#endif
#include "Attribute_MQTT_core.h"
//version dev v1.0.0
#define lib_version "v1.0.0"
//Response Payload format some function can be set.
#define PLAINTEXT 0 //Plaintext
#define JSON 1 //Json

#define ERROR 0
#define TOKEN 1
#define CONTROL_JSON 2
#define CONTROL_PLAINTEXT 3
#define CONFIG_JSON 4
#define CONFIG_PLAINTEXT 5
#define UNIXTIME 6
#define UTCTIME 7
#define RESP_REPORT_JSON 8
#define RESP_REPORT_PLAINTEXT 9
#define RESP_REPORT_TIMESTAMP 10
#define RESP_HEARTBEAT_JSON 11
#define RESP_HEARTBEAT_PLAINTEXT 12
#define GET_ENDPOINT 13

#define mgPort 1883
//for define mode Timestamp
#define SET_UNIXTS 0
#define SET_UTC 1

#define Production 1
#define Staging 2 
#define IoT 3

#define _host_production "magellan.ais.co.th"

#define defaultBuffer 1024

// #define defaultOTABuffer 8192
// // #define bufferSizeForOTA 8192
// #define _default_OverBufferSize 8192

#define UNKNOWN -1
#define OUT_OF_DATE 0
#define UP_TO_DATE 1

typedef struct {
  String Topic;
  String Key;
  String Action;
  String Payload;
  String RESP;
  unsigned int CODE;
  unsigned int PayloadLength;
}EVENTS;

typedef struct {
  boolean isReadyOTA = false;
  int firmwareIsUpToDate = -1; // -1 unknow, 0 out of date, 1 up to date
  boolean inProcessOTA = false;
  unsigned int firmwareTotalSize = 0;
  String firmwareName = "UNKNOWN";
  String firmwareVersion = "UNKNOWN";
  String checksum = "UNKNOWN";
  String checksumAlgorithm = "UNKNOWN";
}OTA_INFO;

typedef std::function<void(String payload)> ctrl_handleCallback;
typedef std::function<void(String payload)> ctrl_Json_handleCallback;  
typedef std::function<void(JsonObject docs)> ctrl_JsonOBJ_handleCallback;  
typedef std::function<void(String payload)> conf_handleCallback;
typedef std::function<void(String payload)> conf_Json_handleCallback;  
typedef std::function<void(JsonObject docs)> conf_JsonOBJ_handleCallback;  

typedef std::function<void(String key, String value)> ctrl_PTAhandleCallback;
typedef std::function<void(String key, String value)> conf_PTAhandleCallback;
typedef std::function<void(EVENTS event)> resp_callback;
// typedef std::function<void(String respCODE)> resp_callback;

typedef std::function<void(void)> func_callback_registerList;

// #define subRemainCallback std::function<void(void)> subRemain

typedef std::function<void(void)> func_callback_ms;

class MAGELLAN_MQTT_device_core
{
public:
  MAGELLAN_MQTT_device_core(){};
  MAGELLAN_MQTT_device_core(Client& c); // for customize client internet interface
  boolean flagToken = false;
  String client_id;
  // String unixtTime;
  void setAuthMagellan(String _thingIden, String _thingSecret, String _imei = "none"); // add on
  void begin(String _thingIden, String _thingSencret, String _imei, unsigned int Zone = Production, uint16_t bufferSize = 1024);
  void beginCustom(String _client_id, String _host, int _port, uint16_t bufferSize); //
  void begin(String _client_id, unsigned int Zone = Production, uint16_t bufferSize = 1024); //
 
  String getHostName(); //
  String readToken();
  String readThingIdentifier();
  String readThingSecret();

  boolean reportSensor();
  boolean report(String payload); //
  boolean report(String key, String value); //
  boolean registerResponseReport(int format = JSON);
  boolean registerResponseReportTimestamp();
  boolean registerResponseHeartbeat(int format = JSON);
  boolean reportTimestamp(String timestamp, String JSONpayload, unsigned int timestamp_type = SET_UNIXTS); // Json Payload
  boolean heartbeat(); //
  void heartbeat(unsigned int triger_ms); //
  boolean registerConfig(int format = JSON); //  PTA and JSON
  boolean registerConfig(String key);//
  boolean registerControl(int format = JSON); //
  boolean registerControl(String key); //
  boolean registerTimestamp(int format = JSON); //
  boolean reqControl(String key);
  boolean reqControlJSON();

  ////// unsub ////////////////
  boolean unregisterControl(int format = JSON);  //
  boolean unregisterControl(String key); //
  boolean unregisterConfig(int format = JSON); //  PTA and JSON
  boolean unregisterConfig(String key); //
  boolean unregisterTimestamp(int format = JSON); //
  boolean unregisterResponseReport(int format = JSON);
  boolean unregisterResponseReportTimestamp();
  boolean unregisterResponseHeartbeat(int format = JSON);
  ////// unsub ////////////////  

  void getControl(String key, ctrl_handleCallback ctrl_callback);
  void getControl(ctrl_PTAhandleCallback ctrl_pta_callback);
  void getControlJSON(ctrl_Json_handleCallback ctrl_json_callback);
  void getControlJSON(ctrl_JsonOBJ_handleCallback jsonOBJ_cb);

  void getConfig(String key, conf_handleCallback _conf_callback);
  void getConfig(conf_PTAhandleCallback conf_pta_callback);
  void getConfigJSON(conf_Json_handleCallback conf_json_callback);
  void getConfigJSON(conf_JsonOBJ_handleCallback jsonOBJ_cb);
  void getRESP(unsigned int resp_event ,resp_callback resp_cb);

  boolean getTimestamp(); //
  boolean reqConfigJSON(); //
  boolean reqConfig(String key); //
  boolean reportClientConfig(String payload); // 
  boolean isConnected();
  void loop(); //
  void setMessageListener(void(*callback)(EVENTS, char*));
  
  void setManualToken(String _token);
  void acceptToken(EVENTS event);
  
  boolean ACKControl(String key, String value);
  boolean ACKControl(String payload);

  void interval_ms(unsigned long ms, func_callback_ms cb_ms);
  void registerList(func_callback_registerList cb_regisList);
  
  // interface MAGELLANJSON 
  StaticJsonDocument<256> docJson;

  String deserialControlJSON(String jsonContent);
  JsonObject deserialJson(String jsonContent);
  void addSensor(String key, String value, JsonDocument &ref_docs);
  void addSensor(String key, const char* value, JsonDocument &ref_docs);
  void addSensor(String key, int value, JsonDocument &ref_docs);
  void addSensor(String key, float value, JsonDocument &ref_docs);
  void addSensor(String key, boolean value, JsonDocument &ref_docs);
  void updateSensor(String key, String value, JsonDocument &ref_docs);
  void updateSensor(String key, const char* value, JsonDocument &ref_docs);
  void updateSensor(String key, int value, JsonDocument &ref_docs);
  void updateSensor(String key, float value, JsonDocument &ref_docs);
  void updateSensor(String key, boolean value, JsonDocument &ref_docs);
  String buildSensorJSON(JsonDocument &ref_docs);
  void remove(String key, JsonDocument &ref_docs);
  boolean findKey(String key, JsonDocument &ref_docs);
  void adjustBufferSensor(size_t sizeJSONbuffer);
  int readBufferSensor(JsonDocument &ref_docs);
  void clearSensorBuffer(JsonDocument &ref_docs);

  void setMQTTBufferSize(uint16_t size);
  static boolean CheckString_isDigit(String valid_payload); //
  boolean CheckString_isDouble(String valid_payload); //

  //OTA Feature//
  void activeOTA(size_t part_size, boolean useChecksum = true);
  void handleOTA(boolean OTA_after_getInfo = true);
  boolean registerInfoOTA();
  boolean registerDownloadOTA();
  boolean unregisterInfoOTA();
  boolean unregisterDownloadOTA();
  boolean updateProgressOTA(String OTA_state, String description);
  boolean requestFW_Info();
  boolean requestFW_Download(unsigned int fw_part, size_t part_size);
  void setChecksum(String md5Checksum);
  void setChunkSize(size_t Chunksize);
  static OTA_INFO OTA_info;
  void reconnect(); //add on
  

private:
  int _default_bufferSize = 1024;

  void checkConnection(); //
  void thingRegister(); //
  void acceptToken(String payload); //
  boolean setBufferSize(uint16_t size); //  default 256 (uplink 128 + downlink 128)
  String byteToString(byte* payload, unsigned int length_payload); // convert byte* to string
  void setCallback_msgHandle();
  unsigned long previouseMillis = 0; //
  boolean flagRegisterToken = false; // 
  boolean flagAuthMagellan = false;

  boolean registerToken(); //
  boolean requestToken(); //
  void reconnectMagellan(); //
  int limit_attempt = 11; // 11 -> for atempt 10 request token
  int cnt_attempt = 0; //
  int recon_attempt = 0;
  int MAXrecon_attempt = 10;
  unsigned long prev_time;
  unsigned long now_time;
  unsigned long threshold_ms;

  unsigned long HB_prev_time;
  unsigned long HB_now_time;
  unsigned long HB_threshold_ms;

  unsigned long cnt_fail = 0;

  String host;
  String thingIden;
  String thingSecret;
  String imei;
  String token;
  String _debug;
  int port;
protected:
  PubSubClient *client = NULL;
};

#endif


