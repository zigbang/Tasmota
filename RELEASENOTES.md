<img src="https://github.com/arendst/Tasmota/blob/master/tools/logo/TASMOTA_FullLogo_Vector.svg" alt="Logo" align="right" height="76"/>

# RELEASE NOTES

## Migration Information

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

This release will be supported from ESP8266/Arduino library Core version **2.7.4.7** due to reported security and stability issues on previous Core version. This will also support gzipped binaries.

Support of Core versions before 2.7.1 has been removed.

## Support of TLS

To save resources when TLS is enabled mDNS needs to be disabled. In addition to TLS using fingerprints now also user supplied CA certs and AWS IoT is supported. Read [full documentation](https://tasmota.github.io/docs/AWS-IoT)

## Initial configuration tools

For initial configuration this release supports Webserver based **WifiManager** or **Serial** based command interface only. Support for **WPS** and **SmartConfig** has been removed.

## Provided Binary Downloads

The following binary downloads have been compiled with ESP8266/Arduino library core version **2.7.4.7**.

- **tasmota.bin** = The Tasmota version with most drivers. **RECOMMENDED RELEASE BINARY**
- **tasmota-BG.bin** to **tasmota-TW.bin** = The Tasmota version in different languages.
- **tasmota-lite.bin** = The Lite version without most drivers and sensors.
- **tasmota-knx.bin** = The Knx version without some features but adds KNX support.
- **tasmota-sensors.bin** = The Sensors version adds more useful sensors.
- **tasmota-ir** = The InfraRed Receiver and transmitter version allowing all available protocols provided by library IRremoteESP8266 but without most other features.
- **tasmota-display.bin** = The Display version without Energy Monitoring but adds display support.
- **tasmota-zbbridge.bin** = The dedicated Sonoff Zigbee Bridge version.
- **tasmota-minimal.bin** = The Minimal version allows intermediate OTA uploads to support larger versions and does NOT change any persistent parameter. This version **should NOT be used for initial installation**.

The attached binaries can also be downloaded from http://ota.tasmota.com/tasmota/release for ESP8266 or http://ota.tasmota.com/tasmota32/release for ESP32. The links can be used for OTA upgrades too like ``OtaUrl http://ota.tasmota.com/tasmota/release/tasmota.bin.gz``

[List](MODULES.md) of embedded modules.

[Complete list](BUILDS.md) of available feature and sensors.

## Changelog v9.1.0.1
### Added
- Command ``SetOption115 1`` to enable ESP32 MiBle
- Command ``SetOption116 1`` to disable auto-query of zigbee light devices (avoids network storms with large groups)
- Commands ``TuyaRGB``, ``TuyaEnum`` and ``TuyaEnumList`` (#9769)
- Zigbee command ``ZbInfo`` and prepare support for EEPROM
- Zigbee command ``ZbLeave`` to unpair a device
- Zigbee support for Mi Door and Contact (#9759)
- Zigbee alarm persistence (#9785)
- Support for additional EZO sensors by Christopher Tremblay
- Support for AS608 optical and R503 capacitive fingerprint sensor

### Changed
- Core library from v2.7.4.5 to v2.7.4.7
- Platformio compiler option `no target align` enabled (#9749)
- Sonoff L1 color up scaling and color margin detection (#9545)

### Fixed
- NTP fallback server functionality (#9739)
- Telegram group chatid not supported (#9831)
- KNX buttons, switches and sensors detection regression from v9.1.0 (#9811)
- GUI MqttUser and MqttPassword updates when TLS is compiled in (#9825)

### Removed
- Version compatibility check
