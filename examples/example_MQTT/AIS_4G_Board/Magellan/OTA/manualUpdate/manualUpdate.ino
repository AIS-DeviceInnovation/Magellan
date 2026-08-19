#include <Arduino.h>
#include <MAGELLAN_MQTT_4G_BOARD.h>

MAGELLAN_MQTT_4G_BOARD magel;

OTA_state checkStatusUpdate = OTA_state::UNKNOWN_STATE;

void setup()
{
  Serial.begin(115200);
  magel.OTA.autoUpdate(false); // disable auto update — control update manually
  setting.clientBufferSize = defaultOTABuffer; // set buffer size compatible for OTA
  magel.begin(setting);

  magel.getServerConfig("autoUpdate", [](String resp) {
    if (resp == "1")
    {
      magel.OTA.autoUpdate(true);
    }
    else
    {
      magel.OTA.autoUpdate(false);
    }
    magel.clientConfig.add("autoUpdateMode", ((magel.OTA.getAutoUpdate()) ? "ENABLE" : "DISABLE"));
    magel.clientConfig.save(); // update client config from device to thing (optional)
  });

  magel.getControl([](String key, String value) {
    if (key == "executeUpdate")
    {
      magel.control.ACK("executeUpdate", value);
      if (value == "1")
      {
        magel.OTA.executeUpdate(); // execute OTA update
      }
    }
    else // acknowledge other control
    {
      magel.control.ACK(key, value);
    }
  });

  // prepare sensor for adding widget control
  magel.report.send("executeUpdate", "0");
}

void loop()
{
  magel.loop();
  magel.subscribesHandler([]() {
    checkStatusUpdate = magel.OTA.checkUpdate(); // check once after connect and after reconnect
  });
  magel.interval(10, []() { // time interval function inside every 10000 millis
    switch (checkStatusUpdate)
    {
    case OTA_state::UP_TO_DATE:
      Serial.print(F("checkStatusUpdate: "));
      Serial.println("# UP_TO_DATE");
      break;
    case OTA_state::OUT_OF_DATE:
      Serial.print(F("checkStatusUpdate: "));
      Serial.println(F("# OUT_OF_DATE"));
      break;
    default:
      Serial.print(F("checkStatusUpdate: "));
      Serial.println("# UNKNOWN");
      break;
    }
  });
}
