/*
  af_AF.h - localization for Afrikaans - Afrikaans for Tasmota

  Copyright (C) 2020  Christiaan Heerze

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

#ifndef _LANGUAGE_AF_AF_H_
#define _LANGUAGE_AF_AF_H_

/*************************** ATTENTION *******************************\
 *
 * Due to memory constraints only UTF-8 is supported.
 * To save code space keep text as short as possible.
 * Time and Date provided by SDK can not be localized (yet).
 * Use online command StateText to translate ON, OFF, HOLD and TOGGLE.
 * Use online command Prefix to translate cmnd, stat and tele.
 *
 * Updated until v9.2.1.0
\*********************************************************************/

//#define LANGUAGE_MODULE_NAME         // Enable to display "Module Generic" (ie Spanish), Disable to display "Generic Module" (ie English)
// https://www.science.co.il/language/Locale-codes.php
#define LANGUAGE_LCID 1078
// HTML (ISO 639-1) Language Code
#define D_HTML_LANGUAGE "af"

// "2017-03-07T11:08:02" - ISO8601:2004
#define D_YEAR_MONTH_SEPARATOR "-"
#define D_MONTH_DAY_SEPARATOR "-"
#define D_DATE_TIME_SEPARATOR "T"
#define D_HOUR_MINUTE_SEPARATOR ":"
#define D_MINUTE_SECOND_SEPARATOR ":"

#define D_DAY3LIST "SonMaaDinWoeDonVryDonSat"
#define D_MONTH3LIST "JanFebMaaAprMeiJunJulAugSepOktNovDes"

// Non JSON decimal separator
#define D_DECIMAL_SEPARATOR "."

// Common
#define D_ADMIN "Admin"
#define D_AIR_QUALITY "Luggehalte"
#define D_AP "AP"                    // Access Point
#define D_AS "as"
#define D_AUTO "AUTO"
#define D_BATT "Batt"                // Short for Battery
#define D_BLINK "Flits"
#define D_BLINKOFF "FlitsAf"
#define D_BOOT_COUNT "Opstarttelling"
#define D_BRIGHTLIGHT "Helder"
#define D_BSSID "BSSId"
#define D_BUTTON "Knoppie"
#define D_BY "deur"                    // Written by me
#define D_BYTES "Bytes"
#define D_CELSIUS "Celsius"
#define D_CHANNEL "Kanaal"
#define D_CO2 "Koolstofdioksied"
#define D_CODE "kode"                // Button code
#define D_COLDLIGHT "Koud"
#define D_COMMAND "Bevel"
#define D_CONNECTED "Gekoppel"
#define D_CORS_DOMAIN "CORS Domain"
#define D_COUNT "Telling"
#define D_COUNTER "Opnemer"
#define D_CT_POWER "CT Power"
#define D_CURRENT "Stroom"          // As in Voltage and Current
#define D_DATA "Data"
#define D_DARKLIGHT "Donker"
#define D_DEBUG "Ontfout"
#define D_DEWPOINT "Dou punt"
#define D_DISABLED "Gedeaktiveer"
#define D_DISTANCE "Afstand"
#define D_DNS_SERVER "DNS"
#define D_DO "Opgeloste suurstof"
#define D_DONE "Klaar"
#define D_DST_TIME "DST"
#define D_EC "EC"
#define D_ECO2 "eCO₂"
#define D_EMULATION "Emulasie"
#define D_ENABLED "Geaktiveer"
#define D_ERASE "Vee uit"
#define D_ERROR "Fout"
#define D_FAHRENHEIT "Fahrenheit"
#define D_FAILED "Misluk	"
#define D_FALLBACK "Terugval"
#define D_FALLBACK_TOPIC "Terugval onderwerp"
#define D_FALSE "Onwaar"
#define D_FILE "Lêer"
#define D_FLOW_RATE "Vloeitempo"
#define D_FREE_MEMORY "Vrye geheue"
#define D_PSR_MAX_MEMORY "PS-RAM geheue"
#define D_PSR_FREE_MEMORY "PS-RAM vrye geheue"
#define D_FREQUENCY "Frekwensie"
#define D_GAS "Gas"
#define D_GATEWAY "Gateway"
#define D_GROUP "Groep"
#define D_HOST "Gasheer"
#define D_HOSTNAME "Gasheername"
#define D_HUMIDITY "Humiditeit"
#define D_IMMEDIATE "onmiddellik"      // Button immediate
#define D_INDEX "Indeks"
#define D_INFO "Inligting"
#define D_INFRARED "Infrarooi"
#define D_INITIALIZED "Geïnisialiseer"
#define D_IP_ADDRESS "IP-adresse"
#define D_LIGHT "Lig"
#define D_LWT "LWT"
#define D_LQI "LQI"                  // Zigbee Link Quality Index
#define D_MODULE "Module"
#define D_MOISTURE "Vog"
#define D_MQTT "MQTT"
#define D_MULTI_PRESS "multi-press"
#define D_NOISE "Geraas"
#define D_NONE "Geen"
#define D_O2 "Suurstof"
#define D_OFF "Uit"
#define D_OFFLINE "Vanlyn"
#define D_OK "Ok"
#define D_ON "aan"
#define D_ONLINE "Aanlyn"
#define D_ORP "ORP"
#define D_PASSWORD "Wagwoord"
#define D_PH "pH"
#define D_PORT "Poort"
#define D_POWER_FACTOR "Krag faktor"
#define D_POWERUSAGE "Krag"
#define D_POWERUSAGE_ACTIVE "Regte krag"
#define D_POWERUSAGE_APPARENT "Oënskynlike krag"
#define D_POWERUSAGE_REACTIVE "reaktiewe krag"
#define D_PRESSURE "Druk"
#define D_PRESSUREATSEALEVEL "Druk op seevlak"
#define D_PROGRAM_FLASH_SIZE "Program Flash Grootte"
#define D_PROGRAM_SIZE "Program Grootte"
#define D_PROJECT "Projek"
#define D_RAIN "Reën"
#define D_RANGE "Gebied"
#define D_RECEIVED "Ontvang"
#define D_RESTART "Herbegin"
#define D_RESTARTING "Herbegin"
#define D_RESTART_REASON "Herlaai rede"
#define D_RESTORE "herstel"
#define D_RETAINED "behou"
#define D_RULE "Reël"
#define D_SAVE "Stoor"
#define D_SENSOR "Sensor"
#define D_SSID "SSId"
#define D_START "Begin"
#define D_STD_TIME "STD"
#define D_STOP "Stop"
#define D_SUBNET_MASK "Subnet Masker"
#define D_SUBSCRIBE_TO "Teken in op"
#define D_UNSUBSCRIBE_FROM "Teken uit van"
#define D_SUCCESSFUL "Suksesvol"
#define D_SUNRISE "Sonsopkoms"
#define D_SUNSET "Sononder"
#define D_TEMPERATURE "Temperatuur"
#define D_TO "naar"
#define D_TOGGLE "Verwissel"
#define D_TOPIC "Onderwerp"
#define D_TOTAL_USAGE "Totale gebruik"
#define D_TRANSMIT "Stuur"
#define D_TRUE "Waar"
#define D_TVOC "TVOC"
#define D_UPGRADE "opgradeer"
#define D_UPLOAD "Laai op"
#define D_UPTIME "Uptyd"
#define D_USER "Gebruiker"
#define D_UTC_TIME "UTC"
#define D_UV_INDEX "UV Indeks"
#define D_UV_INDEX_1 "Laag"
#define D_UV_INDEX_2 "Middel"
#define D_UV_INDEX_3 "Hoog"
#define D_UV_INDEX_4 "Gevaarlik"
#define D_UV_INDEX_5 "1ste/2de graad sonbrand"
#define D_UV_INDEX_6 "3e graad sonbrand"
#define D_UV_INDEX_7 "Buite bereik"         // Out of Range
#define D_UV_LEVEL "UV Level"
#define D_UV_POWER "UV krag"
#define D_VERSION "Weergawe"
#define D_VOLTAGE "Spanning"
#define D_VOLUME "Volume"
#define D_WEIGHT "Gewig"
#define D_WARMLIGHT "Warm"
#define D_WEB_SERVER "Webbediener"

// tasmota.ino
#define D_WARNING_MINIMAL_VERSION "WAARSKUWING Hierdie weergawe ondersteun nie aanhoudende instellings nie"
#define D_LEVEL_10 "level 1-0"
#define D_LEVEL_01 "level 0-1"
#define D_SERIAL_LOGGING_DISABLED "Syslog-logging is uitgeskakel"
#define D_SYSLOG_LOGGING_REENABLED "Syslog-logging is weer geaktiveer"

#define D_SET_BAUDRATE_TO "Stem Baudrate op"
#define D_RECEIVED_TOPIC "Ontvang onderwerp"
#define D_DATA_SIZE "Data grootte"
#define D_ANALOG_INPUT "Analoog"

// support.ino
#define D_OSWATCH "osWatch"
#define D_BLOCKED_LOOP "Geblokkeerde lus"
#define D_WPS_FAILED_WITH_STATUS "WPSconfig kon nie status kry nie"
#define D_ACTIVE_FOR_3_MINUTES "3 minute aktief"
#define D_FAILED_TO_START "kon nie begin nie"
#define D_PATCH_ISSUE_2186 "Patch issue 2186"
#define D_CONNECTING_TO_AP "Koppel tans aan AP"
#define D_IN_MODE "in mode"
#define D_CONNECT_FAILED_NO_IP_ADDRESS "Kon nie verbind nie omdat geen IP-adres ontvang is nie"
#define D_CONNECT_FAILED_AP_NOT_REACHED "Kon nie verbind nie, aangesien toegang nie bereik kan word nie"
#define D_CONNECT_FAILED_WRONG_PASSWORD "Kon nie verbind nie"
#define D_CONNECT_FAILED_AP_TIMEOUT "Kon nie verbinding maak met AP-time-out nie"
#define D_ATTEMPTING_CONNECTION "Attempting connection..."
#define D_CHECKING_CONNECTION "Probeer tans verbinding ..."
#define D_QUERY_DONE "Navraag gedoen. MQTT-dienste gevind"
#define D_MQTT_SERVICE_FOUND "MQTT-diens gevind op"
#define D_FOUND_AT "gevind op"
#define D_SYSLOG_HOST_NOT_FOUND "Syslog Host nie gevind nie"

// settings.ino
#define D_SAVED_TO_FLASH_AT "Gestoor in die flash op"
#define D_LOADED_FROM_FLASH_AT "Uit flash gelaai by"
#define D_USE_DEFAULTS "Gebruik standaardinstellings"
#define D_ERASED_SECTOR "Uitgewis sektor"

// xdrv_02_webserver.ino
#define D_NOSCRIPT "Skakel JavaScript aan om Tasmota te gebruik"
#define D_MINIMAL_FIRMWARE_PLEASE_UPGRADE "MINIMAL firmware <br> gradeer asseblief op"
#define D_WEBSERVER_ACTIVE_ON "Webbediener aktief op"
#define D_WITH_IP_ADDRESS "met IP-adres"
#define D_WEBSERVER_STOPPED "Webbediener gestop"
#define D_FILE_NOT_FOUND "Lêer nie gevind nie"
#define D_REDIRECTED "Herlei na captive portal"
#define D_WIFIMANAGER_SET_ACCESSPOINT_AND_STATION "Wifimanager stel AccessPoint in en hou Station"
#define D_WIFIMANAGER_SET_ACCESSPOINT "Wifimanager het AccessPoint ingestel"
#define D_TRYING_TO_CONNECT "Probeer om toestel aan netwerk te koppel"

#define D_RESTART_IN "Begin weer binne"
#define D_SECONDS "sekondes"
#define D_DEVICE_WILL_RESTART "Toestel sal binne 'n paar sekondes weer begin"
#define D_BUTTON_TOGGLE "Wissel"
#define D_CONFIGURATION "Konfigurasie"
#define D_INFORMATION "Inligting"
#define D_FIRMWARE_UPGRADE "Firmware-opgradering"
#define D_CONSOLE "Konsole"
#define D_CONFIRM_RESTART "Bevestig weer te begin"

#define D_CONFIGURE_MODULE "Stel module in"
#define D_CONFIGURE_WIFI "Stel WiFi in"
#define D_CONFIGURE_MQTT "Stel MQTT in"
#define D_CONFIGURE_DOMOTICZ "Stel Domoticz in"
#define D_CONFIGURE_LOGGING "Stel logging in"
#define D_CONFIGURE_OTHER "Stel ander in"
#define D_CONFIRM_RESET_CONFIGURATION "Bevestig die herstel van die konfigurasie"
#define D_RESET_CONFIGURATION "Stel die konfigurasie terug"
#define D_BACKUP_CONFIGURATION "Rugsteun die konfigurasie"
#define D_RESTORE_CONFIGURATION "Stel die konfigurasie terug"
#define D_MAIN_MENU "Hoofkieslys"

#define D_MODULE_PARAMETERS "Moduleparameters"
#define D_MODULE_TYPE "Module tipe"
#define D_PULLUP_ENABLE "Geen knoppie/skakelaar optrek"
#define D_ADC "ADC"
#define D_GPIO "GPIO"
#define D_SERIAL_IN "Serial in"
#define D_SERIAL_OUT "Serial uit"

#define D_WIFI_PARAMETERS "Wifi-parameters"
#define D_SCAN_FOR_WIFI_NETWORKS "Soek vir wifi-netwerke"
#define D_SCAN_DONE "Skandering gedoen"
#define D_NO_NETWORKS_FOUND "Geen netwerke gevind nie"
#define D_REFRESH_TO_SCAN_AGAIN "Verfris om weer te skandeer"
#define D_DUPLICATE_ACCESSPOINT "Duplikaat AccessPoint"
#define D_SKIPPING_LOW_QUALITY "Slaan oor as gevolg van lae gehalte"
#define D_RSSI "RSSI"
#define D_WEP "WEP"
#define D_WPA_PSK "WPA PSK"
#define D_WPA2_PSK "WPA2 PSK"
#define D_AP1_SSID "AP1 SSId"
#define D_AP1_PASSWORD "AP1 Wagwoord"
#define D_AP2_SSID "AP2 SSId"
#define D_AP2_PASSWORD "AP2 Wagwoord"

#define D_MQTT_PARAMETERS "MQTT-parameters"
#define D_CLIENT "Kliënt"
#define D_FULL_TOPIC "Volledige onderwerp"

#define D_LOGGING_PARAMETERS "Logging parameters"
#define D_SERIAL_LOG_LEVEL "Serial log level"
#define D_MQTT_LOG_LEVEL "Mqtt log level"
#define D_WEB_LOG_LEVEL "Web log level"
#define D_SYS_LOG_LEVEL "Syslog level"
#define D_MORE_DEBUG "More debug"
#define D_SYSLOG_HOST "Syslog host"
#define D_SYSLOG_PORT "Syslog port"
#define D_TELEMETRY_PERIOD "Telemetry period"

#define D_OTHER_PARAMETERS "Ander parameters"
#define D_TEMPLATE "Sjabloon"
#define D_ACTIVATE "Aktiveer"
#define D_DEVICE_NAME "Toestel naam"
#define D_WEB_ADMIN_PASSWORD "Webadministrateurwagwoord"
#define D_MQTT_ENABLE "MQTT inskakel"
#define D_MQTT_TLS_ENABLE "MQTT TLS"
#define D_FRIENDLY_NAME "Vriendelike naam"
#define D_BELKIN_WEMO "Belkin WeMo"
#define D_HUE_BRIDGE "Hue Bridge"
#define D_SINGLE_DEVICE "enkele toestel"
#define D_MULTI_DEVICE "multi toestel"

#define D_CONFIGURE_TEMPLATE "Konfigureer sjabloon"
#define D_TEMPLATE_PARAMETERS "Sjabloon parameters"
#define D_TEMPLATE_NAME "Naam"
#define D_BASE_TYPE "Gebaseer op"
#define D_TEMPLATE_FLAGS "Opsies"

#define D_SAVE_CONFIGURATION "Stoor konfigurasie"
#define D_CONFIGURATION_SAVED "Konfigurasie gestoor"
#define D_CONFIGURATION_RESET "Konfigurasie terugstel"

#define D_PROGRAM_VERSION "Programweergawe"
#define D_BUILD_DATE_AND_TIME "Bou datum en tyd"
#define D_CORE_AND_SDK_VERSION "Core/SDK weergawe"
#define D_FLASH_WRITE_COUNT "Flash skryf tel"
#define D_MAC_ADDRESS "MAC adress"
#define D_MQTT_HOST "MQTT gasheer"
#define D_MQTT_PORT "MQTT poort"
#define D_MQTT_CLIENT "MQTT kliënt"
#define D_MQTT_USER "MQTT gebruiker"
#define D_MQTT_TOPIC "MQTT onderwerp"
#define D_MQTT_GROUP_TOPIC "MQTT groeponderwerp"
#define D_MQTT_FULL_TOPIC "MQTT volledige onderwerp"
#define D_MQTT_NO_RETAIN "MQTT geen behoud"
#define D_MDNS_DISCOVERY "mDNS ontdekking"
#define D_MDNS_ADVERTISE "mDNS adverteer"
#define D_ESP_CHIP_ID "ESP Chip Id"
#define D_FLASH_CHIP_ID "Flash Chip Id"
#define D_FLASH_CHIP_SIZE "Flash Size"
#define D_FREE_PROGRAM_SPACE "Vrye program grootte"

#define D_UPGRADE_BY_WEBSERVER "Opgradeer per webbediener"
#define D_OTA_URL "OTA Url"
#define D_START_UPGRADE "Begin opgradering"
#define D_UPGRADE_BY_FILE_UPLOAD "Gradeer op volgens lêeroplaai"
#define D_UPLOAD_STARTED "Oplaai begin"
#define D_UPGRADE_STARTED "Opgradering is begin"
#define D_UPLOAD_DONE "Oplaai gedoen"
#define D_UPLOAD_TRANSFER "Laai oordrag op"
#define D_TRANSFER_STARTED "Oordrag is begin"
#define D_UPLOAD_ERR_1 "Geen lêer is gekies nie"
#define D_UPLOAD_ERR_2 "Nie genoeg plek nie"
#define D_UPLOAD_ERR_3 "Ongeldige lêerhandtekening"
#define D_UPLOAD_ERR_4 "Programgrootte is groter as geheue"
#define D_UPLOAD_ERR_5 "Laai buffer verkeerd op"
#define D_UPLOAD_ERR_6 "Kon nie oplaai nie. Aktiveer logging 3"
#define D_UPLOAD_ERR_7 "Oplaai afgebreek"
#define D_UPLOAD_ERR_8 "Lêer ongeldig"
#define D_UPLOAD_ERR_9 "Lêer is te groot"
#define D_UPLOAD_ERR_10 "Kon nie RF-skyfie inisieer nie"
#define D_UPLOAD_ERR_11 "Kon nie RF-skyfie uitvee nie"
#define D_UPLOAD_ERR_12 "Kon nie na RF-skyfie skryf nie"
#define D_UPLOAD_ERR_13 "Kon nie RF-firmware dekodeer nie"
#define D_UPLOAD_ERR_14 "Nie versoenbaar nie"
#define D_UPLOAD_ERROR_CODE "Laai foutkode op"

#define D_ENTER_COMMAND "Voer die opdrag in"
#define D_ENABLE_WEBLOG_FOR_RESPONSE "Skakel weblog 2 aan as antwoord verwag word"
#define D_NEED_USER_AND_PASSWORD "Benodig user=<gebruiker>&password=<wagwoord>"

// xdrv_01_mqtt.ino
#define D_FINGERPRINT "Verifieer TLS-vingerafdruk ..."
#define D_TLS_CONNECT_FAILED_TO "TLS Connect kon nie"
#define D_RETRY_IN "Probeer weer"
#define D_VERIFIED "Geverifieer met behulp van vingerafdruk"
#define D_INSECURE "Onveilige verbinding weens ongeldige vingerafdruk"
#define D_CONNECT_FAILED_TO "Kon nie verbind nie"

// xplg_wemohue.ino
#define D_MULTICAST_DISABLED "Multicast gedeaktiveer"
#define D_MULTICAST_REJOINED "Multicast (weer) aangesluit"
#define D_MULTICAST_JOIN_FAILED "Multicast aansluiting het misluk"
#define D_FAILED_TO_SEND_RESPONSE "Kon nie antwoord stuur nie"

#define D_WEMO "WeMo"
#define D_WEMO_BASIC_EVENT "WeMo basiese event"
#define D_WEMO_EVENT_SERVICE "WeMo geleentheids service"
#define D_WEMO_META_SERVICE "WeMo meta service"
#define D_WEMO_SETUP "WeMo opstelling"
#define D_RESPONSE_SENT "Antwoord gestuur"

#define D_HUE "Hue"
#define D_HUE_BRIDGE_SETUP "Hue opstelling"
#define D_HUE_API_NOT_IMPLEMENTED "Hue API nie geïmplementeer nie"
#define D_HUE_API "Hue API"
#define D_HUE_POST_ARGS "Hue POST args"
#define D_3_RESPONSE_PACKETS_SENT "3 antwoordpakkies gestuur"

// xdrv_07_domoticz.ino
#define D_DOMOTICZ_PARAMETERS "Domoticz parameters"
#define D_DOMOTICZ_IDX "Idx"
#define D_DOMOTICZ_KEY_IDX "Key idx"
#define D_DOMOTICZ_SWITCH_IDX "Switch idx"
#define D_DOMOTICZ_SENSOR_IDX "Sensor idx"
  #define D_DOMOTICZ_TEMP "Temp"
  #define D_DOMOTICZ_TEMP_HUM "Temp,Hum"
  #define D_DOMOTICZ_TEMP_HUM_BARO "Temp,Hum,Baro"
  #define D_DOMOTICZ_POWER_ENERGY "Krag, Energie"
  #define D_DOMOTICZ_ILLUMINANCE "Beligting"
  #define D_DOMOTICZ_COUNT "Telling/PM1"
  #define D_DOMOTICZ_VOLTAGE "Spanning/PM2.5"
  #define D_DOMOTICZ_CURRENT "Stroom/PM10"
  #define D_DOMOTICZ_AIRQUALITY "Lug kwaliteit"
  #define D_DOMOTICZ_P1_SMART_METER "P1SmartMeter"
#define D_DOMOTICZ_UPDATE_TIMER "Dateer tydopdatering op"

// xdrv_09_timers.ino
#define D_CONFIGURE_TIMER "Stel timer in"
#define D_TIMER_PARAMETERS "Timer-parameters"
#define D_TIMER_ENABLE "Aktiveer timers"
#define D_TIMER_ARM "Aktiveer"
#define D_TIMER_TIME "Tyd"
#define D_TIMER_DAYS "Dae"
#define D_TIMER_REPEAT "Herhaal"
#define D_TIMER_OUTPUT "Uitset"
#define D_TIMER_ACTION "Aksie"

// xdrv_10_knx.ino
#define D_CONFIGURE_KNX "Stel KNX op"
#define D_KNX_PARAMETERS "KNX-parameters"
#define D_KNX_GENERAL_CONFIG "Algemene"
#define D_KNX_PHYSICAL_ADDRESS "Fisiese adres"
#define D_KNX_PHYSICAL_ADDRESS_NOTE "( Moet uniek wees in die KNX-netwerk )"
#define D_KNX_ENABLE "Aktiveer KNX"
#define D_KNX_GROUP_ADDRESS_TO_WRITE "Data om na groepadresse te stuur"
#define D_ADD "Voeg by"
#define D_DELETE "Vee uit"
#define D_REPLY "Antwoord"
#define D_KNX_GROUP_ADDRESS_TO_READ "Groepadresse om data van te ontvang"
#define D_RECEIVED_FROM "Ontvang van"
#define D_KNX_COMMAND_WRITE "Skryf"
#define D_KNX_COMMAND_READ "Lees"
#define D_KNX_COMMAND_OTHER "Ander"
#define D_SENT_TO "gestuur na"
#define D_KNX_WARNING "Die groepadres ( 0 / 0 / 0 ) is gereserveer en kan nie gebruik word nie."
#define D_KNX_ENHANCEMENT "Kommunikasieverbetering"
#define D_KNX_TX_SLOT "KNX TX"
#define D_KNX_RX_SLOT "KNX RX"
#define D_KNX_TX_SCENE "KNX SCENE TX"
#define D_KNX_RX_SCENE "KNX SCENE RX"

// xdrv_23_zigbee
#define D_ZIGBEE_PERMITJOIN_ACTIVE "Toestelle wat toegelaat word om aan te sluit"
#define D_ZIGBEE_MAPPING_TITLE "Tasmota Zigbee-kartering"
#define D_ZIGBEE_NOT_STARTED "Zigbee het nie begin nie"
#define D_ZIGBEE_MAPPING_IN_PROGRESS_SEC "Kartering aan die gang (% d s. Oor)"
#define D_ZIGBEE_MAPPING_NOT_PRESENT "Geen kartering nie"
#define D_ZIGBEE_MAP_REFRESH "Zigbee kartering Refresh"
#define D_ZIGBEE_MAP "Zigbee-kaart"
#define D_ZIGBEE_PERMITJOIN "Zigbee Permit sluit aan"
#define D_ZIGBEE_GENERATE_KEY "genereer ewekansige Zigbee-netwerksleutel"
#define D_ZIGBEE_UNKNOWN_DEVICE "Onbekende toestel"
#define D_ZIGBEE_UNKNOWN_ATTRIBUTE "Onbekende kenmerk"
#define D_ZIGBEE_INVALID_PARAM "Ongeldige parameter"
#define D_ZIGBEE_MISSING_PARAM "Ontbrekende parameters"
#define D_ZIGBEE_UNKNWON_ATTRIBUTE "Onbekende kenmerknaam (geïgnoreer): %s"
#define D_ZIGBEE_TOO_MANY_CLUSTERS "Nie meer as een groep-ID per opdrag nie"
#define D_ZIGBEE_WRONG_DELIMITER "Verkeerde afbakening vir payload"
#define D_ZIGBEE_UNRECOGNIZED_COMMAND "Onherkenbare zigbee-opdrag: %s"
#define D_ZIGBEE_TOO_MANY_COMMANDS "Slegs 1 opdrag toegelaat (%d)"
#define D_ZIGBEE_NO_ATTRIBUTE "Geen kenmerk in die lys nie"
#define D_ZIGBEE_UNSUPPORTED_ATTRIBUTE_TYPE "Nie-ondersteunde kenmerktipe"
#define D_ZIGBEE_JSON_REQUIRED "Config benodig JSON-voorwerpe"
#define D_ZIGBEE_RESET_1_OR_2 "1 of 2 om te herstel"
#define D_ZIGBEE_EEPROM_FOUND_AT_ADDRESS "ZBBridge EEPROM gevind by adres"
#define D_ZIGBEE_RANDOMIZING_ZBCONFIG "Ewekansige Zigbee parameters, kyk asseblief met 'ZbConfig'"

// xdrv_03_energy.ino
#define D_ENERGY_TODAY "Energie Vandag"
#define D_ENERGY_YESTERDAY "Energie Gister"
#define D_ENERGY_TOTAL "Energie Totaal"

// xdrv_27_shutter.ino
#define D_OPEN "Maak oop"
#define D_CLOSE "Maak toe"
#define D_DOMOTICZ_SHUTTER "Luik"

// xdrv_28_pcf8574.ino
#define D_CONFIGURE_PCF8574 "Configure PCF8574"
#define D_PCF8574_PARAMETERS "PCF8574 parameters"
#define D_INVERT_PORTS "Keer poorte om"
#define D_DEVICE "Toestel"
#define D_DEVICE_INPUT "Invoer"
#define D_DEVICE_OUTPUT "Uitvoer"

// xsns_05_ds18b20.ino
#define D_SENSOR_BUSY "Sensor besig"
#define D_SENSOR_CRC_ERROR "Sensor CRC fout"
#define D_SENSORS_FOUND "Sensors gevind"

// xsns_06_dht.ino
#define D_TIMEOUT_WAITING_FOR "Tyduitstyd wag vir"
#define D_START_SIGNAL_LOW "begin sein laag"
#define D_START_SIGNAL_HIGH "begin sein hoog"
#define D_PULSE "pols"
#define D_CHECKSUM_FAILURE "Checksum het misluk"

// xsns_07_sht1x.ino
#define D_SENSOR_DID_NOT_ACK_COMMAND "Sensor het nie ACK opdrag"
#define D_SHT1X_FOUND "SHT1X gevind"

// xsns_18_pms5003.ino
#define D_STANDARD_CONCENTRATION "CF-1 PM"     // Standard Particle CF-1 Particle Matter
#define D_ENVIRONMENTAL_CONCENTRATION "PM"     // Environmetal Particle Matter
#define D_PARTICALS_BEYOND "Deeltjies"

// xsns_27_apds9960.ino
#define D_GESTURE "Gebaar"
#define D_COLOR_RED "Rooi"
#define D_COLOR_GREEN "Groen"
#define D_COLOR_BLUE "Blou"
#define D_CCT "CCT"
#define D_PROXIMITY "Nabyheid"

// xsns_32_mpu6050.ino
#define D_AX_AXIS "Accel. X-Axis"
#define D_AY_AXIS "Accel. Y-Axis"
#define D_AZ_AXIS "Accel. Z-Axis"
#define D_GX_AXIS "Gyro X-Axis"
#define D_GY_AXIS "Gyro Y-Axis"
#define D_GZ_AXIS "Gyro Z-Axis"

// xsns_34_hx711.ino
#define D_HX_CAL_REMOVE "Verwyder gewig"
#define D_HX_CAL_REFERENCE "Laai verwysingsgewig"
#define D_HX_CAL_DONE "Gekalibreer"
#define D_HX_CAL_FAIL "Kon nie kalibreer nie"
#define D_RESET_HX711 "Stel die skaal terug"
#define D_CONFIGURE_HX711 "Stel skaal op"
#define D_HX711_PARAMETERS "Skaal parameters"
#define D_ITEM_WEIGHT "Gewig van die item"
#define D_REFERENCE_WEIGHT "Verwysingsgewig"
#define D_CALIBRATE "Kalibreer"
#define D_CALIBRATION "Kalibrasie"

//xsns_35_tx20.ino
#define D_TX20_WIND_DIRECTION "Windrigting"
#define D_TX20_WIND_SPEED "Wind spoed"
#define D_TX20_WIND_SPEED_MIN "Wind spoed Min"
#define D_TX20_WIND_SPEED_MAX "Wind spoed Max"
#define D_TX20_NORTH "N"
#define D_TX20_EAST "O"
#define D_TX20_SOUTH "S"
#define D_TX20_WEST "W"

// xsns_53_sml.ino
#define D_TPWRIN "Totaal-In"
#define D_TPWROUT "Totaal-Uit"
#define D_TPWRCURR "Stroom-In/Uit"
#define D_TPWRCURR1 "Stroom-In p1"
#define D_TPWRCURR2 "Stroom-In p2"
#define D_TPWRCURR3 "Stroom-In p3"
#define D_Strom_L1 "Stroom L1"
#define D_Strom_L2 "Stroom L2"
#define D_Strom_L3 "Stroom L3"
#define D_Spannung_L1 "Spanning L1"
#define D_Spannung_L2 "Spanning L2"
#define D_Spannung_L3 "Spanning L3"
#define D_METERNR "Meter nommmer"
#define D_METERSID "Service ID"
#define D_GasIN "Teller"                // Gas-Verbrauch
#define D_H2oIN "Teller"                // H2o-Verbrauch
#define D_StL1L2L3 "Stroom L1+L2+L3"
#define D_SpL1L2L3 "Spanning L1+L2+L3/3"

// tasmota_template.h - keep them as short as possible to be able to fit them in GUI drop down box
#define D_SENSOR_NONE          "Geen"
#define D_SENSOR_USER          "Gebruiker"
#define D_SENSOR_OPTION        "Opsie"
#define D_SENSOR_DHT11         "DHT11"
#define D_SENSOR_AM2301        "AM2301"
#define D_SENSOR_SI7021        "SI7021"
#define D_SENSOR_DS18X20       "DS18x20"
#define D_SENSOR_I2C_SCL       "I2C SCL"
#define D_SENSOR_I2C_SDA       "I2C SDA"
#define D_SENSOR_WS2812        "WS2812"
#define D_SENSOR_DFR562        "MP3 Speler"
#define D_SENSOR_IRSEND        "IRstuurder"
#define D_SENSOR_SWITCH        "SKakelaar"     // Suffix "1"
#define D_SENSOR_BUTTON        "Knoppie"     // Suffix "1"
#define D_SENSOR_RELAY         "Relais"      // Suffix "1i"
#define D_SENSOR_LED           "Led"        // Suffix "1i"
#define D_SENSOR_LED_LINK      "LedLink"    // Suffix "i"
#define D_SENSOR_PWM           "PWM"        // Suffix "1"
#define D_SENSOR_COUNTER       "Teller"    // Suffix "1"
#define D_SENSOR_IRRECV        "IRontvanger"
#define D_SENSOR_MHZ_RX        "MHZ Rx"
#define D_SENSOR_MHZ_TX        "MHZ Tx"
#define D_SENSOR_PZEM004_RX    "PZEM004 Rx"
#define D_SENSOR_PZEM016_RX    "PZEM016 Rx"
#define D_SENSOR_PZEM017_RX    "PZEM017 Rx"
#define D_SENSOR_PZEM0XX_TX    "PZEM0XX Tx"
#define D_SENSOR_SAIR_RX       "SAir Rx"
#define D_SENSOR_SAIR_TX       "SAir Tx"
#define D_SENSOR_SPI_CS        "SPI CS"
#define D_SENSOR_SPI_DC        "SPI DC"
#define D_SENSOR_SPI_MISO      "SPI MISO"
#define D_SENSOR_SPI_MOSI      "SPI MOSI"
#define D_SENSOR_SPI_CLK       "SPI CLK"
#define D_SENSOR_BACKLIGHT     "Backlight"
#define D_SENSOR_PMS5003_TX    "PMS5003 Tx"
#define D_SENSOR_PMS5003_RX    "PMS5003 Rx"
#define D_SENSOR_SDS0X1_RX     "SDS0X1 Rx"
#define D_SENSOR_SDS0X1_TX     "SDS0X1 Tx"
#define D_SENSOR_HPMA_RX       "HPMA Rx"
#define D_SENSOR_HPMA_TX       "HPMA Tx"
#define D_SENSOR_SBR_RX        "SerBr Rx"
#define D_SENSOR_SBR_TX        "SerBr Tx"
#define D_SENSOR_SR04_TRIG     "SR04 Tri/TX"
#define D_SENSOR_SR04_ECHO     "SR04 Ech/RX"
#define D_SENSOR_SDM120_TX     "SDMx20 Tx"
#define D_SENSOR_SDM120_RX     "SDMx20 Rx"
#define D_SENSOR_SDM630_TX     "SDM630 Tx"
#define D_SENSOR_SDM630_RX     "SDM630 Rx"
#define D_SENSOR_WE517_TX      "WE517 Tx"
#define D_SENSOR_WE517_RX      "WE517 Rx"
#define D_SENSOR_TM1638_CLK    "TM16 CLK"
#define D_SENSOR_TM1638_DIO    "TM16 DIO"
#define D_SENSOR_TM1638_STB    "TM16 STB"
#define D_SENSOR_HX711_SCK     "HX711 SCK"
#define D_SENSOR_HX711_DAT     "HX711 DAT"
#define D_SENSOR_FTC532        "FTC532"
#define D_SENSOR_TX2X_TX       "TX2x"
#define D_SENSOR_RFSEND        "RFstuurder"
#define D_SENSOR_RFRECV        "RFontvanger"
#define D_SENSOR_TUYA_TX       "Tuya Tx"
#define D_SENSOR_TUYA_RX       "Tuya Rx"
#define D_SENSOR_MGC3130_XFER  "MGC3130 Xfr"
#define D_SENSOR_MGC3130_RESET "MGC3130 Rst"
#define D_SENSOR_SSPI_MISO     "SSPI MISO"
#define D_SENSOR_SSPI_MOSI     "SSPI MOSI"
#define D_SENSOR_SSPI_SCLK     "SSPI SCLK"
#define D_SENSOR_SSPI_CS       "SSPI CS"
#define D_SENSOR_SSPI_DC       "SSPI DC"
#define D_SENSOR_RF_SENSOR     "RF Sensor"
#define D_SENSOR_AZ_RX         "AZ Rx"
#define D_SENSOR_AZ_TX         "AZ Tx"
#define D_SENSOR_MAX31855_CS   "MX31855 CS"
#define D_SENSOR_MAX31855_CLK  "MX31855 CLK"
#define D_SENSOR_MAX31855_DO   "MX31855 DO"
#define D_SENSOR_MAX31865_CS   "MX31865 CS"
#define D_SENSOR_NRG_SEL       "HLWBL SEL"  // Suffix "i"
#define D_SENSOR_NRG_CF1       "HLWBL CF1"
#define D_SENSOR_HLW_CF        "HLW8012 CF"
#define D_SENSOR_HJL_CF        "BL0937 CF"
#define D_SENSOR_MCP39F5_TX    "MCP39F5 Tx"
#define D_SENSOR_MCP39F5_RX    "MCP39F5 Rx"
#define D_SENSOR_MCP39F5_RST   "MCP39F5 Rst"
#define D_SENSOR_CSE7766_TX    "CSE7766 Tx"
#define D_SENSOR_CSE7766_RX    "CSE7766 Rx"
#define D_SENSOR_PN532_TX      "PN532 Tx"
#define D_SENSOR_PN532_RX      "PN532 Rx"
#define D_SENSOR_SM16716_CLK   "SM16716 CLK"
#define D_SENSOR_SM16716_DAT   "SM16716 DAT"
#define D_SENSOR_SM16716_POWER "SM16716 PWR"
#define D_SENSOR_P9813_CLK     "P9813 Clk"
#define D_SENSOR_P9813_DAT     "P9813 Dat"
#define D_SENSOR_MY92X1_DI     "MY92x1 DI"
#define D_SENSOR_MY92X1_DCKI   "MY92x1 DCKI"
#define D_SENSOR_ARIRFRCV      "ALux IrRcv"
#define D_SENSOR_ARIRFSEL      "ALux IrSel"
#define D_SENSOR_TXD           "Serial Tx"
#define D_SENSOR_RXD           "Serial Rx"
#define D_SENSOR_ROTARY        "Draai"     // Suffix "1A"
#define D_SENSOR_HRE_CLOCK     "HRE Klok"
#define D_SENSOR_HRE_DATA      "HRE Data"
#define D_SENSOR_ADE7953_IRQ   "ADE7953 IRQ"
#define D_SENSOR_BUZZER        "Gonser"
#define D_SENSOR_OLED_RESET    "OLED Herstel"
#define D_SENSOR_ZIGBEE_TXD    "Zigbee Tx"
#define D_SENSOR_ZIGBEE_RXD    "Zigbee Rx"
#define D_SENSOR_ZIGBEE_RST    "Zigbee Rst"
#define D_SENSOR_SOLAXX1_TX    "SolaxX1 Tx"
#define D_SENSOR_SOLAXX1_RX    "SolaxX1 Rx"
#define D_SENSOR_IBEACON_TX    "iBeacon TX"
#define D_SENSOR_IBEACON_RX    "iBeacon RX"
#define D_SENSOR_RDM6300_RX    "RDM6300 RX"
#define D_SENSOR_CC1101_CS     "CC1101 CS"
#define D_SENSOR_A4988_DIR     "A4988 DIR"
#define D_SENSOR_A4988_STP     "A4988 STP"
#define D_SENSOR_A4988_ENA     "A4988 ENA"
#define D_SENSOR_A4988_MS1     "A4988 MS1"
#define D_SENSOR_OUTPUT_HI     "Output Hi"
#define D_SENSOR_OUTPUT_LO     "Output Lo"
#define D_SENSOR_AS608_TX      "AS608 Tx"
#define D_SENSOR_AS608_RX      "AS608 Rx"
#define D_SENSOR_DDS2382_TX    "DDS238-2 Tx"
#define D_SENSOR_DDS2382_RX    "DDS238-2 Rx"
#define D_SENSOR_DDSU666_TX    "DDSU666 Tx"
#define D_SENSOR_DDSU666_RX    "DDSU666 Rx"
#define D_SENSOR_SM2135_CLK    "SM2135 Clk"
#define D_SENSOR_SM2135_DAT    "SM2135 Dat"
#define D_SENSOR_DEEPSLEEP     "Diep slaap"
#define D_SENSOR_EXS_ENABLE    "EXS Inskakel"
#define D_SENSOR_CLIENT_TX      "Client TX"
#define D_SENSOR_CLIENT_RX      "Client RX"
#define D_SENSOR_CLIENT_RESET   "Client RST"
#define D_SENSOR_GPS_RX        "GPS RX"
#define D_SENSOR_GPS_TX        "GPS TX"
#define D_SENSOR_HM10_RX       "HM10 RX"
#define D_SENSOR_HM10_TX       "HM10 TX"
#define D_SENSOR_LE01MR_RX     "LE-01MR Rx"
#define D_SENSOR_LE01MR_TX     "LE-01MR Tx"
#define D_SENSOR_BL0940_RX     "BL0940 Rx"
#define D_SENSOR_CC1101_GDO0   "CC1101 GDO0"
#define D_SENSOR_CC1101_GDO2   "CC1101 GDO2"
#define D_SENSOR_HRXL_RX       "HRXL Rx"
#define D_SENSOR_DYP_RX        "DYP Rx"
#define D_SENSOR_ELECTRIQ_MOODL "MOODL Tx"
#define D_SENSOR_AS3935        "AS3935"
#define D_SENSOR_WINDMETER_SPEED "WindMeter Spd"
#define D_SENSOR_TELEINFO_RX   "TInfo Rx"
#define D_SENSOR_TELEINFO_ENABLE "TInfo EN"
#define D_SENSOR_LMT01_PULSE   "LMT01 Pulse"
#define D_SENSOR_ADC_INPUT     "ADC Insette"
#define D_SENSOR_ADC_TEMP      "ADC Temperatuur"
#define D_SENSOR_ADC_LIGHT     "ADC Lig"
#define D_SENSOR_ADC_BUTTON    "ADC Knoppie"
#define D_SENSOR_ADC_RANGE     "ADC Reekse"
#define D_SENSOR_ADC_CT_POWER  "ADC CT Power"
#define D_SENSOR_ADC_JOYSTICK  "ADC Joystick"
#define D_GPIO_WEBCAM_PWDN     "CAM_PWDN"
#define D_GPIO_WEBCAM_RESET    "CAM_RESET"
#define D_GPIO_WEBCAM_XCLK     "CAM_XCLK"
#define D_GPIO_WEBCAM_SIOD     "CAM_SIOD"
#define D_GPIO_WEBCAM_SIOC     "CAM_SIOC"
#define D_GPIO_WEBCAM_DATA     "CAM_DATA"
#define D_GPIO_WEBCAM_VSYNC    "CAM_VSYNC"
#define D_GPIO_WEBCAM_HREF     "CAM_HREF"
#define D_GPIO_WEBCAM_PCLK     "CAM_PCLK"
#define D_GPIO_WEBCAM_PSCLK    "CAM_PSCLK"
#define D_GPIO_WEBCAM_HSD      "CAM_HSD"
#define D_GPIO_WEBCAM_PSRCS    "CAM_PSRCS"
#define D_SENSOR_ETH_PHY_POWER "ETH POWER"
#define D_SENSOR_ETH_PHY_MDC   "ETH MDC"
#define D_SENSOR_ETH_PHY_MDIO  "ETH MDIO"
#define D_SENSOR_TCP_TXD       "TCP Tx"
#define D_SENSOR_TCP_RXD       "TCP Rx"
#define D_SENSOR_IEM3000_TX    "iEM3000 TX"
#define D_SENSOR_IEM3000_RX    "iEM3000 RX"
#define D_SENSOR_MIEL_HVAC_TX  "MiEl HVAC Tx"
#define D_SENSOR_MIEL_HVAC_RX  "MiEl HVAC Rx"
#define D_SENSOR_SHELLY_DIMMER_BOOT0 "SHD Boot 0"
#define D_SENSOR_SHELLY_DIMMER_RST_INV "SHD Herstel"
#define D_SENSOR_RC522_RST     "RC522 Herstel"

// Units
#define D_UNIT_AMPERE "A"
#define D_UNIT_CELSIUS "C"
#define D_UNIT_CENTIMETER "cm"
#define D_UNIT_DEGREE "°"
#define D_UNIT_FAHRENHEIT "F"
#define D_UNIT_HERTZ "Hz"
#define D_UNIT_HOUR "h"
#define D_UNIT_GALLONS "gal"
#define D_UNIT_GALLONS_PER_MIN "g/m"
#define D_UNIT_INCREMENTS "inc"
#define D_UNIT_KELVIN "K"
#define D_UNIT_KILOMETER "km"
#define D_UNIT_KILOGRAM "kg"
#define D_UNIT_KILOMETER_PER_HOUR "km/h"  // or "km/h"
#define D_UNIT_KILOOHM "kΩ"
#define D_UNIT_KILOWATTHOUR "kWh"
#define D_UNIT_LITERS "L"
#define D_UNIT_LITERS_PER_MIN "L/m"
#define D_UNIT_LUX "lx"
#define D_UNIT_MICROGRAM_PER_CUBIC_METER "µg/m³"
#define D_UNIT_MICROMETER "µm"
#define D_UNIT_MICROSECOND "µs"
#define D_UNIT_MICROSIEMENS_PER_CM "µS/cm"
#define D_UNIT_MILLIAMPERE "mA"
#define D_UNIT_MILLILITERS "ml"
#define D_UNIT_MILLIMETER "mm"
#define D_UNIT_MILLIMETER_MERCURY "mmHg"
#define D_UNIT_MILLISECOND "ms"
#define D_UNIT_MILLIVOLT "mV"
#define D_UNIT_MINUTE "Min"
#define D_UNIT_PARTS_PER_BILLION "ppb"
#define D_UNIT_PARTS_PER_DECILITER "ppd"
#define D_UNIT_PARTS_PER_MILLION "ppm"
#define D_UNIT_PERCENT "%%"
#define D_UNIT_PRESSURE "hPa"
#define D_UNIT_SECOND "sec"
#define D_UNIT_SECTORS "sectors"
#define D_UNIT_VA "VA"
#define D_UNIT_VAR "VAr"
#define D_UNIT_VOLT "V"
#define D_UNIT_WATT "W"
#define D_UNIT_WATTHOUR "Wh"
#define D_UNIT_WATT_METER_QUADRAT "W/m²"

//SDM220, SDM120, LE01MR
#define D_PHASE_ANGLE     "Fasehoek"
#define D_IMPORT_ACTIVE   "Invoer aktief"
#define D_EXPORT_ACTIVE   "Uitvoer aktief"
#define D_IMPORT_REACTIVE "Import reaktief"
#define D_EXPORT_REACTIVE "Uitvoer reaktief"
#define D_TOTAL_REACTIVE  "Totaal reaktief"
#define D_UNIT_KWARH      "kVArh"
#define D_UNIT_ANGLE      "Deg"
#define D_TOTAL_ACTIVE    "Totaal aktief"

//SOLAXX1
#define D_PV1_VOLTAGE     "PV1 Spanning"
#define D_PV1_CURRENT     "PV1 Stroom"
#define D_PV1_POWER       "PV1 Krag"
#define D_PV2_VOLTAGE     "PV2 Spanning"
#define D_PV2_CURRENT     "PV2 Stroom"
#define D_PV2_POWER       "PV2 Krag"
#define D_SOLAR_POWER     "Sonkrag"
#define D_INVERTER_POWER  "Omskakelaarkrag"
#define D_STATUS          "Status"
#define D_WAITING         "Wag"
#define D_CHECKING        "Nagaan"
#define D_WORKING         "Werk"
#define D_FAILURE         "Mislukking"
#define D_SOLAX_ERROR_0   "Geen foutkode nie"
#define D_SOLAX_ERROR_1   "Netspanning verlies fout"
#define D_SOLAX_ERROR_2   "Netspanning spanning fout"
#define D_SOLAX_ERROR_3   "Netspanning frekwensie fout"
#define D_SOLAX_ERROR_4   "Pv stroom fout"
#define D_SOLAX_ERROR_5   "Isolasiefout"
#define D_SOLAX_ERROR_6   "Storing by oormatige temperatuur"
#define D_SOLAX_ERROR_7   "Waaierstoring"
#define D_SOLAX_ERROR_8   "Ander toestelfout"

//xdrv_10_scripter.ino
#define D_CONFIGURE_SCRIPT     "Wysig skrif"
#define D_SCRIPT               "wysig skrif"
#define D_SDCARD_UPLOAD        "lêer oplaai"
#define D_SDCARD_DIR           "sd card directory"
#define D_UPL_DONE             "Klaar"
#define D_SCRIPT_CHARS_LEFT    "karakters oor"
#define D_SCRIPT_CHARS_NO_MORE "nie meer karakters nie"
#define D_SCRIPT_DOWNLOAD      "Aflaai"
#define D_SCRIPT_ENABLE        "skrip inskakel"
#define D_SCRIPT_UPLOAD        "Laai op"
#define D_SCRIPT_UPLOAD_FILES  "Laai lêers op"

//xsns_67_as3935.ino
#define D_AS3935_GAIN "versterking:"
#define D_AS3935_ENERGY "energie:"
#define D_AS3935_DISTANCE "afstand:"
#define D_AS3935_DISTURBER "steur:"
#define D_AS3935_VRMS "µVrms:"
#define D_AS3935_APRX "aprx.:"
#define D_AS3935_AWAY "weg"
#define D_AS3935_LIGHT "weerlig"
#define D_AS3935_OUT "weerlig buite bereik"
#define D_AS3935_NOT "afstand nie bepaal nie"
#define D_AS3935_ABOVE "weerlig oorhoofse"
#define D_AS3935_NOISE "geraas bespeur"
#define D_AS3935_DISTDET "steuring bespeur"
#define D_AS3935_INTNOEV "Onderbreek met geen geleentheid nie!"
#define D_AS3935_FLICKER "IRQ Pin flikker!"
#define D_AS3935_POWEROFF "Skakel af"
#define D_AS3935_NOMESS "luister..."
#define D_AS3935_ON "Aan"
#define D_AS3935_OFF "Uit"
#define D_AS3935_INDOORS "Binnenshuis"
#define D_AS3935_OUTDOORS "Buite"
#define D_AS3935_CAL_FAIL "kalibrasie het misluk"
#define D_AS3935_CAL_OK "kalibrasie ingestel op:"

//xsns_68_opentherm.ino
#define D_SENSOR_BOILER_OT_RX   "OpenTherm RX"
#define D_SENSOR_BOILER_OT_TX   "OpenTherm TX"

// xnrg_15_teleinfo Denky (Teleinfo)
#define D_CONTRACT        "Kontrak"
#define D_POWER_LOAD      "Kraglading"
#define D_CURRENT_TARIFF  "Stroom tarief"
#define D_TARIFF          "Tarief"
#define D_OVERLOAD        "ADPS"
#define D_MAX_POWER       "Max krag"
#define D_MAX_CURRENT     "Max stroom"

// xsns_79_as608.ino
#define D_FP_ENROLL_PLACEFINGER "Plaas vinger"
#define D_FP_ENROLL_REMOVEFINGER "Verwyder vinger"
#define D_FP_ENROLL_PLACESAMEFINGER "Plaas dieselfde vinger weer"
#define D_FP_ENROLL_RETRY "Fout, probeer dus weer"
#define D_FP_ENROLL_RESTART "Begin oor"
#define D_FP_ENROLL_ERROR "Fout"
#define D_FP_ENROLL_RESET "Stel terug"
#define D_FP_ENROLL_ACTIVE "Aktief"
#define D_FP_ENROLL_INACTIVE "Onaktief"
// Indexed by Adafruit_Fingerprint.h defines
#define D_FP_PACKETRECIEVEERR "Komms-fout"    // 0x01 Error when receiving data package
#define D_FP_NOFINGER "Geen vinger op die sensor nie" // 0x02 No finger on the sensor
#define D_FP_IMAGEFAIL "Beeldfout"         // 0x03 Failed to enroll the finger
#define D_FP_IMAGEMESS "Beeld te deurmekaar"       // 0x06 Failed to generate character file due to overly disorderly fingerprint image
#define D_FP_FEATUREFAIL "Vingerafdruk te klein" // 0x07 Failed to generate character file due to the lack of character point or small fingerprint image
#define D_FP_NOMATCH "Nie opgewasse"                // 0x08 Finger doesn't match
#define D_FP_NOTFOUND "Kon nie 'n ekwivalent vind nie"   // 0x09 Failed to find matching finger
#define D_FP_ENROLLMISMATCH "Vingerafdruk stem nie ooreen nie" // 0x0A Failed to combine the character files
#define D_FP_BADLOCATION "Slegte ligging"        // 0x0B Addressed PageID is beyond the finger library
#define D_FP_DBRANGEFAIL "DB-reeksfout"      // 0x0C Error when reading template from library or invalid template
#define D_FP_UPLOADFEATUREFAIL "Fout by die oplaai van funksie" // 0x0D Error when uploading template
#define D_FP_PACKETRESPONSEFAIL "Fout van pakketrespons" // 0x0E Module failed to receive the following data packages
#define D_FP_UPLOADFAIL "Oplaaifout"         // 0x0F Error when uploading image
#define D_FP_DELETEFAIL "Vee fout"         // 0x10 Failed to delete the template
#define D_FP_DBCLEARFAIL "DB leegfout"      // 0x11 Failed to clear finger library
#define D_FP_PASSFAIL "Wagwoordfout"         // 0x13 Find whether the fingerprint passed or failed
#define D_FP_INVALIDIMAGE "Prent ongeldig"      // 0x15 Failed to generate image because of lac of valid primary image
#define D_FP_FLASHERR "Flash skryffout"      // 0x18 Error when writing flash
#define D_FP_INVALIDREG "Ongeldige nommer"       // 0x1A Invalid register number
#define D_FP_ADDRCODE "Adres kode"           // 0x20 Address code
#define D_FP_PASSVERIFY "Wagwoord geverifieer"    // 0x21 Verify the fingerprint passed
#define D_FP_UNKNOWNERROR "Fout"              // Any other error

#endif  // _LANGUAGE_AF_AF_H_
