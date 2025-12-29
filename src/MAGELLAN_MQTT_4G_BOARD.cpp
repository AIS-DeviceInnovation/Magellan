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

// extern Magellan_Setting setting;

const char *_apn = "aisboard.4g.ais";
HardwareSerial _SerialAT(1);
TinyGsm _modem(_SerialAT);
TinyGsmClient _gsmClient(_modem);

// typedef struct
// {
//   String endPoint_IP;
//   String endPoint_DOMAIN;
//   String endPoint_PORT;
// } Centric;

// Centric _centric;
StaticJsonDocument<512> _intern_docJSON;

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
  Serial.println(F("#========= Radio Quality information =========="));
  int rssiNomalized = _modem.getSignalQuality();
  int rssiDbm = mapRSSITodBm(rssiNomalized);
  Serial.println("Signal Strength: " + String(rssiNomalized));
  Serial.println("Signal Strength(dBm): " + String(rssiDbm));
  Serial.println("Description: " + String(getSignalStrengthCategory(rssiDbm)));
  Serial.println(F("#=============================================="));
}

MAGELLAN_MQTT_4G_BOARD::MAGELLAN_MQTT_4G_BOARD() : MAGELLAN_MQTT(_gsmClient)
{
  centric.parent = this;
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
  Serial.println("Restarting modem...");
  digitalWrite(PIN_MODEM_PWR, LOW);
  delay(50);
  digitalWrite(PIN_MODEM_PWR, HIGH);
  delay(50);
}

void MAGELLAN_MQTT_4G_BOARD::connectModem()
{
  Serial.println("Connecting to mobile network...");
  int retry = 0;
  while (!_modem.gprsConnect(_apn))
  {
    Serial.print("Failed to connect! Retry ");
    Serial.print(++retry);
    Serial.println("/10");
    delay(2000);

    if (retry >= 10)
    {
      Serial.println("Max retries reached. Restarting ESP...");
      ESP.restart();
    }
  }
  Serial.println("modem connected!");
}
void MAGELLAN_MQTT_4G_BOARD::checkModem()
{
  if (!_modem.isGprsConnected())
  {
    Serial.println("Reconnecting PPP...");
    _modem.gprsConnect(_apn);
    delay(500); // รอ PPP stable
  }
}

void MAGELLAN_MQTT_4G_BOARD::HandleModem()
{
  if (_modem.isGprsConnected() && !this->MAGELLAN_MQTT::isConnected())
  {
    Serial.println("Reconnecting MQTT...");
    this->MAGELLAN_MQTT::reconnect();
  }
}

void MAGELLAN_MQTT_4G_BOARD::begin(Magellan_Setting _setting)
{

  this->powerModem();
  delay(1000);
  Serial.println(F("# ==== USE AIS 4G BOARD MODE ===="));
  this->initSerialModem();
  this->connectModem();
  getRadio();

#ifdef BYPASS_REQTOKEN
  if (_setting.ThingToken != "null" && _setting.ThingToken.length() > 25)
  {
    this->coreMQTT->setManualToken(_setting.ThingToken);
  }
  else
  {
    Serial.println(F("# Invalid setting ThingToken"));
    Serial.println(F("# Define \"BYPASS_REQTOKEN\" but not setting ThingToken manual back into auto renew ThingToken mode"));
  }
#endif

  if (_setting.clientBufferSize > _default_OverBufferSize)
  {
    Serial.print(F("# You have set a buffer size greater than 8192, adjusts to: "));
    Serial.println(_default_OverBufferSize);
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
    Serial.println(F("================================="));
    Serial.println("ICCID: " + _setting.ThingIdentifier);
    Serial.println("IMSI : " + _setting.ThingSecret);
    Serial.println("IMEI : " + _setting.IMEI);
    Serial.println(F("================================="));
    setting = _setting;
  }
  // second validate after get information
  if (coreMQTT->CheckString_isDigit(_setting.ThingIdentifier) && coreMQTT->CheckString_isDigit(_setting.ThingSecret))
  {
    Serial.print(F("ThingIdentifier: "));
    Serial.println(_setting.ThingIdentifier);
    Serial.print(F("ThingSecret: "));
    Serial.println(_setting.ThingSecret);
    Serial.print(F("IMEI: "));
    Serial.println(_setting.IMEI);
    this->MAGELLAN_MQTT::begin(_setting);
    setting = _setting;
  }
  else
  {
    Serial.println(F("# ThingIdentifier(ICCID) or ThingSecret(IMSI) invalid value please check again"));
    Serial.println("# ThingIdentifier =>" + _setting.ThingIdentifier);
    Serial.println("# ThingSecret =>" + _setting.ThingSecret);
    Serial.println(F("# ==========================="));
    Serial.println(F("# Restart board"));
    delay(5000);
    ESP.restart();
  }
}

void MAGELLAN_MQTT_4G_BOARD::disconnect()
{
  this->MAGELLAN_MQTT::disconnect();
}

void MAGELLAN_MQTT_4G_BOARD::reconnect()
{
  Serial.println(F("# ==== USE AIS 4G BOARD MODE RECONNECT MQTT ===="));
  this->MAGELLAN_MQTT::reconnect();
}

void MAGELLAN_MQTT_4G_BOARD::loop()
{
  this->HandleModem();
  this->MAGELLAN_MQTT::loop();
}

void MAGELLAN_MQTT_4G_BOARD::Centric::begin(Magellan_Setting _setting)
{
  Serial.println(F("# ==== USE AIS 4G BOARD MODE CENTRIC ===="));
  parent->powerModem();
  delay(1000);
  parent->initSerialModem();
  parent->connectModem();
  getRadio();
  // Ensure modem is connected
  if (!_modem.isGprsConnected())
  {
    Serial.println("Connecting to mobile network for Centric...");
    int retry = 0;
    while (!_modem.gprsConnect(_apn))
    {
      Serial.print("Failed to connect! Retry ");
      Serial.print(++retry);
      Serial.println("/10");
      delay(2000);

      if (retry >= 10)
      {
        Serial.println("Max retries reached. Restarting ESP...");
        ESP.restart();
      }
    }
    Serial.println("modem connected for Centric!");
  }

  if (_setting.ThingIdentifier == "null" || _setting.ThingSecret == "null")
  {
    _setting.ThingIdentifier = _modem.getSimCCID();
    delay(50);
    _setting.ThingSecret = _modem.getIMSI();
    delay(50);
    _setting.IMEI = _modem.getIMEI();
    delay(50);
    Serial.println(F("================================="));
    Serial.println("ICCID: " + _setting.ThingIdentifier);
    Serial.println("IMSI : " + _setting.ThingSecret);
    Serial.println("IMEI : " + _setting.IMEI);
    Serial.println(F("================================="));
    setting = _setting;
  }

  // Validate credentials
  if (coreMQTT->CheckString_isDigit(setting.ThingIdentifier) && coreMQTT->CheckString_isDigit(setting.ThingSecret))
  {
    Serial.print(F("Centric ThingIdentifier: "));
    Serial.println(setting.ThingIdentifier);
    Serial.print(F("Centric ThingSecret: "));
    Serial.println(setting.ThingSecret);

    parent->coreMQTT->setAuthMagellan(setting.ThingIdentifier, setting.ThingSecret, setting.IMEI);
    parent->coreMQTT->magellanCentric();
    // Connect to MQTT broker with credentials
    Serial.println(F("Connecting to Centric MQTT..."));
  }
  else
  {
    Serial.println(F("# Centric credentials invalid!"));
    Serial.println("# ThingIdentifier =>" + setting.ThingIdentifier);
    Serial.println("# ThingSecret =>" + setting.ThingSecret);
    Serial.println(F("# ==========================="));
    Serial.println(F("# Restart board"));
    delay(5000);
    ESP.restart();
  }
}
