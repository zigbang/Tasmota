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
#define MQTT_HOST         "a3krw5nopfpfqu-ats.iot.ap-northeast-2.amazonaws.com" // [MqttHost]

#undef  MQTT_PORT
#define MQTT_PORT         8883                   // [MqttPort] MQTT port (10123 on CloudMQTT)

//#define STA_SSID1              "U+Net8F50"                // [Ssid1] Wi-Fi SSID
//#define STA_PASS1              "D2B662A0P@"                // [Password1] Wi-Fi password

#define DEFAULT_SSID      "U+Net8F50"
#define DEFAULT_PASS      "D2B662A0P@"

#ifndef USE_MQTT_TLS
#define USE_MQTT_TLS
#define USE_MQTT_TLS_CA_CERT // Optional but highly recommended
#endif
#ifndef USE_MQTT_AWS_IOT
#define USE_MQTT_AWS_IOT
#endif

#define PROJECT                "ZIoT_Sonoff"         // PROJECT is used as the default topic delimiter
#define FRIENDLY_NAME          "ZIGBANG"         // [FriendlyName] Friendlyname up to 32 characters used by webpages and Alexa
#define MQTT_TOPIC             "ZiotThing_" DEVICE_TYPE "_" FRIENDLY_NAME "_%12X"   // [Topic] unique MQTT device topic including (part of) device MAC address
#define MQTT_GRPTOPIC          FRIENDLY_NAME "s"        // [GroupTopic] MQTT Group topic
#define MQTT_CLIENT_ID         FRIENDLY_NAME "_%06X"       // [MqttClient] Also fall back topic using last 6 characters of MAC address or use "DVES_%12X" for complete MAC address

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





#endif  // _USER_CONFIG_OVERRIDE_H_
