#ifndef MAGELLAN_LIB_CONF_H
#define MAGELLAN_LIB_CONF_H

// =============================================================
// Magellan/AIS 4G Library Configuration (single source of truth)
// =============================================================
// Edit values in this file for Arduino IDE / PlatformIO projects.
// Do not define these in main.ino because library .cpp files are
// compiled in separate translation units.

// 0: Use bundled ArduinoJson v6.18.3 (default)
// 1: Use external ArduinoJson v7 (install via Library Manager)
// Override via platformio.ini: build_flags = -DMAGELLAN_USE_ARDUINOJSON7=1
#ifndef MAGELLAN_USE_ARDUINOJSON7
#define MAGELLAN_USE_ARDUINOJSON7 0
#endif

// Log level: 0=none, 1=error, 2=info, 3=debug (default)
// Override via platformio.ini: build_flags = -DMAGELLAN_LOG_LEVEL=2
#ifndef MAGELLAN_LOG_LEVEL
#define MAGELLAN_LOG_LEVEL 3
#endif

// Filesystem backend on ESP32: 0=LittleFS (default), 1=SPIFFS
// Override via platformio.ini: build_flags = -DMAGELLAN_USE_SPIFFS=1
#ifndef MAGELLAN_USE_SPIFFS
#define MAGELLAN_USE_SPIFFS 0
#endif
#if MAGELLAN_USE_SPIFFS
#ifndef MG_USE_SPIFFS
#define MG_USE_SPIFFS
#endif
#endif

// Token behavior: 0=auto request (default), 1=bypass auto token
// Override via platformio.ini: build_flags = -DMAGELLAN_BYPASS_REQTOKEN=1
#ifndef MAGELLAN_BYPASS_REQTOKEN
#define MAGELLAN_BYPASS_REQTOKEN 0
#endif
#if MAGELLAN_BYPASS_REQTOKEN
#ifndef BYPASS_REQTOKEN
#define BYPASS_REQTOKEN
#endif
#endif


#if MAGELLAN_USE_ARDUINOJSON7
#include <ArduinoJson.h>
#else
#include "./ArduinoJson-v6.18.3.h"
#endif

#endif
