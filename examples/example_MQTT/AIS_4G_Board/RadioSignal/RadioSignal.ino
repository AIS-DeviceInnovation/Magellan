#include <Arduino.h>

#include <MAGELLAN_MQTT_4G_BOARD.h>
MAGELLAN_MQTT_4G_BOARD board;
MAGELLAN_MQTT_4G_BOARD::ConnectivityModem &gsmBoard = board.GSMModem;

void setup()
{
  Serial.begin(115200);
  gsmBoard.begin();
}

void loop()
{
  gsmBoard.handle();         // check modem status and reconnect if needed
  LTE_Signal_INFO signalInfo = board.radioSignal.getDetailedSignal(); // get signal strength and quality
  Serial.print("=================================");
  Serial.print("Signal Mode: ");
  Serial.println(signalInfo.mode);
  Serial.print("Signal Band: ");        
  Serial.println(signalInfo.band);
  Serial.print("Signal RSRQ: ");
  Serial.println(signalInfo.rsrq);
  Serial.print("Signal RSRP: ");
  Serial.println(signalInfo.rsrp);
  Serial.print("Signal RSSI: ");
  Serial.println(signalInfo.rssi);
  Serial.print("Signal SINR: ");
  Serial.println(signalInfo.sinr);
  Serial.print("=================================");
  delay(5000); // wait for 5 seconds before checking again
}
