#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <Arduino.h>
#ifdef ESP32
  #include "SPIFFS.h"
  #include "FS.h"
  #define myFS SPIFFS
#elif defined ESP8266
  #include "LittleFS.h"
  #include "FS.h"
  #define myFS LittleFS
#endif
#include <vector>

typedef std::vector<String> ListFileString;

class FileSystem
{
private:
    
public:
    
    boolean isFileSystemInit = false;
    void begin();
    ListFileString listDirectory(const char* dir_name, uint8_t level, fs::FS &fs = myFS);
    ListFileString listFile(const char* dir_name, fs::FS &fs = myFS);
    String readFile(const char* path, fs::FS &fs = myFS); //limit filesize can read 60081 can't 63207
    // String readBigFile(const char* path, fs::FS &fs = myFS);
    // ListFileString readLargeFile(const char* path, fs::FS &fs = myFS);
    boolean writeFile(const char* path, const char * message, fs::FS &fs = myFS);
    boolean appendFile(const char* path, const char * message, fs::FS &fs = myFS);
    boolean renameFile(const char* old_path, const char * new_path, fs::FS &fs = myFS);
    boolean deleteFile(const char* path, fs::FS &fs = myFS);
    boolean isFileExist(const char* path, fs::FS &fs = myFS);
};
extern FileSystem fileSys;
#endif