#include <Arduino.h>
#include <MAGELLAN_MEDIA_FILE.h>

const char *enumToString(mediaTypeEnum mediatype)
{
    switch (mediatype)
    {
    case mediaTypeEnum::Application:
        return "application";
    case mediaTypeEnum::Audio:
        return "audio";
    case mediaTypeEnum::Font:
        return "font";
    case mediaTypeEnum::Example:
        return "example";
    case mediaTypeEnum::Image:
        return "image";
    case mediaTypeEnum::Message:
        return "message";
    case mediaTypeEnum::Model:
        return "model";
    case mediaTypeEnum::Multipart:
        return "multipart";
    case mediaTypeEnum::Text:
        return "text";
    case mediaTypeEnum::Video:
        return "video";
    default:
        return "Unknown";
    }
}

size_t readBuffersize()
{
    return attr.mqtt_client->getBufferSize();
}

String buildSensorJSON(JsonDocument &ref_docs)
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

void adjustBufferFormedia(size_t len_payload)
{
    if (len_payload <= (size_t)55000)
    {

        if (attr.mqtt_client->getBufferSize() > (attr.calculate_chunkSize * 2))
        {
            attr.mqtt_client->setBufferSize(attr.calculate_chunkSize * 2);
        }

        if (attr.mqtt_client != NULL && attr.ext_Token.length() > 30)
        {
            // if ((len_payload > attr.mqtt_client->getBufferSize() + 10) && (attr.mqtt_client->getBufferSize() <= 55000))
            if ((len_payload > attr.mqtt_client->getBufferSize() + 10) && (attr.mqtt_client->getBufferSize() <= 55000))
            {
                attr.docSensor = new DynamicJsonDocument(len_payload);
                attr.mqtt_client->setBufferSize(len_payload + 3000); // offset mqtt client buffer
            }
        }
    }
    else
    {
        Serial.println(F("# Upload: Failure"));
    }
}

void addSensor(String key, String value, JsonDocument &ref_docs)
{
    int len = value.length();
    char *c_value = new char[len + 1];
    std::copy(value.begin(), value.end(), c_value);
    c_value[len] = '\0';
    ref_docs[key] = c_value;
    delete[] c_value;
}

bool reportJSON(String payload)
{
    String _debug;
    String topic = "api/v2/thing/" + attr.ext_Token + "/report/persist";
    boolean Pub_status = attr.mqtt_client->publish(topic.c_str(), payload.c_str());
    _debug = (Pub_status == true) ? "Success" : "Failure";
    Serial.println(F("-------------------------------"));
    Serial.println("# Report JSON: " + _debug);
    // Serial.println("# Payload: " + payload);
    return Pub_status;
}

String MAGELLAN_MEDIA_FILE::FSHandle(const char *filePath)
{
    fileSys.begin();
    File file = FSmemory.open(filePath, "r");
    if (!file)
    {
        Serial.println(F("Failed to open image file in SPIFFS"));
        return "Failed to open image file in SPIFFS";
    }
    if (!FSmemory.exists(filePath))
    {
        Serial.print(F("### ("));
        Serial.print(filePath);
        Serial.println(F("): File doesn't exist."));
        return "File doesn't exist.";
    }
    size_t size = file.size();
    String fileName = filePath;

    uint8_t *buffer = new uint8_t[size];
    file.read(buffer, size);
    file.close();
    String buf64 = base64::encode(buffer, size);
    delete[] buffer;
    return buf64;
}

String MAGELLAN_MEDIA_FILE::SDHandle(const char *filePath, fs::FS &SD)
{
    fileSys.begin();
    File file = SD.open(filePath, "r");
    if (!file)
    {
        Serial.println("Failed to open image file in SD Card");
        return "Failed to open image file in SD Card";
    }
    if (!SD.exists(filePath))
    {
        Serial.print(F("### ("));
        Serial.print(filePath);
        Serial.println(F("): File doesn't exist."));
        return "File doesn't exist.";
    }
    size_t size = file.size();
    String fileName = filePath;

    uint8_t *buffer = new uint8_t[size];
    while (file.available())
    {
        file.read(buffer, size);
    }
    file.close();
    String buf64 = base64::encode(buffer, size);
    delete[] buffer;
    return buf64;
}

String MAGELLAN_MEDIA_FILE::BIHandle(uint8_t *binaryData, size_t sizeBinaryData)
{
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[sizeBinaryData]);
    std::copy(binaryData, binaryData + sizeBinaryData, buffer.get());
    String buf64 = base64::encode(buffer.get(), sizeBinaryData);

    return buf64;
}

/**
 * Encode binary to Base64 format, then send to Magellan.
 * @param key key for JSONkey.
 * @param binaryData uint8_t* data type (binary)
 * @param sizeBinaryData length of binary data.
 * @param mediatype application, audio, font, example, image, message, model, multipart, text, video.
 * @param subtype filename extension or file suffix (ex: png,jpeg,mped,mp4).
 */
bool MAGELLAN_MEDIA_FILE::uploadFromBinary(const String& key, const uint8_t* binaryData, size_t sizeBinaryData, mediaTypeEnum mediatype, const String& subtype)
{
    ESP.getFreePsram()>0?maximumRawData = maximumRawDataPS:maximumRawData;

    const char* mediatypeStr = enumToString(mediatype);
    bool result = false;
    if (sizeBinaryData <= (size_t)maximumRawData)
    {
        if (attr.mqtt_client->getBufferSize() > (attr.calculate_chunkSize * 2))
            attr.mqtt_client->setBufferSize(attr.calculate_chunkSize * 2);

        if (attr.mqtt_client != NULL && attr.ext_Token.length() > 30)
        {
            std::unique_ptr<uint8_t[]> buffer(new uint8_t[sizeBinaryData+5]);
            std::copy(binaryData, binaryData + sizeBinaryData, buffer.get());
            String topic = "api/v2/thing/" + attr.ext_Token + "/report/persist/pta/?sensor=" + key;

            String buf64 = base64::encode(buffer.get(), sizeBinaryData);
            String b64Mime = "data:"+(String)mediatypeStr+"/"+subtype+";base64," + buf64;
            size_t cal_buff_adj = topic.length() + b64Mime.length() + 10;

            if(b64Mime.length() < 25 || b64Mime == NULL || b64Mime == "")
            {
                Serial.println(F("### Failed to convert to Base64 with Mime type.\n### Insufficient Heap"));
                Serial.println(F("-------------------------------"));
                return false;
            }

            if ((b64Mime.length() > attr.mqtt_client->getBufferSize() + 10) && (attr.mqtt_client->getBufferSize() <= 55000))
                attr.mqtt_client->setBufferSize((uint16_t)cal_buff_adj);

            result = attr.mqtt_client->publish(topic.c_str(), b64Mime.c_str());
            _debug = (result == true) ? "Success" : "Failure";
            Serial.println(F("-------------------------------"));
            Serial.println("# Key: " + key);
            Serial.println("# Upload: " + _debug);
            return result;
        }
        else
        {
            Serial.println(F("# Upload: Failure"));
        }
    }
    else
    {
        Serial.println(F("### Your file is too large."));
    }
    return false;
}

/**
 * Encode binary from file system(FS or SPIFFS) to Base64 format, then send to Magellan.
 * @param key key for JSONkey.
 * @param filePath String of file path that's store in the file system(FS or SPIFFS)
 * @param mediatype application, audio, font, example, image, message, model, multipart, text, video.
 * @param subtype filename extension or file suffix (ex: png,jpeg,mped,mp4).
 */
bool MAGELLAN_MEDIA_FILE::uploadFromSPIFFS(const String& key, const char *filePath, mediaTypeEnum mediatype, const String& subtype)
{
    ESP.getFreePsram()>0?maximumRawData = maximumRawDataPS:maximumRawData;
    Serial.printf("\n\t\tMaximum Size:%d Psram: %d\n",maximumRawData,ESP.getFreePsram());
    String mediatypeStr = enumToString(mediatype);
    bool result = false;
    fileSys.begin();
    File file = FSmemory.open(filePath, "r");
    if (!file)
    {
        Serial.println("Failed to open image file in SPIFFS");
        return false;
    }
    if (!FSmemory.exists(filePath))
    {
        Serial.print(F("### ("));
        Serial.print(filePath);
        Serial.println(F("): File doesn't exist."));
        return false;
    }
    size_t size = file.size();
    if (size <= (size_t)maximumRawData)
    {
        if (attr.mqtt_client->getBufferSize() > (attr.calculate_chunkSize * 2))
            attr.mqtt_client->setBufferSize(attr.calculate_chunkSize * 2);

        if (attr.mqtt_client != NULL && attr.ext_Token.length() > 30)
        {
            String fileName = filePath;

            uint8_t *buffer = new uint8_t[size];
            file.read(buffer, size);
            String buf64 = base64::encode(buffer, size);
            delete[] buffer;
            
            file.close();
            String prefixB64 = "data:" + mediatypeStr + "/" + subtype + ";base64,";
            String topic = "api/v2/thing/" + attr.ext_Token + "/report/persist/pta/?sensor=" + key;
            size_t b64MimeSize = prefixB64.length()+ buf64.length();
            size_t cal_buff_adj = topic.length() + b64MimeSize + 10;

            if ((b64MimeSize > attr.mqtt_client->getBufferSize() + 10) && (attr.mqtt_client->getBufferSize() <= 56000))
            {
                attr.mqtt_client->setBufferSize((const uint16_t)cal_buff_adj);
            }
            String b64Mime;
            b64Mime.reserve(b64MimeSize);
            b64Mime += prefixB64;
            const size_t chunkSize = 512;
            for (size_t i = 0; i < buf64.length(); i += chunkSize)
            {
                size_t len = std::min(chunkSize, buf64.length() - i);
                b64Mime += buf64.substring(i, i + len);
            }
            if(b64MimeSize < 25 || b64Mime.length() < 25 || b64Mime == NULL || b64Mime == "")
            {
                Serial.println(F("### Failed to convert to Base64 with Mime type.\n### Insufficient Heap"));
                Serial.println(F("-------------------------------"));
                return false;
            }
            result = attr.mqtt_client->publish(topic.c_str(), b64Mime.c_str());
            _debug = (result == true) ? "Success" : "Failure";
            Serial.println(F("-------------------------------"));
            Serial.println("# File: " + fileName.substring(1));
            Serial.println("# Upload: " + _debug);
            return result;
        }
        else
        {
            Serial.println(F("# Upload: Failure"));
            return false;
        }
    }
    else
    {
        Serial.println(F("### Your file is too large."));
        return false;
    }
    return false;
}

/**
 * Encode binary from SD Card to Base64 format, then send to Magellan.
 * @param key key for JSONkey.
 * @param filePath String of file path that's store in the SD Card.
 * @param mediatype application, audio, font, example, image, message, model, multipart, text, video.
 * @param subtype filename extension or file suffix (ex: png,jpeg,mped,mp4).
 * @param SD the SD card module that you begin in the setup().
 */
bool MAGELLAN_MEDIA_FILE::uploadFromSDCard(const String& key, const char *filePath, mediaTypeEnum mediatype, const String& subtype,fs::FS &SD)
{
    String mediatypeStr = enumToString(mediatype);
    bool result = false;
    fileSys.begin();
    File file = SD.open(filePath, "r");
    if (!file)
    {
        Serial.println("Failed to open image file in SD Card");
        return false;
    }
    if (!SD.exists(filePath))
    {
        Serial.print(F("### ("));
        Serial.print(filePath);
        Serial.println(F("): File doesn't exist."));
        return false;
    }
    size_t size = file.size();
    if (size <= (size_t)maximumRawData)
    {
        if (attr.mqtt_client->getBufferSize() > (attr.calculate_chunkSize * 2))
            attr.mqtt_client->setBufferSize(attr.calculate_chunkSize * 2);
        if (attr.mqtt_client != NULL && attr.ext_Token.length() > 30)
        {
            Serial.println(attr.mqtt_client->getBufferSize());
            String fileName = filePath;

            uint8_t *buffer = new uint8_t[size];
            while (file.available())
            {
                file.read(buffer, size);
            }
            String buf64 = base64::encode(buffer, size);
            delete[] buffer;
            file.close();
            String prefixB64 = "data:" + mediatypeStr + "/" + subtype + ";base64,";
            String topic = "api/v2/thing/" + attr.ext_Token + "/report/persist/pta/?sensor=" + key;
            size_t b64MimeSize = prefixB64.length()+ buf64.length();
            size_t cal_buff_adj = topic.length() + b64MimeSize + 10;

            if ((b64MimeSize > attr.mqtt_client->getBufferSize() + 10) && (attr.mqtt_client->getBufferSize() <= 56000))
            {
                Serial.println((uint16_t)cal_buff_adj);
                attr.mqtt_client->setBufferSize((const uint16_t)cal_buff_adj);
                Serial.println(attr.mqtt_client->getBufferSize());
            }
            String b64Mime;
            b64Mime.reserve(b64MimeSize);
            b64Mime += prefixB64;
            const size_t chunkSize = 512;
            for (size_t i = 0; i < buf64.length(); i += chunkSize)
            {
                size_t len = std::min(chunkSize, buf64.length() - i);
                b64Mime += buf64.substring(i, i + len);
            }
            if(b64MimeSize < 25 || b64Mime.length() < 25 || b64Mime == NULL || b64Mime == "")
            {
                Serial.println(F("### Failed to convert to Base64 with Mime type.\n### Insufficient Heap"));
                Serial.println(F("-------------------------------"));
                return false;
            }
            result = attr.mqtt_client->publish(topic.c_str(), b64Mime.c_str());
            _debug = (result == true) ? "Success" : "Failure";
            Serial.println(F("-------------------------------"));
            Serial.println("# File: " + fileName.substring(1));
            Serial.println("# Upload: " + _debug);
            return result;
        }
        else
        {
            Serial.println(F("# Upload: Failure"));
            return false;
        }
    }
    else
    {
        Serial.println(F("### Your file is too large."));
    }
    return false;
}

/**
 * Build the in put file data to the Base64 format andthen return to String.
 * @param filePath String of file path that's store in the file system(FS or SPIFFS).
 * @return return String in Base64 format.
 */
String MAGELLAN_MEDIA_FILE::toBase64String(const char *filePath)
{
    return FSHandle(filePath);
}
/**
 * Build the in put file data to the Base64 format andthen return to String.
 * @param filePath String of file path that's store in the SD Card.
 * @param SD the SD card module that you begin in the setup().
 * @return return String in Base64 format.
 */
String MAGELLAN_MEDIA_FILE::toBase64String(const char *filePath, fs::FS &SD)
{
    return SDHandle(filePath, SD);
}
/**
 * Build the in put file data to the Base64 format andthen return to String.
 * @param binaryData uint8_t* data type (binary)
 * @param sizeBinaryData length of binary data.
 * @return return String in Base64 format.
 */
String MAGELLAN_MEDIA_FILE::toBase64String(uint8_t *binaryData, size_t sizeBinaryData)
{
    return BIHandle(binaryData, sizeBinaryData);
}

/**
 * Build the in put file data to the Base64 with media type format andthen return to String.
 * @param filePath String of file path that's store in the file system(FS or SPIFFS).
 * @param mediatype application, audio, font, example, image, message, model, multipart, text, video.
 * @param subtype filename extension or file suffix (ex: png,jpeg,mped,mp4).
 * @return return String in Base64 format.
 */
String MAGELLAN_MEDIA_FILE::toBase64MimeTypeString(const char *filePath, mediaTypeEnum mediatype, const String& subtype)
{
    String mediatypeStr = enumToString(mediatype);
    String rawB64 = FSHandle(filePath);
    String b64Mime = "data:"+mediatypeStr+"/"+subtype+";base64," + rawB64;
    return b64Mime;
}

/**
 * Build the in put file data to the Base64 with media type format andthen return to String.
 * @param filePath String of file path that's store in the SD Card.
 * @param SD the SD card module that you begin in the setup().
 * @param mediatype application, audio, font, example, image, message, model, multipart, text, video.
 * @param subtype filename extension or file suffix (ex: png,jpeg,mped,mp4).
 * @return return String in Base64 format.
 */
String MAGELLAN_MEDIA_FILE::toBase64MimeTypeString(const char *filePath, mediaTypeEnum mediatype, const String& subtype, fs::FS &SD)
{
    String mediatypeStr = enumToString(mediatype);
    String rawB64 = SDHandle(filePath, SD);
    String b64Mime = "data:"+mediatypeStr+"/"+subtype+";base64," + rawB64;
    return b64Mime;
}

/**
 * Build the in put file data to the Base64 with media type format andthen return to String.
 * @param binaryData uint8_t* data type (binary)
 * @param sizeBinaryData length of binary data.
 * @param mediatype application, audio, font, example, image, message, model, multipart, text, video.
 * @param subtype filename extension or file suffix (ex: png,jpeg,mped,mp4).
 * @return return String in Base64 format.
 */
String MAGELLAN_MEDIA_FILE::toBase64MimeTypeString(uint8_t *binaryData, size_t sizeBinaryData, mediaTypeEnum mediatype, const String& subtype)
{
    String mediatypeStr = enumToString(mediatype);
    String rawB64 = BIHandle(binaryData, sizeBinaryData);
    String b64Mime = "data:"+mediatypeStr+"/"+subtype+";base64," + rawB64;
    return b64Mime;
}