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

This release will be supported from ESP8266/Arduino library Core version **2.7.4.9** due to reported security and stability issues on previous Core version. This will also support gzipped binaries.

Support of Core versions before 2.7.1 has been removed.

## Support of TLS

In addition to TLS using fingerprints now also user supplied CA certs and AWS IoT is supported. Read [full documentation](https://tasmota.github.io/docs/AWS-IoT)

## Initial configuration tools

For initial configuration this release supports Webserver based **WifiManager** or **Serial** based command interface only. Support for **WPS** and **SmartConfig** has been removed.

## Provided Binary Downloads

The following binary downloads have been compiled with ESP8266/Arduino library core version **2.7.4.9**.

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

## Changelog v9.2.0.5
### Added
- Command ``CTRange`` to specify the visible CT range the bulb is capable of [#10311](https://github.com/arendst/Tasmota/issues/10311)
- Command ``L1MusicSync <0|Off>|<1|On>|<2|Toggle>, 1..10, 1..100>`` to control Sonoff L1 Music Sync mode sensitivity and speed [#10722](https://github.com/arendst/Tasmota/issues/10722)
- Command ``RuleTimer0`` to access all RuleTimers at once [#10352](https://github.com/arendst/Tasmota/issues/10352)
- Command ``Speed2`` to control a once off fade [#10741](https://github.com/arendst/Tasmota/issues/10741)
- Command ``VirtualCT`` to simulate or fine tune CT bulbs with 3,4,5 channels [#10311](https://github.com/arendst/Tasmota/issues/10311)
- Command ``SetOption40 0..250`` to disable button functionality if activated for over 0.1 second re-introduced
- Command ``SetOption43 1..255`` to control Rotary step (#10407)
- Command ``SetOption118 1`` to move ZbReceived from JSON message and into the subtopic replacing "SENSOR" default [#10353](https://github.com/arendst/Tasmota/issues/10353)
- Command ``SetOption119 1`` to remove the device addr from json payload, can be used with zb_topic_fname where the addr is already known from the topic [#10355](https://github.com/arendst/Tasmota/issues/10355)
- Zigbee command ``SetOption120 1`` or ``ZbEndpointTopic 1`` to add the zigbee endpoint as suffix in topic when using ``SetOption89 1``
- Zigbee command ``ZbScan`` to do an energy scan on each radio channel
- Commands ``ChannelRemap``, ``MultiPWM``, ``AlexaCTRange``, ``PowerOnFade``, ``PWMCT``, ``WhiteBlend`` and ``VirtualCT`` as synonyms for ``SetOption37, 68, 82, 91, 92, 105`` and ``106``
- Commands ``ZbNameKey``, ``ZbDeviceTopic``, ``ZbNoPrefix``, ``ZbEndpointSuffix``, ``ZbNoAutoBind`` and ``ZbNameTopic`` as synonyms for ``SetOption83, 89, 100, 101, 110`` and ``112``
- Commands ``ZbNoAutoBind``, ``ZbReceivedTopic`` and ``ZbOmitDevice`` as synonyms for ``SetOption116, 118`` and ``119``
- Commands ``BuzzerActive`` and ``BuzzerPwm`` as synonyms for ``SetOption67`` and ``111``
- Milliseconds to console output [#10152](https://github.com/arendst/Tasmota/issues/10152)
- Gpio ``Option_a1`` enabling PWM2 high impedance if powered off as used by Wyze bulbs [#10196](https://github.com/arendst/Tasmota/issues/10196)
- Rotary No Pullup GPIO selection ``Rotary A/B_n`` [#10407](https://github.com/arendst/Tasmota/issues/10407)
- BSSID and Signal Strength Indicator to GUI wifi scan result [#10253](https://github.com/arendst/Tasmota/issues/10253)
- Support for P9813 RGB Led MOSFET controller [#10104](https://github.com/arendst/Tasmota/issues/10104)
- Support for GPIO option selection
- Support for FTC532 8-button touch controller by Peter Franck [#10222](https://github.com/arendst/Tasmota/issues/10222)
- Support for BS814A-2 8-button touch buttons by Peter Franck [#10447](https://github.com/arendst/Tasmota/issues/10447)
- Support for up to 4 I2C SEESAW_SOIL Capacitance & Temperature sensors by Peter Franck [#10481](https://github.com/arendst/Tasmota/issues/10481)
- Support for TOF10120 time of flight sensor by Cyril Pawelko [#10190](https://github.com/arendst/Tasmota/issues/10190)
- Support for Afrikaans language translations by Christiaan Heerze
- Support for IR inverted leds using ``#define IR_SEND_INVERTED true`` [#10301](https://github.com/arendst/Tasmota/issues/10301)
- Support for disabling 38kHz IR modulation using ``#define IR_SEND_USE_MODULATION false`` [#10301](https://github.com/arendst/Tasmota/issues/10301)
- Support for SPI display driver for ST7789 TFT by Gerhard Mutz [#9037](https://github.com/arendst/Tasmota/issues/9037)
- Support for time proportioned (``#define USE_TIMEPROP``) and optional PID (``#define USE_PID``) relay control [#10412](https://github.com/arendst/Tasmota/issues/10412)
- Support for 24/26/32/34 bit RFID Wiegand interface (D0/D1) by Sigurd Leuther [#3647](https://github.com/arendst/Tasmota/issues/3647)
- Support for SM2135 current selection using GPIO ``SM2135 DAT`` index [#10634](https://github.com/arendst/Tasmota/issues/10634)
- Support for Sugar Valley NeoPool Controller by Norbert Richter [#10637](https://github.com/arendst/Tasmota/issues/10637)
- Support for ESP32 ``Module 3`` Odroid Go 16MB binary tasmota32-odroidgo.bin [#8630](https://github.com/arendst/Tasmota/issues/8630)
- Support for ESP32 ``Module 5`` Wireless Tag Eth01 [#9496](https://github.com/arendst/Tasmota/issues/9496)
- Support for ESP32 ``Module 7`` M5stack core2 16MB binary tasmota32-core2.bin [#10635](https://github.com/arendst/Tasmota/issues/10635)
- Support rotary encoder on Shelly Dimmer [#10407](https://github.com/arendst/Tasmota/issues/10407#issuecomment-756240920)
- Support character `#` to be replaced by `space`-character in command ``Publish`` topic [#10258](https://github.com/arendst/Tasmota/issues/10258)
- Support trailing silence in buzzer tune [#10694](https://github.com/arendst/Tasmota/issues/10694)
- Rule trigger string comparisons for EndsWith ``$>``, StartsWith ``$<`` and Contains ``$|`` [#10538](https://github.com/arendst/Tasmota/issues/10538)
- SPI display driver SSD1331 Color oled by Jeroen Vermeulen [#10376](https://github.com/arendst/Tasmota/issues/10376)
- Compile time option ``USE_MQTT_TLS_DROP_OLD_FINGERPRINT`` to drop old (less secure) TLS fingerprint

### Breaking Changed
- ESP32 switch from default SPIFFS to default LittleFS file system loosing current (zigbee) files
- Replaced MFRC522 13.56MHz rfid card reader GPIO selection from ``SPI CS`` by ``RC522 CS``
- Replaced NRF24L01 GPIO selection from ``SPI CS`` by ``NRF24 CS`` and ``SPI DC`` by ``NRF24 DC``
- Replaced ILI9341 GPIO selection from ``SPI CS`` by ``ILI9341 CS`` and ``SPI DC`` by ``ILI9341 DC``
- Replaced ST7789 GPIO selection from ``SPI CS`` by ``ST7789 CS`` and ``SPI DC`` by ``ST7789 DC``
- Replaced ILI9488 GPIO selection from ``SPI CS`` by ``ILI9488_CS``
- Replaced EPaper29 GPIO selection from ``SPI CS`` by ``EPaper29 CS``
- Replaced EPaper42 GPIO selection from ``SPI CS`` by ``EPaper42 CS``
- Replaced SSD1351 GPIO selection from ``SPI CS`` by ``SSD1351 CS``
- Replaced RA8876 GPIO selection from ``SPI CS`` by ``RA8876 CS``

### Changed
- Command ``Sleep 0`` removes any sleep from wifi modem except when ESP32 BLE is active
- Logging from heap to stack freeing 700 bytes RAM
- Disabled ``USE_LIGHT`` light support for ZBBridge saving 17.6kB [#10374](https://github.com/arendst/Tasmota/issues/10374)
- Force initial default state ``SetOption57 1`` to scan wifi network every 44 minutes for strongest signal [#10395](https://github.com/arendst/Tasmota/issues/10395)
- PubSubClient MQTT_SOCKET_TIMEOUT from 15 to 4 seconds
- Domoticz fixed 2 decimals resolution by user selectable ``TempRes``, ``HumRes`` and ``PressRes`` resolutions
- ESP32 increase number of relay GPIOs from 8 to 28

### Fixed
- Redesign syslog and mqttlog using log buffer [#10164](https://github.com/arendst/Tasmota/issues/10164)
- Shutter stop issue [#10170](https://github.com/arendst/Tasmota/issues/10170)
- Scripter script_sub_command [#10181](https://github.com/arendst/Tasmota/issues/10181)
- Scripter JSON variable above 32 chars [#10193](https://github.com/arendst/Tasmota/issues/10193)
- Shelly Dimmer power on state [#10154](https://github.com/arendst/Tasmota/issues/10154) [#10182](https://github.com/arendst/Tasmota/issues/10182)
- Wemo emulation for single devices [#10165](https://github.com/arendst/Tasmota/issues/10165) [#10194](https://github.com/arendst/Tasmota/issues/10194)
- ESP32 LoadStoreError when using ``#define USER_TEMPLATE`` [#9506](https://github.com/arendst/Tasmota/issues/9506)
- Compile error when ``#ifdef USE_IR_RECEIVE`` is disabled regression from 9.1.0.2
- Prometheus memory leak [#10221](https://github.com/arendst/Tasmota/issues/10221)
