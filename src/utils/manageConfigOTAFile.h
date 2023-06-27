#ifndef MANAGECONFIGOTAFILE_H
#define MANAGECONFIGOTAFILE_H
#include <Arduino.h>

#ifdef ESP32
  #include "SPIFFS.h"
  #include "FS.h"
#elif defined ESP8266
  #include "LittleFS.h"
  #include "FS.h"
#endif

#include <vector>
#include "FileSystem.h"
#include "./ArduinoJson-v6.18.3.h"
// typedef std::vector<String> ListFileString;

class manageConfigOTAFile
{
private:
    const char* configOTAFilePath = "/configOTAFile.json"; //save every event init fail success
    const char* lastedOTAPath = "/lastedOTAFile.json"; //save only success
    StaticJsonDocument<512> OTAdoc;
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
    boolean compareFirmwareOTA(JsonObject dataOTA); //using for incoming without check update
    boolean compareFirmwareIsUpToDate(JsonObject dataOTA); //using when incoming by check update
};
extern manageConfigOTAFile configOTAFile;
#endif