/*
Copyright (c) 2020, Advanced Wireless Network
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
support esp32, esp8266

Author:(POC Device Magellan team)
Create Date: 25 April 2022.
Modified: 22 dec 2025.
*/

/*
 * This file includes code from TinyGSM
 * Copyright (c) 2016-2024 Volodymyr Shymanskyy
 * Licensed under LGPL-3.0-or-later
 *
 * Modifications:
 *  - Adapted for AIS 4G Board
 */
#include <Arduino.h>
#include "MAGELLAN_MQTT_4G_BOARD.h"

#ifdef ESP32
const char *_apn = "aisboard.4g.ais";
HardwareSerial _SerialAT(1);
TinyGsm _modem(_SerialAT);
TinyGsmClient _gsmClient(_modem);

TinyGsmClient &MAGELLAN_MQTT_4G_BOARD::getGSMClient()
{
  return _gsmClient;
}

TinyGsm &MAGELLAN_MQTT_4G_BOARD::getGSMModem()
{
  return _modem;
}

int mapRSSITodBm(int rssi)
{
  if (rssi == 99)
    return -113; // Not detectable

  return -113 + (rssi * 2); // Map RSSI to dBm
}

String getSignalStrengthCategory(int dBm)
{
  if (dBm <= -113)
    return "Very Poor";
  else if (dBm > -113 && dBm <= -85)
    return "Poor";
  else if (dBm > -85 && dBm <= -70)
    return "Fair";
  else if (dBm > -70 && dBm <= -55)
    return "Good";
  else if (dBm > -55)
    return "Excellent";

  return "Unknown";
}

void getRadio()
{
  MG_LOG_I("#========= Radio Quality information ==========");
  int rssiNomalized = _modem.getSignalQuality();
  int rssiDbm = mapRSSITodBm(rssiNomalized);
  MG_LOG_I_S("Signal Strength: " + String(rssiNomalized));
  MG_LOG_I_S("Signal Strength(dBm): " + String(rssiDbm));
  MG_LOG_I_S("Description: " + String(getSignalStrengthCategory(rssiDbm)));
  MG_LOG_I("#==============================================");
}

MAGELLAN_MQTT_4G_BOARD::MAGELLAN_MQTT_4G_BOARD() : MAGELLAN_MQTT(_gsmClient)
{
  gps.parent = this;
  centric.parent = this;

  // Ensure modem is powered down before OTA-triggered restart on 4G board.
  attr.cb_before_restart = []()
  {
    MG_LOG_I("# GSM shutdown before restart...");
    _modem.poweroff();
    delay(4000);
  };
}

void MAGELLAN_MQTT_4G_BOARD::initSerialModem()
{
  _SerialAT.setRxBufferSize(4096 * 2);
  _SerialAT.begin(115200, SERIAL_8N1, PIN_MODEM_RX, PIN_MODEM_TX);
  delay(1000);
  if (!_modem.init())
  {
    _modem.init();
  }
}

void MAGELLAN_MQTT_4G_BOARD::powerModem()
{
  pinMode(PIN_MODEM_PWR, OUTPUT);
  MG_LOG_I("Restarting modem...");
  digitalWrite(PIN_MODEM_PWR, LOW);
  delay(50);
  digitalWrite(PIN_MODEM_PWR, HIGH);
  delay(50);
}

void MAGELLAN_MQTT_4G_BOARD::connectModem()
{
  MG_LOG_I("Connecting to mobile network...");
  int retry = 0;
  while (!_modem.gprsConnect(_apn))
  {
    MG_LOG_E_S("Failed to connect! Retry " + String(++retry) + "/10");
    delay(1000);

    if (retry >= 10)
    {
      MG_LOG_E("Max retries reached. Restarting ESP...");
      ESP.restart();
    }
  }
  MG_LOG_I("modem connected!");
}
void MAGELLAN_MQTT_4G_BOARD::checkModem()
{
  if (!_modem.isGprsConnected())
  {
    MG_LOG_I("Reconnecting PPP...");
    _modem.gprsConnect(_apn);
    delay(500); // รอ PPP stable
  }
}

void MAGELLAN_MQTT_4G_BOARD::HandleModem()
{
  if (_modem.isGprsConnected() && !this->MAGELLAN_MQTT::isConnected())
  {
    MG_LOG_I("Reconnecting MQTT...");
    this->MAGELLAN_MQTT::reconnect();
  }
}

void MAGELLAN_MQTT_4G_BOARD::InitGSM()
{
  MG_LOG_I("# ==== USE AIS 4G BOARD MODE INIT GSM ====");
  this->powerModem();
  delay(1000);
  this->initSerialModem();
  this->connectModem();
  getRadio();
}

void MAGELLAN_MQTT_4G_BOARD::begin(Magellan_Setting _setting)
{
  this->InitGSM();

#ifdef BYPASS_REQTOKEN
  if (_setting.ThingToken != "null" && _setting.ThingToken.length() > 25)
  {
    this->coreMQTT->setManualToken(_setting.ThingToken);
  }
  else
  {
    MG_LOG_E("# Invalid setting ThingToken");
    MG_LOG_I("# Define \"BYPASS_REQTOKEN\" but not setting ThingToken manual back into auto renew ThingToken mode");
  }
#endif

  if (_setting.clientBufferSize > _default_OverBufferSize)
  {
    MG_LOG_I_S("# You have set a buffer size greater than 8192, adjusts to: " + String(_default_OverBufferSize));
    this->coreMQTT->setMQTTBufferSize(_default_OverBufferSize);
    attr.calculate_chunkSize = _default_OverBufferSize / 2;
  }
  else
  {
    this->coreMQTT->setMQTTBufferSize(_setting.clientBufferSize);
    attr.calculate_chunkSize = _setting.clientBufferSize / 2;
  }

  size_t revertChunkToBufferSize = attr.calculate_chunkSize * 2;
  // ThingIdentifier(ICCID) and ThingSecret(IMSI) .
  _setting.ThingIdentifier.trim();
  _setting.ThingSecret.trim();
  _setting.IMEI.trim();

  credentialFile.initCredentialFile();

  if (_setting.ThingIdentifier == "null" || _setting.ThingSecret == "null")
  {
    // #if defined(USE_4G_BOARD) || defined(USE_AIS_4G_BOARD)
    _setting.ThingIdentifier = _modem.getSimCCID();
    delay(50);
    _setting.ThingSecret = _modem.getIMSI();
    delay(50);
    _setting.IMEI = _modem.getIMEI();
    delay(50);
    MG_LOG_D("============ Board Information ============");
    MG_LOG_D_S("ICCID: " + _setting.ThingIdentifier);
    MG_LOG_D_S("IMSI : " + _setting.ThingSecret);
    MG_LOG_I_S("IMEI : " + _setting.IMEI);
    MG_LOG_D("===========================================");
    setting = _setting;
  }
  // second validate after get information
  if (coreMQTT->CheckString_isDigit(_setting.ThingIdentifier) && coreMQTT->CheckString_isDigit(_setting.ThingSecret))
  {
    // Serial.println(F("=========== Prepare Credentials ============"));
    // Serial.print(F("ThingIdentifier: "));
    // Serial.println(_setting.ThingIdentifier);
    // Serial.print(F("ThingSecret: "));
    // Serial.println(_setting.ThingSecret);
    // Serial.print(F("IMEI: "));
    // Serial.println(_setting.IMEI);
    // Serial.println(F("============================================"));
    this->MAGELLAN_MQTT::begin(_setting);
    setting = _setting;
  }
  else
  {
    MG_LOG_E("# ThingIdentifier(ICCID) or ThingSecret(IMSI) invalid value please check again");
    MG_LOG_D_S("# ThingIdentifier =>" + _setting.ThingIdentifier);
    MG_LOG_D_S("# ThingSecret =>" + _setting.ThingSecret);
    MG_LOG_E("# Restart board");
    delay(5000);
    ESP.restart();
  }

  this->builtInSensor.begin();
}

void MAGELLAN_MQTT_4G_BOARD::disconnect()
{
  this->MAGELLAN_MQTT::disconnect();
}

void MAGELLAN_MQTT_4G_BOARD::reconnect()
{
  MG_LOG_I("# ==== USE AIS 4G BOARD MODE RECONNECT MQTT ====");
  this->MAGELLAN_MQTT::reconnect();
}

void MAGELLAN_MQTT_4G_BOARD::loop()
{
  this->HandleModem();
  this->MAGELLAN_MQTT::loop();
}

void MAGELLAN_MQTT_4G_BOARD::Centric::begin(Magellan_Setting _setting)
{
  this->parent->InitGSM();

  if (!_modem.isGprsConnected())
  {
    MG_LOG_I("Connecting to mobile network for Centric...");
    int retry = 0;
    while (!_modem.gprsConnect(_apn))
    {
      MG_LOG_E_S("Failed to connect! Retry " + String(++retry) + "/10");
      delay(2000);

      if (retry >= 10)
      {
        MG_LOG_E("Max retries reached. Restarting ESP...");
        ESP.restart();
      }
    }
    MG_LOG_I("modem connected for Centric!");
  }

  if (_setting.ThingIdentifier == "null" || _setting.ThingSecret == "null")
  {
    _setting.ThingIdentifier = _modem.getSimCCID();
    delay(50);
    _setting.ThingSecret = _modem.getIMSI();
    delay(50);
    _setting.IMEI = _modem.getIMEI();
    delay(50);
    MG_LOG_D("=================================");
    MG_LOG_D_S("ICCID: " + _setting.ThingIdentifier);
    MG_LOG_D_S("IMSI : " + _setting.ThingSecret);
    MG_LOG_I_S("IMEI : " + _setting.IMEI);
    MG_LOG_D("=================================");
    setting = _setting;
  }

  // Validate credentials
  if (coreMQTT->CheckString_isDigit(setting.ThingIdentifier) && coreMQTT->CheckString_isDigit(setting.ThingSecret))
  {
    MG_LOG_D_S("Centric ThingIdentifier: " + String(setting.ThingIdentifier));
    MG_LOG_D_S("Centric ThingSecret: " + String(setting.ThingSecret));

    parent->coreMQTT->setAuthMagellan(setting.ThingIdentifier, setting.ThingSecret, setting.IMEI);
    parent->coreMQTT->magellanCentric();
    // Connect to MQTT broker with credentials
    MG_LOG_I("Connecting to Centric MQTT...");
  }
  else
  {
    MG_LOG_E("# Centric credentials invalid!");
    MG_LOG_D_S("# ThingIdentifier =>" + setting.ThingIdentifier);
    MG_LOG_D_S("# ThingSecret =>" + setting.ThingSecret);
    MG_LOG_E("# Restart board");
    delay(5000);
    ESP.restart();
  }
  this->parent->builtInSensor.begin();
}

int16_t MAGELLAN_MQTT_4G_BOARD::getSignalStrength(){
  int rssiNomalized = _modem.getSignalQuality();
  int rssiDbm = mapRSSITodBm(rssiNomalized);
  return rssiDbm;
}

String MAGELLAN_MQTT_4G_BOARD::getRSSIQuality(){
  int rssiNomalized = _modem.getSignalQuality();
  int16_t dBm = mapRSSITodBm(rssiNomalized);
  return getSignalStrengthCategory(dBm);
}

// GPS

GPS_Data MAGELLAN_MQTT_4G_BOARD::GPS_utils::getCurrentGPSData()
{
  TinyGsm &modem = this->parent->getGSMModem();
  GPS_Data data;
  if (this->gps_internal.gpsIsOn(modem))
  {
    this->gps_internal.gpsRead(modem, data); 
  }
  this->_gpsData = data;
  return data;
}

boolean MAGELLAN_MQTT_4G_BOARD::GPS_utils::available()
{
  TinyGsm &modem = this->parent->getGSMModem();
  return this->gps_internal.available(modem);
}
float MAGELLAN_MQTT_4G_BOARD::GPS_utils::readLatitude()
{
  float _lat = 0.000000f;
  _lat = this->getCurrentGPSData().lat;
  return _lat;
}
float MAGELLAN_MQTT_4G_BOARD::GPS_utils::readLongitude()
{
  float _lng = 0.000000f;
  _lng = this->getCurrentGPSData().lng;
  return _lng;
}
float MAGELLAN_MQTT_4G_BOARD::GPS_utils::readAltitude()
{
  float _alt = 0.000000f;
  _alt = this->getCurrentGPSData().alt;
  return _alt;
}
float MAGELLAN_MQTT_4G_BOARD::GPS_utils::readSpeed()
{
  float _spd = 0.000000f;
  _spd = this->getCurrentGPSData().speed;
  return _spd;
}
float MAGELLAN_MQTT_4G_BOARD::GPS_utils::readCourse()
{
  float _course = 0.000000f;
  _course = this->getCurrentGPSData().course;
  return _course;
}
String MAGELLAN_MQTT_4G_BOARD::GPS_utils::readLocation()
{
  String _location = "0.000000,0.000000";
  _location = String(this->readLatitude(), 6) + "," + String(this->readLongitude(), 6);
  return _location;
}
unsigned long MAGELLAN_MQTT_4G_BOARD::GPS_utils::getUnixTime()
{
  unsigned long _unix = 0;
  _unix = this->getCurrentGPSData().utc;
  return _unix;
}

void MAGELLAN_MQTT_4G_BOARD::GPS_utils::disable()
{
  TinyGsm &modem = this->parent->getGSMModem();
  this->gps_internal.gpsEnd(modem);
}
void MAGELLAN_MQTT_4G_BOARD::GPS_utils::begin()
{
  TinyGsm &modem = this->parent->getGSMModem();
  modem.enableGPS();
  delay(500);
  this->gps_internal.gpsInit(modem);
}

// Built-in Sensor
void MAGELLAN_MQTT_4G_BOARD::BuiltinSensor::begin()
{
  Wire.begin();
  SHT40.begin();
}

float MAGELLAN_MQTT_4G_BOARD::BuiltinSensor::readTemperature()
{
  return SHT40.readTemperature();
}

float MAGELLAN_MQTT_4G_BOARD::BuiltinSensor::readHumidity()
{
  return SHT40.readHumidity();
}
#endif // ESP32