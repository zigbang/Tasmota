<img src="https://github.com/arendst/Tasmota/blob/master/tools/logo/TASMOTA_FullLogo_Vector.svg" alt="Logo" align="right" height="76"/>

# RELEASE NOTES

## Migration Information

**This version removes support for direct migration from versions before v8.1.0 (Doris)**

See [migration path](https://tasmota.github.io/docs/Upgrading#migration-path) for instructions how to migrate to a major version. Pay attention to the following version breaks due to dynamic settings updates:

1. Migrate to **Sonoff-Tasmota 3.9.x**
2. Migrate to **Sonoff-Tasmota 4.x**
3. Migrate to **Sonoff-Tasmota 5.14**
4. Migrate to **Sonoff-Tasmota 6.7.1** (http://ota.tasmota.com/tasmota/release-6.7.1/)
5. Migrate to **Tasmota 7.2.0** (http://ota.tasmota.com/tasmota/release-7.2.0/)

--- Major change in parameter storage layout ---

6. Migrate to **Tasmota 8.5.1** (http://ota.tasmota.com/tasmota/release-8.5.1/)

--- Major change in internal GPIO function representation ---

7. Migrate to **Tasmota 9.1** (http://ota.tasmota.com/tasmota/release-9.1.0/)

While fallback or downgrading is common practice it was never supported due to Settings additions or changes in newer releases. Starting with release **v9.1.0 Imogen** the internal GPIO function representation has changed in such a way that fallback is only possible to the latest GPIO configuration before installing **v9.1.0**.

## Supported Core versions

This release will be supported from ESP8266/Arduino library Core version **2.7.4.9** due to reported security and stability issues on previous Core version. This will also support gzipped binaries.

Support of Core versions before 2.7.1 has been removed.

## Support of TLS

In addition to TLS using fingerprints now also user supplied CA certs and AWS IoT is supported. Read [full documentation](https://tasmota.github.io/docs/AWS-IoT)

## Initial configuration tools

For initial configuration this release supports Webserver based **WifiManager** or **Serial** based command interface only. Support for **WPS** and **SmartConfig** has been removed.

## Provided Binary Downloads

### ESP8266 or ESP8285 based
The following binary downloads have been compiled with ESP8266/Arduino library core version **2.7.4.9**.

- **tasmota.bin** = The Tasmota version with most drivers. **RECOMMENDED RELEASE BINARY**
- **tasmota-BG.bin** to **tasmota-TW.bin** = The Tasmota version in different languages.
- **tasmota-lite.bin** = The Lite version without most drivers and sensors.
- **tasmota-knx.bin** = The Knx version without some features but adds KNX support.
- **tasmota-sensors.bin** = The Sensors version adds more useful sensors.
- **tasmota-ir.bin** = The InfraRed Receiver and transmitter version allowing all available protocols provided by library IRremoteESP8266 but without most other features.
- **tasmota-display.bin** = The Display version without Energy Monitoring but adds display support.
- **tasmota-zbbridge.bin** = The dedicated Sonoff Zigbee Bridge version.
- **tasmota-minimal.bin** = The Minimal version allows intermediate OTA uploads to support larger versions and does NOT change any persistent parameter. This version **should NOT be used for initial installation**.

Above binaries are also available as gzipped version allowing faster uploads.

The binaries can be downloaded from either https://github.com/arendst/Tasmota/tree/release-firmware or http://ota.tasmota.com/tasmota/release. The latter links can be used for OTA upgrades too like ``OtaUrl http://ota.tasmota.com/tasmota/release/tasmota.bin.gz``

### ESP32 based
The following binary downloads have been compiled with ESP32/Arduino library core version **1.0.6**.

- **tasmota32.bin** = The Tasmota version with most drivers including additional sensors and KNX.  **RECOMMENDED RELEASE BINARY**
- **tasmota32solo1.bin** = The Tasmota version with most drivers including additional sensors and KNX for single core ESP32.
- **tasmota32-BG.bin** to **tasmota32-TW.bin** = The Tasmota version in different languages.
- **tasmota32-ir.bin** = The InfraRed Receiver and transmitter version allowing all available protocols provided by library IRremoteESP8266 but without most other features.
- **tasmota32-display.bin** = The Display version without Energy Monitoring but adds display support.
- **tasmota32-webcam.bin** = The Webcam version adds webcam support.
- **tasmota32-bluetooth.bin** = The Bluetooth version adds BLE support.
- **tasmota32-odroidgo.bin** = The Odroid-Go version is specifically tailored to Odroid_go hardware.
- **tasmota32-core2.bin** = The Core2 version is specifically tailored to M5Stack Core2 hardware.

The binaries can be downloaded from either https://github.com/arendst/Tasmota/tree/release-firmware or http://ota.tasmota.com/tasmota32/release. The latter links can be used for OTA upgrades too like ``OtaUrl http://ota.tasmota.com/tasmota32/release/tasmota32.bin``

[List](MODULES.md) of embedded modules.

[Complete list](BUILDS.md) of available feature and sensors.

## Changelog v9.4.0.5
### Added
- Command ``Status0`` providing all status information on a single line
- Initial support for optional ``Template`` JSON fieldpair ``"CMND":"<any template related command>|<any template related command>|..."`` [#11788](https://github.com/arendst/Tasmota/issues/11788)
- ESP32 pulldown buttons ``Button_d`` and ``Button_id`` and switches ``Switch_d`` [#10814](https://github.com/arendst/Tasmota/issues/10814)
- Support for MQTT using Azure IoT Hub by Kevin Saye [#11906](https://github.com/arendst/Tasmota/issues/11906)
- Make Telegram command ``TmState`` persistent [#11965](https://github.com/arendst/Tasmota/issues/11965)
- Zigbee firmware for Tube's Zigbee coordinator based on EFR32 and ESP32
- Zigbee firmware 6.7.9 for Sonoff ZBBridge
- Defines ``USER_RULE1``, ``USER_RULE2`` and ``USER_RULE3`` to store rules at compile time
- Define ``USER_BACKLOG`` to store commands at compile time to be executed at firmware load or when executing command ``reset``
- LVGL support for TrueType fonts via FreeType library
- Support for BM8563 RTC chip (I2C) found in M5Stack Core2 and M5StickC
- Preliminary support for Esp32C3 - RiscV based

### Breaking Changed

### Changed
- IRremoteESP8266 library from v2.7.16 to v2.7.18
- Allow longer MQTT response messages by removing fixed memory buffer with size 1040 to heap allocated buffer
- Command ``Timers`` layout of JSON message changed to single line
- Command ``Gpio`` layout of JSON message changed to single line
- Command ``Modules`` layout of JSON message changed to single line

### Fixed

### Noted
- ESP32 single core **tasmota32solo1.bin** binary can only be uploaded using the GUI as OTA upload will trigger the watchdog timer