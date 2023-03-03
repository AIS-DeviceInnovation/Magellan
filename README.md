![Library Version](https://img.shields.io/badge/Version-1.0.1-green)

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
   * [MAGELLAN_MEDIA_FILE.h](#include-magellan_media_fileh)
 * [ศึกษาเพิ่มเติม](#ศึกษาเพิ่มเติม)
   * [ตัวอย่างโค้ดโปรแกรม](#ตัวอย่างโค้ดโปรแกรม)

## Magellan Platform Features

 * Device Management - การจัดการเกี่ยวกับอุปกรณ์ IoT การตั้งค่าต่างๆ ของอุปกรณ์ การรับส่งข้อมูลผ่านโปรโตคอลที่รองรับได้แก่ CoAP, MQTT และ HTTP การจัดการเกี่ยวกับ Data Storage ที่มีการทำ Message cache, Data history และ Compare data รวมไปถึงการสั่งงานควบคุมอุปกรณ์ให้เปิด-ปิด ได้ด้วยตัวคุณเอง ทั้งแบบ API และผ่านหน้าเว็บของแพลตฟอร์ม
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
 * เปิดโปรแกรมตัวอย่าง [credential](examples/example_MQTT/credential/credential.ino) โดยใช้คำสั่ง magel.credential.getThingIdentifier() และ magel.credential.getThingSecret() เพื่อทดสอบอ่านหมายเลข ThingIdentifier และ ThingSecret ของโมดูล


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

 * `MAGELLAN_MQTT  magel;` เริ่มต้นใช้งานไลบรารี Magellan Platform สร้างออปเจค magel
 * `Setting (Global Variables)`
   * `setting.ThingIdentifier` ใช้เป็นตัวแปรสำหรับกำหนดค่า ThingIdentifier 
   * `setting.ThingSecret` ใช้เป็นตัวแปรสำหรับกำหนดค่า ThingSecret 
   * `setting.endpoint` ใช้เป็นตัวแปรสำหรับกำหนดค่า IP หรือ URL Path ปลายทางที่ต้องการให้อุปกรณ์เชื่อมต่อ
   * `setting.clientBufferSize` ใช้เป็นตัวแปรสำหรับกำหนดค่า clientBufferSize ของอุปกรณ์ โดยสามารถกำหนดขนาดตามที่ต้องการหรือใช้ค่าตัวแปร Default ที่กำหนดให้ โดยมีดังนี้

    [  ตารางแสดงค่าตัวแปร Default Setting clientBufferSize ]
    | ตัวแปร | Default value (ESP32) | Default value (ESP8266) | การเรียกใช้ตัวแปร |
    |--|--|--|--|
    | defaultbuffer | 1,024 bytes | 1,024 bytes | setting.clientBufferSize = defaultbuffer |
    | defaultOTAbuffer | 8,192 bytes | 4,096 bytes | setting.clientBufferSize = defaultOTAbuffer |

 * `Begin`
   * `magel.begin()` เริ่มต้นใช้งาน และตั้งค่าการเชื่อมต่อ Magellan Platform
   * `magel.begin(setting)` เริ่มต้นใช้งาน  และตั้งค่าการเชื่อมต่อ Magellan Platform ตามที่กำหนดใน Setting (Global Variables)
 * `Credential`
   * `magel.credential.getThingIdentifier()` อ่านค่า Thing Identifier จาก file system ของบอร์ด ESP32
   * `magel.credential.getThingSecret()` อ่านค่า Thing Secret จาก file system ของบอร์ด ESP32
   * `magel.credential.setManual(String newThingIdentifier, String newThingSecret)` กำหนดค่า ThingIdentifier และ ThingSecret ของ Device ตามที่ผู้ใช้งานต้องการ
   * `magel.credential.regenerate()` ใช้ในการล้างค่า ThingIdentifier และ ThingSecret ของ Device ที่ถูกกำหนดไว้และให้ library ทำการกำหนดค่าขึ้นมาใหม่ให้
   * `magel.credential.getPreviousThingIdentifier()` อ่านค่า ThingIdentifier ของ Device ที่เคยกำหนดไว้ก่อนทำการล้างค่าไป
   * `magel.credential.getPreviousThingSecret()` อ่านค่า ThingSecret ของ Device ที่เคยกำหนดไว้ก่อนทำการล้างค่าไป
   * `magel.credential.recovery()` ใช้สำหรับนำค่า Credential เดิมที่เคยถูก Generate ไว้ล่าสุดมาใช้งาน
 * `Loop` 
   * `magel.loop()` ใช้ทำให้คำสั่งต่าง ๆ สามารถทำงานได้อย่างต่อเนื่องในระหว่างการเชื่อมต่อกับ Magellan Platform จำเป็นต้องถูกเรียกใช้
 * `Info` 
   * `magel.Info.getBoardInfo()` ใช้อ่านหมายเลข Thing Identifier, Thing Secret จาก file system ของบอร์ด ESP32 Library ได้ทำการ Generate Thing Key ให้
   * `magel.Info.getThingIdentifier()` ใช้อ่านหมายเลข Thing Identifier
   * `magel.Info.getThingSecret()` ใช้อ่านหมายเลข Thing Secret
   * `magel.Info.getThingToken()` ใช้อ่าน Thing Token
   * `magel.Info.getHostName()` ใช้อ่าน Host Name บนอุปกรณ์ที่ทำการเชื่อมต่ออยู่
 * `Subscribes` 
   * `magel.subscribes([](){ Function Register Subscribe Here })` ใช้ Subscribe Topic หรือ Subscribe Function ที่อยู่ภายใน Function subscribes ให้อัตโนมัติเมื่ออุปกรณ์สามารถเชื่อมต่อ Magellan Platform ได้
 * `Interval` 
   * `magel.interval(unsigned int second, []() { function here })` ใช้กำหนดช่วงเวลาให้ Function ที่ประกาศภายใน Interval ทำงานในแต่ละรอบโดยมีหน่วยเป็น Second
 * `isConnected` 
   * `magel.isConnected()` ใช้ในการตรวจสอบสถานะการเชื่อมต่อกับ Magellan Platform โดย true = Connected และ false = Not Connected
 * `reconnect` 
   * `magel.reconnect()` ใช้ในการเชื่อมต่อกับ Magellan Platform เมื่อมีสถานะไม่การเชื่อมต่อกับ Magellan Platform
 * `Sensor` 
   * `magel.sensor.add(sensorKey, sensorValue)` ใช้เพิ่มข้อมูลเซนเซอร์ โดยเก็บไว้ที่ JSONBuffer ของ Sensor
   * `magel.sensor.update(sensorKey, sensorValue)` ใช้แก้ไขข้อมูลเซนเซอร์ตาม sensorKey และ sensorValue ที่กำหนด
   * `magel.sensor.location.add(locationKey, latitude, longtitude)` ใช้เพิ่มข้อมูลเซนเซอร์ประเภท Location ซึ่งต้องกำหนด locationKey และ longtitude 
   * `magel.sensor.location.update(locationKey, latitude, longtitude)` ใช้แก้ไขข้อมูลเซนเซอร์ประเภท Location ตาม locationKey และ longtitude ที่กำหนด
   * `magel.sensor.findKey(sensorKey)` ใช้ค้นหา sensorKey ใน JSONBuffer ที่เคยเพิ่มเข้าไป ต้องทำการกำหนด sensorKey ที่ต้องการค้นหา
   * `magel.sensor.remove(sensorKey)` ใช้ลบข้อมูลของเซนเซอร์ออกจาก JSONBuffer ด้วย sensorKey 
   * `magel.sensor.toJSONString()` ใช้แปลงรูปแบบข้อมูล JSON ไปเป็นรูปแบบ JSONString จากข้อมูลเซนเซอร์ที่เก็บไว้ใน JSONBuffer
   * `magel.sensor.setJSONBufferSize(int)` ใช้กำหนดขนาดของ JSONBuffer ที่ใช้ในการเก็บข้อมูลของ sensorKey และ sensorValue ซึ่งกำหนดค่าเริ่มต้นไว้ที่ 1,024 ไบต์ สามารถกำหนดได้สูงสุดที่ 8,192 ไบต์
   * `magel.sensor.readJSONBufferSize()` ใช้อ่านขนาดของ JSONBuffer
   * `magel.sensor.report()` ใช้ในการส่งข้อมูลเซนเซอร์ทั้งหมดที่อยู่ใน JSONBuffer ไปยัง Magellan Platform และลบค่าเซนเซอร์ทั้งหมดที่อยู่ภายใน JSONBuffer ให้หลังจากส่งข้อมูล
   * `magel.sensor.clear()` ใช้ลบค่าเซนเซอร์ทั้งหมดที่อยู่ภายใน JSONBuffer
 * `Report` 
   * `magel.subscribe.report.response()` ใช้ในการ Subscribe Response จากการส่งข้อมูลเซนเซอร์ในรูปแบบ JSON
   * `magel.report.send(payload)` ใช้ส่งข้อมูลเซนเซอร์ในรูปแบบ JSON 
   * `magel.subscribe.report.response(PLAINTEXT)` ใช้ในการ Subscribe Response ของการส่งข้อมูลเซนเซอร์ในรูปแบบ Plain Text
   * `magel.report.send(String key, String value)` ใช้ส่งค่าเซนเซอร์ในรูปแบบ Plain Text
   * `magel.subscribe.reportWithTimestamp.response();` ใช้ในการ Subscribe Response การส่งข้อมูลเซนเซอร์ในรูปแบบ JSON พร้อมค่า Timestamp
   * `magel.report.send(Int UNIXtimestamp, String payload);` ใช้ส่งข้อมูลเซนเซอร์ในรูปแบบ JSON พร้อมค่า Timestamp ในรูปแบบ UNIXTS (UnixTimestamp) ไปยัง Magellan Platform
 * `Control` 
   * `magel.subscribe.control()` ใช้ในการ Subscribe เพื่อรอรับค่า Control ในรูปแบบ JSON
   * `magel.control.request()` ใช้ในการร้องขอค่าของ Control ที่ยังไม่ได้ตอบว่ารับทราบการสั่งงาน (Acknowledge) มาทั้งหมดโดยจะได้รับ Response ในรูปแบบ JSON ซึ่งคำสั่งนี้เหมาะสำหรับอุปกรณ์ที่ไม่ได้ใช้งานแบบ Realtime
   * `magel.deserializeControl(payload)` ใช้ในการ Deserialize JSON ของค่า Control ออกมา 
   * `magel.control.ACK(String payload)` ใช้ส่งข้อมูลตอบกลับเพื่อรับทราบการสั่งงาน (Acknowledge) จากการ Control ผ่าน Widget บนหน้า Dashboard ของ Magellan Platform โดยจะส่งค่าไปในรูปแบบ JSON
   * `magel.subscribe.control(PLAINTEXT)` ใช้ในการ Subscribe เพื่อรอรับค่า Control ในรูปแบบ Plain Text
   * `magel.control.request(String controlKey)` ใช้ในการร้องขอค่าของ Control ที่ยังไม่ได้ตอบรับทราบการสั่งงาน (Acknowledge) โดยเฉพาะ controlKey ที่ต้องการอยากจะทราบ โดยที่อุปกรณ์จะได้รับ Response ในรูปแบบ Plain Text ซึ่งเหมาะสำหรับอุปกรณ์ที่ไม่ได้ใช้งานแบบ Realtime
   * `magel.control.ACK(String controlKey, String controlValue)` ใช้ในการส่งข้อมูลตอบกลับเพื่อรับทราบการสั่งงาน (Acknowledge) จากการ Control ผ่าน Widget บนหน้า Dashboard ของ Magellan Platform โดยจะส่งค่าไปในรูปแบบ Plaint Text
   *  เมื่ออุปกรณ์ได้รับการ Control แล้ว แต่ไม่ตอบกลับเพื่อรับทราบการสั่งงาน (Acknowledge) ไปยัง Magellan Platform ดังนั้นทุกครั้งที่อุปกรณ์ทำการส่งข้อมูลมายัง Magellan Platform จะทำให้อุปกรณ์ยังคงได้รับค่า Control (Spam) นั้น ๆ ทุกครั้งเสมอ จนกว่าอุปกรณ์จะตอบกลับเพื่อรับทราบการสั่งงาน
 * `ClientConfig` 
   * `magel.clientConfig.add(ClientConfigKey, ClientConfigValue)` ใช้เพิ่มข้อมูล ClientConfig ลงใน JSONBuffer ของ ClientConfig
   * `magel.clientConfig.update(ClientConfigKey, ClientConfigValue)` ใช้แก้ไขค่า ClientConfig ที่มีการเพิ่มไว้แล้วภายใน JSONBuffer
   * `magel.clientConfig.findKey(ClientConfigKey)` ใช้ในการค้นหา ClientConfigKey ใน JSONBuffer ของ ClientConfig ซึ่งผู้ใช้งานจะต้องกำหนด ClientConfigKey ที่ต้องการค้นหา โดยจะได้รับผลลัพธ์ในรูปแบบ Boolean หากพบ ClientConfigKey จะได้รับผลลัพธ์เป็น true หากไม่จะได้รับผลลัพธ์เป็น false
   * `magel.clientConfig.remove(ClientConfigKey)` ใช้ในการลบข้อมูลของ ClientConfig ออกจาก JSONBuffer ของ ClientConfig ด้วย ClientConfigKey
   * `magel.clientConfig.toJSONString()` ใช้สร้าง JSON String จากข้อมูล ClientConfig ที่ได้ทำการเพิ่มเข้าไปใน JSONBuffer ของ ClientConfig ซึ่งมีขนาดให้ใช้งานจำนวน 512 ไบต์
   * `magel.clientConfig.save()` ใช้ส่งข้อมูล ClientConfig บันทึกไปยัง Magellan Platform จาก clientConfigKey และ clientConfigValue ที่ได้ทำการเพิ่มเข้าไปใน JSONBuffer
   * `magel.clientConfig.save(payload)` ใช้ส่งข้อมูล ClientConfig บันทึกไปยัง Magellan Platform จากข้อมูลในรูปแบบ JSON
   * `magel.clientConfig.clear()` ใช้ลบค่า ClientConfig ใน JSONBuffer ของ ClientConfig ทั้งหมด
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
 * `Callback`
   * `magel.getControl(callback void(String key, String value))` ใช้รับค่า Control เมื่อเกิด Control events จาก Widget บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลเป็น Key และ Value จากการ Control ในรูปแบบ Plain Text
   * `magel.getControl(String focusKey, callback void(String payload))` ใช้รับค่า Control เมื่อเกิด Control events จาก Widget บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูล Value จากการ Control เฉพาะ focusKey ที่ผู้ใช้ได้กำหนดไว้เท่านั้น ในรูปแบบ Plain Text
   * `magel.getControlJSON(callback void(String payload))` ใช้รับค่า Control เมื่อเกิด Control events จาก Widget บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลในรูปแบบ JSON String
   * `magel.getControlJSON(callback void(JsonObject docJson))` ใช้รับค่า Control เมื่อเกิด Control events จาก Widget บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลในรูปแบบ JSON Object
   * `magel.getServerConfig(callback void(String key, String value))` ใช้รับค่า Config เมื่อเกิด events จากเปลี่ยนแปลง Online Config บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลเป็น Key และ Value ในรูปแบบ Plain Text
   * `magel.getServerConfig(String focusKey, callback void(String payload))` ใช้รับค่า Config เมื่อเกิด events จากเปลี่ยนแปลง Online Config บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูล Value เฉพาะ focusKey ที่ผู้ใช้ได้กำหนดไว้เท่านั้น ในรูปแบบ Plain Text
   * `magel.getServerConfigJSON(callback void(String payload))` ใช้รับค่า Config เมื่อเกิด events จากเปลี่ยนแปลง Online Config บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลในรูปแบบ JSONString
   * `magel.getServerConfigJSON(callback void(JsonObject docJson))` ใช้รับค่า Config เมื่อเกิด events จากเปลี่ยนแปลง Online Config บนหน้าเว็บ Magellan Platform ซึ่งจะได้รับข้อมูลในรูปแบบ JSONObject
   * `magel.getResponse(enum eventResponse, [] (EVENTS event){})` ใช้รับข้อมูลของ Response เมื่อเกิด events ต่าง ๆ โดยผู้ใช้งานสามารถกำหนด enum eventResponse จาก event ที่ต้องการจะแสดงค่าลงไปใน function callback getResponse ได้ดังนี้
  

    [ Suggest for use in callback getResponse ]  
    | eventResponse | enum |
    |--|--|
    | UNIXTIME | 6 |
    | RESP_REPORT_JSON | 8 |
    | RESP_REPORT_PLAINTEXT | 9 |
    | RESP_REPORT_TIMESTAMP | 10 |
    | RESP_REPORT_JSON | 11 |
    | RESP_REPORT_PLAINTEXT | 12 |

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
 
   * `magel.OTA.autoUpdate(boolean)` ใช้สำหรับเรียกใช้งานการทำ OTA โดยอัตโนมัติ โดยผู้ใช้งานสามารถกำหนดได้ โดย true คือการกำหนดให้ทำ OTA โดยอัตโนมัติ และ false คือการกำหนดให้ทำ OTA ตามที่ผู้ใช้งานกำหนด
   * `magel.OTA.executeUpdate()` ใช้สำหรับเรียกใช้งานการทำ OTA แบบ Manual
   * `magel.OTA.getAutoUpdate()` ใช้สำหรับอ่านสถานะการตั้งค่าในการทำ OTA
   * `magel.OTA.checkUpdate()` ใช้สำหรับตรวจสอบข้อมูลการ OTA จาก Magellan Platform กับ Firmware ปัจจุบันของอุปกรณ์
   * `magel.OTA.readDeviceInfo()` ใช้สำหรับอ่านข้อมูล Firmware ปัจจุบันของอุปกรณ์

### `#include <MAGELLAN_WiFi_SETTING.h>`

ใช้เชื่อมต่อและตั้งค่าการเชื่อมต่อ WiFi

 * `WiFiSetting (Global Variables)` 
   * `WiFiSetting.SSID` ใช้ในการกำหนดค่า SSID ของ WiFi ที่ต้องการเชื่อมต่อ
   * `WiFiSetting.PASS` ใช้ในการกำหนดค่า Password ของ WiFi ที่ต้องการเชื่อมต่อ
 * `connectWiFi` 
   * `connectWiFi(WiFiSetting)` ใช้ในการเชื่อมต่อ WiFi ตาม WiFiSetting (Global Variables) ที่กำหนด
   * `connectWiFi("set_your_ssid", "set_your_password")` ใช้ในการเชื่อมต่อ WiFi ตามที่ต้องการ
   * `getSSID()` ใช้ในการอ่านค่า SSID ของ WiFi ที่ทำการเชื่อมต่ออยู่
 * `reconnectWiFi` 
   * `reconnectWiFi(magel)` ใช้ในการเชื่อมต่อกับ Magellan Platform เมื่อมีสถานะไม่มีการเชื่อมต่อกับ Magellan Platform

### `#include <MAGELLAN_MEDIA_FILE.h>`

ใช้สำหรับการส่งข้อมูลในรูปแบบของ Media File ไปยัง Magellan Platform

 * `UploadMediaFile`
   * `media.uploadFromSPIFFS(String key, const char *filePath, Enum mediatype, String subtype)` ใช้ส่งข้อมูลของ Media File จาก File System (FS or SPIFFS) ไปยัง Magellan Platform
   * `media.uploadFromSDCard(String key, const char *filePath, Enum mediatype, String subtype, SD)` ใช้ส่งข้อมูลของ Media File จาก SD Card ไปยัง Magellan Platform
   * `media.uploadFromBinary(String key, const uint8_t* binaryData, size_t sizeBinaryData, Enum mediatype, String subtype)` ใช้ส่งข้อมูลของ Media File จากรูปแบบ Binary ไปยัง Magellan Platform

 * `ToBase64String`
   * `media.toBase64String(const char *filePath)` ใช้แปลงค่าของ Media File ใน File System (FS or SPIFFS) เป็น String ในรูปแบบ Base64
   * `media.toBase64String(const char *filePath, fs::FS &SD)` ใช้แปลงค่าของ Media File ใน SD Card เป็น String ในรูปแบบ Base64
   * `media.toBase64String(uint8_t *binaryData, size_t sizeBinaryData)` ใช้แปลงค่าของ Media File จากรูปแบบ Binary เป็น String ในรูปแบบ Base64
   * `media.toBase64MimeTypeString(const char *filePath, mediaTypeEnum mediatype, const String& subtype` ใช้แปลงค่าของ Media File ใน File System (FS or SPIFFS) เป็น String พร้อมกับ prefix ของ Media Type สำหรับใช้ร่วมกับ Base64
   * `media.toBase64MimeTypeString(const char *filePath, mediaTypeEnum mediatype, const String& subtype, fs::FS &SD)` ใช้แปลงค่าของ Media File ใน SD Card เป็น String พร้อมกับ prefix ของ Media Type สำหรับใช้ร่วมกับ Base64
   * `media.toBase64MimeTypeString(uint8_t *binaryData, size_t sizeBinaryData, mediaTypeEnum mediatype, const String& subtype)` ใช้แปลงค่าของ Media File จากรูปแบบ Binary เป็น String พร้อมกับ prefix ของ Media Type สำหรับใช้ร่วมกับ Base64


## ศึกษาเพิ่มเติม

ข้อมูลเพิ่มเติมที่จะช่วยให้เริ่มต้นใช้งาน Magellan Platform ได้ง่ายขึ้น


### ตัวอย่างโค้ดโปรแกรม

โค้ดโปรแกรมตัวอย่างอยู่ในโฟลเดอร์ `examples` แยกตามหมวดหมู่ ดังนี้

 * `Magellan Platform`
   * `credential`
     * [credential](examples/example_MQTT/credential/credential.ino) - ตัวอย่างการอ่านค่า Credential และการกำหนด Credential
   * `getServerConfig`
     * [getServerConfigJSON](examples/example_MQTT/getServerConfig/getServerConfigJSON/getServerConfigJSON.ino) - ตัวอย่างการแสดงค่าที่อุปกรณ์ไปเรียกค่าที่เราทำการ Config ไว้บน Magellan Platform ในรูปแบบ JSON 
     * [getServerConfigPlaintext](examples/example_MQTT/getServerConfig/getServerConfigPlaintext/getServerConfigPlaintext.ino) - ตัวอย่างการแสดงค่าที่อุปกรณ์ไปเรียกค่าที่เราทำการ Config ไว้บน Magellan Platform ในรูปแบบ Plaintext 
   * `getControl`
     * [getControlJSON](examples/example_MQTT/getControl/getControlJSON/getControlJSON.ino) - ตัวอย่างการแสดงค่าที่ตัวอุปกรณ์ทำการ Control บน Dashboard ของ Magellan Platform ในรูปแบบ JSON 
     * [getControlPlaintext](examples/example_MQTT/getControl/getControlPlaintext/getControlPlaintext.ino) - ตัวอย่างการแสดงค่าที่ตัวอุปกรณ์ทำการ Control บน Dashboard ของ Magellan Platform ในรูปแบบ Plaintext 
   * `getControlLED`
     * [getControlJSON_LED](examples/example_MQTT/getControlLED/getControlJSON_LED/getControlJSON_LED.ino) - ตัวอย่างการแสดงค่าที่ตัวอุปกรณ์ทำการ Control LED บน Dashboard ของ Magellan Platform ในรูปแบบ JSON 
     * [getControlPlaintext_LED](examples/example_MQTT/getControlLED/getControlPlaintext_LED/getControlPlaintext_LED.ino) - ตัวอย่างแสดงค่าที่ตัวอุปกรณ์ทำการ Control LED บน Dashboard ของ Magellan Platform ในรูปแบบ Plaintext 
   * `heartbeat`
     * [heartbeat](examples/example_MQTT/heartbeat/heartbeat.ino) - ตัวอย่างการส่งสัญญาณไปยัง Server รูปแบบ Heartbeat เพื่อบอกให้ Magellan Platform ทราบว่าอุปกรณ์ดังกล่าว มีการเชื่อมต่ออยู่
   * `reportData`
     * [reportDataJSON](examples/example_MQTT/reportData/reportDataJSON/reportDataJSON.ino) - ตัวอย่างการส่งค่าตัวเลขแบบสุ่มขึ้นไปยัง Magellan Platform ในรูปแบบ JSON 
     * [reportDataPlaintext](examples/example_MQTT/reportData/reportDataPlaintext/reportDataPlaintext.ino) - ตัวอย่างการส่งค่าตัวเลขแบบสุ่มขึ้นไปยัง Magellan Platform ในรูปแบบ Plaintext 
   * `reportMultiDataType`
     * [reportMultiDataType](examples/example_MQTT/reportMultiDataType/reportMultiDataType.ino) - ตัวอย่างการส่งข้อมูลในรูปแบบหลายประเภท ได้แก่ เลขจำนวนเต็มบวก, เลขจำนวนเต็มลบ, ทศนิยม, ข้อความ, พิกัด GPS และ Boolean Magellan Platform 
   * `reportSensor`
     * [reportSensorJSON](examples/example_MQTT/reportSensor/reportSensorJSON/reportSensorJSON.ino) - ตัวอย่างการส่งข้อมูลจากเซนเซอร์บนอุปกรณ์ไปบน Magellan Platform ในรูปแบบ JSON 
     * [reportSensorPlaintext](examples/example_MQTT/reportSensor/reportSensorPlaintext/reportSensorPlaintext.ino) - ตัวอย่างการส่งข้อมูลจากเซนเซอร์บนอุปกรณ์ไปบน Magellan Platform ในรูปแบบ Plaintext 
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
   * `UploadMediaFile`
     * [uploadMediaFileBinary](examples/example_MQTT/uploadMediaFile/uploadMediaFileBinary/uploadMediaFileBinary.ino) - ตัวอย่างการส่งข้อมูลของ Media File จาก File System (FS or SPIFFS) ไปยัง Magellan Platform
     * [uploadMediaFileSDCard](examples/example_MQTT/uploadMediaFile/uploadMediaFileSDCard/uploadMediaFileSDCard.ino) - ตัวอย่างการส่งข้อมูลของ Media File จาก SD Card ไปยัง Magellan Platform
     * [uploadMediaFileSPIFFS](examples/example_MQTT/uploadMediaFile/uploadMediaFileSPIFFS/uploadMediaFileSPIFFS.ino) - ตัวอย่างกาส่งข้อมูลของ Media File จาก File System (FS or SPIFFS) ไปยัง Magellan
   * `ToBase64String`
     * [toBase64String](examples/example_MQTT/uploadMediaFile/convertToBase64/toBase64String/toBase64String.ino) - ตัวอย่างการแปลงค่าของ Media File ใน [1.]Binary format (FS or SPIFFS), [2.]File System และ [3.]SD Card เป็น String ในรูปแบบ Base64
     * [toBase64MimeTypeString](examples/example_MQTT/uploadMediaFile/convertToBase64/toBase64MimeTypeString/toBase64MimeTypeString.ino) - ตัวอย่างการแปลงค่าของ Media File ใน [1.]Binary format (FS or SPIFFS), [2.]File System และ [3.]SD Card เป็น String พร้อมกับ prefix ของ Media Type สำหรับใช้ร่วมกับ Base64
