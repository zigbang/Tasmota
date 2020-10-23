/*
  bg-BG.h - localization for Bulgaria - Bulgarian for Tasmota

  Copyright (C) 2020  Theo Arends

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

#ifndef _LANGUAGE_BG_BG_H_
#define _LANGUAGE_BG_BG_H_

/*************************** ATTENTION *******************************\
 *
 * Due to memory constraints only UTF-8 is supported.
 * To save code space keep text as short as possible.
 * Time and Date provided by SDK can not be localized (yet).
 * Use online command StateText to translate ON, OFF, HOLD and TOGGLE.
 * Use online command Prefix to translate cmnd, stat and tele.
 *
 * Updated until v8.2.0.6
\*********************************************************************/

//#define LANGUAGE_MODULE_NAME         // Enable to display "Module Generic" (ie Spanish), Disable to display "Generic Module" (ie English)

#define LANGUAGE_LCID 1026
// HTML (ISO 639-1) Language Code
#define D_HTML_LANGUAGE "bg"

// "2017-03-07T11:08:02" - ISO8601:2004
#define D_YEAR_MONTH_SEPARATOR "-"
#define D_MONTH_DAY_SEPARATOR "-"
#define D_DATE_TIME_SEPARATOR "T"
#define D_HOUR_MINUTE_SEPARATOR ":"
#define D_MINUTE_SECOND_SEPARATOR ":"

#define D_DAY3LIST "НедПонВтрСрдЧетПетСъб"
#define D_MONTH3LIST "ЯнуФевМарАпрМайЮниЮлиАвгСепОктНоеДек"

// Non JSON decimal separator
#define D_DECIMAL_SEPARATOR ","

// Common
#define D_ADMIN "Admin"
#define D_AIR_QUALITY "Качество на въздуха"
#define D_AP "Точка за достъп"                    // Access Point
#define D_AS "като"
#define D_AUTO "АВТОМАТИЧНО"
#define D_BATT "Batt"                // Short for Battery
#define D_BLINK "Мигане вкл."
#define D_BLINKOFF "Мигане изкл."
#define D_BOOT_COUNT "Брой на стартиранията"
#define D_BRIGHTLIGHT "Яркост"
#define D_BSSID "BSSId"
#define D_BUTTON "Бутон"
#define D_BY "от"                    // Written by me
#define D_BYTES "Байта"
#define D_CELSIUS "Целзий"
#define D_CHANNEL "Канал"
#define D_CO2 "Въглероден диоксид"
#define D_CODE "код"                // Button code
#define D_COLDLIGHT "Хладна"
#define D_COMMAND "Команда"
#define D_CONNECTED "Свързан"
#define D_CORS_DOMAIN "CORS домейн"
#define D_COUNT "Брой"
#define D_COUNTER "Брояч"
#define D_CT_POWER "CT Power"
#define D_CURRENT "Ток"          // As in Voltage and Current
#define D_DATA "Данни"
#define D_DARKLIGHT "Тъмна"
#define D_DEBUG "Дебъгване"
#define D_DEWPOINT "Dew point"
#define D_DISABLED "Забранен"
#define D_DISTANCE "Разстояние"
#define D_DNS_SERVER "DNS Сървър"
#define D_DONE "Изпълнено"
#define D_DST_TIME "Лятно време"
#define D_ECO2 "eCO₂"
#define D_EMULATION "Емулация"
#define D_ENABLED "Разрешен"
#define D_ERASE "Изтриване"
#define D_ERROR "Грешка"
#define D_FAHRENHEIT "Фаренхайт"
#define D_FAILED "Неуспешно"
#define D_FALLBACK "Помощен"
#define D_FALLBACK_TOPIC "Помощен топик"
#define D_FALSE "Невярно"
#define D_FILE "Файл"
#define D_FLOW_RATE "Дебит"
#define D_FREE_MEMORY "Свободна памет"
#define D_PSR_MAX_MEMORY "PS-RAM Memory"
#define D_PSR_FREE_MEMORY "PS-RAM free Memory"
#define D_FREQUENCY "Честота"
#define D_GAS "Газ"
#define D_GATEWAY "Шлюз"
#define D_GROUP "Група"
#define D_HOST "Хост"
#define D_HOSTNAME "Име на хоста"
#define D_HUMIDITY "Влажност"
#define D_ILLUMINANCE "Осветеност"
#define D_IMMEDIATE "Моментен"      // Button immediate
#define D_INDEX "Индекс"
#define D_INFO "Информация"
#define D_INFRARED "Инфрачервен"
#define D_INITIALIZED "Инициализирано"
#define D_IP_ADDRESS "IP адрес"
#define D_LIGHT "Светлина"
#define D_LWT "LWT"
#define D_LQI "LQI"                  // Zigbee Link Quality Index
#define D_MODULE "Модул"
#define D_MOISTURE "Влага"
#define D_MQTT "MQTT"
#define D_MULTI_PRESS "неколкократно натискане"
#define D_NOISE "Шум"
#define D_NONE "Няма"
#define D_OFF "Изкл."
#define D_OFFLINE "Офлайн"
#define D_OK "Ок"
#define D_ON "Вкл."
#define D_ONLINE "Онлайн"
#define D_ORP "ORP"
#define D_PASSWORD "Парола"
#define D_PH "pH"
#define D_PORT "Порт"
#define D_POWER_FACTOR "Фактор на мощността"
#define D_POWERUSAGE "Мощност"
#define D_POWERUSAGE_ACTIVE "Активна мощност"
#define D_POWERUSAGE_APPARENT "Пълна мощност"
#define D_POWERUSAGE_REACTIVE "Реактивна мощност"
#define D_PRESSURE "Налягане"
#define D_PRESSUREATSEALEVEL "Налягане при морското ниво"
#define D_PROGRAM_FLASH_SIZE "Размер на флаш паметта за програми"
#define D_PROGRAM_SIZE "Размер на програмата"
#define D_PROJECT "Проект"
#define D_RAIN "Дъжд"
#define D_RANGE "Обхват"
#define D_RECEIVED "Получено"
#define D_RESTART "Рестарт"
#define D_RESTARTING "Рестартиране"
#define D_RESTART_REASON "Причина за рестарта"
#define D_RESTORE "възстановяване"
#define D_RETAINED "запазено"
#define D_RULE "Правило"
#define D_SAVE "Запис"
#define D_SENSOR "Датчик"
#define D_SSID "SSId"
#define D_START "Старт"
#define D_STD_TIME "STD"
#define D_STOP "Стоп"
#define D_SUBNET_MASK "Маска на подмрежата"
#define D_SUBSCRIBE_TO "Записване за"
#define D_UNSUBSCRIBE_FROM "Отписване от"
#define D_SUCCESSFUL "Успешно"
#define D_SUNRISE "Изгрев"
#define D_SUNSET "Залез"
#define D_TEMPERATURE "Температура"
#define D_TO "към"
#define D_TOGGLE "Превключване"
#define D_TOPIC "Топик"
#define D_TOTAL_USAGE "Използвана вода"
#define D_TRANSMIT "Предаване"
#define D_TRUE "Вярно"
#define D_TVOC "TVOC"
#define D_UPGRADE "Обновяване"
#define D_UPLOAD "Зареждане"
#define D_UPTIME "Време от стартирането"
#define D_USER "Потребител"
#define D_UTC_TIME "UTC"
#define D_UV_INDEX "UV индекс"
#define D_UV_INDEX_1 "Нисък"
#define D_UV_INDEX_2 "Среден"
#define D_UV_INDEX_3 "Висок"
#define D_UV_INDEX_4 "Много висок"
#define D_UV_INDEX_5 "Изгаряне 1/2 степен"
#define D_UV_INDEX_6 "Изгаряне 3-та степен"
#define D_UV_INDEX_7 "Извън обхват"
#define D_UV_LEVEL "UV ниво"
#define D_UV_POWER "UV мощност"
#define D_VERSION "Версия"
#define D_VOLTAGE "Напрежение"
#define D_WEIGHT "Тегло"
#define D_WARMLIGHT "Топла"
#define D_WEB_SERVER "Уеб сървър"

// tasmota.ino
#define D_WARNING_MINIMAL_VERSION "ПРЕДУПРЕЖДЕНИЕ Тази версия не поддържа постоянни настройки"
#define D_LEVEL_10 "ниво 1-0"
#define D_LEVEL_01 "ниво 0-1"
#define D_SERIAL_LOGGING_DISABLED "Серийният лог изключен"
#define D_SYSLOG_LOGGING_REENABLED "Системният лог активиран"

#define D_SET_BAUDRATE_TO "Задаване скорост на предаване (Baudrate)"
#define D_RECEIVED_TOPIC "Получен топик"
#define D_DATA_SIZE "Размер на данните"
#define D_ANALOG_INPUT "Аналогов вход"

// support.ino
#define D_OSWATCH "osWatch"
#define D_BLOCKED_LOOP "Блокиран цикъл"
#define D_WPS_FAILED_WITH_STATUS "WPS конфигурацията е НЕУСПЕШНА със статус"
#define D_ACTIVE_FOR_3_MINUTES "активно в течение на 3 минути"
#define D_FAILED_TO_START "Неуспешно стартиране"
#define D_PATCH_ISSUE_2186 "Проблем с патч 2186"
#define D_CONNECTING_TO_AP "Свързване към точка за достъп"
#define D_IN_MODE "в режим"
#define D_CONNECT_FAILED_NO_IP_ADDRESS "Грешка при свързването, не е получен IP адрес"
#define D_CONNECT_FAILED_AP_NOT_REACHED "Грешка при свързването, точката за достъп е недостижима"
#define D_CONNECT_FAILED_WRONG_PASSWORD "Грешка при свързването"
#define D_CONNECT_FAILED_AP_TIMEOUT "Грешка при свързването, превишено време за изчакване"
#define D_ATTEMPTING_CONNECTION "Опитва свързване..."
#define D_CHECKING_CONNECTION "Проверка на свързването..."
#define D_QUERY_DONE "Запитването е изпълнено. Намерена е услуга MQTT"
#define D_MQTT_SERVICE_FOUND "MQTT услуга е намерена на"
#define D_FOUND_AT "намерена в"
#define D_SYSLOG_HOST_NOT_FOUND "Хостът на системния лог не е намерен"

// settings.ino
#define D_SAVED_TO_FLASH_AT "Запазено във флаш паметта на"
#define D_LOADED_FROM_FLASH_AT "Заредено от флаш паметта от"
#define D_USE_DEFAULTS "Използване на параметри по подразбиране"
#define D_ERASED_SECTOR "Изтрит сектор"

// xdrv_02_webserver.ino
#define D_NOSCRIPT "Разрешете JavaScript, за да използвате Tasmota"
#define D_MINIMAL_FIRMWARE_PLEASE_UPGRADE "Минимален фърмуер<br>моля надградете го"
#define D_WEBSERVER_ACTIVE_ON "Уеб сървърът е активен на"
#define D_WITH_IP_ADDRESS "с IP адрес"
#define D_WEBSERVER_STOPPED "Уеб сървърът е спрян"
#define D_FILE_NOT_FOUND "Файлът не е намерен"
#define D_REDIRECTED "Пренасочено към адаптивния портал"
#define D_WIFIMANAGER_SET_ACCESSPOINT_AND_STATION "Wifi мениджърът настройва точка за достъп и запомня станцията"
#define D_WIFIMANAGER_SET_ACCESSPOINT "Wifi мениджърът настрои точката за достъп"
#define D_TRYING_TO_CONNECT "Опит за свързване на устройството към мрежата"

#define D_RESTART_IN "Рестарт след"
#define D_SECONDS "секунди"
#define D_DEVICE_WILL_RESTART "Устройството ще се рестартира след няколко секунди"
#define D_BUTTON_TOGGLE "Превключване"
#define D_CONFIGURATION "Конфигурация"
#define D_INFORMATION "Информация"
#define D_FIRMWARE_UPGRADE "Обновяване на фърмуера"
#define D_CONSOLE "Конзола"
#define D_CONFIRM_RESTART "Потвърдете рестартирането"

#define D_CONFIGURE_MODULE "Конфигурация на модула"
#define D_CONFIGURE_WIFI "Конфигурация на WiFi"
#define D_CONFIGURE_MQTT "Конфигурация на MQTT"
#define D_CONFIGURE_DOMOTICZ "Конфигурация на Domoticz"
#define D_CONFIGURE_LOGGING "Конфигурация на лога"
#define D_CONFIGURE_OTHER "Други конфигурации"
#define D_CONFIRM_RESET_CONFIGURATION "Потвърдете изчистването"
#define D_RESET_CONFIGURATION "Изчистване на конфигурацията"
#define D_BACKUP_CONFIGURATION "Запазване на конфигурацията"
#define D_RESTORE_CONFIGURATION "Възстановяване на конфигурацията"
#define D_MAIN_MENU "Основно меню"

#define D_MODULE_PARAMETERS "Параметри на модула"
#define D_MODULE_TYPE "Тип на модула"
#define D_PULLUP_ENABLE "Без pull-up за бутон/ключ"
#define D_ADC "ADC"
#define D_GPIO "GPIO"
#define D_SERIAL_IN "Сериен вход"
#define D_SERIAL_OUT "Сериен изход"

#define D_WIFI_PARAMETERS "Wifi параметри"
#define D_SCAN_FOR_WIFI_NETWORKS "Сканиране за безжични мрежи"
#define D_SCAN_DONE "Сканирането приключи"
#define D_NO_NETWORKS_FOUND "Не бяха открити мрежи"
#define D_REFRESH_TO_SCAN_AGAIN "Обновяване за повторно сканиране"
#define D_DUPLICATE_ACCESSPOINT "Дублиране на точката за достъп (AP)"
#define D_SKIPPING_LOW_QUALITY "Пропускане поради лошо качество"
#define D_RSSI "RSSI"
#define D_WEP "WEP"
#define D_WPA_PSK "WPA PSK"
#define D_WPA2_PSK "WPA2 PSK"
#define D_AP1_SSID "AP1 SSId"
#define D_AP1_PASSWORD "AP1 Парола"
#define D_AP2_SSID "AP2 SSId"
#define D_AP2_PASSWORD "AP2 Парола"

#define D_MQTT_PARAMETERS "Параметри на MQTT"
#define D_CLIENT "Клиент"
#define D_FULL_TOPIC "Пълен топик"

#define D_LOGGING_PARAMETERS "Параметри на лога"
#define D_SERIAL_LOG_LEVEL "Степен на серийния лог"
#define D_MQTT_LOG_LEVEL "Степен на MQTT лога"
#define D_WEB_LOG_LEVEL "Степен на уеб лога"
#define D_SYS_LOG_LEVEL "Степен на системния лог"
#define D_MORE_DEBUG "Допълнителна debug информация"
#define D_SYSLOG_HOST "Хост на системния лог"
#define D_SYSLOG_PORT "Порт на системния лог"
#define D_TELEMETRY_PERIOD "Период на телеметрия"

#define D_OTHER_PARAMETERS "Други параметри"
#define D_TEMPLATE "Модел"
#define D_ACTIVATE "Активирай"
#define D_DEVICE_NAME "Device Name"
#define D_WEB_ADMIN_PASSWORD "Парола на уеб администратора"
#define D_MQTT_ENABLE "Активиране на MQTT"
#define D_MQTT_TLS_ENABLE "MQTT TLS"
#define D_FRIENDLY_NAME "Приятелско име"
#define D_BELKIN_WEMO "Belkin WeMo"
#define D_HUE_BRIDGE "Hue Bridge"
#define D_SINGLE_DEVICE "Единично"
#define D_MULTI_DEVICE "Мулти"

#define D_CONFIGURE_TEMPLATE "Конфигуриране на шаблон"
#define D_TEMPLATE_PARAMETERS "Параметри на шаблона"
#define D_TEMPLATE_NAME "Име"
#define D_BASE_TYPE "Базиран на"
#define D_TEMPLATE_FLAGS "Опции"

#define D_SAVE_CONFIGURATION "Запазване на конфигурацията"
#define D_CONFIGURATION_SAVED "Конфигурацията е запазена"
#define D_CONFIGURATION_RESET "Конфигурацията е изчистена"

#define D_PROGRAM_VERSION "Версия на програмата"
#define D_BUILD_DATE_AND_TIME "Дата и час на компилацията"
#define D_CORE_AND_SDK_VERSION "Версия на Core/SDK"
#define D_FLASH_WRITE_COUNT "Брой на записите във флаш паметта"
#define D_MAC_ADDRESS "MAC адрес"
#define D_MQTT_HOST "MQTT хост"
#define D_MQTT_PORT "MQTT порт"
#define D_MQTT_CLIENT "MQTT ID на клиент"
#define D_MQTT_USER "MQTT потребител"
#define D_MQTT_TOPIC "MQTT топик"
#define D_MQTT_GROUP_TOPIC "MQTT групов топик"
#define D_MQTT_FULL_TOPIC "MQTT пълен топик"
#define D_MQTT_NO_RETAIN "MQTT No Retain"
#define D_MDNS_DISCOVERY "mDNS откриване"
#define D_MDNS_ADVERTISE "mDNS известяване"
#define D_ESP_CHIP_ID "ID на ESP чипа"
#define D_FLASH_CHIP_ID "ID на чипа на флаш паметта"
#define D_FLASH_CHIP_SIZE "Размер на флаш паметта"
#define D_FREE_PROGRAM_SPACE "Свободно пространство за програми"

#define D_UPGRADE_BY_WEBSERVER "Обновяване чрез уеб сървър"
#define D_OTA_URL "OTA Url"
#define D_START_UPGRADE "Започване на обновяване"
#define D_UPGRADE_BY_FILE_UPLOAD "Обновяване чрез зареждане на файл"
#define D_UPLOAD_STARTED "Зареждането започна"
#define D_UPGRADE_STARTED "Обновяването започна"
#define D_UPLOAD_DONE "Зареждането завърши"
#define D_UPLOAD_TRANSFER "Upload transfer"
#define D_TRANSFER_STARTED "Transfer started"
#define D_UPLOAD_ERR_1 "Не е избран файл"
#define D_UPLOAD_ERR_2 "Недостатъчно свободно място"
#define D_UPLOAD_ERR_3 "Magic байтът не е 0xE9"
#define D_UPLOAD_ERR_4 "Размерът на програмата е по-голям от реалния размер на флаш паметта"
#define D_UPLOAD_ERR_5 "Грешка при зареждането в буфера"
#define D_UPLOAD_ERR_6 "Грешка при зареждането. Включено е ниво 3 на лога"
#define D_UPLOAD_ERR_7 "Зареждането е прекъснато"
#define D_UPLOAD_ERR_8 "Файлът е невалиден"
#define D_UPLOAD_ERR_9 "Файлът е прекалено голям"
#define D_UPLOAD_ERR_10 "Грешка при инициализация на RF чипа"
#define D_UPLOAD_ERR_11 "Грешка при изтриване на RF чипа"
#define D_UPLOAD_ERR_12 "Грешка при записване в RF чипа"
#define D_UPLOAD_ERR_13 "Грешка при декодиране на RF фърмуера"
#define D_UPLOAD_ERR_14 "Несъвместим"
#define D_UPLOAD_ERROR_CODE "Код на грешка при зареждането"

#define D_ENTER_COMMAND "Въвеждане на команда"
#define D_ENABLE_WEBLOG_FOR_RESPONSE "Включете ниво 2 на лога, ако очаквате отговор"
#define D_NEED_USER_AND_PASSWORD "Очаква user=<username>&password=<password>"

// xdrv_01_mqtt.ino
#define D_FINGERPRINT "Проверка на TLS отпечатък..."
#define D_TLS_CONNECT_FAILED_TO "Неуспешно TLS свързване към"
#define D_RETRY_IN "Повтори след"
#define D_VERIFIED "Проверен отпечтък"
#define D_INSECURE "Нешифрована връзка, недействителен отпечатък"
#define D_CONNECT_FAILED_TO "Грешка при свързването към"

// xplg_wemohue.ino
#define D_MULTICAST_DISABLED "Multicast е изключен"
#define D_MULTICAST_REJOINED "Multicast е повторно съединен"
#define D_MULTICAST_JOIN_FAILED "Multicast грешка при присъединяването"
#define D_FAILED_TO_SEND_RESPONSE "Неуспех при изпращането на отговор"

#define D_WEMO "WeMo"
#define D_WEMO_BASIC_EVENT "WeMo главно събитие"
#define D_WEMO_EVENT_SERVICE "WeMo услуга за събитията"
#define D_WEMO_META_SERVICE "WeMo мета-услуга"
#define D_WEMO_SETUP "WeMo настройка"
#define D_RESPONSE_SENT "Отговорът е изпратен"

#define D_HUE "Hue"
#define D_HUE_BRIDGE_SETUP "Настройка на Hue bridge"
#define D_HUE_API_NOT_IMPLEMENTED "Hue API не е внедрен"
#define D_HUE_API "Hue API"
#define D_HUE_POST_ARGS "Hue POST аргументи"
#define D_3_RESPONSE_PACKETS_SENT "Изпратени са 3 пакета за отговор"

// xdrv_07_domoticz.ino
#define D_DOMOTICZ_PARAMETERS "Domoticz параметри"
#define D_DOMOTICZ_IDX "Idx"
#define D_DOMOTICZ_KEY_IDX "Key idx"
#define D_DOMOTICZ_SWITCH_IDX "Switch idx"
#define D_DOMOTICZ_SENSOR_IDX "Sensor idx"
  #define D_DOMOTICZ_TEMP "Temp"
  #define D_DOMOTICZ_TEMP_HUM "Temp,Hum"
  #define D_DOMOTICZ_TEMP_HUM_BARO "Temp,Hum,Baro"
  #define D_DOMOTICZ_POWER_ENERGY "Мощност,Енергия"
  #define D_DOMOTICZ_ILLUMINANCE "Осветеност"
  #define D_DOMOTICZ_COUNT "Брояч/PM1"
  #define D_DOMOTICZ_VOLTAGE "Напрежение/PM2,5"
  #define D_DOMOTICZ_CURRENT "Ток/PM10"
  #define D_DOMOTICZ_AIRQUALITY "Качество на въздуха"
  #define D_DOMOTICZ_P1_SMART_METER "P1SmartMeter"
#define D_DOMOTICZ_UPDATE_TIMER "Период на опресняване"

// xdrv_09_timers.ino
#define D_CONFIGURE_TIMER "Конфигуриране на таймер"
#define D_TIMER_PARAMETERS "Параметри на таймера"
#define D_TIMER_ENABLE "Активиране на таймера"
#define D_TIMER_ARM "Активиран"
#define D_TIMER_TIME "Време"
#define D_TIMER_DAYS "Дни"
#define D_TIMER_REPEAT "Повтори"
#define D_TIMER_OUTPUT "Изход"
#define D_TIMER_ACTION "Действие"

// xdrv_10_knx.ino
#define D_CONFIGURE_KNX "Конфигуриране на KNX"
#define D_KNX_PARAMETERS "KNX параметри"
#define D_KNX_GENERAL_CONFIG "Основни"
#define D_KNX_PHYSICAL_ADDRESS "Физически адрес"
#define D_KNX_PHYSICAL_ADDRESS_NOTE "( Трябва да е уникален в KNX мрежата )"
#define D_KNX_ENABLE "Активиране на KNX"
#define D_KNX_GROUP_ADDRESS_TO_WRITE "Групови адреси за изпращане на данни"
#define D_ADD "Добаване"
#define D_DELETE "Изтриване"
#define D_REPLY "Отговор"
#define D_KNX_GROUP_ADDRESS_TO_READ "Групови адреси за получаване на данни"
#define D_RECEIVED_FROM "Получен от"
#define D_KNX_COMMAND_WRITE "Писане"
#define D_KNX_COMMAND_READ "Четене"
#define D_KNX_COMMAND_OTHER "Друго"
#define D_SENT_TO "изпратен до"
#define D_KNX_WARNING "Груповият адрес (0/0/0) е резервиран и не може да бъде използван."
#define D_KNX_ENHANCEMENT "Подобрена комуникация"
#define D_KNX_TX_SLOT "KNX TX"
#define D_KNX_RX_SLOT "KNX RX"
#define D_KNX_TX_SCENE "KNX SCENE TX"
#define D_KNX_RX_SCENE "KNX SCENE RX"

// xdrv_03_energy.ino
#define D_ENERGY_TODAY "Използвана енергия днес"
#define D_ENERGY_YESTERDAY "Използвана енергия вчера"
#define D_ENERGY_TOTAL "Използвана енергия общо"

// xdrv_27_shutter.ino
#define D_OPEN "Отворена"
#define D_CLOSE "Затворена"
#define D_DOMOTICZ_SHUTTER "Щора"

// xdrv_28_pcf8574.ino
#define D_CONFIGURE_PCF8574 "Конфигуриране на PCF8574"
#define D_PCF8574_PARAMETERS "PCF8574 параметри"
#define D_INVERT_PORTS "Обърни портовете"
#define D_DEVICE "Устройство"
#define D_DEVICE_INPUT "Вход"
#define D_DEVICE_OUTPUT "Изход"

// xsns_05_ds18b20.ino
#define D_SENSOR_BUSY "Датчикът DS18x20 е зает"
#define D_SENSOR_CRC_ERROR "Датчик DS18x20 - грешка CRC"
#define D_SENSORS_FOUND "Намерен е датчик DS18x20"

// xsns_06_dht.ino
#define D_TIMEOUT_WAITING_FOR "Изтекло време за очакване на"
#define D_START_SIGNAL_LOW "Нисък стартов сигнал"
#define D_START_SIGNAL_HIGH "Висок стартов сигнал"
#define D_PULSE "Импулс"
#define D_CHECKSUM_FAILURE "Грешка в контролната сума"

// xsns_07_sht1x.ino
#define D_SENSOR_DID_NOT_ACK_COMMAND "Датчикът не прие команда ACK"
#define D_SHT1X_FOUND "Намерен е SHT1X"

// xsns_18_pms5003.ino
#define D_STANDARD_CONCENTRATION "CF-1 PM"     // Standard Particle CF-1 Particle Matter
#define D_ENVIRONMENTAL_CONCENTRATION "PM"     // Environmetal Particle Matter
#define D_PARTICALS_BEYOND "Частици"

// xsns_27_apds9960.ino
#define D_GESTURE "Жест"
#define D_COLOR_RED "Червен"
#define D_COLOR_GREEN "Зелен"
#define D_COLOR_BLUE "Син"
#define D_CCT "CCT"
#define D_PROXIMITY "Близост"

// xsns_32_mpu6050.ino
#define D_AX_AXIS "Ускорение - ос X"
#define D_AY_AXIS "Ускорение - ос Y"
#define D_AZ_AXIS "Ускорение - ос Z"
#define D_GX_AXIS "Жироскоп - ос X"
#define D_GY_AXIS "Жироскоп - ос Y"
#define D_GZ_AXIS "Жироскоп - ос Z"

// xsns_34_hx711.ino
#define D_HX_CAL_REMOVE "Премахване на тегло"
#define D_HX_CAL_REFERENCE "Зареждане на референтно тегло"
#define D_HX_CAL_DONE "Калибриран"
#define D_HX_CAL_FAIL "Неуспешно калибриране"
#define D_RESET_HX711 "Нулиране на кантара"
#define D_CONFIGURE_HX711 "Конфигуриране на кантара"
#define D_HX711_PARAMETERS "Параметри на кантара"
#define D_ITEM_WEIGHT "Тегло"
#define D_REFERENCE_WEIGHT "Референтно тегло"
#define D_CALIBRATE "Калибриране"
#define D_CALIBRATION "Калибровка"

//xsns_35_tx20.ino
#define D_TX20_WIND_DIRECTION "Посока на вятъра"
#define D_TX20_WIND_SPEED "Скорост на вятъра"
#define D_TX20_WIND_SPEED_MIN "Мин. скорост на вятъра"
#define D_TX20_WIND_SPEED_MAX "Макс. скорост на вятъра"
#define D_TX20_NORTH "С"
#define D_TX20_EAST "И"
#define D_TX20_SOUTH "Ю"
#define D_TX20_WEST "З"

// xsns_53_sml.ino
#define D_TPWRIN "Общо енергия - IN"
#define D_TPWROUT "Общо енергия - OUT"
#define D_TPWRCURR "Активна мощност - In/Out"
#define D_TPWRCURR1 "Активна мощност - In p1"
#define D_TPWRCURR2 "Активна мощност - In p2"
#define D_TPWRCURR3 "Активна мощност - In p3"
#define D_Strom_L1 "Ток L1"
#define D_Strom_L2 "Ток L2"
#define D_Strom_L3 "Ток L3"
#define D_Spannung_L1 "Напрежение L1"
#define D_Spannung_L2 "Напрежение L2"
#define D_Spannung_L3 "Напрежение L3"
#define D_METERNR "Номер_електромер"
#define D_METERSID "ID на услугата"
#define D_GasIN "Брояч"
#define D_H2oIN "Брояч"
#define D_StL1L2L3 "Ток L1+L2+L3"
#define D_SpL1L2L3 "Напрежение L1+L2+L3/3"

// tasmota_template.h - keep them as short as possible to be able to fit them in GUI drop down box
#define D_SENSOR_NONE          "Няма"
#define D_SENSOR_USER          "Потребит."
#define D_SENSOR_DHT11         "DHT11"
#define D_SENSOR_AM2301        "AM2301"
#define D_SENSOR_SI7021        "SI7021"
#define D_SENSOR_DS18X20       "DS18x20"
#define D_SENSOR_I2C_SCL       "I2C SCL"
#define D_SENSOR_I2C_SDA       "I2C SDA"
#define D_SENSOR_WS2812        "WS2812"
#define D_SENSOR_DFR562        "MP3 плейър"
#define D_SENSOR_IRSEND        "IRsend"
#define D_SENSOR_SWITCH        "Ключ"       // Suffix "1"
#define D_SENSOR_BUTTON        "Бутон"      // Suffix "1"
#define D_SENSOR_RELAY         "Реле"       // Suffix "1i"
#define D_SENSOR_LED           "Led"        // Suffix "1i"
#define D_SENSOR_LED_LINK      "LedLink"    // Suffix "i"
#define D_SENSOR_PWM           "PWM"        // Suffix "1"
#define D_SENSOR_COUNTER       "Брояч"      // Suffix "1"
#define D_SENSOR_IRRECV        "IRrecv"
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
#define D_SENSOR_BACKLIGHT     "Подсветка"
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
#define D_SENSOR_TX2X_TX       "TX2x"
#define D_SENSOR_RFSEND        "RFSend"
#define D_SENSOR_RFRECV        "RFrecv"
#define D_SENSOR_TUYA_TX       "Tuya Tx"
#define D_SENSOR_TUYA_RX       "Tuya Rx"
#define D_SENSOR_MGC3130_XFER  "MGC3130 Xfr"
#define D_SENSOR_MGC3130_RESET "MGC3130 Rst"
#define D_SENSOR_SSPI_MISO     "SSPI MISO"
#define D_SENSOR_SSPI_MOSI     "SSPI MOSI"
#define D_SENSOR_SSPI_SCLK     "SSPI SCLK"
#define D_SENSOR_SSPI_CS       "SSPI CS"
#define D_SENSOR_SSPI_DC       "SSPI DC"
#define D_SENSOR_RF_SENSOR     "RF датчик"
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
#define D_SENSOR_MY92X1_DI     "MY92x1 DI"
#define D_SENSOR_MY92X1_DCKI   "MY92x1 DCKI"
#define D_SENSOR_ARIRFRCV      "ALux IrRcv"
#define D_SENSOR_ARIRFSEL      "ALux IrSel"
#define D_SENSOR_TXD           "Serial Tx"
#define D_SENSOR_RXD           "Serial Rx"
#define D_SENSOR_ROTARY        "Rotary"     // Suffix "1A"
#define D_SENSOR_HRE_CLOCK     "HRE Clock"
#define D_SENSOR_HRE_DATA      "HRE Data"
#define D_SENSOR_ADE7953_IRQ   "ADE7953 IRQ"
#define D_SENSOR_BUZZER        "Зумер"
#define D_SENSOR_OLED_RESET    "Нулиране OLED"
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
#define D_SENSOR_DDS2382_TX    "DDS238-2 Tx"
#define D_SENSOR_DDS2382_RX    "DDS238-2 Rx"
#define D_SENSOR_DDSU666_TX    "DDSU666 Tx"
#define D_SENSOR_DDSU666_RX    "DDSU666 Rx"
#define D_SENSOR_SM2135_CLK    "SM2135 Clk"
#define D_SENSOR_SM2135_DAT    "SM2135 Dat"
#define D_SENSOR_DEEPSLEEP     "DeepSleep"
#define D_SENSOR_EXS_ENABLE    "EXS Enable"
#define D_SENSOR_CLIENT_TX    "Client TX"
#define D_SENSOR_CLIENT_RX    "Client RX"
#define D_SENSOR_CLIENT_RESET "Client RST"
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
#define D_SENSOR_ADC_INPUT     "ADC Input"
#define D_SENSOR_ADC_TEMP      "ADC Temp"
#define D_SENSOR_ADC_LIGHT     "ADC Light"
#define D_SENSOR_ADC_BUTTON    "ADC Button"
#define D_SENSOR_ADC_RANGE     "ADC Range"
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

// Units
#define D_UNIT_AMPERE "A"
#define D_UNIT_CELSIUS "C"
#define D_UNIT_CENTIMETER "cm"
#define D_UNIT_DEGREE "°"
#define D_UNIT_FAHRENHEIT "F"
#define D_UNIT_HERTZ "Hz"
#define D_UNIT_HOUR "h"
#define D_UNIT_GALLONS "gal"
#define D_UNIT_GALLONS_PER_MIN "gal/min"
#define D_UNIT_KILOGRAM "kg"
#define D_UNIT_INCREMENTS "inc"
#define D_UNIT_KELVIN "K"
#define D_UNIT_KILOMETER "km"
#define D_UNIT_KILOMETER_PER_HOUR "km/h"
#define D_UNIT_KILOOHM "kΩ"
#define D_UNIT_KILOWATTHOUR "kWh"
#define D_UNIT_LUX "lx"
#define D_UNIT_MICROGRAM_PER_CUBIC_METER "µg/m³"
#define D_UNIT_MICROMETER "µm"
#define D_UNIT_MICROSECOND "µs"
#define D_UNIT_MILLIAMPERE "mA"
#define D_UNIT_MILLIMETER "mm"
#define D_UNIT_MILLIMETER_MERCURY "mmHg"
#define D_UNIT_MILLISECOND "ms"
#define D_UNIT_MILLIVOLT "mV"
#define D_UNIT_MINUTE "min"
#define D_UNIT_PARTS_PER_BILLION "ppb"
#define D_UNIT_PARTS_PER_DECILITER "ppd"
#define D_UNIT_PARTS_PER_MILLION "ppm"
#define D_UNIT_PERCENT "%%"
#define D_UNIT_PRESSURE "hPa"
#define D_UNIT_SECOND "s"
#define D_UNIT_SECTORS "сектори"
#define D_UNIT_VA "VA"
#define D_UNIT_VAR "VAr"
#define D_UNIT_VOLT "V"
#define D_UNIT_WATT "W"
#define D_UNIT_WATTHOUR "Wh"
#define D_UNIT_WATT_METER_QUADRAT "W/m²"

//SDM220, SDM120, LE01MR
#define D_PHASE_ANGLE     "Фазов ъгъл"
#define D_IMPORT_ACTIVE   "Входна активна мощност"
#define D_EXPORT_ACTIVE   "Изходна активна мощност"
#define D_IMPORT_REACTIVE "Входна реактивна мощност"
#define D_EXPORT_REACTIVE "Изходна реактивна мощност"
#define D_TOTAL_REACTIVE  "Общо реактивна мощност"
#define D_UNIT_KWARH      "kVArh"
#define D_UNIT_ANGLE      "°"
#define D_TOTAL_ACTIVE    "Общо активна мощност"

//SOLAXX1
#define D_PV1_VOLTAGE     "Напрежение на PV1"
#define D_PV1_CURRENT     "Ток на PV1"
#define D_PV1_POWER       "Мощност на PV1"
#define D_PV2_VOLTAGE     "Напрежение на PV2"
#define D_PV2_CURRENT     "Ток на PV2"
#define D_PV2_POWER       "Мощност на PV2"
#define D_SOLAR_POWER     "Слънчева мощност"
#define D_INVERTER_POWER  "Мощност на инвертера"
#define D_STATUS          "Състояние"
#define D_WAITING         "Очакване"
#define D_CHECKING        "Проверка"
#define D_WORKING         "Работи"
#define D_FAILURE         "Грешка"
#define D_SOLAX_ERROR_0   "Грешка - няма код"
#define D_SOLAX_ERROR_1   "Грешка - загуба на мрежата"
#define D_SOLAX_ERROR_2   "Грешка - мрежово напрежение"
#define D_SOLAX_ERROR_3   "Грешка - мрежова честота"
#define D_SOLAX_ERROR_4   "Грешка - напрежение на Pv"
#define D_SOLAX_ERROR_5   "Грешка - проблем с изолацията"
#define D_SOLAX_ERROR_6   "Грешка - прегряване"
#define D_SOLAX_ERROR_7   "Грешка - вентилатор"
#define D_SOLAX_ERROR_8   "Грешка - друго оборудване"

//xdrv_10_scripter.ino
#define D_CONFIGURE_SCRIPT     "Редакция на скрипт"
#define D_SCRIPT               "редактирай скрипт"
#define D_SDCARD_UPLOAD        "изпрати файл"
#define D_SDCARD_DIR           "директория на SD картата"
#define D_UPL_DONE             "Готово"
#define D_SCRIPT_CHARS_LEFT    "оставащи символи"
#define D_SCRIPT_CHARS_NO_MORE "няма повече символи"
#define D_SCRIPT_DOWNLOAD      "Изтегляне"
#define D_SCRIPT_ENABLE        "активирай скрипт"
#define D_SCRIPT_UPLOAD        "Изпращане"
#define D_SCRIPT_UPLOAD_FILES  "Изпращане на файлове"

//xsns_67_as3935.ino
#define D_AS3935_GAIN "усилване:"
#define D_AS3935_ENERGY "енергия:"
#define D_AS3935_DISTANCE "разстояние:"
#define D_AS3935_DISTURBER "смущение:"
#define D_AS3935_VRMS "µVrms:"
#define D_AS3935_APRX "прибл.:"
#define D_AS3935_AWAY "далече"
#define D_AS3935_LIGHT "осветление"
#define D_AS3935_OUT "осветление извън обхват"
#define D_AS3935_NOT "неопределено разстояние"
#define D_AS3935_ABOVE "околно осветление"
#define D_AS3935_NOISE "открит шум"
#define D_AS3935_DISTDET "открито смущение"
#define D_AS3935_INTNOEV "Прекъсване без Събитие!"
#define D_AS3935_FLICKER "IRQ flicker!"
#define D_AS3935_POWEROFF "Power Off"
#define D_AS3935_NOMESS "слушане..."
#define D_AS3935_ON "Вкл."
#define D_AS3935_OFF "Изкл."
#define D_AS3935_INDOORS "На закрито"
#define D_AS3935_OUTDOORS "На открито"
#define D_AS3935_CAL_FAIL "калибрирането е неуспешно"
#define D_AS3935_CAL_OK "калибрирането е зададено на:"

//xsns_68_opentherm.ino
#define D_SENSOR_BOILER_OT_RX   "OpenTherm RX"
#define D_SENSOR_BOILER_OT_TX   "OpenTherm TX"

// xnrg_15_teleinfo Denky (Teleinfo)
#define D_CONTRACT        "Contract"
#define D_POWER_LOAD      "Power load"
#define D_CURRENT_TARIFF  "Current Tariff"
#define D_TARIFF          "Tariff"
#define D_OVERLOAD        "ADPS"
#define D_MAX_POWER       "Max Power"
#define D_MAX_CURRENT     "Max Current"

#endif  // _LANGUAGE_BG_BG_H_
