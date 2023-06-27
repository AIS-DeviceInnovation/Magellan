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
support esp32, esp8266

Author:(POC Device Magellan team)
Create Date: 25 April 2022.
Modified: 22 may 2023.
*/

#include "MAGELLAN_MQTT_device_core.h"

String Attribute_MQTT_core::statusActivateCode = "null";
StaticJsonDocument<512> intern_docJSON;
boolean Attribute_MQTT_core::isBypassAutoUpdate = false; // false = autoUpdate, true = unuse autoUpdate
boolean Attribute_MQTT_core::isFirmwareUptodate = false;
boolean Attribute_MQTT_core::checkFirmwareUptodate = false;
boolean Attribute_MQTT_core::usingCheckUpdate = false;
boolean Attribute_MQTT_core::flagAutoOTA = true;
size_t Attribute_MQTT_core::calculate_chunkSize = 0; // calculate part size from clientBuffer
size_t Attribute_MQTT_core::incomingChunkSize = 0;
unsigned int Attribute_MQTT_core::fw_total_size = 0;
unsigned int Attribute_MQTT_core::fw_count_chunk = 0;
unsigned int Attribute_MQTT_core::chunk_size = 0;
unsigned int Attribute_MQTT_core::totalChunk = 0;
unsigned int Attribute_MQTT_core::current_chunk = 0;
unsigned int Attribute_MQTT_core::current_size = 0;
unsigned int Attribute_MQTT_core::default_chunk_size = 4096;
Client *Attribute_MQTT_core::ClientNET = NULL;
PubSubClient *Attribute_MQTT_core::mqtt_client = NULL; // MQTT Client
boolean Attribute_MQTT_core::ctrl_regis_key = false;
boolean Attribute_MQTT_core::ctrl_regis_pta = false;
boolean Attribute_MQTT_core::ctrl_regis_json = false;
boolean Attribute_MQTT_core::conf_regis_key = false;
boolean Attribute_MQTT_core::conf_regis_pta = false;
boolean Attribute_MQTT_core::conf_regis_json = false;
boolean Attribute_MQTT_core::resp_regis = false;
boolean Attribute_MQTT_core::ctrl_jsonOBJ = false;
boolean Attribute_MQTT_core::conf_jsonOBJ = false;
boolean Attribute_MQTT_core::using_Checksum = false;
boolean Attribute_MQTT_core::useAdvanceCallback = false;
String Attribute_MQTT_core::ext_Token;
String Attribute_MQTT_core::ext_EndPoint;
boolean Attribute_MQTT_core::inProcessOTA = false;
boolean Attribute_MQTT_core::remain_ota_fw_info_match = false;
boolean Attribute_MQTT_core::flag_remain_ota = false;
String Attribute_MQTT_core::valid_remain_fw_name = "";
unsigned int Attribute_MQTT_core::valid_remain_fw_size = 0;
boolean Attribute_MQTT_core::triggerRemainOTA = true;
boolean Attribute_MQTT_core::triggerRemainSub = true;
boolean Attribute_MQTT_core::remind_Event_GET_FW_infoOTA = false;
boolean Attribute_MQTT_core::startReqDownloadOTA = false;
boolean Attribute_MQTT_core::checkTimeout_request_download_fw = false;
unsigned int Attribute_MQTT_core::timeout_req_download_fw = 180000;
unsigned long Attribute_MQTT_core::prv_cb_timeout_millis = 0;
StaticJsonDocument<512> Attribute_MQTT_core::docClientConf;
DynamicJsonDocument *Attribute_MQTT_core::adjDoc = new DynamicJsonDocument(256);
DynamicJsonDocument *Attribute_MQTT_core::docSensor = new DynamicJsonDocument(1024);
// 1.1.0
unsigned int Attribute_MQTT_core::delayRequest_download = 0;
boolean Attribute_MQTT_core::checkUpdate_inside = false;
unsigned int Attribute_MQTT_core::delayCheckUpdate_inside = 60000;
size_t Attribute_MQTT_core::buffer_JSON_bufferSize = 1024;
size_t Attribute_MQTT_core::max_payload_report = 50000;
int Attribute_MQTT_core::matchMsgId_cb = -1;
int Attribute_MQTT_core::matchMsgId_send = -1;
int Attribute_MQTT_core::maxRetransmit = 5;
unsigned int Attribute_MQTT_core::durationRetransmit = 10000;
boolean Attribute_MQTT_core::isMatchMsgId = false;
boolean Attribute_MQTT_core::reqRetransmit = false;

OTA_INFO MAGELLAN_MQTT_device_core::OTA_info;
func_callback_registerList MAGELLAN_MQTT_device_core::duplicate_subs_list;

boolean attemp_download_1 = false;
boolean attemp_download_2 = false;

String b2str(byte *payload, unsigned int length) // convert byte* to String
{
  char buffer_payload[length + 1] = {0};
  memcpy(buffer_payload, (char *)payload, length);
  return String(buffer_payload);
}

typedef struct
{
  String registerKey;
  ctrl_handleCallback ctrl_key_callback;
  ctrl_Json_handleCallback ctrl_Json_callback;
  ctrl_PTAhandleCallback ctrl_pta_callback;
  ctrl_JsonOBJ_handleCallback ctrl_obj_callback;

  conf_handleCallback conf_key_callback;
  conf_Json_handleCallback conf_json_callback;
  conf_PTAhandleCallback conf_pta_callback;
  conf_JsonOBJ_handleCallback conf_obj_callback;
  resp_callback resp_h_callback;

  void *next;
  unsigned int Event;
  unsigned int RESP_Events;

} regisAPI;

regisAPI *_startRegis = NULL;         // buffer callback <void String payload>
regisAPI *_startRegisPTA = NULL;      // buffer callback <void String key, String value>
regisAPI *_startRegisJSON = NULL;     // buffer callback <void String key, String value>
regisAPI *_startRegisConf = NULL;     // buffer callback <void String payload>
regisAPI *_startRegisPTAConf = NULL;  // buffer callback <void String key, String value>
regisAPI *_startRegisJSONConf = NULL; // buffer callback <void String key, String value>
regisAPI *_startRESP = NULL;

regisAPI *_startOBJ_CTRL = NULL;
regisAPI *_startOBJ_CONF = NULL;

boolean ext_useAdvanceCallback = false;

void (*cb_internal)(EVENTS events, char *);

JsonObject deJson(String jsonContent)
{
  JsonObject buffer;
  intern_docJSON.clear();
  if (jsonContent != NULL && jsonContent != "clear")
  {
    DeserializationError error = deserializeJson(intern_docJSON, jsonContent);
    buffer = intern_docJSON.as<JsonObject>();
    if (error)
      Serial.println("# Error to DeserializeJson Control");
  }
  return buffer;
}

String deControl(String jsonContent)
{
  String content = "40300";
  JsonObject buffdoc = deJson(jsonContent);
  String statusCode = buffdoc["Code"];
  String buffDelta;
  if (statusCode == "20000")
  {
    if (jsonContent.indexOf("Delta") != -1)
    {
      buffDelta = buffdoc["Delta"].as<String>();
      content = buffDelta;
    }
    else if (jsonContent.indexOf("Sensor") != -1)
    {
      buffDelta = buffdoc["Sensor"].as<String>();
      content = buffDelta;
    }
  }
  return content;
}

String deConfig(String jsonContent)
{
  String content = "40300";
  JsonObject buffdoc = deJson(jsonContent);
  String statusCode = buffdoc["Code"];
  String buffDelta;
  if (statusCode == "20000")
  {
    if (jsonContent.indexOf("Config") != -1)
    {
      buffDelta = buffdoc["Config"].as<String>();
      content = buffDelta;
    }
  }
  return content;
}
/////////// Feature OTA function none member in class //////////////////////
boolean pubClientConfig(String payload) // for external function member
{
  String topic = "api/v2/thing/" + attr.ext_Token + "/config/persist";
  boolean Pub_status = attr.mqtt_client->publish(topic.c_str(), payload.c_str());
  bool _debug_ = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Save ClientConfig: " + _debug_);
  Serial.println("# [ClientConfigs]: " + payload);
  return Pub_status;
}

boolean sub_InfoOTA()
{
  String topic = "api/v2/thing/" + attr.ext_Token + "/firmwareinfo/resp";
  boolean Sub_status = attr.mqtt_client->subscribe(topic.c_str());
  // Serial.println(topic);
  String Debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Subscribe Firmware Information: " + Debug);
  return Sub_status;
}

boolean unsub_InfoOTA()
{
  String topic = "api/v2/thing/" + attr.ext_Token + "/firmwareinfo/resp";
  boolean Sub_status = attr.mqtt_client->unsubscribe(topic.c_str());
  // Serial.println(topic);
  String Debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe Firmware Information: " + Debug);
  return Sub_status;
}

boolean pub_Info()
{
  String topic = "api/v2/thing/" + attr.ext_Token + "/firmwareinfo/req";
  boolean Pub_status = attr.mqtt_client->publish(topic.c_str(), " ");
  // Serial.println(topic);
  String Debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Request Firmware Information: " + Debug);
  return Pub_status;
}

boolean sub_DownloadOTA()
{
  String topic = "api/v2/thing/" + attr.ext_Token + "/firmwaredownload/resp/+";
  boolean Sub_status = attr.mqtt_client->subscribe(topic.c_str());
  // Serial.println(topic);
  String Debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Subscribe Firmware Download: " + Debug);
  return Sub_status;
}

boolean unsub_DownloadOTA()
{
  String topic = "api/v2/thing/" + attr.ext_Token + "/firmwaredownload/resp/+";
  boolean Sub_status = attr.mqtt_client->unsubscribe(topic.c_str());
  // Serial.println(topic);
  String Debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe Firmware Download: " + Debug);
  return Sub_status;
}

boolean pub_Download(unsigned int fw_chunkpart, size_t chunk_size)
{
  if (fw_chunkpart == 0)
  {
    attr.startReqDownloadOTA = true;
  }
  attr.checkTimeout_request_download_fw = true;
  String topic = "api/v2/thing/" + attr.ext_Token + "/firmwaredownload/req/" + String(fw_chunkpart) + "?filesize=" + String(chunk_size);
  boolean Pub_status = attr.mqtt_client->publish(topic.c_str(), " ");
  // Serial.println(topic);
  String Debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("------------------------------>"));
  Serial.println("# ->Request Firmware Download on chunk: " + String(fw_chunkpart) + " Status: " + Debug);
  Serial.println("# ->chunk size request: " + String(chunk_size));
  return Pub_status;
}
// pre ver.1.1.0
boolean pub_Download(unsigned int fw_chunk, size_t chunk_size, String versionName)
{
  if (fw_chunk == 0)
  {
    attr.startReqDownloadOTA = true;
  }
  attr.checkTimeout_request_download_fw = true;
  String topic = "api/v2/thing/" + attr.ext_Token + "/firmwaredownload/req/" + String(fw_chunk) + "?filesize=" + String(chunk_size);
  String payload = "{\"FirmwareVersion\" : \"" + versionName + "\"}";
  boolean Pub_status = attr.mqtt_client->publish(topic.c_str(), payload.c_str());
  // Serial.println(topic);
  String Debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("------------------------------>"));
  Serial.println("# ->Request Firmware Download by version name \"" + versionName + "\"on chunk: " + String(fw_chunk) + " Status: " + Debug);
  Serial.println("# ->Chunk size request: " + String(chunk_size));
  return Pub_status;
}

boolean pub_UpdateProgress(String FOTA_State, String description)
{
  delay(500);
  String topic = "api/v2/thing/" + attr.ext_Token + "/fotaupdateprogress/req/?FOTAState=" + FOTA_State;
  boolean Pub_status = false;
  // Serial.println("#===========================");
  // Serial.println("# topic: "+topic);
  // Serial.println("# FOTA_STATE: "+FOTA_State);
  // Serial.println("# DESCRIPTION: "+description);
  Serial.println("#===========================");
  if (description.indexOf("description") != -1 || description.indexOf("Version") != -1)
  {
    Pub_status = attr.mqtt_client->publish(topic.c_str(), description.c_str());
    Pub_status = attr.mqtt_client->publish(topic.c_str(), description.c_str());
    Serial.println(F("-------------------------------"));
    Serial.println("# STATE OTA Description: " + description);
    Serial.println(F("-------------------------------"));
  }
  else
  {
    Pub_status = attr.mqtt_client->publish(topic.c_str(), "");
    Pub_status = attr.mqtt_client->publish(topic.c_str(), "");
  }

  String Debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Update Progress OTA state discription: \"" + FOTA_State + "\" Status: " + Debug);
  return Pub_status;
}

boolean check_remain_fw_isMatch(String validate_fw_name, unsigned int validate_fw_size, String descriptionWhenFail)
{

  if (MAGELLAN_MQTT_device_core::OTA_info.firmwareName == validate_fw_name &&
      MAGELLAN_MQTT_device_core::OTA_info.firmwareTotalSize == validate_fw_size)
  {
    Serial.println(F("# Check firmware information incoming is match OTA still working"));
    return true;
  }
  else
  {
    Serial.println(F("# Check firmware information does not match after reconnect"));
    pub_UpdateProgress("FAILED", "{\"errordescription\":\"Downloading firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + " : " + descriptionWhenFail + "\"}");
    configOTAFile.saveSuccessOrFail("fail");
    return false;
  }
}

void checkUpdate(String topic, String payload)
{
  if (topic.indexOf("/firmwareinfo/resp") != -1)
  {
    Serial.println(F("======================="));
    Serial.println(F("# Check incoming firmware update"));
    if (payload != "{}" && payload.indexOf("20000") != -1)
    {
      JsonObject fw_doc = deJson(payload);
      String name = fw_doc["namefirmware"];
      unsigned int size = fw_doc["sizefirmware"];
      String version = fw_doc["versionfirmware"];
      String c_sum = fw_doc["checksum"];
      String al_c_sum = fw_doc["checksumAlgorithm"];
      attr.valid_remain_fw_name = name;
      attr.valid_remain_fw_size = size;
      if (name == "null" && size <= 0)
      {
        Serial.println(F("# [warning]Firmware Information is wrong or empty!"));
      }
      else
      {
        attr.checkFirmwareUptodate = configOTAFile.compareFirmwareIsUpToDate(fw_doc);

        MAGELLAN_MQTT_device_core::OTA_info.firmwareName = name;
        MAGELLAN_MQTT_device_core::OTA_info.firmwareTotalSize = size;
        MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion = version;
        MAGELLAN_MQTT_device_core::OTA_info.checksum = c_sum;
        MAGELLAN_MQTT_device_core::OTA_info.checksumAlgorithm = al_c_sum;

        if (attr.checkFirmwareUptodate)
        {
          MAGELLAN_MQTT_device_core::OTA_info.firmwareIsUpToDate = UP_TO_DATE;
        }
        else
        {
          MAGELLAN_MQTT_device_core::OTA_info.firmwareIsUpToDate = OUT_OF_DATE;
        }
        // MAGELLAN_MQTT_device_core::OTA_info.firmwareIsUpToDate = ((attr.checkFirmwareUptodate)? UP_TO_DATE : OUT_OF_DATE);
        attr.usingCheckUpdate = false;
      }
    }
  }
}

void save_fw_info(String topic, String payload)
{
  // Serial.println("-save_fw_info: "+payload);
  if (topic.indexOf("/firmwareinfo/resp") != -1)
  {
    Serial.println(F("======================="));
    Serial.println(F("# Detect incoming firmware information"));
    if (payload != "{}" && payload.indexOf("20000") != -1)
    {
      JsonObject fw_doc = deJson(payload);
      String name = fw_doc["namefirmware"];
      unsigned int size = fw_doc["sizefirmware"];
      String version = fw_doc["versionfirmware"];
      String c_sum = fw_doc["checksum"];
      String al_c_sum = fw_doc["checksumAlgorithm"];
      attr.valid_remain_fw_name = name;
      attr.valid_remain_fw_size = size;
      if (name == "null" && size <= 0)
      {
        Serial.println(F("# [warning]Firmware Information is wrong or empty!"));
      }
      else // validate data pass
      {
        if ((attr.inProcessOTA) && (attr.flag_remain_ota)) // inprocess but remain connect with broker
        {
          attr.remain_ota_fw_info_match = check_remain_fw_isMatch(attr.valid_remain_fw_name,
                                                                  attr.valid_remain_fw_size, "is mismatch from server");

          attr.flag_remain_ota = false;
          Serial.println(F("======================="));
          Serial.println(F("# Validate from reconnect Firmware OTA Information #"));
          // Serial.println("  ->Firmware Name: "+MAGELLAN_MQTT_device_core::OTA_info.firmwareName);
          // Serial.println("  ->Firmware total size: "+String(MAGELLAN_MQTT_device_core::OTA_info.firmwareTotalSize));
          Serial.println("  ->Firmware version: " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion);
          // Serial.println("  ->Firmware checksum Algorithm: "+MAGELLAN_MQTT_device_core::OTA_info.checksumAlgorithm);
          Serial.println("  ->Firmware checksum: " + MAGELLAN_MQTT_device_core::OTA_info.checksum);
          Serial.println(F("======================="));
          if (!attr.remain_ota_fw_info_match)
          {
            Serial.println(F("# [ERROR] Device must restart because firmware change #"));
            Serial.println(F("# firmware not match validate OTA information after reconnect"));
            delay(5000);
            ESP.restart();
          }
          else
          {
            pub_Download(attr.fw_count_chunk, attr.chunk_size);
          }
        }
        else if ((attr.inProcessOTA) && (!attr.triggerRemainOTA)) // inprocess get info fw
        {
          attr.remain_ota_fw_info_match = check_remain_fw_isMatch(attr.valid_remain_fw_name,
                                                                  attr.valid_remain_fw_size, "is obsolete");
          if (!attr.remain_ota_fw_info_match)
          {
            Serial.println(F("======================="));
            Serial.println(F("# Firmware OTA Information Incoming While inProcessOTA #"));
            // Serial.println("  ->Firmware Name: "+MAGELLAN_MQTT_device_core::OTA_info.firmwareName);
            // Serial.println("  ->Firmware total size: "+String(MAGELLAN_MQTT_device_core::OTA_info.firmwareTotalSize));
            Serial.println("  ->Firmware version: " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion);
            // Serial.println("  ->Firmware checksum Algorithm: "+MAGELLAN_MQTT_device_core::OTA_info.checksumAlgorithm);
            Serial.println("  ->Firmware checksum: " + MAGELLAN_MQTT_device_core::OTA_info.checksum);
            Serial.println(F("# [ERROR] Device must restart because firmware change #"));
            Serial.println(F("======================="));
            delay(5000);
            ESP.restart();
          }
        }

        if (!attr.inProcessOTA) // first get info and save to variable
        {
          attr.isFirmwareUptodate = configOTAFile.compareFirmwareOTA(fw_doc);

          // ver 1.1.0 UPDATED
          if (attr.isFirmwareUptodate == UP_TO_DATE)
          {
            JsonObject infoCurrentFW = configOTAFile.readObjectLastedOTA();
            String fw_ver = infoCurrentFW["versionfirmware"];
            String fw_size = infoCurrentFW["sizefirmware"];

            pub_UpdateProgress("UPDATED", "{\"description\":\"Firmware " + String(fw_ver) + " is Up to Date\",\"Version\":\"" + String(fw_ver) + "\"}");
          }
          // ver 1.1.0 UPDATED

          MAGELLAN_MQTT_device_core::OTA_info.firmwareIsUpToDate = ((attr.isFirmwareUptodate) ? UP_TO_DATE : OUT_OF_DATE);
          MAGELLAN_MQTT_device_core::OTA_info.firmwareName = configOTAFile.readSpacificFromConfFile("namefirmware");
          size_t buffReadSizefirmware = configOTAFile.readSpacificFromConfFile("sizefirmware").toInt();
          MAGELLAN_MQTT_device_core::OTA_info.firmwareTotalSize = buffReadSizefirmware;
          attr.fw_total_size = buffReadSizefirmware;
          MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion = configOTAFile.readSpacificFromConfFile("versionfirmware");
          MAGELLAN_MQTT_device_core::OTA_info.checksum = configOTAFile.readSpacificFromConfFile("checksum");
          MAGELLAN_MQTT_device_core::OTA_info.checksumAlgorithm = configOTAFile.readSpacificFromConfFile("checksumAlgorithm");

          unsigned int b_cal_total_chunk = (MAGELLAN_MQTT_device_core::OTA_info.firmwareTotalSize / attr.chunk_size) + 1;
          ;
          attr.totalChunk = b_cal_total_chunk;

          if (!attr.isFirmwareUptodate)
          {
#ifdef ESP8266
            size_t offset_size = 100;
            Update.begin(MAGELLAN_MQTT_device_core::OTA_info.firmwareTotalSize + offset_size);
#endif
            Serial.println("# Estimate OTA total chunk : " + String(attr.totalChunk));
            pub_UpdateProgress("INITIALIZE", "{\"description\":\"Firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + "\"}");
            MAGELLAN_MQTT_device_core::OTA_info.isReadyOTA = true;
            Serial.println(F("========================================"));
            Serial.println(F("# Firmware OTA Information Available #"));
            Serial.println("  ->Firmware version: " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion);
            Serial.println("  ->Firmware checksum: " + MAGELLAN_MQTT_device_core::OTA_info.checksum);
            Serial.println(F("========================================"));

            // save Client config when firmware change and file ota config change

            attr.remind_Event_GET_FW_infoOTA = false;
          }
        }
      }
    }
    else
    {
      Serial.println(F("# Fail to get firmware Information or you don't have firmware OTA"));
      Serial.println("# Detail: " + payload);
    }
  }
}

String ERORRdescriptionUpdate()
{
#ifdef ESP32
  switch (Update.getError())
  {
  case 0:
    return "UPDATE_ERROR_OK";
    break;
  case 1:
    return "UPDATE_ERROR_WRITE";
    break;
  case 2:
    return "UPDATE_ERROR_ERASE";
    break;
  case 3:
    return "UPDATE_ERROR_READ";
    break;
  case 4:
    return "UPDATE_ERROR_SPACE";
    break;
  case 5:
    return "UPDATE_ERROR_SIZE";
    break;
  case 6:
    return "UPDATE_ERROR_STREAM";
    break;
  case 7:
    return "UPDATE_ERROR_MD5";
    break;
  case 8:
    return "UPDATE_ERROR_MAGIC_BYTE";
    break;
  case 9:
    return "UPDATE_ERROR_ACTIVATE{firmware is mismatch this board}";
    break;
  case 10:
    return "UPDATE_ERROR_NO_PARTITION";
    break;
  case 11:
    return "UPDATE_ERROR_BAD_ARGUMENT";
    break;
  case 12:
    return "UPDATE_ERROR_ABORT";
    break;
  default:
    return "ERROR_UNKNOWN";
    break;
  }
#elif defined ESP8266
  switch (Update.getError())
  {
  case 0:
    return "UPDATE_ERROR_OK";
    break;
  case 1:
    return "UPDATE_ERROR_WRITE";
    break;
  case 2:
    return "UPDATE_ERROR_ERASE";
    break;
  case 3:
    return "UPDATE_ERROR_READ";
    break;
  case 4:
    return "UPDATE_ERROR_SPACE";
    break;
  case 5:
    return "UPDATE_ERROR_SIZE";
    break;
  case 6:
    return "UPDATE_ERROR_STREAM";
    break;
  case 7:
    return "UPDATE_ERROR_MD5";
    break;
  case 8:
    return "UPDATE_ERROR_FLASH_CONFIG";
    break;
  case 9:
    return "UPDATE_ERROR_NEW_FLASH_CONFIG";
    break;
  case 10:
    return "UPDATE_ERROR_MAGIC_BYTE";
    break;
  case 11:
    return "UPDATE_ERROR_BOOTSTRAP";
    break;
  case 12:
    return "UPDATE_ERROR_SIGN";
    break;
  case 13:
    return "UPDATE_ERROR_NO_DATA";
    break;
  default:
    return "ERROR_UNKNOWN";
    break;
  }
#endif
}

void validate_lostOTA_Data_incoming()
{
  if (attr.current_chunk + 1 < attr.totalChunk)
  {
    if (attr.incomingChunkSize < attr.chunk_size)
    {
      Serial.println(F("# [Warning]Lost some data while in process OTA"));
      Serial.println(F("# [Warning]Device must restart"));
      pub_UpdateProgress("FAILED", "{\"errordescription\":\"Downloading firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + " is incorrect or lost data\"}");
      configOTAFile.saveSuccessOrFail("fail");

      delay(5000);
      ESP.restart();
    }
  }
}

void updateFirmware(uint8_t *data, size_t len)
{
  Update.write(data, len);
  attr.current_size += len;
  attr.incomingChunkSize = (int)len;
  Serial.println("# <-Incoming chunk size: " + String(attr.incomingChunkSize));
  unsigned int buffer_crrSize = attr.current_size;
  unsigned int calc_percent = map(buffer_crrSize, 0, attr.fw_total_size, 0, 100);
  Serial.println("# <-Current firmware size: " + String(buffer_crrSize) + "/" + String(attr.fw_total_size) + " => [" + String(calc_percent) + " %]");
  validate_lostOTA_Data_incoming();
  if (attr.current_size != attr.fw_total_size)
  {
    return;
  }

  if (Update.end(true))
  {
    pub_UpdateProgress("DOWNLOADED", "{\"description\":\"Firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + "\"}");
    if (attr.using_Checksum)
    {
      pub_UpdateProgress("VERIFIED", "");
    }
    Serial.println(F("-------------------------------"));
    Serial.println(F("# OTA done!"));
    if (Update.isFinished())
    {
      Serial.println(F("# Update successfully completed. Rebooting."));
      configOTAFile.saveSuccessOrFail("done");
      String readfileConfig = configOTAFile.readConfigFileOTA();
      configOTAFile.saveLastedOTA(readfileConfig);

      String fw_infoInFIleSys;
      JsonObject fw_last = configOTAFile.readObjectLastedOTA();
      int bufferFW_size = fw_last["sizefirmware"];

      fw_last.remove("namefirmware");
      fw_last.remove("sizefirmware");
      fw_last.remove("checksumAlgorithm");
      String bufferFW_v = fw_last["versionfirmware"];
      fw_last["firmwareVersion"] = bufferFW_v;
      fw_last.remove("versionfirmware");

      pub_UpdateProgress("UPDATED", "{\"Version\":\"" + String(bufferFW_v) + "\"}");

      serializeJson(fw_last, fw_infoInFIleSys);
      if (fw_infoInFIleSys.indexOf("null") == -1)
      {
        pubClientConfig(fw_infoInFIleSys);
      }
      else if ((bufferFW_v.length() > 4 || bufferFW_v.indexOf("null") == -1) && (fw_infoInFIleSys.indexOf("null") != -1)) // handle if fw version !null but some key value found null is still pub client config
      {
        pubClientConfig(fw_infoInFIleSys);
      }

      else if ((bufferFW_v.indexOf("null") != -1) && (bufferFW_size > 0))
      {
        pubClientConfig(fw_infoInFIleSys);
      }

      // write new data only success OTA
      // Serial.println("#Debug: "+ configOTAFile.readConfigFileOTA());
      delay(5000);
      ESP.restart();
    }
    else
    {
      pub_UpdateProgress("FAILED", "{\"errordescription\":\"Firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + " Something went wrong (UNKNOWN ERROR)\"}");

      configOTAFile.saveSuccessOrFail("fail");

      Serial.println(F("# Update not finished? Something went wrong!"));
    }
  }
  else
  {
    String error_des = ERORRdescriptionUpdate();
    Serial.println("# OTA Fail Error Occurred. Error #: " + error_des + " # Error Enum {" + String(Update.getError()) + "}");
    pub_UpdateProgress("FAILED", "{\"errordescription\":\"Firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + " : " + error_des + "\"}");
    configOTAFile.saveSuccessOrFail("fail");
  }
  delay(5000);
  ESP.restart();
}

void hook_FW_download(String topic, uint8_t *payload, unsigned int length)
{
  // Serial.println("Debug in HOOK topic: "+String(topic));
  // Serial.println("Debug in HOOK length: "+String(length));
  if (topic.indexOf("/firmwaredownload/resp/") != -1)
  {
    int index = topic.indexOf("/resp/");
    String crrnt_part = topic.substring(index + 6); // crrnt_part is part start from index 0
    attr.current_chunk = crrnt_part.toInt();
    Serial.println(F("<--------------------------------"));
    Serial.println("# <-Firmware current chunk: " + String(attr.current_chunk + 1) + "/" + String(attr.totalChunk));
    if (length > 0 && (attr.fw_count_chunk <= attr.totalChunk))
    {
      attr.checkTimeout_request_download_fw = false;
      attr.prv_cb_timeout_millis = millis();
      updateFirmware(payload, length);
      attr.fw_count_chunk++;
      attemp_download_1 = false;
      attemp_download_2 = false;

      if (attr.delayRequest_download > 0)
      {
        delay(attr.delayRequest_download);
        Serial.println("# Using delay download every chunk in: " + String(attr.delayRequest_download) + " ms.");
        pub_Download(attr.fw_count_chunk, attr.chunk_size);
      }
      else
      {
        pub_Download(attr.fw_count_chunk, attr.chunk_size);
      }

      if (!attr.inProcessOTA)
      {

        pub_UpdateProgress("DOWNLOADING", "{\"description\":\"Firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + "\"}");
      }
      attr.inProcessOTA = true;
      MAGELLAN_MQTT_device_core::OTA_info.inProcessOTA = attr.inProcessOTA;
    }
    if (attr.fw_count_chunk == attr.totalChunk)
    {
      if (attr.current_size != attr.fw_total_size)
      {
        Serial.println(F("#[Warning] Complete Request total of chunk but lost or incorrect DATA from OTA"));
        Serial.println(F("#[Warning] Must restart board"));
        pub_UpdateProgress("FAILED", "{\"errordescription\":\"Downloaded firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + " is incorrect or lost data\"}");

        configOTAFile.saveSuccessOrFail("fail");

        delay(3000);
        ESP.restart();
      }
    }
  }
}

/////////// Feature OTA function none member inclass //////////////////////

// ver.1.1.0
//  @attention function compare between MsgId_cb(RCV from cb) and MsgId_send  when using reqRetransmission
void checkRequestRetransmit()
{
  if (attr.reqRetransmit)
  {
    attr.isMatchMsgId = (attr.matchMsgId_send == attr.matchMsgId_cb) ? true : false;
    if (attr.isMatchMsgId)
    {
      Serial.print(F("\n# ========================================\n"));
      Serial.println("# Received MsgId: " + String(attr.matchMsgId_send) + " retransmit from server");
      Serial.print(F("# ========================================\n\n"));
      attr.reqRetransmit = false;
    }
  }
}

void msgCallback_internalHandler(char *topic, byte *payload, unsigned int length)
{
  String action = "ERROR";
  unsigned int buffEvent = ERROR;
  String b_topic = String(topic);
  String _payload = b2str(payload, length);
  String key = "null"; // if this topic is'nt PLAINTEXT
  String code = "0";
  int _MsgId = -1;

  EVENTS intern_EVENT;
  intern_EVENT.RESP = "EMPTY";

  regisAPI *handleRegisPTA = _startRegisPTA;
  regisAPI *handleRegisJSON = _startRegisJSON;
  regisAPI *handleRegisJSON_CTRL_OBJ = _startOBJ_CTRL;
  regisAPI *handleRegisKEY = _startRegis;

  regisAPI *handleRegisPTAConf = _startRegisPTAConf;
  regisAPI *handleRegisJSONConf = _startRegisJSONConf;
  regisAPI *handleRegisJSON_CONF_OBJ = _startOBJ_CONF;
  regisAPI *handleRegisKEYConf = _startRegisConf;

  regisAPI *handleRESP = _startRESP;
  // OTA Feature /////////////////

  if (attr.usingCheckUpdate && (!attr.inProcessOTA))
  {
    checkUpdate(b_topic, _payload);
  }
  else if (!attr.usingCheckUpdate && attr.flagAutoOTA)
  {
    save_fw_info(b_topic, _payload);
  }
  hook_FW_download(b_topic, payload, length);

  ///////////////////////////////
  char *b_payload = (char *)_payload.c_str(); // payload for advance_cb and endpoint centric

  if (b_topic.indexOf("/auth/resp/") != -1)
  {
    buffEvent = TOKEN;
    action = "TOKEN";
    attr.ext_Token = *&_payload;
  }
  if (b_topic.indexOf("/delta/resp/pta") != -1)
  {
    int indexfound2 = String(b_topic).indexOf("=");
    String keyOnTopic = b_topic.substring(indexfound2 + 1);
    key = keyOnTopic;
    buffEvent = CONTROL_PLAINTEXT;
    action = "CONTROL_PLAINTEXT";

    if ((_payload == "40300" || (_payload == "40400") && (_payload.length() == 5)))
    {
      code = _payload;
      intern_EVENT.RESP = "FAIL";
    }
    else
    {
      code = "20000";
      intern_EVENT.RESP = "SUCCESS";
    }

    if (attr.ctrl_regis_key)
    {
      while (handleRegisKEY != NULL)
      {
        if (handleRegisKEY->registerKey == key)
        {
          break;
        }
        else
        {
          handleRegisKEY = (regisAPI *)handleRegisKEY->next;
        }
      }
      if (handleRegisKEY != NULL)
      {
        if (handleRegisKEY->registerKey == key)
        {
          handleRegisKEY->ctrl_key_callback(_payload);
        }
      }
    }
    if (attr.ctrl_regis_pta)
    {
      while (handleRegisPTA != NULL)
      {
        if (handleRegisPTA->Event == buffEvent)
        {
          break;
        }
        else
        {
          handleRegisPTA = (regisAPI *)handleRegisPTA->next;
        }
      }
      if (handleRegisPTA != NULL)
      {
        handleRegisPTA->ctrl_pta_callback(key, _payload);
      }
    }
  }

  else if (b_topic.indexOf("/delta/resp") != -1)
  {
    buffEvent = CONTROL_JSON;
    action = "CONTROL_JSON";

    if (_payload.indexOf("20000") != -1)
    {
      code = _payload.substring(_payload.indexOf("\":\"") + 3, _payload.indexOf("\":\"") + 8);
      intern_EVENT.RESP = "SUCCESS";
      // Serial.println("test CODE ->"+ String(code) +" RESP :"+ String(intern_EVENT.RESP));
    }
    else
    {
      code = _payload.substring(_payload.indexOf("\":\"") + 3, _payload.length() + 8);
      intern_EVENT.RESP = "FAIL";
      // Serial.println("test CODE ->"+ String(code) +" RESP :"+ String(intern_EVENT.RESP));
    }

    if (attr.ctrl_regis_json)
    {
      while (handleRegisJSON != NULL)
      {
        if (handleRegisJSON->Event == buffEvent)
        {
          break;
        }
        else
        {
          handleRegisJSON = (regisAPI *)handleRegisJSON->next;
        }
      }
      if (handleRegisJSON != NULL)
      {
        handleRegisJSON->ctrl_Json_callback(_payload);
      }
    }

    if (attr.ctrl_jsonOBJ)
    {
      while (handleRegisJSON_CTRL_OBJ != NULL)
      {
        if (handleRegisJSON_CTRL_OBJ->Event == buffEvent)
        {
          break;
        }
        else
        {
          handleRegisJSON_CTRL_OBJ = (regisAPI *)handleRegisJSON_CTRL_OBJ->next;
        }
      }
      if (handleRegisJSON_CTRL_OBJ != NULL)
      {
        // Serial.println(_payload);
        String buffDocs = deControl(_payload);
        // Serial.println(buffDocs);
        JsonObject Docs = deJson(buffDocs);
        handleRegisJSON_CTRL_OBJ->ctrl_obj_callback(Docs);
      }
    }
  }

  if (b_topic.indexOf("/config/resp/pta/?config=") != -1)
  {
    int indexfound2 = String(b_topic).indexOf("=");
    String keyOnTopic = b_topic.substring(indexfound2 + 1);
    key = keyOnTopic;
    buffEvent = CONFIG_PLAINTEXT;
    action = "CONFIG_PLAINTEXT";

    if ((_payload == "40300" || (_payload == "40400") && (_payload.length() == 5)))
    {
      code = _payload;
      intern_EVENT.RESP = "FAIL";
    }
    else
    {
      code = "20000";
      intern_EVENT.RESP = "SUCCESS";
    }

    if (attr.conf_regis_key)
    {
      while (handleRegisKEYConf != NULL)
      {
        if (handleRegisKEYConf->registerKey == key)
        {
          break;
        }
        else
        {
          handleRegisKEYConf = (regisAPI *)handleRegisKEYConf->next;
        }
      }
      if (handleRegisKEYConf != NULL)
      {
        if (handleRegisKEYConf->registerKey == key)
        {
          handleRegisKEYConf->conf_key_callback(_payload);
        }
      }
    }

    if (attr.conf_regis_pta)
    {
      while (handleRegisJSON != NULL)
      {
        if (handleRegisPTAConf->Event == buffEvent)
        {
          break;
        }
        else
        {
          handleRegisPTAConf = (regisAPI *)handleRegisPTAConf->next;
        }
      }
      if (handleRegisPTAConf != NULL)
      {
        handleRegisPTAConf->conf_pta_callback(key, _payload);
      }
    }
  }
  else if (b_topic.indexOf("/config/resp") != -1)
  {
    buffEvent = CONFIG_JSON;
    action = "CONFIG_JSON";

    if (_payload.indexOf("20000") != -1)
    {
      code = _payload.substring(_payload.indexOf("\":\"") + 3, _payload.indexOf("\":\"") + 8);
      intern_EVENT.RESP = "SUCCESS";
      // Serial.println("test CODE ->"+ String(code) +" RESP :"+ String(intern_EVENT.RESP));
    }
    else
    {
      code = _payload.substring(_payload.indexOf("\":\"") + 3, _payload.length() + 8);
      intern_EVENT.RESP = "FAIL";
      // Serial.println("test CODE ->"+ String(code) +" RESP :"+ String(intern_EVENT.RESP));
    }

    if (attr.conf_regis_json)
    {
      while (handleRegisJSONConf != NULL)
      {
        if (handleRegisJSONConf->Event == buffEvent)
        {
          break;
        }
        else
        {
          handleRegisJSONConf = (regisAPI *)handleRegisJSONConf->next;
        }
      }
      if (handleRegisJSONConf != NULL)
      {
        handleRegisJSONConf->conf_json_callback(_payload);
      }
    }

    if (attr.conf_jsonOBJ)
    {
      while (handleRegisJSON_CONF_OBJ != NULL)
      {
        if (handleRegisJSON_CONF_OBJ->Event == buffEvent)
        {
          break;
        }
        else
        {
          handleRegisJSON_CONF_OBJ = (regisAPI *)handleRegisJSON_CONF_OBJ->next;
        }
      }
      if (handleRegisJSON_CONF_OBJ != NULL)
      {
        // Serial.println(_payload);
        String buffDocs = deConfig(_payload);
        // Serial.println(buffDocs);
        JsonObject Docs = deJson(buffDocs);
        handleRegisJSON_CONF_OBJ->conf_obj_callback(Docs);
      }
    }
  }

  if (b_topic.indexOf("dateTime") != -1)
  {
    buffEvent = UNIXTIME;
    action = "UNIXTIME";
  }
  if (b_topic.indexOf("/report/resp/pta/?") != -1)
  {
    buffEvent = RESP_REPORT_PLAINTEXT;
    action = "RESP_REPORT_PLAINTEXT";

    int valid_found_only_msgId = b_topic.indexOf("?id="); // only id
    int valid_sensor = b_topic.indexOf("?sensor=");       // focus only sensor
    int valid_msgId = b_topic.indexOf("&id=");            // start with key sensor and id

    if (valid_found_only_msgId != -1) // found only msg id {?id}
    {
      int indexfound = valid_found_only_msgId;
      String msgIdOnTopic = b_topic.substring(indexfound + 4);
      _MsgId = msgIdOnTopic.toInt();
    }

    if (valid_msgId != -1) // found MsgId
    {
      int indexfoundKey = valid_sensor;
      int indexfoundId = valid_msgId;
      String keyOnTopic = b_topic.substring(indexfoundKey + 8, indexfoundId);
      String msgIdOnTopic = b_topic.substring(indexfoundId + 4);

      key = keyOnTopic;
      _MsgId = msgIdOnTopic.toInt();
    }
    else if (valid_msgId == -1 && valid_sensor != -1) // not found MsgId , found only key sensor
    {
      String keyOnTopic = b_topic.substring(valid_sensor + 8);
      key = keyOnTopic;
      _MsgId = -1;
    }

    if (_payload.indexOf("20000") != -1)
    {
      code = _payload;
      intern_EVENT.RESP = "SUCCESS";
    }
    else
    {
      code = _payload;
      intern_EVENT.RESP = "FAIL";
    }
  }
  else if (b_topic.indexOf("/report/resp") != -1)
  {
    buffEvent = RESP_REPORT_JSON;
    action = "RESP_REPORT_JSON";

    JsonObject OBJ_report_resp = deJson(_payload);
    String _code = OBJ_report_resp["Code"];

    if (_code != "null")
    {
      code = _code;
    }

    if (_payload.indexOf("id") != -1)
    {
      String _msg_id = OBJ_report_resp["id"];
      if (_msg_id != "null")
      {
        _MsgId = _msg_id.toInt();
      }
    }
    else
    {
      _MsgId = -1;
    }

    if (_payload.indexOf("20000") != -1)
    {
      intern_EVENT.RESP = "SUCCESS";
    }
    else
    {
      intern_EVENT.RESP = "FAIL";
    }
  }

  if (b_topic.indexOf("/heartbeat/resp/pta") != -1)
  {
    buffEvent = RESP_HEARTBEAT_PLAINTEXT;
    action = "RESP_HEARTBEAT_PLAINTEXT";

    if (_payload.indexOf("20000") != -1)
    {
      code = _payload;
      intern_EVENT.RESP = "SUCCESS";
    }
    else
    {
      code = _payload;
      intern_EVENT.RESP = "FAIL";
    }
  }
  else if (b_topic.indexOf("/heartbeat/resp") != -1)
  {
    buffEvent = RESP_HEARTBEAT_JSON;
    action = "RESP_HEARTBEAT_JSON";

    if (_payload.indexOf("20000") != -1)
    {
      code = _payload.substring(_payload.indexOf("\":\"") + 3, _payload.length() - 2);
      intern_EVENT.RESP = "SUCCESS";
      // Serial.println("test CODE ->"+ String(code) +" RESP :"+ String(intern_EVENT.RESP));
    }
    else
    {
      code = _payload.substring(_payload.indexOf("\":\"") + 3, _payload.length() - 2);
      intern_EVENT.RESP = "FAIL";
      // Serial.println("test CODE ->"+ String(code) +" RESP :"+ String(intern_EVENT.RESP));
    }
  }
  if (b_topic.indexOf("/report/timestamp/resp") != -1)
  {
    buffEvent = RESP_REPORT_TIMESTAMP;
    action = "RESP_REPORT_TIMESTAMP";
    if (_payload.indexOf("20000") != -1)
    {
      code = _payload.substring(_payload.indexOf("\":\"") + 3, _payload.length() - 2);
      intern_EVENT.RESP = "SUCCESS";
      // Serial.println("test CODE ->"+ String(code) +" RESP :"+ String(intern_EVENT.RESP));
    }
    else
    {
      code = _payload.substring(_payload.indexOf("\":\"") + 3, _payload.length() - 2);
      intern_EVENT.RESP = "FAIL";
      // Serial.println("test CODE ->"+ String(code) +" RESP :"+ String(intern_EVENT.RESP));
    }
  }
  if (b_topic.indexOf("/server/destination/response") != -1)
  {
    buffEvent = GET_ENDPOINT;
    action = "GET_ENDPOINT";
    attr.ext_EndPoint = b_payload;
  }
  // Serial.println("#INSIDE ");
  // Serial.println(action);
  // Serial.println(_payload);
  // Serial.println("#INSIDE ");

  intern_EVENT.CODE = code.toInt();
  intern_EVENT.Topic = b_topic;
  intern_EVENT.Payload = _payload;
  intern_EVENT.PayloadLength = length;
  intern_EVENT.Action = action;
  intern_EVENT.Key = key;
  intern_EVENT.MsgId = _MsgId;
  attr.matchMsgId_cb = intern_EVENT.MsgId;
  checkRequestRetransmit();

  if (ext_useAdvanceCallback) // routing to onMessage Callback when use setMessageListener(Callback)
  {
    cb_internal(intern_EVENT, b_payload);
  }

  if (attr.resp_regis)
  {
    while (handleRESP != NULL)
    {
      if (handleRESP->Event == buffEvent)
      {
        break;
      }
      else
      {
        handleRESP = (regisAPI *)handleRESP->next;
      }
    }
    // Serial.println("# IN RESP :"+ String(handleRESP->Event));
    // Serial.println("# IN RESP2 :"+ String(buffEvent));
    if (handleRESP != NULL)
    {
      if (handleRESP->Event == buffEvent)
      {
        handleRESP->resp_h_callback(intern_EVENT);
      }
    }
  }
  // Serial.println("#DEBUG INSIDE :"+intern_EVENT.Action);
  // Serial.println("#DEBUG INSIDE :"+intern_EVENT.Payload);
}

MAGELLAN_MQTT_device_core::MAGELLAN_MQTT_device_core(Client &c)
{

  prev_time = 0;
  now_time = millis();
  HB_prev_time = 0;
  HB_now_time = millis();

  Client *newClient = &c;
  attr.ClientNET = *&newClient;
  attr.mqtt_client = new PubSubClient(*attr.ClientNET);
  this->client = *&attr.mqtt_client;
}

String MAGELLAN_MQTT_device_core::getHostName()
{
  return this->host;
}

void MAGELLAN_MQTT_device_core::setMQTTBufferSize(uint16_t sizeBuffer)
{
  // Serial.println(F("# SetBufferSize: "));
  // Serial.println(sizeBuffer);
  this->_default_bufferSize = sizeBuffer;
}

void MAGELLAN_MQTT_device_core::setAuthMagellan(String _thingIden, String _thingSecret, String _imei)
{
  Serial.println(F("#====== Setting Magellan Authentication ======="));
  if (!(CheckString_isDigit(_thingIden) && CheckString_isDigit(_thingSecret)))
  {
    Serial.print(F("# ERROR Can't connect to Magellan"));
    Serial.print(F("# Parameter from you setting invalid \n [thingIdentifier]=> "));
    Serial.print(_thingIden);
    Serial.print(F("   [thingSecret]=> "));
    Serial.println(_thingSecret);
    Serial.println(F("# Invalid Parameter!! Please check [thingIdentifier] and [thingSecret]"));
    while (true)
    {
      Serial.print(".");
      delay(300);
      this->cnt_fail++;
      if (cnt_fail >= 100) // timeout Restart board 30 sec
      {
        ESP.restart();
      }
    }
  }
  this->thingIden = _thingIden;
  this->thingSecret = _thingSecret;
  this->imei = _imei;
}

String MAGELLAN_MQTT_device_core::readToken()
{
  return this->token;
}

String MAGELLAN_MQTT_device_core::readThingIdentifier()
{
  return this->thingIden;
}
String MAGELLAN_MQTT_device_core::readThingSecret()
{
  return this->thingSecret;
}

boolean MAGELLAN_MQTT_device_core::CheckString_isDigit(String valid_payload)
{
  for (byte i = 0; i < valid_payload.length(); i++)
  {
    if (!isDigit(valid_payload.charAt(i)))
      return false;
  }
  return true;
}

boolean MAGELLAN_MQTT_device_core::CheckString_isDouble(String valid_payload)
{
  char *input = (char *)valid_payload.c_str();
  char *end;
  strtod(input, &end);
  if (*input == '\0')
  {
    return false;
  }
  if (end == input || *end != '\0')
  {
    return false;
  }
  return true;
}

void MAGELLAN_MQTT_device_core::setCallback_msgHandle()
{
  this->client->setCallback(msgCallback_internalHandler);
}

void MAGELLAN_MQTT_device_core::setMessageListener(void (*callback)(EVENTS, char *))
{
  attr.useAdvanceCallback = true;
  if (callback)
    cb_internal = callback;
}

void MAGELLAN_MQTT_device_core::reconnect()
{
  while (!isConnected())
  {
    Serial.print(F("Device Disconected from Magellan..."));
    checkConnection();
    if (flagToken)
    {
      Serial.print(F("# Remain Subscribes list\n"));
      attr.triggerRemainSub = true;

      attr.triggerRemainOTA = true;
      if (!attr.flagAutoOTA)
      {
        sub_InfoOTA();
      }
    }
  }
}

void MAGELLAN_MQTT_device_core::acceptToken(String payload)
{
  if (payload.length() >= 36)
  {
    this->flagToken = true;
    this->token = payload;
    Serial.println("# Thingtoken: " + token);
  }
}

void MAGELLAN_MQTT_device_core::acceptToken(EVENTS event)
{
  String _payload = event.Payload;
  if ((event.Topic == "api/v2/thing/" + String(thingIden) + "/" + String(thingSecret) + "/auth/resp/pta") && !flagToken)
  {
    if (_payload.length() >= 36)
    {
      this->flagToken = true;
      this->token = _payload;
      Serial.println("# Token >> :" + token);
    }
  }
}

String MAGELLAN_MQTT_device_core::byteToString(byte *payload, unsigned int length_payload)
{
  return b2str(payload, length_payload);
}

void MAGELLAN_MQTT_device_core::loop()
{
  this->client->loop();
}

void MAGELLAN_MQTT_device_core::reconnectMagellan()
{
  // DEBUG_SERIAL("@THIS DEBUG");
  // ERROR_SERIAL("#THIS ERROR");
  // INFO_SERIAL("%THIS INFO");

  while (!isConnected())
  {
    srand(time(NULL));
    int randnum = rand() % 10000;   // generate number concat in Client id
    int randnum_2 = rand() % 10000; // generate number concat in Client id
    String client_idBuff = client_id + "_" + String(randnum) + "_" + String(randnum_2);
    Serial.println(F("Attempting MQTT connection ..."));
    this->client->setServer(this->host.c_str(), this->port);
    this->client->setCallback(msgCallback_internalHandler);
    Serial.println("Connecting Magellan on: " + String(this->host) + ", Port: " + String(this->port));
    if (this->client->connect(client_idBuff.c_str(), this->thingIden.c_str(), this->thingSecret.c_str()))
    {
      Serial.println("Client id: " + client_idBuff + " is connected");
      recon_attempt = 0;
    }
    else
    {
      Serial.print(F("failed, reconnect ="));
      Serial.print(this->client->state());
      Serial.println(F(" try again in 5 seconds"));
      if (!flagToken)
      {
        Serial.println(F("# Please check the thing device is activated "));
      }
      delay(5000);
      recon_attempt++;
      Serial.print(F("# attempt connect on :"));
      Serial.println(String(recon_attempt) + " times");
      if (recon_attempt >= MAXrecon_attempt)
      {
        Serial.println(" attempt to connect more than " + String(MAXrecon_attempt) + " Restart Board");
        ESP.restart();
      }
    }
  }
  thingRegister();
}

void MAGELLAN_MQTT_device_core::checkConnection()
{
  if (!isConnected())
  {
    reconnectMagellan();
  }
}

bool MAGELLAN_MQTT_device_core::isConnected()
{
  return this->client->connected();
}

void MAGELLAN_MQTT_device_core::beginCustom(String _client_id, String _host, int _port, uint16_t bufferSize)
{
  Serial.println("=================== Begin MAGELLAN Library  " + String(lib_version) + " ===================");

  delay(5000);
  this->host = _host;
  this->port = _port;
  this->client_id = _client_id;
  if (bufferSize > _default_OverBufferSize)
  {
    Serial.print(F("# Buffer size from you set over than 8192 set buffer to: "));
    Serial.println();
    this->setBufferSize(_default_OverBufferSize);
    attr.calculate_chunkSize = _default_bufferSize / 2;
  }
  else
  {
    this->setBufferSize(bufferSize);
    attr.calculate_chunkSize = bufferSize / 2;
  }
  if (this->thingIden != NULL && this->thingSecret != NULL)
    checkConnection();
  else
  {
    Serial.println(F("# ThingIdentifier or ThingSecret is incorrect please check"));
    Serial.println(F("# can't connect to server"));
    Serial.println("# ThingIdentifier: " + this->thingIden + "\nThingSecret: " + this->thingSecret);
    delay(5000);
    ESP.restart();
  }
}

void MAGELLAN_MQTT_device_core::begin(String _thingIden, String _thingSecret, String _imei, unsigned int Zone, uint16_t bufferSize)
{
  setAuthMagellan(_thingIden, _thingSecret, _imei);
  Serial.print(F("ThingIdentifier: "));
  Serial.println(_thingIden);
  Serial.print(F("ThingSecret: "));
  Serial.println(_thingSecret);
  Serial.print(F("IMEI: "));
  Serial.println(_imei);
  if (_thingIden != NULL && _thingSecret != NULL)
    begin(_thingIden, Zone, bufferSize);
  else
  {
    Serial.println(F("# ThingIdentifier or ThingSecret is incorrect please check"));
    Serial.println(F("# can't connect to server"));
    Serial.println("# ThingIdentifier: " + _thingIden + "\nThingSecret: " + _thingSecret);
    delay(5000);
    ESP.restart();
  }
}

void MAGELLAN_MQTT_device_core::begin(String _client_id, unsigned int Zone, uint16_t bufferSize)
{
  Serial.println("=================== Begin MAGELLAN Library  " + String(lib_version) + " ===================");
  delay(5000);
  String _host = _host_production;
  switch (Zone)
  {
  case Production:
    _host = _host_production;
    break;
  default:
    _host = _host_production;
    Serial.println(F("# OUT OF LENGTH ZONE MOVE TO Production"));
    break;
  }
  this->host = _host;
  this->port = mgPort;
  this->client_id = _client_id;
  if (bufferSize > _default_OverBufferSize)
  {
    Serial.print(F("# Buffer size from you set over than 8192 set buffer to: "));
    Serial.println();
    this->setBufferSize(_default_OverBufferSize);
    attr.calculate_chunkSize = _default_bufferSize / 2;
  }
  else
  {
    this->setBufferSize(bufferSize);
    attr.calculate_chunkSize = bufferSize / 2;
  }
  if (this->thingIden != NULL && this->thingSecret != NULL)
    checkConnection();
  else
  {
    Serial.println(F("# ThingIdentifier or ThingSecret is incorrect please check"));
    Serial.println(F("# can't connect to server"));
    Serial.println("# ThingIdentifier: " + this->thingIden + "\nThingSecret: " + this->thingSecret);
    delay(5000);
    ESP.restart();
  }
}

boolean MAGELLAN_MQTT_device_core::registerToken()
{
  String topic = "api/v2/thing/" + this->thingIden + "/" + this->thingSecret + "/auth/resp/pta";
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println(F("# Register Token to magellan"));
  Serial.println("# Register Token Status: " + _debug);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::report(String payload)
{

  String topic = "api/v2/thing/" + token + "/report/persist";
  boolean Pub_status = client->publish(topic.c_str(), payload.c_str());
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Report JSON: " + _debug);
  Serial.println("# [Sensors]: " + payload);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::report(String key, String value)
{
  String topic = "api/v2/thing/" + token + "/report/persist/pta/?sensor=" + key;
  boolean Pub_status = client->publish(topic.c_str(), value.c_str());
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Report Plaintext: " + _debug);
  Serial.println("# [key]: " + key);
  Serial.println("# [value]: " + value);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::reportSensor()
{
  String bufferPlayload = buildSensorJSON(*attr.docSensor);
  boolean Pub_status = false;
  if (bufferPlayload.indexOf("null") == -1)
  {
    Pub_status = report(bufferPlayload);
    clearSensorBuffer(*attr.docSensor);
  }
  else
  {
    Serial.println(F("# Can't reportSensor Because Not set function \"void addSensor(key,value)\" before reportSensor"));
  }
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::ACKControl(String key, String value)
{
  String topic = "api/v2/thing/" + token + "/report/persist/pta/?sensor=" + key;
  boolean Pub_status = this->client->publish(topic.c_str(), value.c_str());
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# ACKNOWNLEDGE Control Plaintext: " + _debug);
  Serial.println("# [key]: " + key);
  Serial.println("# [value]: " + value);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::ACKControl(String payload)
{
  String topic = "api/v2/thing/" + token + "/report/persist";
  boolean Pub_status = this->client->publish(topic.c_str(), payload.c_str());
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# ACKNOWNLEDGE Control JSON: " + _debug);
  Serial.println("# [Controls]: " + payload);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::reportTimestamp(String timestamp, String JSONpayload, unsigned int timestamp_type)
{
  boolean Pub_status = false;
  String topic_ = "api/v2/thing/" + token + "/report/timestamp/persist";
  if (timestamp != NULL && JSONpayload != NULL)
  {
    String payload_ = "[{\"UNIXTS\":" + timestamp + ",\"Sensor\":" + JSONpayload + "}]";
    Pub_status = this->client->publish(topic_.c_str(), payload_.c_str());
    _debug = (Pub_status == true) ? "Success" : "Failure";
    Serial.println("# Report with timestamp: " + _debug);
    Serial.println("# [Sensors]: " + payload_);
    return Pub_status;
  }
  else
  {
    Serial.println(F("# Report with timestamp: Failure"));
    Serial.println(F("# Error Empty timestamp or payload"));
  }
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::reportClientConfig(String payload)
{
  String topic = "api/v2/thing/" + token + "/config/persist";
  boolean Pub_status = this->client->publish(topic.c_str(), payload.c_str());
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Save ClientConfig: " + _debug);
  Serial.println("# [ClientConfigs]: " + payload);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::requestToken()
{
  boolean Pub_status = false;
  if (!flagToken)
  {
    if (cnt_attempt >= limit_attempt)
    {
      Serial.println("Device Attempt to request token more than " + String(limit_attempt) + " time. restart board");
      delay(1000);
      ESP.restart();
    }
    if (millis() - previouseMillis > 10000)
    {
      previouseMillis = millis();
      String topic = "api/v2/thing/" + thingIden + "/" + thingSecret + "/auth/req";
      Pub_status = this->client->publish(topic.c_str(), " ");
      _debug = (Pub_status == true) ? "Success" : "Failure";
      // Serial.println("topic :" + topic);
      Serial.print("# Request Token: " + _debug);
      if (cnt_attempt > 0)
      {
        Serial.print(" Attempt >> " + String(cnt_attempt - 1) + " time");
      }
      Serial.println();
      cnt_attempt++;
    }
  }
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::setBufferSize(uint16_t size)
{
  Serial.println("# set BufferSize: " + String(size));
  return this->client->setBufferSize(size);
}

boolean MAGELLAN_MQTT_device_core::heartbeat()
{
  String topic = "api/v2/thing/" + token + "/heartbeat";
  boolean Pub_status = this->client->publish(topic.c_str(), " ");
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Heartbeat Trigger: " + _debug);
  return Pub_status;
}

boolean firstHBdoing = true;
void MAGELLAN_MQTT_device_core::heartbeat(unsigned int triger_ms)
{
  HB_threshold_ms = triger_ms;
  HB_now_time = millis();
  unsigned long different_ms = HB_now_time - HB_prev_time;
  if (different_ms >= HB_threshold_ms || firstHBdoing)
  {
    firstHBdoing = false;
    heartbeat();
    HB_prev_time = HB_now_time;
  }
}

void MAGELLAN_MQTT_device_core::setManualToken(String _token)
{
  Serial.println(F("# SET MANUAL TOKEN ====="));
  Serial.println("#Token: " + _token);
  _token.trim();
  if (_token.length() >= 36)
  {
    token = _token;
    acceptToken(token);
  }
}

boolean MAGELLAN_MQTT_device_core::reqControlJSON()
{

  String topic = "api/v2/thing/" + token + "/delta/req";
  boolean Pub_status = this->client->publish(topic.c_str(), " ");
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Request Control [JSON]: " + _debug);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::reqControl(String key)
{
  String topic = "api/v2/thing/" + token + "/delta/req/?sensor=" + key;
  boolean Pub_status = this->client->publish(topic.c_str(), " ");
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Request Control Plaintext by [Key]: \"" + key + "\": " + _debug);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::reqConfigJSON()
{
  String topic = "api/v2/thing/" + token + "/config/req";
  boolean Pub_status = this->client->publish(topic.c_str(), " ");
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Request Config [JSON]: " + _debug);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::reqConfig(String key)
{
  String topic = "api/v2/thing/" + token + "/config/req/?config=" + key; // fact C c
  boolean Pub_status = this->client->publish(topic.c_str(), " ");
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Request Config Plaintext [Key]: \"" + key + "\": " + _debug);
  return Pub_status;
}

void MAGELLAN_MQTT_device_core::getControl(String key, ctrl_handleCallback ctrl_callback)
{
  attr.ctrl_regis_key = true;
  regisAPI *newRegis = new regisAPI;
  newRegis->Event = CONTROL_PLAINTEXT;
  newRegis->registerKey = key;
  newRegis->ctrl_key_callback = ctrl_callback;
  newRegis->next = NULL;

  if (_startRegis != NULL)
  {
    regisAPI *focusRegis = _startRegis;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startRegis = newRegis;
  }
}

void MAGELLAN_MQTT_device_core::getControl(ctrl_PTAhandleCallback ctrl_pta_callback)
{
  attr.ctrl_regis_pta = true;

  regisAPI *newRegis = new regisAPI;
  newRegis->Event = CONTROL_PLAINTEXT;
  newRegis->ctrl_pta_callback = ctrl_pta_callback;
  newRegis->next = NULL;

  if (_startRegisPTA != NULL)
  {
    regisAPI *focusRegis = _startRegisPTA;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startRegisPTA = newRegis;
  }
}

void MAGELLAN_MQTT_device_core::getControlJSON(ctrl_Json_handleCallback ctrl_json_callback)
{
  attr.ctrl_regis_json = true;

  regisAPI *newRegis = new regisAPI;
  newRegis->Event = CONTROL_JSON;
  newRegis->ctrl_Json_callback = ctrl_json_callback;
  newRegis->next = NULL;

  if (_startRegisJSON != NULL)
  {
    regisAPI *focusRegis = _startRegisJSON;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startRegisJSON = newRegis;
  }
}

void MAGELLAN_MQTT_device_core::getControlJSON(ctrl_JsonOBJ_handleCallback jsonOBJ_cb)
{
  attr.ctrl_jsonOBJ = true;
  regisAPI *newRegis = new regisAPI;
  newRegis->Event = CONTROL_JSON;
  newRegis->ctrl_obj_callback = jsonOBJ_cb;
  newRegis->next = NULL;

  if (_startOBJ_CTRL != NULL)
  {
    regisAPI *focusRegis = _startOBJ_CTRL;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startOBJ_CTRL = newRegis;
  }
}

void MAGELLAN_MQTT_device_core::getConfig(String key, conf_handleCallback _conf_callback)
{
  attr.conf_regis_key = true;

  regisAPI *newRegis = new regisAPI;
  newRegis->Event = CONFIG_PLAINTEXT;
  newRegis->registerKey = key;
  newRegis->conf_key_callback = _conf_callback;
  newRegis->next = NULL;

  if (_startRegisConf != NULL)
  {
    regisAPI *focusRegis = _startRegisConf;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startRegisConf = newRegis;
  }
}

void MAGELLAN_MQTT_device_core::getConfig(conf_PTAhandleCallback conf_pta_callback)
{
  attr.conf_regis_pta = true;
  regisAPI *newRegis = new regisAPI;
  newRegis->Event = CONFIG_PLAINTEXT;
  newRegis->conf_pta_callback = conf_pta_callback;
  newRegis->next = NULL;

  if (_startRegisPTAConf != NULL)
  {
    regisAPI *focusRegis = _startRegisPTAConf;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startRegisPTAConf = newRegis;
  }
}

void MAGELLAN_MQTT_device_core::getConfigJSON(conf_Json_handleCallback conf_json_callback)
{
  attr.conf_regis_json = true;
  regisAPI *newRegis = new regisAPI;
  newRegis->Event = CONFIG_JSON;
  newRegis->conf_json_callback = conf_json_callback;
  newRegis->next = NULL;

  if (_startRegisJSONConf != NULL)
  {
    regisAPI *focusRegis = _startRegisJSONConf;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startRegisJSONConf = newRegis;
  }
}

void MAGELLAN_MQTT_device_core::getConfigJSON(conf_JsonOBJ_handleCallback jsonOBJ_cb)
{
  attr.conf_jsonOBJ = true;
  regisAPI *newRegis = new regisAPI;
  newRegis->Event = CONFIG_JSON;
  newRegis->conf_obj_callback = jsonOBJ_cb;
  newRegis->next = NULL;

  if (_startOBJ_CONF != NULL)
  {
    regisAPI *focusRegis = _startOBJ_CONF;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startOBJ_CONF = newRegis;
  }
}

void MAGELLAN_MQTT_device_core::getRESP(unsigned int resp_event, resp_callback resp_cb)
{
  attr.resp_regis = true;
  regisAPI *newRegis = new regisAPI;
  newRegis->Event = resp_event;
  newRegis->resp_h_callback = resp_cb;
  newRegis->next = NULL;

  if (_startRESP != NULL)
  {
    regisAPI *focusRegis = _startRESP;
    while (focusRegis->next != NULL)
    {
      focusRegis = (regisAPI *)focusRegis->next;
    }
    focusRegis->next = newRegis;
  }
  else
  {
    _startRESP = newRegis;
  }
}

boolean MAGELLAN_MQTT_device_core::registerResponseReport(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/thing/" + token + "/report/resp/pta/+";
    break;
  case 1:
    topic = "api/v2/thing/" + token + "/report/resp";
    break;
  default:
    Serial.println(F("out of length resp args format support [\"0\" or PLAINTEXT] is Plaint text(default) and [\"1\" or JSON]"));
    topic = "api/v2/thing/" + token + "/report/resp";
    break;
  }
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  // Serial.println("# RegisterRESP Report: "+ _debug);
  Serial.println("# Subscribe Response Report: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::registerResponseReportTimestamp()
{
  String topic = "api/v2/thing/" + token + "/report/timestamp/resp";
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  // Serial.println("# RegisterRESP ReportTimestamp: "+ _debug);
  Serial.println("# Subscribe Response ReportTimestamp: " + _debug);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::registerResponseHeartbeat(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/thing/" + token + "/heartbeat/resp/pta";
    break;
  case 1:
    topic = "api/v2/thing/" + token + "/heartbeat/resp";
    break;
  default:
    Serial.println(F("out of length resp args format support [\"0\" or PLAINTEXT] is Plaint text(default) and [\"1\" or JSON]"));
    topic = "api/v2/thing/" + token + "/heartbeat/resp";
    break;
  }
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  // Serial.println("# RegisterRESP Heartbeat: "+ _debug);
  Serial.println("# Subscribe Response Heartbeat: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::registerConfig(String key)
{
  String topic = "api/v2/thing/" + token + "/config/resp/pta/?config=" + key; // fact C c
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  // Serial.println("# Register Server Config [Key]: \""+key+"\" Register: "+ _debug);
  Serial.println("# Subscribe ServerConfig [Key]: \"" + key + "\" Subscribe: " + _debug);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::registerConfig(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/thing/" + token + "/config/resp/pta/+";
    break;
  case 1:
    topic = "api/v2/thing/" + token + "/config/resp";
    break;
  default:
    Serial.println(F("out of length resp args format support [\"0\" or PLAINTEXT] is Plaint text(default) and [\"1\" or JSON]"));
    topic = "api/v2/thing/" + token + "/config/resp";
    break;
  }
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  // Serial.println("# Register Server Config: "+ _debug);
  Serial.println("# Subscribe ServerConfig: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::getTimestamp()
{
  String topic = "api/v2/server/dateTime/req";
  boolean Pub_status = this->client->publish(topic.c_str(), " ");
  _debug = (Pub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Get ServerTime Request: " + _debug);
  return Pub_status;
}

boolean MAGELLAN_MQTT_device_core::registerTimestamp(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/server/dateTime/resp/pta";
    break;
  case 1:
    topic = "api/v2/server/dateTime/resp";
    break;
  default:
    Serial.println(F("out of length resp args format support [\"0\" or PLAINTEXT] is Plaint text(default) and [\"1\" or JSON]"));
    topic = "api/v2/server/dateTime/resp";
    break;
  }
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  Serial.println(F("# Subscribe Timestamp magellan"));
  // Serial.println(F("# RegisterTimestamp magellan"));
  Serial.println("# Subscribe ServerTime: " + _debug);
  // Serial.println("# RegisterTimestamp: "+ _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::registerControl(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/thing/" + token + "/delta/resp/pta/+";
    break;
  case 1:
    topic = "api/v2/thing/" + token + "/delta/resp";
    break;
  default:
    Serial.println(F("out of length resp args format support [\"0\" or PLAINTEXT] is Plaint text(default) and [\"1\" or JSON]"));
    topic = "api/v2/thing/" + token + "/delta/resp";
    break;
  }
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  // Serial.println("# RegisterControl: "+ _debug);
  Serial.println("# Subscribe Control: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::registerControl(String key)
{
  String topic = "api/v2/thing/" + token + "/delta/resp/pta/?sensor=" + key; // fact S s
  boolean Sub_status = this->client->subscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(topic);
  Serial.println(F("-------------------------------"));
  // Serial.println("# RegisterControl [Key]: \""+key+"\" Register: "+ _debug);
  Serial.println("# Subscribe Control [Key]: \"" + key + "\" Subscribe: " + _debug);
  return Sub_status;
}

void MAGELLAN_MQTT_device_core::thingRegister()
{
  while (!flagRegisterToken)
  {
    this->flagRegisterToken = registerToken();
  }
  while (!flagToken)
  {
    loop();
    requestToken();
    if (attr.ext_Token.length() >= 30)
    {
      token = attr.ext_Token;
      acceptToken(token);
    }
  }
}

void MAGELLAN_MQTT_device_core::registerList(func_callback_registerList cb_regisList)
{
  if (attr.triggerRemainSub)
  {
    if (attr.inProcessOTA)
    {
      Serial.println(F("# Subscribes List is terminated when Inprocess OTA"));
    }
    else
    {
      Serial.println(F("# Subscribes List"));
      cb_regisList();
      if (!attr.flagAutoOTA)
      {
        sub_InfoOTA();
      }
    }
    attr.triggerRemainSub = false;
    Serial.println(F("#============================"));
  }
}

boolean firstTimedoing = true;
void MAGELLAN_MQTT_device_core::interval_ms(unsigned long ms, func_callback_ms cb_ms)
{
  threshold_ms = ms;
  now_time = millis();
  unsigned long different_ms = now_time - prev_time;
  if (different_ms >= threshold_ms || firstTimedoing)
  {
    firstTimedoing = false;
    prev_time = millis();
    cb_ms();
  }
}

JsonObject MAGELLAN_MQTT_device_core::deserialJson(String jsonContent)
{
  JsonObject buffer;
  if (jsonContent != NULL && jsonContent != "clear")
  {
    DeserializationError error = deserializeJson(docJson, jsonContent);
    buffer = docJson.as<JsonObject>();
    if (error)
      Serial.println("# Error to DeserializeJson Control");
  }
  return buffer;
}

String MAGELLAN_MQTT_device_core::deserialControlJSON(String jsonContent)
{
  String content = "40300";
  JsonObject buffdoc = deserialJson(jsonContent);
  String statusCode = buffdoc["Code"];
  String buffDelta;
  if (statusCode == "20000")
  {
    if (jsonContent.indexOf("Delta") != -1)
    {
      buffDelta = buffdoc["Delta"].as<String>();
      content = buffDelta;
    }
    else if (jsonContent.indexOf("Sensor") != -1)
    {
      buffDelta = buffdoc["Sensor"].as<String>();
      content = buffDelta;
    }
  }
  return content;
}

void MAGELLAN_MQTT_device_core::updateSensor(String key, String value, JsonDocument &ref_docs)
{
  int len = value.length();
  char *c_value = new char[len + 1];
  std::copy(value.begin(), value.end(), c_value);
  c_value[len] = '\0';
  ref_docs[key] = c_value;
  delete[] c_value;
}

void MAGELLAN_MQTT_device_core::updateSensor(String key, const char *value, JsonDocument &ref_docs)
{
  ref_docs[key] = value;
}

void MAGELLAN_MQTT_device_core::updateSensor(String key, int value, JsonDocument &ref_docs)
{
  ref_docs[key] = value;
}

void MAGELLAN_MQTT_device_core::updateSensor(String key, float value, JsonDocument &ref_docs)
{
  ref_docs[key] = value;
}

void MAGELLAN_MQTT_device_core::updateSensor(String key, boolean value, JsonDocument &ref_docs)
{
  ref_docs[key] = value;
}

void MAGELLAN_MQTT_device_core::addSensor(String key, String value, JsonDocument &ref_docs)
{
  int len = value.length();
  char *c_value = new char[len + 1];
  std::copy(value.begin(), value.end(), c_value);
  c_value[len] = '\0';
  ref_docs[key] = c_value;
  delete[] c_value;
}

void MAGELLAN_MQTT_device_core::addSensor(String key, const char *value, JsonDocument &ref_docs)
{
  // Serial.println("[Key]: "+key+" [Value]: "+value);
  ref_docs[key] = value;
}

void MAGELLAN_MQTT_device_core::addSensor(String key, int value, JsonDocument &ref_docs)
{
  // Serial.println("[Key]: "+key+" [Value]: "+String(value));
  ref_docs[key] = value;
}

void MAGELLAN_MQTT_device_core::addSensor(String key, float value, JsonDocument &ref_docs)
{
  // Serial.println("[Key]: "+key+" [Value]: "+String(value));
  ref_docs[key] = value;
}

void MAGELLAN_MQTT_device_core::addSensor(String key, boolean value, JsonDocument &ref_docs)
{
  // Serial.println("[Key]: "+key+" [Value]: "+String(value));
  ref_docs[key] = value;
}

void MAGELLAN_MQTT_device_core::remove(String key, JsonDocument &ref_docs)
{
  Serial.println("Remove [Key]: " + key);
  ref_docs.remove(key);
}

boolean MAGELLAN_MQTT_device_core::findKey(String key, JsonDocument &ref_docs)
{
  return ref_docs.containsKey(key);
}

String MAGELLAN_MQTT_device_core::buildSensorJSON(JsonDocument &ref_docs)
{
  String bufferJsonStr;
  // Serial.println("# [Build JSON Key is]: "+ String(ref_docs.size()) +" key");
  size_t mmr_usage = ref_docs.memoryUsage();
  size_t max_size = ref_docs.memoryPool().capacity();
  size_t safety_size = max_size * (0.97);
  // Serial.println("Safety size: "+String(safety_size));
  if (mmr_usage >= safety_size)
  {
    bufferJsonStr = "null";
    Serial.println("# [Overload memory toJSONString] *Maximum Safety Memory size to use is: " + String(safety_size));
  }
  else
  {
    serializeJson(ref_docs, bufferJsonStr);
    Serial.println("# [to JSON String Key is]: " + String(ref_docs.size()) + " key");
  }

  Serial.println("# MemoryUsage: " + String(mmr_usage) + "/" + String(safety_size) + " from(" + String(ref_docs.memoryPool().capacity()) + ")");
  return bufferJsonStr;
}

void MAGELLAN_MQTT_device_core::adjustBufferSensor(size_t sizeJSONbuffer)
{
  attr.docSensor = new DynamicJsonDocument(sizeJSONbuffer);
}

int MAGELLAN_MQTT_device_core::readBufferSensor(JsonDocument &ref_docs)
{
  return ref_docs.memoryPool().capacity();
}

void MAGELLAN_MQTT_device_core::clearSensorBuffer(JsonDocument &ref_docs)
{
  Serial.println(F("# [Clear JSON buffer]"));
  ref_docs.clear();
  Serial.println(F("-------------------------------"));
}

boolean MAGELLAN_MQTT_device_core::registerInfoOTA()
{
  return sub_InfoOTA();
}

boolean MAGELLAN_MQTT_device_core::unregisterInfoOTA()
{
  return unsub_InfoOTA();
}

boolean MAGELLAN_MQTT_device_core::requestFW_Info()
{
  return pub_Info();
}

boolean MAGELLAN_MQTT_device_core::registerDownloadOTA()
{
  return sub_DownloadOTA();
}

boolean MAGELLAN_MQTT_device_core::unregisterDownloadOTA()
{
  return unsub_DownloadOTA();
}

boolean MAGELLAN_MQTT_device_core::requestFW_Download(unsigned int fw_chunkpart, size_t chunk_size)
{
  return pub_Download(fw_chunkpart, chunk_size);
}

boolean MAGELLAN_MQTT_device_core::updateProgressOTA(String OTA_state, String description)
{
  return pub_UpdateProgress(OTA_state, description);
}

void MAGELLAN_MQTT_device_core::activeOTA(size_t chunk_size, boolean useChecksum)
{
  Serial.println(F("#============================"));
  Serial.println(F("# Activated OTA"));
#ifdef ESP32
  Update.begin(UPDATE_SIZE_UNKNOWN);
#elif defined ESP8266

  // Update.begin(0xFFFFFFFF, U_FLASH);
#endif
  attr.using_Checksum = useChecksum;
  String isC_sum = (attr.using_Checksum == true) ? "ENABLE" : "DISABLE";
  Serial.println(F(" "));
  Serial.print(isC_sum);
  Serial.println(F(" Checksum FirmwareOTA"));
  // Serial.println(*attr.chunk_size);
  if (chunk_size > 4096)
  {
    Serial.print(F("# [Warning] Chunk Size Maximun is 4096 (use Default \""));
    Serial.print(attr.default_chunk_size);
    Serial.println(F("\")"));
    setChunkSize(attr.default_chunk_size);
  }
  else
  {
    setChunkSize(chunk_size);
  }
  configOTAFile.beginFileSystem();
  if (!configOTAFile.checkFileOTA())
  {
    configOTAFile.createConfigFileOTA();
  }

  if (!configOTAFile.checkLastedOTA())
  {
    configOTAFile.createLastedOTA();
    if (configOTAFile.checkFileOTA())
    {
      String fw_infoInFIleSys = configOTAFile.readLastedOTA();
    }
  }
  else
  {
    String fw_infoInFIleSys;
    JsonObject fw_last = configOTAFile.readObjectLastedOTA();
    int bufferFW_size = fw_last["sizefirmware"];
    fw_last.remove("namefirmware");
    fw_last.remove("sizefirmware");
    fw_last.remove("checksumAlgorithm");
    String bufferFW_v = fw_last["versionfirmware"];
    fw_last["firmwareVersion"] = bufferFW_v;
    fw_last.remove("versionfirmware");
    serializeJson(fw_last, fw_infoInFIleSys);

    if (fw_infoInFIleSys.indexOf("null") == -1)
    {
      this->reportClientConfig(fw_infoInFIleSys);
    }
    else if ((bufferFW_v.length() > 4 || bufferFW_v.indexOf("null") == -1) && (fw_infoInFIleSys.indexOf("null") != -1)) // handle if fw version !null but some key value found null is still pub client config
    {
      this->reportClientConfig(fw_infoInFIleSys);
    }
    else if ((bufferFW_v.indexOf("null") != -1) && (bufferFW_size > 0))
    {
      this->reportClientConfig(fw_infoInFIleSys);
    }
  }
}
void MAGELLAN_MQTT_device_core::setChecksum(String md5Checksum)
{
  Serial.println("# Set Checksum md5: " + md5Checksum + " Status:" + (Update.setMD5(md5Checksum.c_str()) == true ? " Success" : " Fail"));
}

void MAGELLAN_MQTT_device_core::setChunkSize(size_t Chunksize)
{
  attr.chunk_size = Chunksize;
  Serial.println("# Set Chunk size: " + String(attr.chunk_size));
}

// unsigned long prv_cb_timeout_millis = 0;

void checkTimeoutReq_fw_download()
{
  if (attr.checkTimeout_request_download_fw)
  {
    unsigned long differentTime = millis() - attr.prv_cb_timeout_millis;
    if (differentTime > 60000 && !attemp_download_1)
    {
      Serial.println("#Attemp resume download 1 after checktimeout 1 minute");
      pub_Download(attr.fw_count_chunk, attr.chunk_size);
      attemp_download_1 = true;
    }
    if (differentTime > 120000 && !attemp_download_2)
    {
      Serial.println("#Attemp resume download 2 after checktimeout 2 minute");
      pub_Download(attr.fw_count_chunk, attr.chunk_size);
      attemp_download_2 = true;
    }
    if (differentTime > attr.timeout_req_download_fw)
    {
      pub_UpdateProgress("FAILED", "{\"errordescription\":\"Downloading firmware " + MAGELLAN_MQTT_device_core::OTA_info.firmwareVersion + " is timeout on chunk (" + String(attr.current_chunk) + "/" + String(attr.totalChunk) + ")\"}");
      configOTAFile.saveSuccessOrFail("fail");

      Serial.println("#device must restart timeout from request firmware dowload " + String(attr.timeout_req_download_fw / 60000) + " minute");
      delay(5000);
      ESP.restart();
    }
    // Serial.println("Counting Timeout: "+String(diferentTime/1000));
  }
  else
  {
    attr.prv_cb_timeout_millis = millis();
  }
}

unsigned long prv_mills_usingCheckOTA = 0;
void checkTimeoutCheckUpdate()
{
  if (attr.usingCheckUpdate && !attr.inProcessOTA)
  {
    unsigned long differentTime = millis() - prv_mills_usingCheckOTA;
    if (differentTime > 10000)
    {
      attr.usingCheckUpdate = false;
    }
  }
  else if (!attr.usingCheckUpdate && !attr.inProcessOTA)
  {
    prv_mills_usingCheckOTA = millis();
  }
}

boolean remind_unsub_when_inProcessOTA = false;
unsigned long prv_mills_pubinfo = 0;
void MAGELLAN_MQTT_device_core::handleOTA(boolean OTA_after_getInfo)
{
  checkTimeoutReq_fw_download();
  checkTimeoutCheckUpdate();
  if (attr.triggerRemainOTA)
  {
    Serial.println(F("# Active handleOTA"));
    registerInfoOTA();
    if (attr.flagAutoOTA)
      registerDownloadOTA();
    Serial.println(F("#============================"));
    if (attr.inProcessOTA) // if get fw in hook fw will auto count and request dowload fw
    {
      attr.flag_remain_ota = true;
      attr.remain_ota_fw_info_match = false;

      if (!attr.remain_ota_fw_info_match)
      {
        if (millis() - prv_mills_pubinfo > 5000)
        {
          pub_Info();
          prv_mills_pubinfo = millis();
        }
      }
      else if (attr.remain_ota_fw_info_match) // remain get firmware
      {
        pub_Download(attr.fw_count_chunk, attr.chunk_size);
      }
    }
    if (attr.startReqDownloadOTA && !attr.inProcessOTA) // if get part 0 fail but start ota still work when reconnect
    {
      pub_Download(attr.fw_count_chunk, attr.chunk_size);
    }
    attr.triggerRemainOTA = false;
  }
  if ((attr.fw_total_size > 0) && !(attr.remind_Event_GET_FW_infoOTA))
  {
    // Serial.println(F("======================="));
    // Serial.println(F("# Firmware OTA Information Available #"));
    // Serial.println("  ->Firmware Name: "+MAGELLAN_MQTT_device_core::OTA_info.fw_name);
    // Serial.println("  ->Firmware total size: "+String(MAGELLAN_MQTT_device_core::OTA_info.fw_totalSize));
    // Serial.println("  ->Firmware version: "+MAGELLAN_MQTT_device_core::OTA_info.fw_version);
    // Serial.println("  ->Firmware checksum Algorithm: "+MAGELLAN_MQTT_device_core::OTA_info.algorithm_check_sum);
    // Serial.println("  ->Firmware checksum: "+MAGELLAN_MQTT_device_core::OTA_info.check_sum);
    // Serial.println(F("======================="));
    if (attr.using_Checksum && !attr.isFirmwareUptodate)
    {
      if (MAGELLAN_MQTT_device_core::OTA_info.checksumAlgorithm == "md5" || MAGELLAN_MQTT_device_core::OTA_info.checksumAlgorithm == "MD5")
        setChecksum(MAGELLAN_MQTT_device_core::OTA_info.checksum);
      else
      {
        Serial.println(F("#[Warning] Can't set checksum because algorithm checksum is not \"md5\""));
        Serial.println(F("#[Warning] But OTA Process still working without checksum "));
        attr.using_Checksum = false;
      }
    }
    if (OTA_after_getInfo && !attr.isFirmwareUptodate) // auto request OTA after get fw information
    {
      pub_Download(0, attr.chunk_size);
    }
    attr.remind_Event_GET_FW_infoOTA = true;
  }
  if (attr.inProcessOTA && !remind_unsub_when_inProcessOTA)
  {
    Serial.println(F("================================================="));
    Serial.println(F("# Inprocess OTA terminate other incoming message"));
    Serial.println(F("# Unsubscribe unuse function"));
    if (attr.ctrl_regis_key || attr.ctrl_regis_pta)
    {
      unregisterControl(PLAINTEXT);
    }
    if (attr.ctrl_jsonOBJ || attr.ctrl_regis_json)
    {
      unregisterControl(JSON);
    }
    if (attr.conf_regis_key || attr.conf_regis_pta)
    {
      unregisterConfig(PLAINTEXT);
    }
    if (attr.conf_jsonOBJ || attr.conf_regis_json)
    {
      unregisterConfig(JSON);
    }
    if (attr.resp_regis)
    {
      unregisterResponseHeartbeat(PLAINTEXT);
      unregisterResponseHeartbeat(JSON);
      unregisterResponseReport(PLAINTEXT);
      unregisterResponseReport(JSON);
      unregisterResponseReportTimestamp();
      unregisterTimestamp(PLAINTEXT);
      unregisterTimestamp(JSON);
    }
    remind_unsub_when_inProcessOTA = true;
    Serial.println(F("================================================="));
  }
}
////////////////// Unsub ///////////
boolean MAGELLAN_MQTT_device_core::unregisterControl(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/thing/" + token + "/delta/resp/pta/+";
    break;
  case 1:
    topic = "api/v2/thing/" + token + "/delta/resp";
    break;
  default:
    topic = "api/v2/thing/" + token + "/delta/resp";
    break;
  }
  boolean Sub_status = this->client->unsubscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  // Serial.println("# RegisterControl: "+ _debug);
  Serial.println("# Unsubscribe Control: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
} //
boolean MAGELLAN_MQTT_device_core::unregisterControl(String key)
{
  String topic = "api/v2/thing/" + token + "/delta/resp/pta/?sensor=" + key; // fact S s
  boolean Sub_status = this->client->unsubscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(topic);
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe Control [Key]: \"" + key + "\" Unsubscribe: " + _debug);
  return Sub_status;
} //
boolean MAGELLAN_MQTT_device_core::unregisterConfig(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/thing/" + token + "/config/resp/pta/+";
    break;
  case 1:
    topic = "api/v2/thing/" + token + "/config/resp";
    break;
  default:
    topic = "api/v2/thing/" + token + "/config/resp";
    break;
  }
  boolean Sub_status = this->client->unsubscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe ServerConfig: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
} //
boolean MAGELLAN_MQTT_device_core::unregisterConfig(String key)
{
  String topic = "api/v2/thing/" + token + "/config/resp/pta/?config=" + key; // fact C c
  boolean Sub_status = this->client->unsubscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe ServerConfig [Key]: \"" + key + "\" Unsubscribe: " + _debug);
  return Sub_status;
} //
boolean MAGELLAN_MQTT_device_core::unregisterTimestamp(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/server/dateTime/resp/pta";
    break;
  case 1:
    topic = "api/v2/server/dateTime/resp";
    break;
  default:
    topic = "api/v2/server/dateTime/resp";
    break;
  }
  boolean Sub_status = this->client->unsubscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe ServerTime: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
} //
boolean MAGELLAN_MQTT_device_core::unregisterResponseReport(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/thing/" + token + "/report/resp/pta/+";
    break;
  case 1:
    topic = "api/v2/thing/" + token + "/report/resp";
    break;
  default:
    topic = "api/v2/thing/" + token + "/report/resp";
    break;
  }
  boolean Sub_status = this->client->unsubscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe Response Report: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
}
boolean MAGELLAN_MQTT_device_core::unregisterResponseReportTimestamp()
{
  String topic = "api/v2/thing/" + token + "/report/timestamp/resp";
  boolean Sub_status = this->client->unsubscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe Response ReportTimestamp: " + _debug);
  return Sub_status;
}

boolean MAGELLAN_MQTT_device_core::unregisterResponseHeartbeat(int format)
{
  String topic;
  switch (format)
  {
  case 0:
    topic = "api/v2/thing/" + token + "/heartbeat/resp/pta";
    break;
  case 1:
    topic = "api/v2/thing/" + token + "/heartbeat/resp";
    break;
  default:
    topic = "api/v2/thing/" + token + "/heartbeat/resp";
    break;
  }
  boolean Sub_status = this->client->unsubscribe(topic.c_str());
  _debug = (Sub_status == true) ? "Success" : "Failure";
  String respType = (format == 0) ? "Plaintext" : "JSON";
  Serial.println(F("-------------------------------"));
  Serial.println("# Unsubscribe Response Heartbeat: " + _debug);
  Serial.println("# Response type: " + respType);
  return Sub_status;
}
////////////////// Unsub //////////