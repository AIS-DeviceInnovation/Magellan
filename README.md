![Library Version](https://img.shields.io/badge/Version-1.1.0-green)

# Magellan Library for Arduino

Magellan คือ IoT Platform (Internet of Things Platform) ครบวงจรของ ประเทศไทยที่จัดทำขึ้นเพื่อส่งเสริมสนับสนุนบุคคลากรหน่วยงานหรือองค์กรต่างๆ ในประเทศให้สามารถศึกษาวิจัยและพัฒนาความรู้ความสามารถจนสามารถนำไปใช้พัฒนาเป็นสินค้า บริการ หรือนวัตกรรม ด้าน IoT แบบเต็มรูปแบบ อีกทั้งยังเป็นแพลตฟอร์มตั้งต้นสำหรับนักพัฒนา เพราะเรามี API และ SDK ในการรองรับการใช้งานทำให้สามารถพัฒนาโปรแกรมได้โดยง่าย นอกจากนี้ยังมีหน้าเว็บที่รองรับในส่วนของ Dashboard และ Widget ไว้ให้นักพัฒนาสามารถสร้างสรรค์ผลงานได้สะดวกสบายมากยิ่งขึ้น

ไลบรารี Magellan สำหรับ ESP32 และ ESP8266 ใช้กับโปรแกรม Arduino IDE รองรับการเชื่อมต่อ MQTT รับ-ส่งค่าเซนเซอร์ สั่งงานอุปกรณ์ และตั้งค่าต่าง ๆ ให้กับอุปกรณ์ เป็นต้น

-------

## สารบัญ

 * [Magellan Platform Features](#magellan-platform-features)
 * [การเริ่มต้นใช้งาน](#การเริ่มต้นใช้งาน)
 * [การส่งข้อมูลขึ้น Magellan Platform](#การส่งข้อมูลขึ้น-magellan-platform)
   * [เตรียมโค้ดโปรแกรมเชื่อมต่อ Magellan platform](#เตรียมโค้ดโปรแกรมเชื่อมต่อ-magellan-platform)
   * [สมัคร AIS Playground และ Magellan Platform](#สมัคร-ais-playground-และ-magellan-platform)
   * [ลงทะเบียน Device และสร้างโปรเจค](#ลงทะเบียน-device-และสร้างโปรเจค)
   * [เพิ่ม Device เข้าโปรเจค และตั้งค่า Heartbeat](#เพิ่ม-device-เข้าโปรเจค-และตั้งค่า-heartbeat)
   * [แก้ไขโค้ดโปรแกรมให้เชื่อมต่อ](#แก้ไขโค้ดโปรแกรม)
   * [ตรวจสอบผลการทำงานบน Magellan Platform](#ตรวจสอบผลการทำงานบน-magellan-platform)
 * [คำสั่งที่มีให้ใช้งาน](#คำสั่งที่มีให้ใช้งาน)
   * [MAGELLAN_MQTT.h](#include-magellan_mqtth)
   * [MAGELLAN_WiFi_SETTING.h](#include-magellan_wifi_settingh)
 * [ศึกษาเพิ่มเติม](#ศึกษาเพิ่มเติม)
   * [ตัวอย่างโค้ดโปรแกรม](#ตัวอย่างโค้ดโปรแกรม)

## Magellan Platform Features

 * Device Management - การจัดการเกี่ยวกับอุปกรณ์ IoT การตั้งค่าต่างๆ ของอุปกรณ์ การรับส่งข้อมูลผ่านโปรโตคอลที่รองรับได้แก่ CoAP , MQTT และ HTTP การจัดการเกี่ยวกับ Data Storage ที่มีการทำ Message cache, Data history และ Compare data รวมไปถึงการสั่งงานควบคุมอุปกรณ์ให้เปิด-ปิด ได้ด้วยตัวคุณเอง ทั้งแบบ API และผ่านหน้าเว็บของแพลตฟอร์ม
 * Plug & Play เรามีการจัดการการเชื่อมต่อระหว่างอุปกรณ์ AIS NB-IoT กับแพลตฟอร์มไว้แล้ว เพียงแค่ติดตั้ง SDK ที่ทางเรามีพร้อมให้ใช้งาน ก็จะสามารถเริ่มใช้งานได้ ทั้งการ รับ-ส่ง ข้อมูลระหว่างอุปกรณ์ การตั้งค่าเพื่อการควบคุมและใช้งาน
 * Data Visualization เรามีการนำข้อมูลที่ได้จากอุปกรณ์มาแสดงผลทั้งแบบกราฟ location map และอีกหลายอย่าง โดยเป็น widget หลากหลายแบบให้คุณสามารถเลือกออกแบบหน้าจอของการแสดงผลของคุณเองได้ รวมทั้งควบคุมอุปกรณ์ของคุณผ่านทางเว็บได้อย่างง่ายดาย
 * Teams Collaboration เป็นฟังก์ชันสำหรับรองรับการทำงานเป็นทีม คุณสามารถ แชร์โปรเจกต์หรือDashboard ของคุณที่สร้างในแพลตฟอร์มไปให้เพื่อนร่วมทีมของคุณได้ เพื่อให้ทุกคนเข้ามามีส่วนรวมในการทำงานร่วมกันได้อย่างง่ายดาย
 * Open Platformคือการที่คุณสามารถแลกเปลี่ยนข้อมูล data ต่าง ๆ กับแพลตฟอร์มอื่นได้

## การเริ่มต้นใช้งาน

 * เสียบสาย USB เข้ากับ ESP32 หรือ ESP8266 ปลายอีกด้านเสียบเข้ากับคอมพิวเตอร์
 * ดาวน์โหลดไดร์เวอร์ FT231X จาก [VCP Drivers](https://ftdichip.com/drivers/vcp-drivers/) และติดตั้งตามขั้นตอน [Installation Guides](https://ftdichip.com/document/installation-guides/)
 * ดาวน์โหลดโปรแกรม Arduino IDE V2.xx.xx จาก [Software | Arduino](https://www.arduino.cc/en/software) แล้วติดตั้งโปรแกรมตามขั้นตอน [Install the Arduino Software (IDE) on Windows PCs](https://www.arduino.cc/en/Guide/Windows)
 * เปิดโปรแกรม Arduino IDE ขึ้นมา แล้วติดตั้งแพ็กเกจบอร์ดเพิ่ม 
 * ติดตั้งไลบรารี่ `Magellan` ผ่าน Library Manager (อ่านเพิ่มเติม [Installing Additional Arduino Libraries](https://www.arduino.cc/en/Guide/Libraries))
 * หากเป็น ESP32 ให้เลือกบอร์ดเป็น ESP32 Dev Module หรือเป็น ESP8266 ให้เลือกบอร์ดเป็น Generic ESP8266 Module แล้วเลือกพอร์ตเป็น COM port ที่ใช้
 * เปิดโปรแกรมตัวอย่าง [credential](examples/example_MQTT/credential/credential.ino) โดยใช้คำสั่ง getThingIdentifier() และ magel.credential.getThingSecret() เพื่อทดสอบอ่านหมายเลข ThingIdentifier และ ThingSecret ของโมดูล


## การส่งข้อมูลขึ้น Magellan Platform

### เตรียมโค้ดโปรแกรมเชื่อมต่อ Magellan platform

 * ติดตั้งไลบรารี Magellan ตามหัวข้อ [การเริ่มต้นใช้งาน](#การเริ่มต้นใช้งาน)
 * ใช้ตัวอย่าง [reportSensorJSON](https://github.com/AIS-DeviceInnovation/Magellan/blob/main/examples/example_MQTT/reportData/reportDataJSON/reportDataJSON.ino) ในการทดสอบส่งข้อมูลขึ้น Magellan Platform

### สมัคร AIS Playground และ Magellan Platform

 * เข้าไปที่ https://magellan.ais.co.th/ กด REGISTER เพื่อสมัครสมาชิก หากไม่มี Account ของ AIS Playground ให้กดเลือก [Register](https://apisgl.ais.co.th/auth/v3.1/oauth/authorize?response_type=code&client_id=iQftE1wqrGJMCbco8D4MADHySRZpgMXlI5tU3sBYNmY%3D&redirect_uri=https%3A%2F%2Fmagellan.ais.co.th%2F&state=sgl&scope=profile#) กรอกข้อมูลแล้วกด ปุ่ม Done จากนั้นรอ Email ยืนยัน 
 * เมื่อได้รับ Email ยืนยันเรียบร้อยแล้ว สามารถ Login ใช้งาน Magellan Platform ได้ผ่าน Email ที่สมัครไว้

### ลงทะเบียน Device และสร้างโปรเจค

 * กดที่แถบ menu ซ้ายมือเลือก MY THINGS กดปุ่ม RESIGTER THING กรอก ThingIdentifier และ ThingSecret ตั้งชื่อ Thing กดปุ่ม SAVE
 * กดที่แถบ menu ซ้ายมือเลือก ALL PROJECT กดเลือกที่ MY PROJECT กดปุ่ม CREATE NEW PROJECT แล้วตั้งชื่อ กดปุ่ม CREATE 

### เพิ่ม Device เข้าโปรเจค และตั้งค่า Heartbeat
 * กดเลือกโปรเจคที่สร้าง กดเลือกที่แถบ THING และกดปุ่ม ADD THING แล้วเลือก Device ที่ลงทะเบียนไว้โดยกดปุ่ม ADD TO PROJECT
 * กดเลือกที่กล่อง Thing กดปุ่ม EDIT THING ให้กดเลือก Heartbeat กำหนดเวลาที่ต้องการให้แสดงหาก Device ขาดการเชื่อมต่อในที่นี้ยกตัวอย่างเป็น 20 วินาที และกดปุ่ม SAVE


### แก้ไขโค้ดโปรแกรม

 * ไปที่โค้ดตัวอย่าง [reportSensorJSON](https://github.com/AIS-DeviceInnovation/Magellan/blob/main/examples/example_MQTT/reportData/reportDataJSON/reportDataJSON.ino)
 * อัพโหลดโปรแกรมลงบอร์ด

### ตรวจสอบผลการทำงานบน Magellan Platform

 * เปิด Serial Monitor ขึ้นมา ในหน้าต่าง Serial Monitor จะแจ้งสถานะการเชื่อมต่อกับ Magellan Platform 
 * เมื่อเชื่อมต่อสำเร็จ ค่าอุณหภูมิและความชื้นจะส่งขึ้น Magellan Platfrom ทุก ๆ 10 วินาที
 * ใน Magellan Platform ที่หน้าอุปกรณ์ จะแสดงสถานะอุปกรณ์เป็น Connected 
 * กดดูข้อมูลแบบละเอียดได้ในแถบ Data History 
 * สังเกตว่าใน Serial Monitor จะแสดงผลข้อความแจ้งค่าอุณหภูมิและความชื้นจากเซ็นเซอร์บนบอร์ดตามคำสั่งที่กำหนด

## คำสั่งที่มีให้ใช้งาน

### `#include <MAGELLAN_MQTT.h>`

ใช้เชื่อมต่อ รับ-ส่งข้อมูลกับ Magellan Platform ด้วยโปรโตคอล MQTT (Message Queuing Telemetry Transport)

 * `MAGELLAN_MQTT magel(Client &_Client);` เริ่มต้นใช้งานไลบรารี Magellan Platform สืบทอดคลาส MAGELLAN_MQTT ด้วย 
  ```cpp
    #include <MAGELLAN_MQTT.h>
    WiFiClient WiFi_client;
    MAGELLAN_MQTT magel(WiFi_client);
  ```
 * `setting (Global Object Variables)`
   * `setting.ThingIdentifier` ใช้เป็นตัวแปรสำหรับกำหนดค่า ThingIdentifier 
   * `setting.ThingSecret` ใช้เป็นตัวแปรสำหรับกำหนดค่า ThingSecret 
   * `setting.endpoint` ใช้เป็นตัวแปรสำหรับกำหนดค่า IP หรือ URL Path ปลายทางที่ต้องการให้อุปกรณ์เชื่อมต่อ
   * `setting.clientBufferSize` ใช้เป็นตัวแปรสำหรับกำหนดค่า clientBufferSize ของอุปกรณ์ โดยสามารถกำหนดขนาดตามที่ต้องการหรือใช้ค่าตัวแปร Default ที่กำหนดให้ โดยมีดังนี้

    [  ตารางแสดงค่าตัวแปร Default Setting clientBufferSize ]
    | ตัวแปร | Default value (ESP32) | Default value (ESP8266) | การเรียกใช้ตัวแปร |
    |--|--|--|--|
    | defaultbuffer | 1,024 bytes | 1,024 bytes | setting.clientBufferSize = defaultbuffer |
    | defaultOTAbuffer | 8,192 bytes | 4,096 bytes | setting.clientBufferSize = defaultOTAbuffer |

# วิธีใช้งาน setting กับ magel.begin(setting)
```cpp
   setting.ThingIdentifier = "123441023449232XXX";
   setting.ThingSecret = "14912559924224242XXX";
   setting.endpoint = "magellan.ais.co.th"; //if not set *default: magellan.ais.co.th
   setting.clientBufferSize = defaultOTAbuffer; // if not set *default: 1024
   magel.begin(setting);
```

 * `Begin`
   * `magel.begin()` เริ่มต้นใช้งาน และตั้งค่าการเชื่อมต่อ Magellan Platform
   * `magel.begin(setting)` เริ่มต้นใช้งาน  และตั้งค่าการเชื่อมต่อ Magellan Platform ตามที่กำหนดใน Setting (Global Object Variables)
 
 * `Credential`
   * `magel.credential.getThingIdentifier()` อ่านค่า Thing Identifier ของโมดูล
   * `magel.credential.getThingSecret()` อ่านค่า Thing Secret ของโมดูล
   * `magel.credential.setManual(String newThingIdentifier, String newThingSecret)` กำหนดค่า ThingIdentifier และ ThingSecret ของ Device ตามที่ผู้ใช้งานต้องการ
   * `magel.credential.regenerate()` ใช้ในการ reset ค่า ThingIdentifier และ ThingSecret ของ Device ที่ถูกกำหนดไว้และให้ library ทำการกำหนดค่าขึ้นมาใหม่ให้
   * `magel.credential.getPreviousThingIdentifier()` อ่านค่า ThingIdentifier ของ Device ที่เคยกำหนดไว้ก่อนทำการล้างค่าไป
   * `magel.credential.getPreviousThingSecret()` อ่านค่า ThingSecret ของ Device ที่เคยกำหนดไว้ก่อนทำการล้างค่าไป
   * `magel.credential.recovery()` ใช้สำหรับนำค่า Credential เดิมที่เคยถูก Generate ไว้ล่าสุดมาใช้งาน
 
 * `Loop handle message MQTT` 
   * `magel.loop()` ใช้ทำให้คำสั่งต่าง ๆ สามารถทำงานได้อย่างต่อเนื่องในระหว่างการเชื่อมต่อกับ Magellan Platform จำเป็นต้องถูกเรียกใช้
 
 * `Information (Info)` 
   * `magel.Info.getBoardInfo()` ใช้อ่านหมายเลข Thing Identifier, Thing Secret ของโมดูลที่ Library ได้ทำการ Generate Thing Key ให้
   * `magel.Info.getThingIdentifier()` ใช้อ่านหมายเลข Thing Identifier
   * `magel.Info.getThingSecret()` ใช้อ่านหมายเลข Thing Secret
   * `magel.Info.getThingToken()` ใช้อ่าน Thing Token
   * `magel.Info.getHostName()` ใช้อ่าน Host Name บนอุปกรณ์ที่ทำการเชื่อมต่ออยู่

 * `Subscribes (lists of subscribe)` 
   * `magel.subscribes.([](){ Function Register Subscribe Here })` ใช้ Subscribe Topic หรือ Subscribe Function ภายใน Function subscribes นี้เมื่อมีการ connect หรือ reconnect ตัว function นี้จะ triger function ที่บรรจุไว้ภายในให้อัตโนมัติ
 # วิธีใช้งาน Subscribes
```cpp
   void loop()
   {
     magel.loop();
     magel.subscribes([](){ //*lambda function
       magel.subscribe.control();
       magel.subscribe.report.response();
       Serial.println("Subscribe list!!!");
       /* subscribe something or doing something at once after connect/reconnect */
     });
     /* do something */
   }
```
# หรือ
```cpp
   void listSubscribe(){
      magel.subscribe.control();
      magel.subscribe.report.response();
      Serial.println("Subscribe list!!!");
   }

   void loop()
   {
     magel.loop();
     magel.subscribes(listSubscribe);
     /* do something */
   }
```
 
 * `Interval timer` 
   * `magel.interval(unsigned int second, []() { function here })` ใช้กำหนดช่วงเวลาให้ Function ที่ประกาศภายใน Interval ทำงานในแต่ละรอบโดยมีหน่วยเป็น Second   
>ℹ️ Information`Function "Interval" เป็น Function optional เท่านั้น สามารถใช้ function timer ทดแทนได้`
 
<a name="handleConnectWiFi"></a>
 * `Check Connection` 
   * `magel.isConnected()` ใช้ในการตรวจสอบสถานะการเชื่อมต่อกับ Magellan Platform โดย true = Connected และ false = Not Connected
 
 * `Reconnect Connection` 
   * `magel.reconnect()` ใช้ในการเชื่อมต่อกับ Magellan Platform เมื่อมีสถานะไม่การเชื่อมต่อกับ Magellan Platform
>⚠️ Warning`หากเขียนในส่วน Connection WiFi อื่นๆเอง นอกเหนือจาก <MAGELLAN_WiFi_SETTING.h> จำเป็นต้องเช็ค Connection ระหว่างอุปกรณ์กับ Platform ได้ดังนี้` 
# วิธี handle connection ระหว่างอุปกรณ์ กับ platform
```cpp
   void loop()
   {
     magel.loop();
     magel.subscribes([](){
       /* subscribe something or doing something at once after connect/reconnect */
     });

    if(!magel.isConnected()) //*
    {
      magel.reconnect();
    }
     /* do something */
   }
```

 * `Report` 
   * `magel.subscribe.report.response()` ใช้ในการ Subscribe Response จากการส่งข้อมูลเซนเซอร์ในรูปแบบ JSON
   * `magel.report.send(sensors)` ใช้ส่งข้อมูลเซนเซอร์ในรูปแบบ JSON 
   * `magel.subscribe.report.response(PLAINTEXT)` ใช้ในการ Subscribe Response ของการส่งข้อมูลเซนเซอร์ในรูปแบบ Plain Text
   * `magel.report.send(String reportKey, String reportValue)` ใช้ส่งค่าเซนเซอร์ในรูปแบบ Plain Text
   * `magel.subscribe.reportWithTimestamp.response();` ใช้ในการ Subscribe Response การส่งข้อมูลเซนเซอร์ในรูปแบบ JSON พร้อมค่า Timestamp
   * `magel.report.send(Int UNIXtimestamp, String sensors);` ใช้ส่งข้อมูลเซนเซอร์ในรูปแบบ JSON พร้อมค่า Timestamp ในรูปแบบ UNIXTS (UnixTimestamp) ไปยัง Magellan Platform
 
 * `Report with message id` 
   * `magel.subscribe.report.response()` ใช้ในการ Subscribe Response จากการส่งข้อมูลเซนเซอร์ในรูปแบบ JSON
   * `magel.report.send(String sensors, int msgId);` ใช้ส่งข้อมูลเซนเซอร์ในรูปแบบ JSON ด้วย manual MessageId  
   * `magel.subscribe.report.response(PLAINTEXT)` ใช้ในการ Subscribe Response ของการส่งข้อมูลเซนเซอร์ในรูปแบบ Plain Text
   * `magel.report.send(String reportKey, String reportValue, int msgId)` ใช้ส่งค่าเซนเซอร์ในรูปแบบ Plain Text ด้วย manual MessageId  
   * 🆕`magel.report.send(String sensors, RetransmitSetting &retransSetting)` ใช้ส่งค่าเซนเซอร์ในรูปแบบ JSON ด้วย RetransmitSetting
   * 🆕`magel.report.send(String reportKey, String reportValue, RetransmitSetting &retransSetting)` ใช้ส่งค่าเซนเซอร์ในรูปแบบ Plain Text ด้วย RetransmitSetting

<a name="retransmitStructor"></a>
* 🆕`ResultReport` [`Properties variable inside struct ResultReport`]
  * `statusReport` boolean สำหรับ check publish MQTT Status
  * `msgId` MessageId default: -1 เมื่อไม่มี MsgId 

* 🆕`RetransmitSetting` [`Properties function inside struct RetransmitSetting`]
   * `.option(bool enabled, unsigned int repeat, unsigned int duration, int msgId)`
   * `.setEnabled(bool enabled)`
   * `.setMsgId(int msgId)`
   * `.setRepeat(unsigned int repeat)`
   * `.setDuration(unsigned int duration)`
   * `.generateMsgId()`

>⚠️ Warning`หากมีการเปิดใช้งาน enabled retransmit ด้วย setEnabled จะมีการ report จนกว่าจะได้ Response, msgId โดยทุกๆ duration และ repeat ที่ตั้งค่าไว้ซึ่งจะใช้เวลาในการทำงาน เนื่องจากต้องรอการตอบกลับ ทั้งนี้ขึ้นอยู่กับคุณภาพของสัญญาณของ network connection ของอุปกรณ์แต่สามารถมั่นใจได้ว่า message ของที่ส่งไปถึงหรือไม่ หรือในกรณีผิดพลาดสามารถ track ได้จากจาก`[Status code](https://magellan.ais.co.th/api-document/3/3) 

# วิธีใช้งาน Report with message id ด้วย RetransmitSetting [เพิ่มเติม](#retransmitStructor)
```cpp
   void loop()
   {
     magel.loop();
     magel.subscribes([](){
       magel.subscribe.report.response(); //if using MessageId please subscribe response report to check status and MessageId is acepted from platform.
     });
     magel.interval(15, [](){
       RetransmitSetting settingReport; //decleare object variable for setting report
       ResultReport result; //decleare object variable for receive result report
       settingReport.setEnabled(true); //true: retransmit / false: report with MsgId only
       settingReport.setRepeat(5); //default: 2 attempt *retransmit max 2 time to cancel attempt
       settingReport.setDuration(7); //default: 5 sec. delay wait duration every retransmit
       settingReport.generateMsgId(); //optional: regenerateMsgId if manual using ".setMsgId(msgId)"
       result = magel.report.send("{\"hello\":\"magellan\"}", settingReport);
       Serial.print("[MsgId report]: ");
       Serial.println(result.msgId);
       Serial.print("[Status report]: ");
       Serial.println((result.statusReport)? "SUCCESS" : "FAIL");
     });
     /* do something */
   }
```

 * `Sensor` 
   * `magel.sensor.add(sensorKey, sensorValue)` ใช้เพิ่มข้อมูลเซนเซอร์ โดยเก็บไว้ที่ JSONBuffer ของ Sensor
   * `magel.sensor.update(sensorKey, sensorValue)` ใช้แก้ไขข้อมูลเซนเซอร์ตาม sensorKey และ sensorValue ที่กำหนด
   * `magel.sensor.location.add(locationKey, latitude , longtitude)` ใช้เพิ่มข้อมูลเซนเซอร์ประเภท Location ซึ่งต้องกำหนด locationKey,latitude และ longtitude 
   * `magel.sensor.location.update(locationKey, latitude , longtitude)` ใช้แก้ไขข้อมูลเซนเซอร์ประเภท Location ตาม locationKey,latitude และ longtitude ที่กำหนด
   * `magel.sensor.findKey(sensorKey)` ใช้ค้นหา sensorKey ใน JSONBuffer ที่เคยเพิ่มเข้าไป ต้องทำการกำหนด sensorKey ที่ต้องการค้นหา
   * `magel.sensor.remove(sensorKey)` ใช้ลบข้อมูลของเซนเซอร์ออกจาก JSONBuffer ด้วย sensorKey 
   * `magel.sensor.toJSONString()` ใช้แปลงรูปแบบข้อมูล JSON ไปเป็นรูปแบบ JSONString จากข้อมูลเซนเซอร์ที่เก็บไว้ใน JSONBuffer
   * `magel.sensor.setJSONBufferSize(int)` ใช้กำหนดขนาดของ JSONBuffer ที่ใช้ในการเก็บข้อมูลของ sensorKey และ sensorValue ซึ่งกำหนดค่าเริ่มต้นไว้ที่ 1,024 ไบต์ สามารถกำหนดได้สูงสุดที่ 8,192 ไบต์
   * `magel.sensor.readJSONBufferSize()` ใช้อ่านขนาดของ JSONBuffer
   * `magel.sensor.report()` ใช้ในการส่งข้อมูลเซนเซอร์ทั้งหมดที่อยู่ใน JSONBuffer ไปยัง Magellan Platform และลบค่าเซนเซอร์ทั้งหมดที่อยู่ภายใน JSONBuffer ให้หลังจากส่งข้อมูล
   * 🆕`magel.report.report(RetransmitSetting &retransSetting)` ใช้ส่งค่าเซนเซอร์ในรูปแบบ JSON ด้วย RetransmitSetting
   * `magel.sensor.clear()` ใช้ลบค่าเซนเซอร์ทั้งหมดที่อยู่ภายใน JSONBuffer

# วิธีใช้งาน sensor.report with message id ด้วย RetransmitSetting [เพิ่มเติม](#retransmitStructor)
```cpp
   void loop()
   {
     magel.loop();
     magel.subscribes([](){
       magel.subscribe.report.response(); //if using MessageId please subscribe response report to check status and MessageId is acepted from platform.
     });
     magel.interval(15, [](){
       RetransmitSetting settingReport; //decleare object variable for setting report
       ResultReport result; //decleare object variable for receive result report
       settingReport.setEnabled(true); //true: retransmit / false: report with MsgId only
       settingReport.setRepeat(5); //default: 2 attempt *retransmit max 2 time to cancel attempt
       settingReport.setDuration(7); //default: 5 sec. delay wait duration every retransmit
       settingReport.generateMsgId(); //optional: regenerateMsgId if manual using ".setMsgId(msgId)"
       magel.sensor.add("hello","magellan");
       magel.sensor.add("numbers",1234);
       result = magel.sensor.report(settingReport);
       Serial.print("[MsgId report]: ");
       Serial.println(result.msgId);
       Serial.print("[Status report]: ");
       Serial.println((result.statusReport)? "SUCCESS" : "FAIL");
     });
     /* do something */
   }
```

 * `Control` 
   * `magel.subscribe.control()` ใช้ในการ Subscribe เพื่อรอรับค่า Control ในรูปแบบ JSON
   * `magel.control.request()` ใช้ในการร้องขอค่าของ Control ที่ยังไม่ได้ตอบว่ารับทราบการสั่งงาน (Acknowledge) มาทั้งหมดโดยจะได้รับ Response ในรูปแบบ JSON ซึ่งคำสั่งนี้เหมาะสำหรับอุปกรณ์ที่ไม่ได้ใช้งานแบบ Realtime
   * `magel.deserializeControl(controls)` ใช้ในการ Deserialize JSON ของค่า Control ออกมา 
   * `magel.control.ACK(String controls)` ใช้ส่งข้อมูลตอบกลับเพื่อรับทราบการสั่งงาน (Acknowledge) จากการ Control ผ่าน Widget บนหน้า Dashboard ของ Magellan Platform โดยจะส่งค่าไปในรูปแบบ JSON
   * `magel.subscribe.control(PLAINTEXT)` ใช้ในการ Subscribe เพื่อรอรับค่า Control ในรูปแบบ Plain Text
   * `magel.control.request(String controlKey)` ใช้ในการร้องขอค่าของ Control ที่ยังไม่ได้ตอบรับทราบการสั่งงาน (Acknowledge) โดยเฉพาะ controlKey ที่ต้องการอยากจะทราบ โดยที่อุปกรณ์จะได้รับ Response ในรูปแบบ Plain Text ซึ่งเหมาะสำหรับอุปกรณ์ที่ไม่ได้ใช้งานแบบ Realtime
   * `magel.control.ACK(String controlKey, String controlValue)` ใช้ในการส่งข้อมูลตอบกลับเพื่อรับทราบการสั่งงาน (Acknowledge) จากการ Control ผ่าน Widget บนหน้า Dashboard ของ Magellan Platform โดยจะส่งค่าไปในรูปแบบ Plaint Text
   *  เมื่ออุปกรณ์ได้รับการ Control แล้ว แต่ไม่ตอบกลับเพื่อรับทราบการสั่งงาน (Acknowledge) ไปยัง Magellan Platform ดังนั้นทุกครั้งที่อุปกรณ์ทำการส่งข้อมูลมายัง Magellan Platform จะทำให้อุปกรณ์ยังคงได้รับค่า Control (Spam) นั้น ๆ ทุกครั้งเสมอ จนกว่าอุปกรณ์จะตอบกลับเพื่อรับทราบการสั่งงาน 

# วิธีการใช้งาน Callback getControl and Acknowledge control
```cpp
  #include <MAGELLAN_MQTT.h>
  #include <MAGELLAN_WiFi_SETTING.h>
  WiFiClient WiFi_client;
  MAGELLAN_MQTT magel(WiFi_client);
  String SSID = "AIS_XXX";
  String PASS = "XXXXXXX";
  void setup()
  {
    Serial.begin(115200);
    WiFiSetting.SSID = SSID;
    WiFiSetting.PASS = PASS;
    connectWiFi(WiFiSetting);
    setting.ThingIdentifier = "123441023449232XXX";
    setting.ThingSecret = "14912559924224242XXX";
    setting.endpoint = "magellan.ais.co.th"; //if not set *default: magellan.ais.co.th
    setting.clientBufferSize = defaultOTAbuffer; // if not set *default: 1024
    magel.begin(setting);
    //* callback getControl
    magel.getControlJSON([](String controls){ 
      Serial.print("# Control incoming JSON: ");
      Serial.println(controls);
      String control = magel.deserializeControl(controls);
      magel.control.ACK(control); //ACKNOWLEDGE control to magellan ⚠️ important to Acknowledge control value to platform
    });
  }
```
* `หลังจาก decleare function callback getControl มาแล้วไม่ว่าจะ format 'JSON' หรือ 'Plaintext' ก็ตามหากทำการ triger control จาก widget บน platform เพื่อให้อุปกรณ์ได้รับค่า control แต่ตัวอุปกรณ์เกิด disconnect หรือปัญหาที่ไม่ได้รับ message value ทัน event นั้นๆ สามารถเรียกขอค่า control ที่ค้างหรือยังไม่ได้ Acknowledge ได้ดังนี้`
>ℹ️ Information`หากทำการ Request control แล้วไม่มีค่าค้างอยู่ค่าที่จะได้รับเข้ามาใน Callback getControl จะมีแค่ code 40400 หากมีค้างอยู่จะได้รับ code 20000  และ value control` [Status code](https://magellan.ais.co.th/api-document/3/3)
# วิธีการ Request control value ที่ค้างอยู่ (ยังไม่ได้รับการ Acknowledge จากอุปกรณ์) 
```cpp
   void loop()
   {
     magel.loop();
     magel.subscribes([](){
       magel.subscribe.control();
       magel.control.request();  //* using here for request once after new connect or reconnect
     });
     
     magel.interval(20, [](){
       magel.control.request(); //*
     });
   }
```

 * `ClientConfig` 
   * `magel.clientConfig.add(ClientConfigKey, ClientConfigValue)` ใช้เพิ่มข้อมูล ClientConfig ลงใน JSONBuffer ของ ClientConfig
   * `magel.clientConfig.update(ClientConfigKey, ClientConfigValue)` ใช้แก้ไขค่า ClientConfig ที่มีการเพิ่มไว้แล้วภายใน JSONBuffer
   * `magel.clientConfig.findKey(ClientConfigKey)` ใช้ในการค้นหา ClientConfigKey ใน JSONBuffer ของ ClientConfig ซึ่งผู้ใช้งานจะต้องกำหนด ClientConfigKey ที่ต้องการค้นหา โดยจะได้รับผลลัพธ์ในรูปแบบ Boolean หากพบ ClientConfigKey จะได้รับผลลัพธ์เป็น true หากไม่จะได้รับผลลัพธ์เป็น false
   * `magel.clientConfig.remove(ClientConfigKey)` ใช้ในการลบข้อมูลของ ClientConfig ออกจาก JSONBuffer ของ ClientConfig ด้วย ClientConfigKey
   * `magel.clientConfig.toJSONString()` ใช้สร้าง JSON String จากข้อมูล ClientConfig ที่ได้ทำการเพิ่มเข้าไปใน JSONBuffer ของ ClientConfig ซึ่งมีขนาดให้ใช้งานจำนวน 512 ไบต์
   * `magel.clientConfig.save()` ใช้ส่งข้อมูล ClientConfig บันทึกไปยัง Magellan Platform จาก clientConfigKey และ clientConfigValue ที่ได้ทำการเพิ่มเข้าไปใน JSONBuffer
   * `magel.clientConfig.save(clientConfigs)` ใช้ส่งข้อมูล ClientConfig บันทึกไปยัง Magellan Platform จากข้อมูลในรูปแบบ JSON
   * `magel.clientConfig.clear()` ใช้ลบค่า ClientConfig ใน JSONBuffer ของ ClientConfig ทั้งหมด
# วิธีการใช้งาน Client config 
>ℹ️ Information `Client config จุดประสงค์หรือการใช้งาน feature นี้ถูกสร้างมาเพื่อให้ตัวอุปกรณ์อัพเดทค่า "การตั้งค่าของอุปกรณ์" คล้ายกับ report แต่ไปแสดงข้อมูลใน thing information ด้านล่างแทน activity realtime เพื่อให้ผู้ใช้ง่ายทราบว่า ณ ตอนนี้อุปกรณ์มีการตั้งค่าอย่างไรโดยไม่ไปปะปนกับการส่งค่า sensor ผ่านการ report`  
```cpp
  #include <MAGELLAN_MQTT.h>
  #include <MAGELLAN_WiFi_SETTING.h>
  WiFiClient WiFi_client;
  MAGELLAN_MQTT magel(WiFi_client);
  String SSID = "AIS_XXX";
  String PASS = "XXXXXXX";
  void setup()
  {
    Serial.begin(115200);
    WiFiSetting.SSID = SSID;
    WiFiSetting.PASS = PASS;
    connectWiFi(WiFiSetting);
    setting.ThingIdentifier = "123441023449232XXX";
    setting.ThingSecret = "14912559924224242XXX";
    setting.endpoint = "magellan.ais.co.th"; //if not set *default: magellan.ais.co.th
    magel.begin(setting);
    magel.clientConfig.add("location", "15.0000, 58.0000"); //* update location once after connect platform
    magel.clientConfig.add("battery", 100); //* update battery level once after connect platform
    magel.clientConfig.add("interval", 15000); //* update interval value level once after connect platform
    magel.clientConfig.send(); //* send all added clientConfig to platform
  }
```
 * `ServerConfig`
   * `magel.subscribe.serverConfig()` ใช้ในการ Subscribe รับข้อมูลการตั้งค่าของอุปกรณ์ (serverConfig) ในรูปแบบ JSON
   * `magel.serverConfig.request()` ใช้ในการร้องขอข้อมูลการตั้งค่าของอุปกรณ์รูปแบบ JSON
   * `magel.subscribe.serverConfig(PLAINTEXT)` ใช้ในการ Subscribe รับข้อมูลการตั้งค่าของอุปกรณ์ในรูปแบบ Plain Text
   * `magel.serverConfig.request(String serverConfigKey)` ใช้ในการร้องขอข้อมูลการตั้งค่าของอุปกรณ์ด้วย serverConfigKey ในรูปแบบ Plain Text
 * `Heartbeat`
   * `magel.subscribe.heartbeat.response()` ใช้ในการ Subscribe Response เมื่อมีการส่ง Heartbeat ไปยัง Magellan Platform ในรูปแบบ JSON
   * `magel.subscribe.heartbeat.response(PLAINTEXT)` ใช้ในการ Subscribe getServerTime รับค่า Timestamp เมื่ออุปกรณ์มีการร้องขอเวลาไปยัง Magellan Platfrom ในรูปแบบ JSON
   * `magel.heartbeat(unsign int second)` ใช้ในการส่งสถานะของอุปกรณ์ไปยัง Magellan Platform ด้วยความถีเป็นวินาทีที่สม่ำเสมอ
 * `GetServerTime`
   * `magel.getServerTime()` ใช้ในการร้องขอเวลา Timestamp (Unix) จาก Magellan Platform
   * `magel.subscribe.getServerTime()` ใช้ในการ Subscribe getServerTime รับค่า Timestamp เมื่ออุปกรณ์มีการร้องขอเวลาไปยัง Magellan Platfrom ในรูปแบบ JSON
   * `magel.subscribe.getServerTime(PLAINTEXT)` ใช้ในการ Subscribe getServerTime รับค่า Timestamp เมื่ออุปกรณ์มีการร้องขอเวลาไปยัง Magellan Platfrom ในรูปแบบ Plant Text
# วิธีการใช้งาน GetServerTime

```cpp
  #include <MAGELLAN_MQTT.h>
  #include <MAGELLAN_WiFi_SETTING.h>
  WiFiClient WiFi_client;
  MAGELLAN_MQTT magel(WiFi_client);
  String SSID = "AIS_XXX";
  String PASS = "XXXXXXX";
  unsigned long unixTimeMG;
  void setup()
  {
    Serial.begin(115200);
    WiFiSetting.SSID = SSID;
    WiFiSetting.PASS = PASS;
    connectWiFi(WiFiSetting);
    setting.ThingIdentifier = "123441023449232XXX";
    setting.ThingSecret = "14912559924224242XXX";
    setting.endpoint = "magellan.ais.co.th"; //if not set *default: magellan.ais.co.th
    magel.begin(setting);
    magel.getResponse(UNIXTIME, [](EVENTS events) 
    { //* for get unixTime from magellan
      unixTimeMG = events.Payload.toInt();
      Serial.print("[unixTimeMG from magellan]: ");
      Serial.println(unixTimeMG);

      String timeString = magel.utils.toDateTimeString(unixTimeMG, 7);
      Serial.printf("\nMagellan: %s\n", timeString.c_str());
    });
  }
   void loop()
   {
     magel.loop();
     magel.subscribes([](){
       magel.subscribe.getServerTime(PLAINTEXT); 
       magel.getServerTime(); // request time from magellan server once after new connect or reconnect
     });
     
     magel.interval(20, [](){
       magel.getServerTime(); // request time from magellan server //*
     });
   }
```
>ℹ️ Information `หากอุปกรณ์ต้องการค่าเวลาไปใช้ไม่ว่าจะเป็นการ sync RTC Module หรืออื่นๆ สามารถ request ขอค่าเวลาจาก platform ได้โดยค่าเวลาจะเป็น UnixTimestamp format เช่น 1688011509` 
 * `Callback`
   * `magel.getControl(callback void(String controlKey, String controlValue))` ใช้รับค่า Control เมื่อเกิด Control events จาก Widget บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลเป็น Key และ Value จากการ Control ในรูปแบบ Plain Text
   * `magel.getControl(String focusKey, callback void(String controlValue))` ใช้รับค่า Control เมื่อเกิด Control events จาก Widget บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูล Value จากการ Control เฉพาะ focusKey ที่ผู้ใช้ได้กำหนดไว้เท่านั้น ในรูปแบบ Plain Text
   * `magel.getControlJSON(callback void(String controls))` ใช้รับค่า Control เมื่อเกิด Control events จาก Widget บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลในรูปแบบ JSON String
   * `magel.getControlJSON(callback void(JsonObject docJsonControls))` ใช้รับค่า Control เมื่อเกิด Control events จาก Widget บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลในรูปแบบ JSON Object
   * `magel.getServerConfig(callback void(String serverConfigKey, String serverConfigValue))` ใช้รับค่า Config เมื่อเกิด events จากเปลี่ยนแปลง Online Config บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลเป็น Key และ Value ในรูปแบบ Plain Text
   * `magel.getServerConfig(String focusKey, callback void(String serverConfigValue))` ใช้รับค่า Config เมื่อเกิด events จากเปลี่ยนแปลง Online Config บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูล Value เฉพาะ focusKey ที่ผู้ใช้ได้กำหนดไว้เท่านั้น ในรูปแบบ Plain Text
   * `magel.getServerConfigJSON(callback void(String serverConfigs))` ใช้รับค่า Config เมื่อเกิด events จากเปลี่ยนแปลง Online Config บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลในรูปแบบ JSONString
   * `magel.getServerConfigJSON(callback void(JsonObject docJsonServerConfigs))` ใช้รับค่า Config เมื่อเกิด events จากเปลี่ยนแปลง Online Config บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลในรูปแบบ JSONObject
   * `magel.getResponse(enum eventResponse, [] (EVENTS event){})` ใช้รับข้อมูลของ Response เมื่อเกิด events ต่าง ๆ โดยผู้ใช้งานสามารถกำหนด enum eventResponse จาก event ที่ต้องการจะแสดงค่าลงไปใน function callback getResponse ได้ดังนี้
   * `magel.onDisconnect(callback void())` ใช้สำหรับ triger function หรือการทำงานบางอย่างเมื่ออุปกรณ์มีการ disconnect จาก platform [optional]
  

    [ Suggest for use in callback getResponse ]  
    | eventResponse | enum |
    |--|--|
    | UNIXTIME | 6 |
    | RESP_REPORT_JSON | 8 |
    | RESP_REPORT_PLAINTEXT | 9 |
    | RESP_REPORT_TIMESTAMP | 10 |
    | RESP_REPORT_JSON | 11 |
    | RESP_REPORT_PLAINTEXT | 12 |

    <a name="infoEvent"></a>

    [  Inside data type struct EVENTS ]
    | Available function | Results |
    |--|--|
    | event.Key | Type Plaintext |
    | event.Payload | Type JSON or Plaintext |
    | event.RESP | String message response “SUCCESS” or “FAIL” |
    | event.CODE | success code such as 20000 or 40400 etc |

 * `OTA`
   * `magel.OTA.utility(). Properties Here;` เป็นคำสั่งที่ให้ผู้ใช้งานสามารถอ่านค่าการ OTA เพื่อนำมาใช้เป็นเงื่อนไขหรือเปรียบเทียบ
    
    [  ตารางแสดงข้อมูล Properties ของ Utility ]
    | Properties | Data Type | Default value | Description |
    |--|--|--|--|
    | isReadyOTA | boolean | false | สถานะข้อมูลการ OTA ของอุปกรณ์ โดย  <br /> - true = มีข้อมูลให้ทำการ OTA <br /> - false = ไม่มีข้อมูลให้ทำการ OTA |
    | firmwareIsUpToDate | int  | false | สถานะของ Firmware บนอุปกรณ์ เมื่อเทียบกับข้อมูลการ OTA ของอุปกรณ์ที่ได้รับมาล่าสุด ได้แก่ <br /> 1) UNKNOWN / (-1) = ไม่ทราบสถานะ <br /> 2) OUT_OF_DATE / (0) = ไม่เป็นปัจจุบัน <br /> 3) UP_TO_DATE / (1) = ปัจจุบัน |
    | inProcessOTA | boolean | false | สถานะของการดำเนินการ OTA โดย <br /> - true = กำลังดำเนินการ OTA <br /> - false = ไม่ได้กำลังดำเนินการ OTA|    
    | firmwareTotalSize | unsigned int | false | ขนาดของ Firmware มีหน่วยเป็น Byte |
    | firmwareName | String | UNKNOWN | ชื่อของ Firmware |
    | firmwareVersion | String | UNKNOWN | Version ของ Firmware | 
    | checksum | String | UNKNOWN | ค่า checksum ใช้รับรองความถูกต้องของ Firmware |
    | checksumAlgorithm | String | UNKNOWN | ประเภท Algorithm ของ Checksum |
>⚠️ Warning `หากมีการ ใช้งาน OTA ควรจะหยุดการทำงานอื่นๆของอุปกรณ์ในส่วนทำงานของโปรแกรม void loop() มีไม่ให้เกิดการขัดขวางการทำงานในการ download และ buffer firmware data บนตัวอุปกรณ์ โดยสามารถ เช็คและหยุดการทำงานในส่วนอื่นๆที่ไม่จำเป็นเพื่อให้อุปกรณ์สามารถ OTA ได้อย่างมีประสิทธิภาพดังนี้`
```cpp
   void loop()
   {
     magel.loop();
     magel.subscribes([](){
       magel.subscribe.control();
     });
     
    if(!magel.OTA.utility().inProcessOTA){
      /* 
      do something if device not in OTA process such as read and send data
      */
    }
   }
```
   * `magel.OTA.autoUpdate(boolean)` ใช้สำหรับเรียกใช้งานการทำ OTA โดยอัตโนมัติ โดยผู้ใช้งานสามารถกำหนดได้ โดย true คือการกำหนดให้ทำ OTA โดยอัตโนมัติ และ false คือการกำหนดให้ทำ OTA ตามที่ผู้ใช้งานกำหนด
   * `magel.OTA.executeUpdate()` ใช้สำหรับเรียกใช้งานการทำ OTA แบบ Manual
   * `magel.OTA.getAutoUpdate()` ใช้สำหรับอ่านสถานะการตั้งค่าในการทำ OTA
   * `magel.OTA.checkUpdate()` ใช้สำหรับตรวจสอบข้อมูลการ OTA จาก Magellan Platform กับ Firmware ปัจจุบันของอุปกรณ์
   * `magel.OTA.magel.OTA.readDeviceInfo()` ใช้สำหรับอ่านข้อมูล Firmware ปัจจุบันของอุปกรณ์


### `#include <MAGELLAN_WiFi_SETTING.h>`

# ใช้เชื่อมต่อและตั้งค่าการเชื่อมต่อ WiFi
>ℹ️ Information`MAGELLAN_WiFi_SETTING" เป็น utilities optional เท่านั้น สามารถใช้ connection WiFi หรืออื่นด้วยตัวเองหรือ third party library อย่าง WiFimanager ได้แต่ ⚠️จำเป็นต้องเช็ค Connection ระหว่างอุปกรณ์กับ Platform ด้วย `[Click](#handleConnectWiFi)

 * `WiFiSetting (Global Object Variables)` 
   * `WiFiSetting.SSID` ใช้ในการกำหนดค่า SSID ของ WiFi ที่ต้องการเชื่อมต่อ
   * `WiFiSetting.PASS` ใช้ในการกำหนดค่า Password ของ WiFi ที่ต้องการเชื่อมต่อ
 * `connectWiFi` 
   * `connectWiFi(WiFiSetting)` ใช้ในการเชื่อมต่อ WiFi ตาม WiFiSetting (Global Variables) ที่กำหนด
   * `connectWiFi("set_your_ssid", "set_your_password")` ใช้ในการเชื่อมต่อ WiFi ตามที่ต้องการ
   * `getSSID()` ใช้ในการอ่านค่า SSID ของ WiFi ที่ทำการเชื่อมต่ออยู่
 * `reconnectWiFi` 
   * `reconnectWiFi(magel)` ใช้ในการเชื่อมต่อกับ Magellan Platform เมื่อมีสถานะไม่การเชื่อมต่อกับ Magellan Platform



## ศึกษาเพิ่มเติม

ข้อมูลเพิ่มเติมที่จะช่วยให้เริ่มต้นใช้งาน Magellan Platform ได้ง่ายขึ้น


### ตัวอย่างโค้ดโปรแกรม

โค้ดโปรแกรมตัวอย่างอยู่ในโฟลเดอร์ `examples` แยกตามหมวดหมู่ ดังนี้

 * `Magellan Platform`
   * `credential`
     * [credential](examples/example_MQTT/credential/credential.ino) - ตัวอย่างการอ่านค่า Credential และการกำหนด Credential
   * `getServerConfig`
     * [getServerConfigJSON](examples/example_MQTT/getServerConfig/getServerConfigJSON/getServerConfigJSON.ino) - ตัวอย่างการแสดงค่าที่อุปกรณ์ไปเรียกค่าที่เราทำการ Config ไว้บน Magellan Platform ในรูปแบบ JSON 
     * [getServerConfigPlaintext](examples/example_MQTT/getServerConfig/getServerConfigPlaintext/getServerConfigPlaintext.ino) - ตัวอย่างการแสดงค่าที่อุปกรณ์ไปเรียกค่าที่เราทำการ Config ไว้บน Magellan Platform ในรูปแบบ Plain Text 
   * `getControl`
     * [getControlJSON](examples/example_MQTT/getControl/getControlJSON/getControlJSON.ino) - ตัวอย่างการแสดงค่าที่ตัวอุปกรณ์ทำการ Control บน Dashboard ของ Magellan Platform ในรูปแบบ JSON 
     * [getControlPlaintext](examples/example_MQTT/getControl/getControlPlaintext/getControlPlaintext.ino) - ตัวอย่างการแสดงค่าที่ตัวอุปกรณ์ทำการ Control บน Dashboard ของ Magellan Platform ในรูปแบบ Plain Text 
   * `getControlLED`
     * [getControlJSON_LED](examples/example_MQTT/getControlLED/getControlJSON_LED/getControlJSON_LED.ino) - ตัวอย่างการแสดงค่าที่ตัวอุปกรณ์ทำการ Control LED บน Dashboard ของ Magellan Platform ในรูปแบบ JSON 
     * [getControlPlaintext_LED](examples/example_MQTT/getControlLED/getControlPlaintext_LED/getControlPlaintext_LED.ino) - ตัวอย่างแสดงค่าที่ตัวอุปกรณ์ทำการ Control LED บน Dashboard ของ Magellan Platform ในรูปแบบ Plain Text 
   * `heartbeat`
     * [heartbeat](examples/example_MQTT/heartbeat/heartbeat.ino) - ตัวอย่างการส่งสัญญาณไปยัง Server รูปแบบ Heartbeat เพื่อบอกให้ Magellan Platform ทราบว่าอุปกรณ์ดังกล่าว มีการเชื่อมต่ออยู่
   * `reportData`
     * [reportDataJSON](examples/example_MQTT/reportData/reportDataJSON/reportDataJSON.ino) - ตัวอย่างการส่งค่าตัวเลขแบบสุ่มขึ้นไปยัง Magellan Platform ในรูปแบบ JSON 
   * `reportData with messageId`
     * [reportMsgId](examples/example_MQTT/reportWithMsgId/reportMsgId.ino) - ตัวอย่างการส่งค่าตัวขึ้นไปยัง Magellan Platform ด้วย MessageId
     * [reportMsgIdReportSetting](examples/example_MQTT/reportWithMsgId/reportMsgIdReportSetting.ino) - ตัวอย่างการส่งค่าตัวขึ้นไปยัง Magellan Platform ด้วย MessageId ผ่านการใช้ ReportSetting
     * [reportRetransmit](examples/example_MQTT/reportWithMsgId/reportRetransmit.ino) - ตัวอย่างการส่งค่าตัวขึ้นไปยัง Magellan Platform ด้วยการเปิดใช้งาน retransmit 
   * `reportMultiDataType`
     * [reportMultiDataType](examples/example_MQTT/reportMultiDataType/reportMultiDataType.ino) - ตัวอย่างการส่งข้อมูลในรูปแบบหลายประเภท ได้แก่ เลขจำนวนเต็มบวก, เลขจำนวนเต็มลบ, ทศนิยม, ข้อความ, พิกัด GPS และ Boolean Magellan Platform 
   * `reportSensor`
     * [reportSensorJSON](examples/example_MQTT/reportSensor/reportSensorJSON/reportSensorJSON.ino) - ตัวอย่างการส่งข้อมูลจากเซนเซอร์บนอุปกรณ์ไปบน Magellan Platform ในรูปแบบ JSON 
     * [reportSensorPlaintext](examples/example_MQTT/reportSensor/reportSensorPlaintext/reportSensorPlaintext.ino) - ตัวอย่างการส่งข้อมูลจากเซนเซอร์บนอุปกรณ์ไปบน Magellan Platform ในรูปแบบ Plain Text 
   * `reportWithTimestamp`
     * [reportWithTimestamp](examples/example_MQTT/reportWithTimestamp/reportWithTimestamp.ino) - ตัวอย่างการส่งข้อมูลจากเซนเซอร์พร้อม Timestamp บนอุปกรณไปบน Magellan Platform 
   * `saveClientConfig`
     * [saveClientConfig](examples/example_MQTT/saveClientConfig/saveClientConfig.ino) - ตัวอย่างการส่งข้อมูลจากเซนเซอร์พร้อม Timestamp บนอุปกรณไปบน Magellan Platform 
   * `getServerTime`
     * [getServerTime](examples/example_MQTT/getServerTime/getServerTime.ino) - ตัวอย่างการขอเวลา Timestamp จาก Server ของ Magellan Platform 
   * `utilityTimeConverter`
     * [utilityTimeConverter](examples/example_MQTT/utilityTimeConverter/utilityTimeConverter.ino) - ตัวอย่างการแปลงค่า Utility Time
   * `OTA`
     * [autoUpdate](examples/example_MQTT/OTA/autoUpdate/autoUpdate.ino) - ตัวอย่างการกำหนดให้อุปกรณ์ทำการอัพเดท Firmware อัตโนมัติ
     * [manualUpdate](examples/example_MQTT/OTA/manualUpdate/manualUpdate.ino) - ตัวอย่างการกำหนดให้อุปกรณ์ทำการอัพเดท Firmware ตามที่ผู้ใช้งานกำหนดเอง
     * [utilityInformation](examples/example_MQTT/OTA/utilityInformation/utilityInformation.ino) - ตัวอย่างการอ่านค่าข้อมูลการ OTA
>⚠️ Warning `ข้อควรระวังในการใช้งาน OTA ด้วยบอร์ด ESP8266 จำเป็นจะต้องทดสอบ Binary file (.bin) ของ firmware ก่อนใช้งาน OTA จริงเสมอเนื่องจากหาก Build Binary file (.bin) จากคนละบอร์ดเช่นใช้ binary file ของ ESP32 มาใช้ OTA เข้ายังบอร์ด ESP8266 อาจจะทำให้ firmware ดั่งเดิมที่ใช้งานได้เสียหายและไม่สามารถทำงานต่อได้จำเป็นต้องแก้ไขด้วยการ erase flash หรือ upload firmware ใหม่ผ่านสายเชื่อมโดยตรงแทน *แต่ในบน ESP32 ตัว standard library ได้มีการ validate board ใน Binary file ที่จะ OTA มาแล้วในระดับหนึ่ง แต่ทั้งนี้ก็ควรจะทดสอบก่อนใช้งาน OTA จริงเสมอเผื่อให้แน่ใจว่า firmware ใหม่ที่ OTA เข้าไปมีความเสถียรภาพพร้อมใช้งาน`



