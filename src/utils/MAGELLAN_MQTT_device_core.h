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
Modified: 22 may 2023.
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
#define defaultBuffer defaultOTABuffer
#define _default_OverBufferSize defaultOTABuffer
#elif defined ESP8266
#define defaultOTABuffer 4096
#define defaultBuffer defaultOTABuffer
#define _default_OverBufferSize defaultOTABuffer
#endif
#include "Attribute_MQTT_core.h"

#define lib_version lib_ver
#define major_version _major_ver
#define feature_version _feature_ver
#define enhance_version _enhance_ver
// Response Payload format some function can be set.
#define PLAINTEXT 0 // Plaintext
#define JSON 1      // Json

#define M_ERROR 0
#define TOKEN 1
#define CONTROL_JSON 2
#define CONTROL_PLAINTEXT 3
#define CONFIG_JSON 4
#define CONFIG_PLAINTEXT 5
#define UNIXTIME 6
// #define UTCTIME 7
#define RESP_REPORT_JSON 8
#define RESP_REPORT_PLAINTEXT 9
#define RESP_REPORT_TIMESTAMP 10
#define RESP_HEARTBEAT_JSON 11
#define RESP_HEARTBEAT_PLAINTEXT 12
#define GET_ENDPOINT 13

#define mgPort 1883
// for define mode Timestamp
#define SET_UNIXTS 0

#define _host_centric "centric-magellan.ais.co.th"
#define _host_production "device-entmagellan.ais.co.th"

enum class OTA_state
{
  UNKNOWN_STATE = -1,
  OUT_OF_DATE,
  UP_TO_DATE,
  NOT_AVAILABLE_STATE
};

typedef struct
{
  String Topic;
  String Key;
  String Action;
  String Payload;
  String RESP;
  unsigned int CODE;
  unsigned int PayloadLength;
  int MsgId = -1; // update 1.1.0 msgId

} EVENTS;

typedef struct
{
  boolean isReadyOTA = false;
  OTA_state firmwareIsUpToDate = OTA_state::UNKNOWN_STATE; // -1 unknow, 0 out of date, 1 up to date
  boolean inProcessOTA = false;
  unsigned int firmwareTotalSize = 0;
  String firmwareName = "UNKNOWN";
  String firmwareVersion = "UNKNOWN";
  String checksum = "UNKNOWN";
  String checksumAlgorithm = "UNKNOWN";
} OTA_INFO;

typedef std::function<void(String payload)> ctrl_handleCallback;
typedef std::function<void(String payload)> ctrl_Json_handleCallback;
typedef std::function<void(JsonObject docs)> ctrl_JsonOBJ_handleCallback;
typedef std::function<void(String payload)> conf_handleCallback;
typedef std::function<void(String payload)> conf_Json_handleCallback;
typedef std::function<void(JsonObject docs)> conf_JsonOBJ_handleCallback;

typedef std::function<void(String key, String value)> ctrl_PTAhandleCallback;
typedef std::function<void(String key, String value)> conf_PTAhandleCallback;
typedef std::function<void(EVENTS event)> resp_callback;

typedef std::function<void(void)> func_callback_registerList;

typedef std::function<void(void)> func_callback_ms;

class MAGELLAN_MQTT_device_core
{
public:
  MAGELLAN_MQTT_device_core();
  MAGELLAN_MQTT_device_core(Client &c);                                                // for customize client internet interface
  void setAuthMagellan(String _thingIden, String _thingSecret, String _imei = "none"); // add on
  void begin(String _thingIden, String _thingSencret, String _imei, uint16_t bufferSize = defaultOTABuffer);
  void beginCustom(String _client_id, String _host, int _port, uint16_t bufferSize); //
  void initMQTTClient(String _client_id, uint16_t bufferSize = defaultOTABuffer);    //
  void magellanCentric();

  String getHostName(); //
  String readToken();
  String readThingIdentifier();
  String readThingSecret();

  boolean reportSensor();
  boolean report(String payload);           //
  boolean report(String key, String value); //
  boolean registerResponseReport(int format = JSON);
  boolean registerResponseReportTimestamp();
  boolean registerResponseHeartbeat(int format = JSON);
  boolean reportTimestamp(String timestamp, String JSONpayload, unsigned int timestamp_type = SET_UNIXTS); // Json Payload
  boolean heartbeat();                                                                                     //
  void heartbeat(unsigned int triger_ms);                                                                  //
  boolean registerConfig(int format = JSON);                                                               //  PTA and JSON
  boolean registerConfig(String key);                                                                      //
  boolean registerControl(int format = JSON);                                                              //
  boolean registerControl(String key);                                                                     //
  boolean registerTimestamp(int format = JSON);                                                            //
  boolean reqControl(String key);
  boolean reqControlJSON();

  ////// unsub ////////////////
  boolean unregisterControl(int format = JSON);   //
  boolean unregisterControl(String key);          //
  boolean unregisterConfig(int format = JSON);    //  PTA and JSON
  boolean unregisterConfig(String key);           //
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
  void getRESP(unsigned int resp_event, resp_callback resp_cb);

  boolean getTimestamp();                     //
  boolean reqConfigJSON();                    //
  boolean reqConfig(String key);              //
  boolean reportClientConfig(String payload); //
  boolean isConnected();
  void loop(); //
  void setMessageListener(void (*callback)(EVENTS, char *));

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
  void addSensor(String key, const char *value, JsonDocument &ref_docs);
  void addSensor(String key, int value, JsonDocument &ref_docs);
  void addSensor(String key, float value, JsonDocument &ref_docs);
  void addSensor(String key, boolean value, JsonDocument &ref_docs);
  void updateSensor(String key, String value, JsonDocument &ref_docs);
  void updateSensor(String key, const char *value, JsonDocument &ref_docs);
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
  boolean CheckString_isDouble(String valid_payload);       //

  // OTA Feature//
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
  static func_callback_registerList duplicate_subs_list; // ver.1.1.0

  void reconnect();  // add on
  void disconnect(); // add on
  void setCallback_msgHandle();

private:
  int _default_bufferSize = _default_OverBufferSize;
  boolean flagToken = false;
  String client_id;
  void checkConnection();                                          //
  void thingRegister();                                            //
  void acceptToken(String payload);                                //
  boolean setBufferSize(uint16_t size);                            //  default 256 (uplink 128 + downlink 128)
  String byteToString(byte *payload, unsigned int length_payload); // convert byte* to string
  // void setCallback_msgHandle();
  unsigned long previouseMillis = 0; //
  boolean flagRegisterToken = false; //
  boolean flagAuthMagellan = false;

  boolean registerToken();  //
  boolean requestToken();   //
  void reconnectMagellan(); //

  boolean flagRegisterEndPoint = false; //
  boolean flagGetEndPoint = false;      //
  void getEndPoint();
  boolean requestEndpoint();
  boolean acceptEndPoint(String payload);

  const int limit_attempt = 11; // 11 -> for atempt 10 request token
  int cnt_attempt = 0;          //
  int recon_attempt = 0;
  int MAXrecon_attempt = 10;

  unsigned long prev_time = 0;
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

  void dead_reconnect_handler();
  const int threshold_dead_reconnect_time = 7500;
  ulong fallback_dead_reconnect_time = 0;
  ulong cnt_dead_reconnect_time = 0;
  const int max_cnt_dead_reconnect_time = 5;

protected:
  PubSubClient *client = NULL;
};

#endif
