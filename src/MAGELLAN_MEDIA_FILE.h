#include <Arduino.h>
#include "utils/Attribute_MQTT_core.h"
#include <base64.h>
#include "utils/PubSubClient.h"
extern "C"
{
#include "libb64/cdecode.h"
#include "libb64/cencode.h"
}

#ifdef ESP32
#define FSmemory SPIFFS
#elif defined ESP8266
#define memory LittleFS
#endif

//Client Internet interface connection
#define useGSMClient  0
#define useExternalClient 1

enum mediaTypeEnum
{
    Application,
    Audio,
    Font,
    Example,
    Image,
    Message,
    Model,
    Multipart,
    Text,
    Video
};

class MAGELLAN_MEDIA_FILE
{
private:
    size_t maximumRawData = 38500;
    size_t maximumRawDataPS = 100000;
    void setBuffersize(size_t clientBufferSize);
    String FSHandle(const char *filePath);
    String SDHandle(const char *filePath, fs::FS &SD);
    String BIHandle(uint8_t *binaryData, size_t sizeBinaryData);
    static String buildMIMEtype(String base64Str);

public:
    MAGELLAN_MEDIA_FILE(){};
    size_t readBuffersize();
    boolean uploadFromSPIFFS(const String& key, const char *filePath, mediaTypeEnum mediatype, const String& subtype);
    boolean uploadFromSDCard(const String& key, const char *filePath, mediaTypeEnum mediatype, const String& subtype,fs::FS &SD);
    boolean uploadFromBinary(const String& key, const uint8_t* binaryData, size_t sizeBinaryData, mediaTypeEnum mediatype, const String& subtype);

    String toBase64String(const char *filePath);
    String toBase64String(const char *filePath, fs::FS &SD);
    String toBase64String(uint8_t *binaryData, size_t sizeBinaryData);
    
    String toBase64MimeTypeString(const char *filePath, mediaTypeEnum mediatype, const String& subtype);
    String toBase64MimeTypeString(const char *filePath, mediaTypeEnum mediatype, const String& subtype, fs::FS &SD);
    String toBase64MimeTypeString(uint8_t *binaryData, size_t sizeBinaryData, mediaTypeEnum mediatype, const String& subtype);

protected:
    String _debug;
    PubSubClient *client = NULL;
};
