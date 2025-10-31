#ifndef MANAGECREDENTIALFILE_H
#define MANAGECREDENTIALFILE_H
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
#include "./ArduinoJson-v6.18.3.h"
#include "./manageConfigOTAFile.h"
#include "utils/Generate_Credential.h"
#include "./MAGELLAN_MQTT_device_core.h"
class manageCredentialFile: private Generate_Credential
{
private:
    const char* credentialPath = "/credentialFile.json";
    const char* prev_credentialPath = "/prv_credentialFile.json";
    StaticJsonDocument<256> CredDoc;
    boolean saveStatusCredential(String status);
public:
    void initCredentialFile();
    void beginFileSystem();
    boolean checkCredentialFile();
    boolean checkPreviousCredentialFile();
    boolean createCredentialFile();
    boolean deleteCredentialFile();
    boolean deletePreviousCredentialFile();
    boolean writePreviousCredential();
    boolean setNewCredential(String thingIdentifier, String thingSecret);
    String readCredentialFile();
    String readPreviousCredentialFile();
    JsonObject readObjectCredentialFile();
    JsonObject readObjectPreviousCredentialFile();
    String readSpacificCredentialFile(String readKey);
    String readSpacificPreviousCredentialFile(String readKey);
    // boolean saveCredential(JsonObject credentialObj);
    void setActivateCredential(boolean activate);
    // boolean credentialIsActivated();
};
extern manageCredentialFile credentialFile;
#endif