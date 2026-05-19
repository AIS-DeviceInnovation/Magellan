#pragma once

// ---------------------------------------------------------------------------
// Magellan SDK - compile-time log level control
//
// Three hierarchical levels - each level includes all levels below it:
//   0  NONE   - silent (no Serial output, lowest RAM/flash footprint)
//   1  ERROR  - fatal errors and configuration failures only
//   2  INFO   - normal operational status: connect, report, OTA lifecycle
//                (DEFAULT - same verbosity as original SDK)
//   3  DEBUG  - verbose per-call detail: chunk progress, retransmit counts,
//                memory usage, token/ICCID/IMSI values
//
// Override in platformio.ini (per-environment):
//   build_flags = -D MAGELLAN_LOG_LEVEL=0   ; silent production build
//   build_flags = -D MAGELLAN_LOG_LEVEL=1   ; error-only
//   build_flags = -D MAGELLAN_LOG_LEVEL=2   ; info (default)
//   build_flags = -D MAGELLAN_LOG_LEVEL=3   ; full debug
//
// Two macro families:
//   MG_LOG_X(fmt, ...)   - printf-style (literal format string, no heap)
//   MG_LOG_X_S(expr)     - Arduino String expression (e.g. "text " + var)
//                          When the level is disabled the expression is
//                          never evaluated -> zero String allocations.
// ---------------------------------------------------------------------------

#ifndef MAGELLAN_LOG_LEVEL
#define MAGELLAN_LOG_LEVEL 3 // Default to full debug if not defined    
#endif

#define MAGELLAN_LOG_LEVEL_NONE  0
#define MAGELLAN_LOG_LEVEL_ERROR 1
#define MAGELLAN_LOG_LEVEL_INFO  2
#define MAGELLAN_LOG_LEVEL_DEBUG 3

// ERROR
#if (MAGELLAN_LOG_LEVEL >= MAGELLAN_LOG_LEVEL_ERROR)
#define MG_LOG_E(fmt, ...) Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define MG_LOG_E_S(expr)   Serial.println(expr)
#else
#define MG_LOG_E(fmt, ...) ((void)0)
#define MG_LOG_E_S(expr)   ((void)0)
#endif

// INFO
#if (MAGELLAN_LOG_LEVEL >= MAGELLAN_LOG_LEVEL_INFO)
#define MG_LOG_I(fmt, ...) Serial.printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define MG_LOG_I_S(expr)   Serial.println(expr)
#else
#define MG_LOG_I(fmt, ...) ((void)0)
#define MG_LOG_I_S(expr)   ((void)0)
#endif

// DEBUG
#if (MAGELLAN_LOG_LEVEL >= MAGELLAN_LOG_LEVEL_DEBUG)
#define MG_LOG_D(fmt, ...) Serial.printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define MG_LOG_D_S(expr)   Serial.println(expr)
#else
#define MG_LOG_D(fmt, ...) ((void)0)
#define MG_LOG_D_S(expr)   ((void)0)
#endif
