#include <Arduino.h>
#include "./Generate_Credential.h"

String Generate_Credential::getChipID()
{
    String chipID;
  #ifdef ESP32
    uint32_t id = 0;
    for(int i=0; i<17; i=i+8)
    {
      id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    // Serial.printf("%08X\n", id);
    chipID = String(id, HEX);
  #elif defined ESP8266
    chipID = String(ESP.getChipId(), HEX);
  #endif
    return chipID;
}

String Generate_Credential::getMacAddress()
{
    for(int i=0; i < 3; i++)
    {
        int buffRan = (int)random(0, 255);
        // Serial.println(buffRan);
        macRandom += String(buffRan, HEX); 
    }
    macAddress = macRandom + getChipID();
    return macAddress;
}

ListString Generate_Credential::macAddressSlice()
{
    ListString lists;
    String mac = getMacAddress();
    int macLen = mac.length();
    for(int i =0; i < macLen; i+=2)
    {
        String slice = mac.substring(i, i+2);
        lists.push_back(slice);
    }
    return lists;   
}

String Generate_Credential::generateThingIdentifier(ListString listMac)
{
     String sum_thingIdent = "NULL";
    if(listMac.size() > 0)
    {
        sum_thingIdent = "";
        for(int i =0; i < listMac.size(); i++)
        {
            sum_thingIdent += String((int)strtol(listMac[i].c_str(), NULL, 16) * random(1, 99));
        }
    }
    else
    {
        return sum_thingIdent;
    }
    if(sum_thingIdent.length() > 30)
    {
        sum_thingIdent = sum_thingIdent.substring(0,28);
    }
    return sum_thingIdent;   
}
String Generate_Credential::generateThingSecret(ListString listMac)
{
    String sum_thingSecret = "NULL";
    if(listMac.size() > 0)
    {
        sum_thingSecret = "";
        for(int i =0; i < listMac.size(); i++)
        {
            sum_thingSecret += String(((int)strtol(listMac[i].c_str(), NULL, 16) + random(10, 99)) % random(0, 999));
        }
    }
    else
    {
        return sum_thingSecret;
    }
    if(sum_thingSecret.length() > 20)
    {
        sum_thingSecret = sum_thingSecret.substring(0,18);
    }
    return sum_thingSecret;
}

Credential Generate_Credential::generateCredential()
{
    Credential cre_gen;
    ListString mac_gen = macAddressSlice();
    cre_gen.ThingIdentifier = generateThingIdentifier(mac_gen);
    cre_gen.ThingSecret = generateThingSecret(mac_gen);
    return cre_gen;
}