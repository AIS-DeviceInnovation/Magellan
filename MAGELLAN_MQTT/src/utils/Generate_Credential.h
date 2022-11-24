#ifndef GENERATE_CREDENTIAL_H
#define GENERATE_CREDENTIAL_H
#include <Arduino.h>
#include <vector>

typedef std::vector<String> ListString;
struct Credential{
    String ThingIdentifier;
    String ThingSecret;
};
class Generate_Credential
{
private:
    String macRandom;
    String macAddress;
    String getChipID();
    String getMacAddress();
    String generateThingIdentifier(ListString listMac);
    String generateThingSecret(ListString listMac);
public:
    Generate_Credential(){};
    ListString macAddressSlice();
    Credential generateCredential();
};


#endif
