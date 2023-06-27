#include <Arduino.h>
#include "./manageCredentialFile.h"

void manageCredentialFile::beginFileSystem()
{
  fileSys.begin();
}

void manageCredentialFile::initCredentialFile()
{
  fileSys.begin();
  if(!checkCredentialFile())
  {
    Serial.println(F("#Initial generate credentrial"));
    createCredentialFile();
  }
}

boolean manageCredentialFile::checkCredentialFile()
{
  return fileSys.isFileExist(credentialPath);
  // #ifdef ESP32
  //   return fileSys.isFileExist(credentialPath, SPIFFS);
  // #elif defined ESP8266
  //   return fileSys.isFileExist(credentialPath, LittleFS);
  // #endif
}

boolean manageCredentialFile::checkPreviousCredentialFile()
{
  return fileSys.isFileExist(prev_credentialPath);
  // #ifdef ESP32
  //   return fileSys.isFileExist(prev_credentialPath, SPIFFS);
  // #elif defined ESP8266
  //   return fileSys.isFileExist(prev_credentialPath, LittleFS);  
  // #endif
}



boolean manageCredentialFile::createCredentialFile()
{
  Credential cred = this->generateCredential();
  String init_data = "{\"thingIdentifier\":\""+cred.ThingIdentifier+"\",\"thingSecret\":\""+cred.ThingSecret+"\",\"status\":\"Inactive\"}";
  Serial.println(F("#====== credential information ========="));
  Serial.println("ThingIdentifier: "+String(cred.ThingIdentifier));
  Serial.println("ThingSecret: "+String(cred.ThingSecret));
  Serial.println(F("#======================================="));
  // #ifdef ESP32
  //   return fileSys.writeFile(credentialPath, init_data.c_str(), SPIFFS);
  // #elif defined ESP8266
  //   return fileSys.writeFile(credentialPath, init_data.c_str(), LittleFS);  
  // #endif
  return fileSys.writeFile(credentialPath, init_data.c_str());
}


boolean manageCredentialFile::deleteCredentialFile()
{
    if(!credentialFile.checkCredentialFile())
    {
      return false;
    }
    return fileSys.deleteFile(credentialPath);
}

boolean manageCredentialFile::deletePreviousCredentialFile()
{
    if(!credentialFile.checkPreviousCredentialFile())
    {
      return false;
    }
    return fileSys.deleteFile(prev_credentialPath);
}

boolean manageCredentialFile:: writePreviousCredential()
{
  if(!credentialFile.checkCredentialFile())
  {
    return false;
  }
  return fileSys.writeFile(prev_credentialPath, readCredentialFile().c_str());
}

boolean manageCredentialFile::setNewCredential(String thingIdentifier, String thingSecret)
{
  fileSys.begin();
  thingIdentifier.trim(); 
  thingSecret.trim();

  if(!(MAGELLAN_MQTT_device_core::CheckString_isDigit(thingIdentifier) && 
  MAGELLAN_MQTT_device_core::CheckString_isDigit(thingSecret)))
  {
    Serial.println(F("=================================================="));
    Serial.println(F("# Set new credential \"FALSE\" require digit only"));
    Serial.println(F("=================================================="));
    return false;
  }
  if(thingIdentifier.length() > 30){
    Serial.print(F("# ThingIdentifier length greater than 30 \nadjust "));
    Serial.print(thingIdentifier);
    Serial.print(F(" >> "));
    thingIdentifier = thingIdentifier.substring(0, 28);
    Serial.println(thingIdentifier);
  } 
  if(thingSecret.length() > 20){
    Serial.print(F("# ThingSecret length greater than 20 \nadjust "));
    Serial.print(thingSecret);
    Serial.print(F(" >> "));
    thingSecret = thingSecret.substring(0, 18);
    Serial.println(thingSecret);
  } 

  writePreviousCredential();

  Serial.println(F("#===================================="));
  Serial.println(F("# Set new credential"));
  Serial.println("# ThingIdentifier: "+ thingIdentifier);
  Serial.println("# ThingSecret: "+ thingSecret);
  Serial.println(F("#===================================="));

  String init_data = "{\"thingIdentifier\":\""+thingIdentifier+"\",\"thingSecret\":\""+thingSecret+"\",\"status\":\"Inactivated\"}";
  return fileSys.writeFile(credentialPath, init_data.c_str());
}



String manageCredentialFile::readCredentialFile()
{
  return fileSys.readFile(credentialPath);
}

String manageCredentialFile::readPreviousCredentialFile()
{
  return fileSys.readFile(prev_credentialPath);
}

JsonObject manageCredentialFile::readObjectCredentialFile()
{
  String buffReadCredential = readCredentialFile();
  JsonObject buffer;
  CredDoc.clear();
  if(buffReadCredential != NULL)
  {
    DeserializationError error = deserializeJson(CredDoc, buffReadCredential);
    buffer = CredDoc.as<JsonObject>();
    if(error)
      Serial.println("# Error to DeserializeJson readCredentialFile");
  }
  return buffer;
}

JsonObject manageCredentialFile::readObjectPreviousCredentialFile()
{
  String buffReadCredential = readPreviousCredentialFile();
  JsonObject buffer;
  CredDoc.clear();
  if(buffReadCredential != NULL)
  {
    DeserializationError error = deserializeJson(CredDoc, buffReadCredential);
    buffer = CredDoc.as<JsonObject>();
    if(error)
      Serial.println("# Error to DeserializeJson readPreviousCredentialFile");
  }
  return buffer;
}



String manageCredentialFile::readSpacificCredentialFile(String readKey)
{
    String result = readObjectCredentialFile()[readKey.c_str()];
    return result;
}

String manageCredentialFile::readSpacificPreviousCredentialFile(String readKey)
{
    String result = readObjectPreviousCredentialFile()[readKey.c_str()];
    return result;
}

boolean manageCredentialFile::saveStatusCredential(String status)
{
    JsonObject buffCred = readObjectCredentialFile();

    StaticJsonDocument<256> docsBuffer = buffCred;
    docsBuffer["status"] = status.c_str();
    String credentialUpdate;
    serializeJson(docsBuffer, credentialUpdate);

    return fileSys.writeFile(credentialPath, credentialUpdate.c_str());
}

// boolean manageCredentialFile::credentialIsActivated()
// {
//   String status = readSpacificCredentialFile("status");
//   boolean IsActivated = ((status == "Activated")? true : false);
//   return IsActivated;
// }

void manageCredentialFile::setActivateCredential(boolean activate)
{
  saveStatusCredential((activate)? "Activated" : "Inactivaed");
}