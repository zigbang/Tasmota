/*
  ko-KO.h - localization for Korean - Korean for Tasmota

  Copyright (C) 2020  Theo Arends (translated by NyaamZ)

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

#ifndef _LANGUAGE_KO_KO_H_
#define _LANGUAGE_KO_KO_H_

/*************************** ATTENTION *******************************\
 *
 * Due to memory constraints only UTF-8 is supported.
 * To save code space keep text as short as possible.
 * Time and Date provided by SDK can not be localized (yet).
 * Use online command StateText to translate ON, OFF, HOLD and TOGGLE.
 * Use online command Prefix to translate cmnd, stat and tele.
 *
 * Updated until v6.2.1.11
\*********************************************************************/

//#define LANGUAGE_MODULE_NAME         // Enable to display "Module Generic" (ie Spanish), Disable to display "Generic Module" (ie English)
// https://www.science.co.il/language/Locale-codes.php
#define LANGUAGE_LCID 1042
// HTML (ISO 639-1) Language Code
#define D_HTML_LANGUAGE "ko"

// "2017-03-07T11:08:02" - ISO8601:2004
#define D_YEAR_MONTH_SEPARATOR "-"
#define D_MONTH_DAY_SEPARATOR "-"
#define D_DATE_TIME_SEPARATOR "T"
#define D_HOUR_MINUTE_SEPARATOR ":"
#define D_MINUTE_SECOND_SEPARATOR ":"

#define D_DAY3LIST "일  월  화  수  목  금  토  "
#define D_MONTH3LIST "1월 2월 3월 4월 5월 6월 7월 8월 9월 10월11월12월"

// Non JSON decimal separator
#define D_DECIMAL_SEPARATOR "."

// Common
#define D_ADMIN "Admin"
#define D_AIR_QUALITY "공기질"
#define D_AP "AP"                    // Access Point
#define D_AS "as"
#define D_AUTO "자동"
#define D_BATT "Batt"                // Short for Battery
#define D_BLINK "깜박임"
#define D_BLINKOFF "깜박임 끄기"
#define D_BOOT_COUNT "부팅 횟수"
#define D_BRIGHTLIGHT "밝게"
#define D_BSSID "BSSId"
#define D_BUTTON "버튼"
#define D_BY "by"                    // Written by me
#define D_BYTES "Bytes"
#define D_CELSIUS "섭씨"
#define D_CHANNEL "채널"
#define D_CO2 "이산화탄소"
#define D_CODE "code"                // Button code
#define D_COLDLIGHT "차갑게"
#define D_COMMAND "커맨드"
#define D_CONNECTED "연결됨"
#define D_COUNT "횟수"
#define D_CORS_DOMAIN "CORS Domain"
#define D_COUNTER "Counter"
#define D_CT_POWER "CT Power"
#define D_CURRENT "전류"          // As in Voltage and Current
#define D_DATA "Data"
#define D_DARKLIGHT "어둡게"
#define D_DEBUG "디버그"
#define D_DEWPOINT "Dew point"
#define D_DISABLED "사용안함"
#define D_DISTANCE "거리"
#define D_DNS_SERVER "DNS 서버"
#define D_DONE "완료"
#define D_DST_TIME "DST"
#define D_ECO2 "eCO₂"
#define D_EMULATION "에뮬레이션"
#define D_ENABLED "사용"
#define D_ERASE "삭제"
#define D_ERROR "에러"
#define D_FAHRENHEIT "화씨"
#define D_FAILED "실패"
#define D_FALLBACK "Fallback"
#define D_FALLBACK_TOPIC "Fallback Topic"
#define D_FALSE "거짓"
#define D_FILE "파일"
#define D_FLOW_RATE "Flow rate"
#define D_FREE_MEMORY "남은 메모리"
#define D_PSR_MAX_MEMORY "PS-RAM Memory"
#define D_PSR_FREE_MEMORY "PS-RAM free Memory"
#define D_FREQUENCY "Frequency"
#define D_GAS "가스"
#define D_GATEWAY "게이트웨이"
#define D_GROUP "그룹"
#define D_HOST "호스트"
#define D_HOSTNAME "호스트이름"
#define D_HUMIDITY "습도"
#define D_ILLUMINANCE "조도"
#define D_IMMEDIATE "immediate"      // Button immediate
#define D_INDEX "인덱스"
#define D_INFO "정보"
#define D_INFRARED "적외선"
#define D_INITIALIZED "초기화됨"
#define D_IP_ADDRESS "IP 주소"
#define D_LIGHT "밝게"
#define D_LWT "LWT"
#define D_LQI "LQI"                  // Zigbee Link Quality Index
#define D_MODULE "모듈"
#define D_MOISTURE "Moisture"
#define D_MQTT "MQTT"
#define D_MULTI_PRESS "multi-press"
#define D_NOISE "소음"
#define D_NONE "없음"
#define D_OFF "꺼짐"
#define D_OFFLINE "오프라인"
#define D_OK "Ok"
#define D_ON "켜짐"
#define D_ONLINE "온라인"
#define D_PASSWORD "비밀번호"
#define D_PORT "포트"
#define D_POWER_FACTOR "Power Factor"
#define D_POWERUSAGE "전원"
#define D_POWERUSAGE_ACTIVE "Active Power"
#define D_POWERUSAGE_APPARENT "Apparent Power"
#define D_POWERUSAGE_REACTIVE "Reactive Power"
#define D_PRESSURE "기압"
#define D_PRESSUREATSEALEVEL "해수면기압"
#define D_PROGRAM_FLASH_SIZE "플래시 용량"
#define D_PROGRAM_SIZE "프로그램 용량"
#define D_PROJECT "프로젝트"
#define D_RAIN "비"
#define D_RANGE "Range"
#define D_RECEIVED "받음"
#define D_RESTART "재시작"
#define D_RESTARTING "재시작 중.."
#define D_RESTART_REASON "재시작 사유"
#define D_RESTORE "복구"
#define D_RETAINED "보류"
#define D_RULE "규칙"
#define D_SAVE "저장"
#define D_SENSOR "센서"
#define D_SSID "SSId"
#define D_START "시작"
#define D_STD_TIME "STD"
#define D_STOP "정지"
#define D_SUBNET_MASK "서브넷 마스크"
#define D_SUBSCRIBE_TO "구독"
#define D_UNSUBSCRIBE_FROM "구독 해제"
#define D_SUCCESSFUL "성공"
#define D_SUNRISE "일출"
#define D_SUNSET "일몰"
#define D_TEMPERATURE "온도"
#define D_TO "to"
#define D_TOGGLE "켜기/끄기"
#define D_TOPIC "Topic"
#define D_TOTAL_USAGE "전체 사용량"
#define D_TRANSMIT "전송"
#define D_TRUE "참"
#define D_TVOC "TVOC"
#define D_UPGRADE "업그레이드"
#define D_UPLOAD "업로드"
#define D_UPTIME "가동시간"
#define D_USER "User"
#define D_UTC_TIME "UTC"
#define D_UV_INDEX "UV 색인"
#define D_UV_INDEX_1 "낮음"
#define D_UV_INDEX_2 "보통"
#define D_UV_INDEX_3 "높음"
#define D_UV_INDEX_4 "위험"
#define D_UV_INDEX_5 "BurnL1/2"
#define D_UV_INDEX_6 "BurnL3"
#define D_UV_INDEX_7 "OoR"         // Out of Range
#define D_UV_LEVEL "UV 레벨"
#define D_UV_POWER "UV 파워"
#define D_VERSION "버전"
#define D_VOLTAGE "전압"
#define D_WEIGHT "무게"
#define D_WARMLIGHT "따뜻하게"
#define D_WEB_SERVER "웹 서버"

// tasmota.ino
#define D_WARNING_MINIMAL_VERSION "경고: 이 버전은 영구 설정을 지원하지 않습니다"
#define D_LEVEL_10 "level 1-0"
#define D_LEVEL_01 "level 0-1"
#define D_SERIAL_LOGGING_DISABLED "Serial log 사용 안함"
#define D_SYSLOG_LOGGING_REENABLED "Syslog log 다시 사용"

#define D_SET_BAUDRATE_TO "Set Baudrate to"
#define D_RECEIVED_TOPIC "Received Topic"
#define D_DATA_SIZE "데이터 용량"
#define D_ANALOG_INPUT "아날로그"

// support.ino
#define D_OSWATCH "osWatch"
#define D_BLOCKED_LOOP "Blocked Loop"
#define D_WPS_FAILED_WITH_STATUS "WPS설정 실패"
#define D_ACTIVE_FOR_3_MINUTES "3분동안 활성화"
#define D_FAILED_TO_START "시작 실패"
#define D_PATCH_ISSUE_2186 "Patch issue 2186"
#define D_CONNECTING_TO_AP "AP에 연결 중"
#define D_IN_MODE "in mode"
#define D_CONNECT_FAILED_NO_IP_ADDRESS "IP 주소가 수신되지 않아 연결이 실패했습니다"
#define D_CONNECT_FAILED_AP_NOT_REACHED "연결이 닿지 않아 AP에 연결할 수 없습니다"
#define D_CONNECT_FAILED_WRONG_PASSWORD "연결할 수 없습니다"
#define D_CONNECT_FAILED_AP_TIMEOUT "시간초과로 AP에 연결할 수 없습니다"
#define D_ATTEMPTING_CONNECTION "연결 시도 중..."
#define D_CHECKING_CONNECTION "연결 체크 중..."
#define D_QUERY_DONE "쿼리 완료. MQTT 서비스 발견"
#define D_MQTT_SERVICE_FOUND "MQTT 서비스 발견"
#define D_FOUND_AT "다음을 발견"
#define D_SYSLOG_HOST_NOT_FOUND "Syslog 호스트가 발견되지 않았습니다"

// settings.ino
#define D_SAVED_TO_FLASH_AT "플래시에 저장"
#define D_LOADED_FROM_FLASH_AT "플래시에서 로드"
#define D_USE_DEFAULTS "디폴트 사용"
#define D_ERASED_SECTOR "삭제된 섹터"

// xdrv_02_webserver.ino
#define D_NOSCRIPT "Tasmota를 사용하려면 JavaScript를 활성화 하십시오."
#define D_MINIMAL_FIRMWARE_PLEASE_UPGRADE "MINIMAL firmware<br>업그레이드가 필요합니다"
#define D_WEBSERVER_ACTIVE_ON "Web 서버 작동 중"
#define D_WITH_IP_ADDRESS "IP 주소"
#define D_WEBSERVER_STOPPED "Web 서버 멈춤"
#define D_FILE_NOT_FOUND "파일을 찾을 수 없습니다"
#define D_REDIRECTED "인증 페이지로 리디렉션"
#define D_WIFIMANAGER_SET_ACCESSPOINT_AND_STATION "와이파이 매니저가 AccessPoint와 keep Station을 설정"
#define D_WIFIMANAGER_SET_ACCESSPOINT "와이파이 매니저가 AccessPoint를 설정"
#define D_TRYING_TO_CONNECT "장치를 네트워크에 연결하려고 시도 중"

#define D_RESTART_IN "재시작"
#define D_SECONDS "초"
#define D_DEVICE_WILL_RESTART "이 장치는 몇 초 후 재시작됩니다"
#define D_BUTTON_TOGGLE "켜기/끄기"
#define D_CONFIGURATION "설정 메뉴"
#define D_INFORMATION "정보"
#define D_FIRMWARE_UPGRADE "펌웨어 업그레이드"
#define D_CONSOLE "콘솔"
#define D_CONFIRM_RESTART "재시작"

#define D_CONFIGURE_MODULE "모듈 설정"
#define D_CONFIGURE_WIFI "WiFi 설정"
#define D_CONFIGURE_MQTT "MQTT 설정"
#define D_CONFIGURE_DOMOTICZ "Domoticz 설정"
#define D_CONFIGURE_LOGGING "로그 설정"
#define D_CONFIGURE_OTHER "기타 설정"
#define D_CONFIRM_RESET_CONFIGURATION "설정 초기화 확인"
#define D_RESET_CONFIGURATION "설정 초기화"
#define D_BACKUP_CONFIGURATION "설정 백업"
#define D_RESTORE_CONFIGURATION "설정 복구"
#define D_MAIN_MENU "메인 메뉴"

#define D_MODULE_PARAMETERS "모듈 설정"
#define D_MODULE_TYPE "모듈 타입"
#define D_PULLUP_ENABLE "버튼 없음/스위치 당김"
#define D_ADC "ADC"
#define D_GPIO "GPIO"
#define D_SERIAL_IN "Serial In"
#define D_SERIAL_OUT "Serial Out"

#define D_WIFI_PARAMETERS "Wifi 설정"
#define D_SCAN_FOR_WIFI_NETWORKS "Wifi 네트워크를 검색.."
#define D_SCAN_DONE "검색 완료"
#define D_NO_NETWORKS_FOUND "발견된 네트워크가 없습니다"
#define D_REFRESH_TO_SCAN_AGAIN "검색 재시도"
#define D_DUPLICATE_ACCESSPOINT "중복된 AccessPoint"
#define D_SKIPPING_LOW_QUALITY "약한 네트워크 신호 무시"
#define D_RSSI "RSSI"
#define D_WEP "WEP"
#define D_WPA_PSK "WPA PSK"
#define D_WPA2_PSK "WPA2 PSK"
#define D_AP1_SSID "AP1 SSId"
#define D_AP1_PASSWORD "AP1 비밀번호"
#define D_AP2_SSID "AP2 SSId"
#define D_AP2_PASSWORD "AP2 비밀번호"

#define D_MQTT_PARAMETERS "MQTT 설정"
#define D_CLIENT "클라이언트"
#define D_FULL_TOPIC "Full Topic"

#define D_LOGGING_PARAMETERS "로그 설정"
#define D_SERIAL_LOG_LEVEL "Serial 로그 레벨"
#define D_MQTT_LOG_LEVEL "Mqtt 로그 레벨"
#define D_WEB_LOG_LEVEL "Web 로그 레벨"
#define D_SYS_LOG_LEVEL "시스템로그 레벨"
#define D_MORE_DEBUG "More debug"
#define D_SYSLOG_HOST "시스템로그 호스트"
#define D_SYSLOG_PORT "시스템로그 포트"
#define D_TELEMETRY_PERIOD "보고 주기"

#define D_OTHER_PARAMETERS "기타 설정"
#define D_TEMPLATE "템플릿"
#define D_ACTIVATE "활성화"
#define D_DEVICE_NAME "Device Name"
#define D_WEB_ADMIN_PASSWORD "Web Admin 비밀번호"
#define D_MQTT_ENABLE "MQTT 사용"
#define D_FRIENDLY_NAME "Friendly Name"
#define D_BELKIN_WEMO "Belkin WeMo"
#define D_HUE_BRIDGE "Hue Bridge"
#define D_SINGLE_DEVICE "단일 장치"
#define D_MULTI_DEVICE "다수 장치"

#define D_CONFIGURE_TEMPLATE "템플릿 설정"
#define D_TEMPLATE_PARAMETERS "템플릿 설정"
#define D_TEMPLATE_NAME "이름"
#define D_BASE_TYPE "Based on"
#define D_TEMPLATE_FLAGS "옵션"

#define D_SAVE_CONFIGURATION "설정 저장"
#define D_CONFIGURATION_SAVED "설정 저장 완료"
#define D_CONFIGURATION_RESET "설정 초기화"

#define D_PROGRAM_VERSION "프로그램 버전"
#define D_BUILD_DATE_AND_TIME "빌드 날짜"
#define D_CORE_AND_SDK_VERSION "Core/SDK 버전"
#define D_FLASH_WRITE_COUNT "플래시 쓰기 횟수"
#define D_MAC_ADDRESS "MAC 주소"
#define D_MQTT_HOST "MQTT 호스트"
#define D_MQTT_PORT "MQTT 포트"
#define D_MQTT_CLIENT "MQTT 클라이언트"
#define D_MQTT_USER "MQTT 아이디"
#define D_MQTT_TOPIC "MQTT Topic"
#define D_MQTT_GROUP_TOPIC "MQTT Group Topic"
#define D_MQTT_FULL_TOPIC "MQTT Full Topic"
#define D_MDNS_DISCOVERY "mDNS Discovery"
#define D_MDNS_ADVERTISE "mDNS Advertise"
#define D_ESP_CHIP_ID "ESP 칩 Id"
#define D_FLASH_CHIP_ID "플래시 칩 Id"
#define D_FLASH_CHIP_SIZE "플래시 용량"
#define D_FREE_PROGRAM_SPACE "여유 프로그램 공간"

#define D_UPGRADE_BY_WEBSERVER "웹 서버에서 업그레이드"
#define D_OTA_URL "OTA Url"
#define D_START_UPGRADE "업그레이드 시작"
#define D_UPGRADE_BY_FILE_UPLOAD "파일에서 업로드해 업그레이드"
#define D_UPLOAD_STARTED "업로드 시작됨"
#define D_UPGRADE_STARTED "업그레이드 시작됨"
#define D_UPLOAD_DONE "업그레이드 완료"
#define D_UPLOAD_ERR_1 "파일이 선택되지 않았습니다"
#define D_UPLOAD_ERR_2 "용량이 충분하지 않습니다"
#define D_UPLOAD_ERR_3 "Magic 바이트가 0xE9가 아닙니다"
#define D_UPLOAD_ERR_4 "플래시 프로그램이 실제 플래시 용량보다 큽니다"
#define D_UPLOAD_ERR_5 "업로드 버퍼가 일치하지 않습니다"
#define D_UPLOAD_ERR_6 "업로드 실패. 로그 3 사용"
#define D_UPLOAD_ERR_7 "업로드 중단"
#define D_UPLOAD_ERR_8 "파일이 유효하지 않습니다"
#define D_UPLOAD_ERR_9 "용량이 초과되었습니다"
#define D_UPLOAD_ERR_10 "RF 칩 초기화 실패"
#define D_UPLOAD_ERR_11 "RF 칩 삭제 실패"
#define D_UPLOAD_ERR_12 "RF 칩 쓰기 실패"
#define D_UPLOAD_ERR_13 "RF 펌웨어 decode 실패"
#define D_UPLOAD_ERR_14 "Not compatible"
#define D_UPLOAD_ERROR_CODE "업로드 에러 코드"

#define D_ENTER_COMMAND "커맨드 입력"
#define D_ENABLE_WEBLOG_FOR_RESPONSE "응답이 있다면 Weblog 2를 사용"
#define D_NEED_USER_AND_PASSWORD "user=<아이디>&password=<비밀번호> 필요"

// xdrv_01_mqtt.ino
#define D_FINGERPRINT "TLS 지문 확인..."
#define D_TLS_CONNECT_FAILED_TO "TLS 연결 실패"
#define D_RETRY_IN "재시도 중"
#define D_VERIFIED "지문 확인 완료"
#define D_INSECURE "유효하지 않은 지문으로 연결이 되지 않았습니다"
#define D_CONNECT_FAILED_TO "연결 실패"

// xplg_wemohue.ino
#define D_MULTICAST_DISABLED "Multicast 사용 불가"
#define D_MULTICAST_REJOINED "Multicast (다시)가입됨"
#define D_MULTICAST_JOIN_FAILED "Multicast 가입 실패"
#define D_FAILED_TO_SEND_RESPONSE "요청 전송 실패"

#define D_WEMO "WeMo"
#define D_WEMO_BASIC_EVENT "WeMo 기본 이벤트"
#define D_WEMO_EVENT_SERVICE "WeMo 이벤트 서비스"
#define D_WEMO_META_SERVICE "WeMo meta 서비스"
#define D_WEMO_SETUP "WeMo 설정"
#define D_RESPONSE_SENT "요청 전송됨"

#define D_HUE "Hue"
#define D_HUE_BRIDGE_SETUP "Hue 설정"
#define D_HUE_API_NOT_IMPLEMENTED "Hue API가 포함되지 않음"
#define D_HUE_API "Hue API"
#define D_HUE_POST_ARGS "Hue POST args"
#define D_3_RESPONSE_PACKETS_SENT "3 요청 패킷이 전송됨"

// xdrv_07_domoticz.ino
#define D_DOMOTICZ_PARAMETERS "Domoticz 설정"
#define D_DOMOTICZ_IDX "Idx"
#define D_DOMOTICZ_KEY_IDX "Key idx"
#define D_DOMOTICZ_SWITCH_IDX "스위치 idx"
#define D_DOMOTICZ_SENSOR_IDX "센서 idx"
  #define D_DOMOTICZ_TEMP "온도"
  #define D_DOMOTICZ_TEMP_HUM "온도,습도"
  #define D_DOMOTICZ_TEMP_HUM_BARO "온도,습도,기압"
  #define D_DOMOTICZ_POWER_ENERGY "전력,전력량"
  #define D_DOMOTICZ_ILLUMINANCE "조도"
  #define D_DOMOTICZ_COUNT "횟수/PM1"
  #define D_DOMOTICZ_VOLTAGE "전압/PM2.5"
  #define D_DOMOTICZ_CURRENT "전류/PM10"
  #define D_DOMOTICZ_AIRQUALITY "공기질"
  #define D_DOMOTICZ_P1_SMART_METER "P1SmartMeter"
#define D_DOMOTICZ_UPDATE_TIMER "타이머 갱신"

// xdrv_09_timers.ino
#define D_CONFIGURE_TIMER "타이머 설정"
#define D_TIMER_PARAMETERS "타이머 설정"
#define D_TIMER_ENABLE "타이머 사용"
#define D_TIMER_ARM "Arm"
#define D_TIMER_TIME "시간"
#define D_TIMER_DAYS "일"
#define D_TIMER_REPEAT "반복"
#define D_TIMER_OUTPUT "출력"
#define D_TIMER_ACTION "행동"

// xdrv_10_knx.ino
#define D_CONFIGURE_KNX "KNX 설정"
#define D_KNX_PARAMETERS "KNX 설정"
#define D_KNX_GENERAL_CONFIG "일반"
#define D_KNX_PHYSICAL_ADDRESS "물리적 주소"
#define D_KNX_PHYSICAL_ADDRESS_NOTE "( KNX 네트워크 상에서 반드시 고유한 이름이어야 합니다 )"
#define D_KNX_ENABLE "KNX 사용"
#define D_KNX_GROUP_ADDRESS_TO_WRITE "그룹 주소로 데이타를 보냅니다"
#define D_ADD "추가"
#define D_DELETE "삭제"
#define D_REPLY "응답"
#define D_KNX_GROUP_ADDRESS_TO_READ "받은 데이터의 그룹 주소"
#define D_RECEIVED_FROM "다음에서 받음"
#define D_KNX_COMMAND_WRITE "쓰기"
#define D_KNX_COMMAND_READ "읽기"
#define D_KNX_COMMAND_OTHER "기타"
#define D_SENT_TO "다음으로 보내기"
#define D_KNX_WARNING "그룹 주소 ( 0 / 0 / 0 )은 예약되어 사용할 수 없습니다"
#define D_KNX_ENHANCEMENT "커뮤니케이션 강화"
#define D_KNX_TX_SLOT "KNX TX"
#define D_KNX_RX_SLOT "KNX RX"
#define D_KNX_TX_SCENE "KNX SCENE TX"
#define D_KNX_RX_SCENE "KNX SCENE RX"

// xdrv_03_energy.ino
#define D_ENERGY_TODAY "금일 전력 사용량"
#define D_ENERGY_YESTERDAY "어제 전력 사용량"
#define D_ENERGY_TOTAL "총 전력 사용량"

// xdrv_27_shutter.ino
#define D_OPEN "열기"
#define D_CLOSE "닫기"
#define D_DOMOTICZ_SHUTTER "셔터"

// xdrv_28_pcf8574.ino
#define D_CONFIGURE_PCF8574 "PCF8574 설정"
#define D_PCF8574_PARAMETERS "PCF8574 파라미터"
#define D_INVERT_PORTS "포트 반전"
#define D_DEVICE "장치"
#define D_DEVICE_INPUT "입력"
#define D_DEVICE_OUTPUT "출력"

// xsns_05_ds18b20.ino
#define D_SENSOR_BUSY "센서가 사용 중"
#define D_SENSOR_CRC_ERROR "센서 CRC 에러"
#define D_SENSORS_FOUND "센서 발견"

// xsns_06_dht.ino
#define D_TIMEOUT_WAITING_FOR "대기 시간 초과"
#define D_START_SIGNAL_LOW "시작 신호 낮음"
#define D_START_SIGNAL_HIGH "시작 신호 높음"
#define D_PULSE "pulse"
#define D_CHECKSUM_FAILURE "체크섬 실패"

// xsns_07_sht1x.ino
#define D_SENSOR_DID_NOT_ACK_COMMAND "센서가 ACK 명령을 수행하지 않음"
#define D_SHT1X_FOUND "SHT1X 발견"

// xsns_18_pms5003.ino
#define D_STANDARD_CONCENTRATION "CF-1 PM"     // Standard Particle CF-1 Particle Matter
#define D_ENVIRONMENTAL_CONCENTRATION "PM"     // Environmetal Particle Matter
#define D_PARTICALS_BEYOND "입자"

// xsns_27_apds9960.ino
#define D_GESTURE "Gesture"
#define D_COLOR_RED "Red"
#define D_COLOR_GREEN "Green"
#define D_COLOR_BLUE "Blue"
#define D_CCT "CCT"
#define D_PROXIMITY "Proximity"

// xsns_32_mpu6050.ino
#define D_AX_AXIS "Accel. X-Axis"
#define D_AY_AXIS "Accel. Y-Axis"
#define D_AZ_AXIS "Accel. Z-Axis"
#define D_GX_AXIS "Gyro X-Axis"
#define D_GY_AXIS "Gyro Y-Axis"
#define D_GZ_AXIS "Gyro Z-Axis"

// xsns_34_hx711.ino
#define D_HX_CAL_REMOVE "중량 제거"
#define D_HX_CAL_REFERENCE "참조 중량 로드"
#define D_HX_CAL_DONE "교정됨"
#define D_HX_CAL_FAIL "교정 실패"
#define D_RESET_HX711 "스케일 초기화"
#define D_CONFIGURE_HX711 "스케일 설정"
#define D_HX711_PARAMETERS "스케일 상세"
#define D_ITEM_WEIGHT "아이템 중량"
#define D_REFERENCE_WEIGHT "참조 중량"
#define D_CALIBRATE "교정"
#define D_CALIBRATION "교정"

//xsns_35_tx20.ino
#define D_TX20_WIND_DIRECTION "풍향"
#define D_TX20_WIND_SPEED "풍속"
#define D_TX20_WIND_SPEED_MIN "풍속 최소"
#define D_TX20_WIND_SPEED_MAX "풍속 최대"
#define D_TX20_NORTH "N"
#define D_TX20_EAST "E"
#define D_TX20_SOUTH "S"
#define D_TX20_WEST "W"

// xsns_53_sml.ino
#define D_TPWRIN "Energy Total-In"
#define D_TPWROUT "Energy Total-Out"
#define D_TPWRCURR "Active Power-In/Out"
#define D_TPWRCURR1 "Active Power-In p1"
#define D_TPWRCURR2 "Active Power-In p2"
#define D_TPWRCURR3 "Active Power-In p3"
#define D_Strom_L1 "Current L1"
#define D_Strom_L2 "Current L2"
#define D_Strom_L3 "Current L3"
#define D_Spannung_L1 "Voltage L1"
#define D_Spannung_L2 "Voltage L2"
#define D_Spannung_L3 "Voltage L3"
#define D_METERNR "Meter_number"
#define D_METERSID "Service ID"
#define D_GasIN "Counter"
#define D_H2oIN "Counter"
#define D_StL1L2L3 "Current L1+L2+L3"
#define D_SpL1L2L3 "Voltage L1+L2+L3/3"

// tasmota_template.h - keep them as short as possible to be able to fit them in GUI drop down box
#define D_SENSOR_NONE          "없음"
#define D_SENSOR_USER          "User"
#define D_SENSOR_DHT11         "DHT11"
#define D_SENSOR_AM2301        "AM2301"
#define D_SENSOR_SI7021        "SI7021"
#define D_SENSOR_DS18X20       "DS18x20"
#define D_SENSOR_I2C_SCL       "I2C SCL"
#define D_SENSOR_I2C_SDA       "I2C SDA"
#define D_SENSOR_WS2812        "WS2812"
#define D_SENSOR_DFR562        "MP3 Player"
#define D_SENSOR_IRSEND        "IRsend"
#define D_SENSOR_SWITCH        "Switch"     // Suffix "1"
#define D_SENSOR_BUTTON        "Button"     // Suffix "1"
#define D_SENSOR_RELAY         "Relay"      // Suffix "1i"
#define D_SENSOR_LED           "Led"        // Suffix "1i"
#define D_SENSOR_LED_LINK      "LedLink"    // Suffix "i"
#define D_SENSOR_PWM           "PWM"        // Suffix "1"
#define D_SENSOR_COUNTER       "Counter"    // Suffix "1"
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
#define D_SENSOR_RF_SENSOR     "RF Sensor"
#define D_SENSOR_AZ_RX         "AZ Rx"
#define D_SENSOR_AZ_TX         "AZ Tx"
#define D_SENSOR_MAX31855_CS   "MX31855 CS"
#define D_SENSOR_MAX31855_CLK  "MX31855 CLK"
#define D_SENSOR_MAX31855_DO   "MX31855 DO"
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
#define D_SENSOR_BUZZER        "Buzzer"
#define D_SENSOR_OLED_RESET    "OLED Reset"
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
#define D_SENSOR_A4988_MS2     "A4988 MS2"
#define D_SENSOR_A4988_MS3     "A4988 MS3"
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

// Units
#define D_UNIT_AMPERE "A"
#define D_UNIT_CELSIUS "C"
#define D_UNIT_CENTIMETER "cm"
#define D_UNIT_DEGREE "°"
#define D_UNIT_FAHRENHEIT "F"
#define D_UNIT_HERTZ "Hz"
#define D_UNIT_HOUR "시"
#define D_UNIT_GALLONS "gal"
#define D_UNIT_GALLONS_PER_MIN "g/m"
#define D_UNIT_INCREMENTS "inc"
#define D_UNIT_KELVIN "K"
#define D_UNIT_KILOMETER "km"
#define D_UNIT_KILOGRAM "kg"
#define D_UNIT_KILOMETER_PER_HOUR "km/h"  // or "km/h"
#define D_UNIT_KILOOHM "kΩ"
#define D_UNIT_KILOWATTHOUR "kWh"
#define D_UNIT_LUX "lx"
#define D_UNIT_MICROGRAM_PER_CUBIC_METER "µg/m³"
#define D_UNIT_MICROMETER "마이크로미터"
#define D_UNIT_MICROSECOND "마이크로초"
#define D_UNIT_MILLIAMPERE "mA"
#define D_UNIT_MILLIMETER "mm"
#define D_UNIT_MILLIMETER_MERCURY "mmHg"
#define D_UNIT_MILLISECOND "밀리초"
#define D_UNIT_MINUTE "분"
#define D_UNIT_PARTS_PER_BILLION "ppb"
#define D_UNIT_PARTS_PER_DECILITER "ppd"
#define D_UNIT_PARTS_PER_MILLION "ppm"
#define D_UNIT_PERCENT "%%"
#define D_UNIT_PRESSURE "hPa"
#define D_UNIT_SECOND "초"
#define D_UNIT_SECTORS "섹터"
#define D_UNIT_VA "VA"
#define D_UNIT_VAR "VAr"
#define D_UNIT_VOLT "V"
#define D_UNIT_WATT "W"
#define D_UNIT_WATTHOUR "Wh"
#define D_UNIT_WATT_METER_QUADRAT "W/m²"

//SDM220, SDM120, LE01MR
#define D_PHASE_ANGLE     "Phase Angle"
#define D_IMPORT_ACTIVE   "Import Active"
#define D_EXPORT_ACTIVE   "Export Active"
#define D_IMPORT_REACTIVE "Import Reactive"
#define D_EXPORT_REACTIVE "Export Reactive"
#define D_TOTAL_REACTIVE  "Total Reactive"
#define D_UNIT_KWARH      "kVArh"
#define D_UNIT_ANGLE      "Deg"
#define D_TOTAL_ACTIVE    "Total Active"

//SOLAXX1
#define D_PV1_VOLTAGE     "PV1 Voltage"
#define D_PV1_CURRENT     "PV1 Current"
#define D_PV1_POWER       "PV1 Power"
#define D_PV2_VOLTAGE     "PV2 Voltage"
#define D_PV2_CURRENT     "PV2 Current"
#define D_PV2_POWER       "PV2 Power"
#define D_SOLAR_POWER     "Solar Power"
#define D_INVERTER_POWER  "Inverter Power"
#define D_STATUS          "Status"
#define D_WAITING         "Waiting"
#define D_CHECKING        "Checking"
#define D_WORKING         "Working"
#define D_FAILURE         "Failure"
#define D_SOLAX_ERROR_0   "No Error Code"
#define D_SOLAX_ERROR_1   "Grid Lost Fault"
#define D_SOLAX_ERROR_2   "Grid Voltage Fault"
#define D_SOLAX_ERROR_3   "Grid Frequency Fault"
#define D_SOLAX_ERROR_4   "Pv Voltage Fault"
#define D_SOLAX_ERROR_5   "Isolation Fault"
#define D_SOLAX_ERROR_6   "Over Temperature Fault"
#define D_SOLAX_ERROR_7   "Fan Fault"
#define D_SOLAX_ERROR_8   "Other Device Fault"

//xdrv_10_scripter.ino
#define D_CONFIGURE_SCRIPT     "Edit script"
#define D_SCRIPT               "edit script"
#define D_SDCARD_UPLOAD        "file upload"
#define D_SDCARD_DIR           "sd card directory"
#define D_UPL_DONE             "Done"
#define D_SCRIPT_CHARS_LEFT    "chars left"
#define D_SCRIPT_CHARS_NO_MORE "no more chars"
#define D_SCRIPT_DOWNLOAD      "Download"
#define D_SCRIPT_ENABLE        "script enable"
#define D_SCRIPT_UPLOAD        "Upload"
#define D_SCRIPT_UPLOAD_FILES  "Upload files"

//xsns_67_as3935.ino
#define D_AS3935_GAIN "gain:"
#define D_AS3935_ENERGY "energy:"
#define D_AS3935_DISTANCE "distance:"
#define D_AS3935_DISTURBER "disturber:"
#define D_AS3935_VRMS "µVrms:"
#define D_AS3935_APRX "aprx.:"
#define D_AS3935_AWAY "away"
#define D_AS3935_LIGHT "lightning"
#define D_AS3935_OUT "lightning out of range"
#define D_AS3935_NOT "distance not determined"
#define D_AS3935_ABOVE "lightning overhead"
#define D_AS3935_NOISE "noise detected"
#define D_AS3935_DISTDET "disturber detected"
#define D_AS3935_INTNOEV "Interrupt with no Event!"
#define D_AS3935_NOMESS "listening..."
#define D_AS3935_ON "On"
#define D_AS3935_OFF "Off"
#define D_AS3935_INDOORS "Indoors"
#define D_AS3935_OUTDOORS "Outdoors"
#define D_AS3935_CAL_FAIL "calibration failed"
#define D_AS3935_CAL_OK "calibration set to:"

//xsns_68_opentherm.ino
#define D_SENSOR_BOILER_OT_RX   "OpenTherm RX"
#define D_SENSOR_BOILER_OT_TX   "OpenTherm TX"

#endif  // _LANGUAGE_KO_KO_H_
