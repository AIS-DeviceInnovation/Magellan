#ifndef MANAGECONFIGOTAFILE_H
#define MANAGECONFIGOTAFILE_H
#include <Arduino.h>

#ifdef ESP32
#ifdef MG_USE_SPIFFS
#include "SPIFFS.h"
#else
#include "LittleFS.h"

#endif
#elif defined ESP8266
#include "LittleFS.h"

#endif
#include <vector>
#include "FileSystem.h"
#include "./MAGELLAN_LIB_CONF.h"
// typedef std::vector<String> ListFileString;

class manageConfigOTAFile
{
private:
  const char *configOTAFilePath = "/configOTAFile.json"; // save every event init fail success
  const char *lastedOTAPath = "/lastedOTAFile.json";     // save only success
#if !MAGELLAN_USE_ARDUINOJSON7
  StaticJsonDocument<512> OTAdoc;
#else
  JsonDocument OTAdoc;
#endif

public:
  void beginFileSystem();
  boolean checkFileOTA();
  boolean checkLastedOTA();
  boolean createConfigFileOTA();
  boolean createLastedOTA();
  String readConfigFileOTA();
  String readLastedOTA();
  JsonObject readObjectConfigFileOTA();
  JsonObject readObjectLastedOTA();
  String readSpacificFromConfFile(String readKey);
  boolean saveProfileOTA(JsonObject dataOTA, String stateOTA);
  boolean saveLastedOTA(String lastedDataOTA);
  boolean saveSuccessOrFail(String stateOTA);
  boolean compareFirmwareOTA(JsonObject dataOTA);        // using for incoming without check update
  boolean compareFirmwareIsUpToDate(JsonObject dataOTA); // using when incoming by check update
};
extern manageConfigOTAFile configOTAFile;
#endif