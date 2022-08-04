/*
  user_config_override.h - user configuration overrides my_user_config.h for Tasmota
  Copyright (C) 2021  Theo Arends
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_

/*****************************************************************************************************\
 * USAGE:
 *   To modify the stock configuration without changing the my_user_config.h file:
 *   (1) copy this file to "user_config_override.h" (It will be ignored by Git)
 *   (2) define your own settings below
 *
 ******************************************************************************************************
 * ATTENTION:
 *   - Changes to SECTION1 PARAMETER defines will only override flash settings if you change define CFG_HOLDER.
 *   - Expect compiler warnings when no ifdef/undef/endif sequence is used.
 *   - You still need to update my_user_config.h for major define USE_MQTT_TLS.
 *   - All parameters can be persistent changed online using commands via MQTT, WebConsole or Serial.
\*****************************************************************************************************/

#undef  CFG_HOLDER
#define CFG_HOLDER        4617
#undef  MQTT_HOST
#define MQTT_HOST         "a3b608yz74ta5k-ats.iot.ap-northeast-2.amazonaws.com" // [MqttHost]
#define MQTT_HOST_DEV    "a3b608yz74ta5k-ats.iot.ap-northeast-2.amazonaws.com" // [MqttHost]
#define MQTT_HOST_PROD    "a3b608yz74ta5k-ats.iot.ap-northeast-2.amazonaws.com" // [MqttHost]

#undef  MQTT_PORT
#define MQTT_PORT         8883                   // [MqttPort] MQTT port (10123 on CloudMQTT)

#ifdef FIRMWARE_ZIOT
#define API_HOST_DEV          "fpshfqmwfl.execute-api.ap-northeast-2.amazonaws.com"
#define API_HOST_PROD          "fpshfqmwfl.execute-api.ap-northeast-2.amazonaws.com"
#define LAMBDA_CERT_URL_DEV   "/dev/certificate"
#define LAMBDA_CERT_URL_PROD   "/prod/certificate"

#define AWS_FINGERPRINT   "98 0a 41 04 6e 50 88 9c 0c d0 7f 73 21 12 e3 95 aa 67 7f af"
#else
#define API_HOST_DEV          "3srkj4h3d9.execute-api.ap-northeast-2.amazonaws.com"
#define API_HOST_PROD          "3srkj4h3d9.execute-api.ap-northeast-2.amazonaws.com"

#define LAMBDA_CERT_URL_DEV   "/dev/cert"
#define LAMBDA_CERT_URL_PROD   "/prod/cert"

#define AWS_FINGERPRINT   "e8 c3 8d d8 41 15 1b 6d a9 9c 26 36 29 30 b2 14 f7 71 0d 1c"
#endif  // FIRMWARE_ZIOT

// -- Ota -----------------------------------------
#define USE_ARDUINO_OTA                          // Add optional support for Arduino OTA (+13k code)
#define OTA_URL                ""  // [OtaUrl]

#ifdef ESP32
#define OTA_URL_DEV         "https://3srkj4h3d9.execute-api.ap-northeast-2.amazonaws.com/dev/ota"
#define OTA_URL_PROD        "https://3srkj4h3d9.execute-api.ap-northeast-2.amazonaws.com/prod/ota"
#else  // ESP8266
#ifdef FIRMWARE_ZIOT_MINIMAL
#define OTA_URL_DEV    "http://13.209.165.165/ota"
#define OTA_URL_PROD    "http://13.209.165.165/ota"
#else
#define OTA_URL_DEV    "http://13.209.165.165/ota-minimal"
#define OTA_URL_PROD    "http://13.209.165.165/ota-minimal"
#endif  // FIRMWARE_ZIOT_MINIMAL
#endif  // ESP32

#define DEFAULT_SSID      ""
#define DEFAULT_PASS      ""

#ifndef USE_MQTT_TLS
#define USE_MQTT_TLS
#define USE_MQTT_TLS_CA_CERT // Optional but highly recommended
#endif
#ifndef USE_MQTT_AWS_IOT
#define USE_MQTT_AWS_IOT
#endif

#define PROJECT                "ZIoT_Sonoff"         // PROJECT is used as the default topic delimiter
#define FRIENDLY_NAME          "ZIGBANG"         // [FriendlyName] Friendlyname up to 32 characters used by webpages and Alexa
#define MQTT_TOPIC             "sonoff-%12x"   // [Topic] unique MQTT device topic including (part of) device MAC address
#define MQTT_GRPTOPIC          FRIENDLY_NAME "s"        // [GroupTopic] MQTT Group topic
#define MQTT_CLIENT_ID         FRIENDLY_NAME "-%06x"       // [MqttClient] Also fall back topic using last 6 characters of MAC address or use "DVES_%12X" for complete MAC address

#undef MDNS_ENABLED
#define MDNS_ENABLED           true             // [SetOption55] Use mDNS (false = Disable, true = Enable)
#define USE_DISCOVERY                            // Enable mDNS for the following services (+8k code or +23.5k code with core 2_5_x, +0.3k mem)
#define WEBSERVER_ADVERTISE                    // Provide access to webserver by name <Hostname>.local/
#define MQTT_HOST_DISCOVERY                    // Find MQTT host server (overrides MQTT_HOST if found)

#undef MODULE
#define MODULE                 WEMOS      // [Module] Select default module from tasmota_template.h
#undef FALLBACK_MODULE
#define FALLBACK_MODULE        WEMOS      // [Module2] Select default module on fast reboot where USER_MODULE is user template
#undef USER_TEMPLATE
#define USER_TEMPLATE "{\"NAME\":\"Generic\",\"GPIO\":[1,1,1,1,1,1,1,1,1,1,1,1,1,1],\"FLAG\":0,\"BASE\":18}"  // [Template] Set JSON template

#define DEVICE_TYPE            "Light"

// -- MQTT - Domoticz -----------------------------
#undef USE_DOMOTICZ                                         // Enable Domoticz (+6k code, +0.3k mem)
  #undef DOMOTICZ_IN_TOPIC                                  // Domoticz Input Topic
  #undef DOMOTICZ_OUT_TOPIC                                 // Domoticz Output Topic

// -- MQTT - Home Assistant Discovery -------------
#undef USE_HOME_ASSISTANT                                   // Enable Home Assistant Discovery Support (+12k code, +6 bytes mem)
  #undef HOME_ASSISTANT_DISCOVERY_PREFIX                    // Home Assistant discovery prefix
  #undef HOME_ASSISTANT_LWT_TOPIC                           // home Assistant Birth and Last Will Topic (default = homeassistant/status)
  #undef HOME_ASSISTANT_LWT_SUBSCRIBE                       // Subscribe to Home Assistant Birth and Last Will Topic (default = true)

// -- Telegram Protocol ---------------------------
  #undef USE_TELEGRAM
  #undef USE_TELEGRAM_FINGERPRINT                           // Telegram api.telegram.org TLS public key fingerpring

// -- KNX IP Protocol -----------------------------
  #undef USE_KNX
  #undef USE_KNX_WEB_MENU                                   // Enable KNX WEB MENU (+8.3k code, +144 mem)

// -- HTTP ----------------------------------------
#undef USE_EMULATION_HUE                                    // Enable Hue Bridge emulation for Alexa (+14k code, +2k mem common)
  #undef USE_EMULATION_WEMO                                 // Enable Belkin WeMo emulation for Alexa (+6k code, +2k mem common)

// -- Time ----------------------------------------
#undef USE_TIMERS                                           // Add support for up to 16 timers (+2k2 code)
  #undef USE_TIMERS_WEB                                     // Add timer webpage support (+4k5 code)
  #undef USE_SUNRISE                                        // Add support for Sunrise and sunset tools (+16k)
    #undef SUNRISE_DAWN_ANGLE                               // Select desired Dawn Angle from (DAWN_NORMAL, DAWN_CIVIL, DAWN_NAUTIC, DAWN_ASTRONOMIC)

// -- Optional modules ----------------------------
#undef ROTARY_V1                                            // Add support for Rotary Encoder as used in MI Desk Lamp (+0k8 code)
  #undef ROTARY_MAX_STEPS                                   // Rotary step boundary
#undef USE_SONOFF_RF                                        // Add support for Sonoff Rf Bridge (+3k2 code)
  #undef USE_RF_FLASH                                       // Add support for flashing the EFM8BB1 chip on the Sonoff RF Bridge. C2CK must be connected to GPIO4, C2D to GPIO5 on the PCB (+2k7 code)
#undef USE_SONOFF_SC                                        // Add support for Sonoff Sc (+1k1 code)
#undef USE_TUYA_MCU                                         // Add support for Tuya Serial MCU
  #undef TUYA_DIMMER_ID                                     // Default dimmer Id
  #undef USE_TUYA_TIME                                      // Add support for Set Time in Tuya MCU
#undef USE_ARMTRONIX_DIMMERS                                // Add support for Armtronix Dimmers (+1k4 code)
#undef USE_PS_16_DZ                                         // Add support for PS-16-DZ Dimmer (+2k code)
#undef USE_SONOFF_IFAN                                      // Add support for Sonoff iFan02 and iFan03 (+2k code)
#undef USE_BUZZER                                           // Add support for a buzzer (+0k6 code)
#undef USE_ARILUX_RF                                        // Add support for Arilux RF remote controller (+0k8 code, 252 iram (non 2.3.0))
#undef USE_SHUTTER                                          // Add Shutter support for up to 4 shutter with different motortypes (+11k code)
#undef USE_EXS_DIMMER                                       // Add support for ES-Store Wi-Fi Dimmer (+1k5 code)
#undef USE_PWM_DIMMER                                       // Add support for MJ-SD01/acenx/NTONPOWER PWM dimmers (+2k3 code, DGR=0k7)
  #undef USE_PWM_DIMMER_REMOTE                              // Add support for remote switches to PWM Dimmer (requires USE_DEVICE_GROUPS) (+0k6 code)
#undef USE_SONOFF_D1                                        // Add support for Sonoff D1 Dimmer (+0k7 code)
#undef USE_SHELLY_DIMMER                                    // Add support for Shelly Dimmer (+3k code)
  #undef SHELLY_CMDS                                        // Add command to send co-processor commands (+0k3 code)
  #undef SHELLY_FW_UPGRADE                                  // Add firmware upgrade option for co-processor (+3k4 code)

// -- Optional light modules ----------------------
#undef USE_LIGHT                                            // Add support for light control
#undef USE_WS2812                                           // WS2812 Led string using library NeoPixelBus (+5k code, +1k mem, 232 iram) - Disable by //
  #undef USE_WS2812_RMT                                     // ESP32 only, hardware RMT support (default). Specify the RMT channel 0..7. This should be preferred to software bit bang.

  #undef USE_WS2812_HARDWARE                                // Hardware type (NEO_HW_WS2812, NEO_HW_WS2812X, NEO_HW_WS2813, NEO_HW_SK6812, NEO_HW_LC8812, NEO_HW_APA106, NEO_HW_P9813)
  #undef USE_WS2812_CTYPE                                   // Color type (NEO_RGB, NEO_GRB, NEO_BRG, NEO_RBG, NEO_RGBW, NEO_GRBW)
#undef USE_MY92X1                                           // Add support for MY92X1 RGBCW led controller as used in Sonoff B1, Ailight and Lohas
#undef USE_SM16716                                          // Add support for SM16716 RGB LED controller (+0k7 code)
#undef USE_SM2135                                           // Add support for SM2135 RGBCW led control as used in Action LSC (+0k6 code)
#undef USE_SONOFF_L1                                        // Add support for Sonoff L1 led control
#undef USE_ELECTRIQ_MOODL                                   // Add support for ElectriQ iQ-wifiMOODL RGBW LED controller (+0k3 code)
#undef USE_LIGHT_PALETTE                                    // Add support for color palette (+0k7 code)
#undef USE_LIGHT_VIRTUAL_CT                                 // Add support for Virtual White Color Temperature (+1.1k code)
#undef USE_DGR_LIGHT_SEQUENCE                               // Add support for device group light sequencing (requires USE_DEVICE_GROUPS) (+0k2 code)

// -- Counter input -------------------------------
#undef USE_COUNTER                                          // Enable inputs as counter (+0k8 code)

// -- One wire sensors ----------------------------
#undef USE_DS18x20                                          // Add support for DS18x20 sensors with id sort, single scan and read retry (+2k6 code)

// -- I2C sensors ---------------------------------
    #undef USE_VEML6070_RSET                                // VEML6070, Rset in Ohm used on PCB board, default 270K = 270000ohm, range for this sensor: 220K ... 1Meg
    #undef USE_VEML6070_SHOW_RAW                            // VEML6070, shows the raw value of UV-A
    #undef MGS_SENSOR_ADDR                                  // Default Mutichannel Gas sensor i2c address
    #undef USE_APDS9960_GESTURE                             // Enable APDS9960 Gesture feature (+2k code)
    #undef USE_APDS9960_PROXIMITY                           // Enable APDS9960 Proximity feature (>50 code)
    #undef USE_APDS9960_COLOR                               // Enable APDS9960 Color feature (+0.8k code)
    #undef USE_APDS9960_STARTMODE                           // Default to enable Gesture mode
  #undef USE_ADE7953                                        // [I2cDriver7] Enable ADE7953 Energy monitor as used on Shelly 2.5 (I2C address 0x38) (+1k5)

// -- SPI sensors ---------------------------------
    #undef USE_MIBLE                                        // BLE-bridge for some Mijia-BLE-sensors (+4k7 code)
    #undef USE_DISPLAY_ILI9341                              // [DisplayModel 4] Enable ILI9341 Tft 480x320 display (+19k code)

// -- Power monitoring sensors --------------------
#undef USE_ENERGY_SENSOR                                    // Add support for Energy Monitors (+14k code)
#undef USE_ENERGY_MARGIN_DETECTION                          // Add support for Energy Margin detection (+1k6 code)
  #undef USE_ENERGY_POWER_LIMIT                             // Add additional support for Energy Power Limit detection (+1k2 code)
#undef USE_ENERGY_DUMMY                                     // Add support for dummy Energy monitor allowing user values (+0k7 code)
#undef USE_HLW8012                                          // Add support for HLW8012, BL0937 or HJL-01 Energy Monitor for Sonoff Pow and WolfBlitz
#undef USE_CSE7766                                          // Add support for CSE7766 Energy Monitor for Sonoff S31 and Pow R2
#undef USE_PZEM004T                                         // Add support for PZEM004T Energy monitor (+2k code)
#undef USE_PZEM_AC                                          // Add support for PZEM014,016 Energy monitor (+1k1 code)
#undef USE_PZEM_DC                                          // Add support for PZEM003,017 Energy monitor (+1k1 code)
#undef USE_MCP39F501                                        // Add support for MCP39F501 Energy monitor as used in Shelly 2 (+3k1 code)
  #undef SDM72_SPEED                                        // SDM72-Modbus RS485 serial speed (default: 9600 baud)
  #undef SDM120_SPEED                                       // SDM120-Modbus RS485 serial speed (default: 2400 baud)
  #undef SDM630_SPEED                                       // SDM630-Modbus RS485 serial speed (default: 9600 baud)
  #undef DDS2382_SPEED                                      // Hiking DDS2382 Modbus RS485 serial speed (default: 9600 baud)
  #undef DDSU666_SPEED                                      // Chint DDSU666 Modbus RS485 serial speed (default: 9600 baud)
  #undef SOLAXX1_SPEED                                      // Solax X1 Modbus RS485 serial speed (default: 9600 baud)
  #undef SOLAXX1_PV2                                        // Solax X1 using second PV
  #undef LE01MR_SPEED                                       // LE-01MR modbus baudrate (default: 9600)
  #undef LE01MR_ADDR                                        // LE-01MR modbus address (default: 0x01)
#undef USE_BL0940                                           // Add support for BL0940 Energy monitor as used in Blitzwolf SHP-10 (+1k6 code)
  #undef IEM3000_SPEED                                      // iEM3000-Modbus RS485 serial speed (default: 19200 baud)
  #undef IEM3000_ADDR                                       // iEM3000-Modbus modbus address (default: 0x01)

// -- Low level interface devices -----------------
#undef USE_DHT                                              // Add support for DHT11, AM2301 (DHT21, DHT22, AM2302, AM2321) and SI7021 Temperature and Humidity sensor (1k6 code)
  #undef MAX31865_PTD_WIRES                                 // PTDs come in several flavors, pick yours. Specific settings per sensor possible with MAX31865_PTD_WIRES1..MAX31865_PTD_WIRES6
  #undef MAX31865_PTD_RES                                   // Nominal PTD resistance at 0°C (100Ω for a PT100, 1000Ω for a PT1000, YMMV!). Specific settings per sensor possible with MAX31865_PTD_RES1..MAX31865_PTD_RES6
  #undef MAX31865_REF_RES                                   // Reference resistor (Usually 430Ω for a PT100, 4300Ω for a PT1000). Specific settings per sensor possible with MAX31865_REF_RES1..MAX31865_REF_RES6
  #undef MAX31865_PTD_BIAS                                  // To calibrate your not-so-good PTD. Specific settings per sensor possible with MAX31865_PTD_BIAS1..MAX31865_PTD_BIAS6

// -- IR Remote features - subset of IR protocols --------------------------
#undef USE_IR_REMOTE                                        // Send IR remote commands using library IRremoteESP8266 and ArduinoJson (+4k3 code, 0k3 mem, 48 iram)
  // #undef IR_SEND_INVERTED                                   // Invert the output. (default = false) e.g. LED is illuminated when GPIO is LOW rather than HIGH.
  //                                                           // Setting inverted to something other than the default could easily destroy your IR LED if you are overdriving it.
  //                                                           // Unless you REALLY know what you are doing, don't change this.
  // #undef IR_SEND_USE_MODULATION                             // Do we do frequency modulation during transmission? i.e. If not, assume a 100% duty cycle.
  // #undef USE_IR_SEND_NEC                                    // Support IRsend NEC protocol
  // #undef USE_IR_SEND_RC5                                    // Support IRsend Philips RC5 protocol
  // #undef USE_IR_SEND_RC6                                    // Support IRsend Philips RC6 protocol

  // #undef USE_IR_RECEIVE                                     // Support for IR receiver (+7k2 code, 264 iram)
  //   #undef IR_RCV_BUFFER_SIZE                               // Max number of packets allowed in capture buffer (default 100 (*2 bytes ram))
  //   #undef IR_RCV_TIMEOUT                                   // Number of milli-Seconds of no-more-data before we consider a message ended (default 15)
  //   #undef IR_RCV_MIN_UNKNOWN_SIZE                          // Set the smallest sized "UNKNOWN" message packets we actually care about (default 6, max 255)
  //   #undef IR_RCV_WHILE_SENDING                             // Turns on receiver while sending messages, i.e. receive your own. This is unreliable and can cause IR timing issues   

  #undef USE_BERRY

// -- HTTP GUI Colors -----------------------------
// HTML hex color codes. Only 3 and 6 digit hex string values are supported!! See https://www.w3schools.com/colors/colors_hex.asp
// Light theme - pre v7
// WebColor {"WebColor":["#000","#fff","#f2f2f2","#000","#fff","#000","#fff","#f00","#008000","#fff","#1fa3ec","#0e70a4","#d43535","#931f1f","#47c266","#5aaf6f","#fff","#999","#000"]}
/*
#define COLOR_TEXT                  "#000"       // [WebColor1] Global text color - Black
#define COLOR_BACKGROUND            "#fff"       // [WebColor2] Global background color - White
#define COLOR_FORM                  "#f2f2f2"    // [WebColor3] Form background color - Greyish
#define COLOR_INPUT_TEXT            "#000"       // [WebColor4] Input text color - Black
#define COLOR_INPUT                 "#fff"       // [WebColor5] Input background color - White
#define COLOR_CONSOLE_TEXT          "#000"       // [WebColor6] Console text color - Black
#define COLOR_CONSOLE               "#fff"       // [WebColor7] Console background color - White
#define COLOR_TEXT_WARNING          "#f00"       // [WebColor8] Warning text color - Red
#define COLOR_TEXT_SUCCESS          "#008000"    // [WebColor9] Success text color - Dark lime green
#define COLOR_BUTTON_TEXT           "#fff"       // [WebColor10] Button text color - White
#define COLOR_BUTTON                "#1fa3ec"    // [WebColor11] Button color - Vivid blue
#define COLOR_BUTTON_HOVER          "#0e70a4"    // [WebColor12] Button color when hovered over - Dark blue
#define COLOR_BUTTON_RESET          "#d43535"    // [WebColor13] Restart/Reset/Delete button color - Strong red
#define COLOR_BUTTON_RESET_HOVER    "#931f1f"    // [WebColor14] Restart/Reset/Delete button color when hovered over - Dark red
#define COLOR_BUTTON_SAVE           "#47c266"    // [WebColor15] Save button color - Moderate lime green
#define COLOR_BUTTON_SAVE_HOVER     "#5aaf6f"    // [WebColor16] Save button color when hovered over - Dark moderate lime green
#define COLOR_TIMER_TAB_TEXT        "#fff"       // [WebColor17] Config timer tab text color - White
#define COLOR_TIMER_TAB_BACKGROUND  "#999"       // [WebColor18] Config timer tab background color - Dark gray
#define COLOR_TITLE_TEXT            "#000"       // [WebColor19] Title text color - Whiteish
*/
// Dark theme
// WebColor {"WebColor":["#eaeaea","#252525","#4f4f4f","#000","#ddd","#65c115","#1f1f1f","#ff5661","#008000","#faffff","#1fa3ec","#0e70a4","#d43535","#931f1f","#47c266","#5aaf6f","#faffff","#999","#eaeaea"]}
#define COLOR_TEXT                  "#e9e9e9"    // [WebColor1] Global text color - Very light gray
#define COLOR_BACKGROUND            "#31353c"    // [WebColor2] Global background color - Very dark gray (mostly black)
#define COLOR_FORM                  "#31353c"    // [WebColor3] Form background color - Very dark gray
#define COLOR_INPUT_TEXT            "#000"       // [WebColor4] Input text color - Black
#define COLOR_INPUT                 "#ddd"       // [WebColor5] Input background color - Very light gray
#define COLOR_CONSOLE_TEXT          "#65c115"    // [WebColor6] Console text color - Strong Green
#define COLOR_CONSOLE               "#1f1f1f"    // [WebColor7] Console background color - Very dark gray (mostly black)
#define COLOR_TEXT_WARNING          "#da4139"    // [WebColor8] Warning text color - Brick Red
#define COLOR_TEXT_SUCCESS          "#008000"    // [WebColor9] Success text color - Dark lime green
#define COLOR_BUTTON_TEXT           "#fff"    // [WebColor10] Button text color - Very pale (mostly white) cyan
#define COLOR_BUTTON                "#ffa400"    // [WebColor11] Button color - Vivid blue
#define COLOR_BUTTON_HOVER          "#996300"    // [WebColor12] Button color when hovered over - Dark blue
#define COLOR_BUTTON_RESET          "#da4139"    // [WebColor13] Restart/Reset/Delete button color - Strong red
#define COLOR_BUTTON_RESET_HOVER    "#931f1f"    // [WebColor14] Restart/Reset/Delete button color when hovered over - Dark red
#define COLOR_BUTTON_SAVE           "47c266"    // [WebColor15] Save button color - Moderate lime green
#define COLOR_BUTTON_SAVE_HOVER     "#5aaf6f"    // [WebColor16] Save button color when hovered over - Dark moderate lime green
#define COLOR_TIMER_TAB_TEXT        "#faffff"    // [WebColor17] Config timer tab text color - Very pale (mostly white) cyan.
#define COLOR_TIMER_TAB_BACKGROUND  "#999"       // [WebColor18] Config timer tab background color - Dark gray
#define COLOR_TITLE_TEXT            "#e9e9e9"    // [WebColor19] Title text color - Very light gray

/*
Examples :
// -- Master parameter control --------------------
#undef  CFG_HOLDER
#define CFG_HOLDER        4617                   // [Reset 1] Change this value to load SECTION1 configuration parameters to flash
// -- Setup your own Wifi settings  ---------------
#undef  STA_SSID1
#define STA_SSID1         "YourSSID"             // [Ssid1] Wifi SSID
#undef  STA_PASS1
#define STA_PASS1         "YourWifiPassword"     // [Password1] Wifi password
// -- Setup your own MQTT settings  ---------------
#undef  MQTT_HOST
#define MQTT_HOST         "your-mqtt-server.com" // [MqttHost]
#undef  MQTT_PORT
#define MQTT_PORT         1883                   // [MqttPort] MQTT port (10123 on CloudMQTT)
#undef  MQTT_USER
#define MQTT_USER         "YourMqttUser"         // [MqttUser] Optional user
#undef  MQTT_PASS
#define MQTT_PASS         "YourMqttPass"         // [MqttPassword] Optional password
// You might even pass some parameters from the command line ----------------------------
// Ie:  export PLATFORMIO_BUILD_FLAGS='-DUSE_CONFIG_OVERRIDE -DMY_IP="192.168.1.99" -DMY_GW="192.168.1.1" -DMY_DNS="192.168.1.1"'
#ifdef MY_IP
#undef  WIFI_IP_ADDRESS
#define WIFI_IP_ADDRESS     MY_IP                // Set to 0.0.0.0 for using DHCP or enter a static IP address
#endif
#ifdef MY_GW
#undef  WIFI_GATEWAY
#define WIFI_GATEWAY        MY_GW                // if not using DHCP set Gateway IP address
#endif
#ifdef MY_DNS
#undef  WIFI_DNS
#define WIFI_DNS            MY_DNS               // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif
// !!! Remember that your changes GOES AT THE BOTTOM OF THIS FILE right before the last #endif !!! 
*/

#undef DEBUG_TASMOTA_CORE                       // Enable core debug messages
#undef DEBUG_TASMOTA_DRIVER                     // Enable driver debug messages
#undef DEBUG_TASMOTA_SENSOR                     // Enable sensor debug messages
#undef USE_DEBUG_DRIVER                         // Use xdrv_99_debug.ino providing commands CpuChk, CfgXor, CfgDump, CfgPeek and CfgPoke





#endif  // _USER_CONFIG_OVERRIDE_H_