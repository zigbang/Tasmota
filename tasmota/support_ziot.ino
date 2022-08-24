/*
    support_ziot.ino - ziot support for esp8266 and esp32
*/

#ifdef FIRMWARE_ZIOT

#define HEALTH_CHECK_PERIOD 300
#define URL_SCHEME "http://"
#define URL_SIZE 100

#define TIMER_HELLO_START 0
#define TIMER_CHITCHAT_START 1
#define TIMER_CHITCHAT_CLEAR 2
#define TIMER_MAX_COUNTS 3

class tls_entry_t
{
public:
    uint32_t name;  // simple 4 letters name. Currently 'skey', 'crt ', 'crt1', 'crt2'
    uint16_t start; // start offset
    uint16_t len;   // len of object
};                  // 8 bytes

class tls_dir_t
{
public:
    tls_entry_t entry[4]; // 4 entries max, only 4 used today, for future use
};                        // 4*8 = 64 bytes

tls_dir_t tls_dir; // memory copy of tls_dir from flash

#ifdef ESP32
static uint8_t *ziot_spi_start = nullptr;
static uint8_t *tls_spi_start = nullptr;
const static size_t tls_spi_len = 0x0400;      // 1kb blocs
const static size_t tls_block_offset = 0x0000; // don't need offset in FS
#else                                          // Not ESP32
// const static uint16_t tls_spi_start_sector = EEPROM_LOCATION + 4;  // 0xXXFF
// const static uint8_t* tls_spi_start    = (uint8_t*) ((tls_spi_start_sector * SPI_FLASH_SEC_SIZE) + 0x40200000);  // 0x40XFF000
const static uint16_t tls_spi_start_sector = 0xFF;           // Force last bank of first MB
const static uint8_t *tls_spi_start = (uint8_t *)0x402FF000; // 0x402FF000
const static size_t tls_spi_len = 0x1000;                    // 4kb blocs
const static size_t tls_block_offset = 0x0400;
#endif                                         // ESP32
const static size_t tls_block_len = 0x0400;    // 1kb
const static size_t tls_obj_store_offset = tls_block_offset + sizeof(tls_dir_t);

struct ZIoT
{
    char *version;
    char *schemeVersion;
    char *vendor;
    char *thingType;
    char *mainTopic;
    bool ready = false;
    bool wifiConfigured = false;
    uint32_t sessionId = 0;
    uint32_t second = 0;
    bool needHello = true;

    typedef void (*timerCallback)(void);
    timerCallback timerList[TIMER_MAX_COUNTS];
} ziot;

#ifndef FIRMWARE_ZIOT_MINIMAL

uint32_t GetSessionId(void)
{
    return ziot.sessionId;
}

uint32_t UpdateAndGetSessionId(void)
{
    return ++ziot.sessionId;
}

void InitZIoT(char *version, char *schemeVersion, char *vendor, char *thingType, char *mainTopic, void (*startTimerHelloCb)(void), void (*startTimerChitChatCb)(void), void (*clearTimerChitChatCb)(void))
{
    ziot.version = (char *)malloc(strlen(version));
    ziot.schemeVersion = (char *)malloc(strlen(schemeVersion));
    ziot.vendor = (char *)malloc(strlen(vendor));
    ziot.thingType = (char *)malloc(strlen(thingType));
    ziot.mainTopic = (char *)malloc(strlen(mainTopic));

    ziot.timerList[TIMER_HELLO_START] = startTimerHelloCb;
    ziot.timerList[TIMER_CHITCHAT_START] = startTimerChitChatCb;
    ziot.timerList[TIMER_CHITCHAT_CLEAR] = clearTimerChitChatCb;

    if (ziot.version && ziot.schemeVersion && ziot.vendor && ziot.thingType)
    {
        memcpy(ziot.version, version, strlen(version) + 1);
        memcpy(ziot.schemeVersion, schemeVersion, strlen(schemeVersion) + 1);
        memcpy(ziot.vendor, vendor, strlen(vendor) + 1);
        memcpy(ziot.thingType, thingType, strlen(thingType) + 1);
        memcpy(ziot.mainTopic, mainTopic, strlen(mainTopic) + 1);

        ziot.ready = true;
    }
    else
    {
        free(ziot.version);
        free(ziot.schemeVersion);
        free(ziot.vendor);
        free(ziot.thingType);
        free(ziot.mainTopic);
    }
}

struct WiFiConfig
{
    char *ssid;
    char *bssid;
    char *ipAddr;
    char *gatewayAddr;
};

WiFiConfig wifiConfig;

void InitWifiConfig(void)
{
    wifiConfig.ssid = (char *)malloc(30);
    strcpy(wifiConfig.ssid, SettingsText(SET_STASSID1));

    wifiConfig.bssid = (char *)malloc(WiFi.BSSIDstr().length());
    strcpy(wifiConfig.bssid, WiFi.BSSIDstr().c_str());

    uint32_t temp2 = (uint32_t)NetworkAddress();
    wifiConfig.ipAddr = (char *)malloc(16);
    snprintf_P(wifiConfig.ipAddr, 16, PSTR("%u.%u.%u.%u"), temp2 & 0xFF, (temp2 >> 8) & 0xFF, (temp2 >> 16) & 0xFF, (temp2 >> 24) & 0xFF);

    temp2 = Settings->ipv4_address[1];
    wifiConfig.gatewayAddr = (char *)malloc(16);
    snprintf_P(wifiConfig.gatewayAddr, 16, PSTR("%u.%u.%u.%u"), temp2 & 0xFF, (temp2 >> 8) & 0xFF, (temp2 >> 16) & 0xFF, (temp2 >> 24) & 0xFF);

    ziot.wifiConfigured = true;
}

/*********************************************************************************************\
* Time Utility
\*********************************************************************************************/

uint32_t GetUsTime(void)
{
    uint32_t usTime = 0;
    usTime = micros();
    return usTime;
}

bool IsTimeout(uint32_t savedTime, uint32_t timeoutInUs)
{
    bool result = false;
    uint32_t tempTime;
    uint32_t currentTime = micros();

    if (savedTime > currentTime)
    {
        tempTime = currentTime + (0xFFFFFFFF - savedTime);
    }
    else
    {
        tempTime = currentTime - savedTime;
    }

    if (tempTime >= timeoutInUs)
    {
        result = true;
    }

    return result;
}

void GetTimestampInMillis(char *src)
{
    String timestamp = String(UtcTime()) + String(RtcMillis());
    strcpy(src, timestamp.c_str());
}

/*********************************************************************************************\
* Utility
\*********************************************************************************************/

int numberOfSetBits(uint32_t i)
{
    uint32_t temp = i;
    temp = temp - ((temp >> 1) & 0x55555555);                // add pairs of bits
    temp = (temp & 0x33333333) + ((temp >> 2) & 0x33333333); // quads
    temp = (temp + (temp >> 4)) & 0x0F0F0F0F;                // groups of 8
    return (temp * 0x01010101) >> 24;                        // horizontal sum of bytes
}

/*********************************************************************************************\
* MQTT
\*********************************************************************************************/

void PublishTopicWithPostfix(char *payload, char *topic, char *postfix)
{
    uint8_t topicSize = strlen(topic) + strlen(postfix);
    char *fullTopic = (char *)malloc(sizeof(char) * (topicSize + 1));

    strcpy(fullTopic, topic);
    strcat(fullTopic, postfix);

    MqttPublishPayload(fullTopic, payload);
    free(fullTopic);
}

void PublishHello(char *topic)
{
    char payload[530] = "";
    char requestTopic[73] = "";

    snprintf_P(requestTopic, sizeof(requestTopic), PSTR("%s%s"), topic, "/hello");

    snprintf_P(payload, sizeof(payload),
               PSTR("{\"clientId\":\"%s\",\"sessionId\":\"%d\",\"requestTopic\":\"%s\",\"responseTopic\":\"%s/accepted\",\"errorTopic\":\"%s/rejected\",\"data\":{\"vendor\":\"sonoff\",\"thingName\":\"%s\",\"pnu\":\"%s\",\"dongho\":\"%s\",\"certArn\":\"%s\",\"firmwareVersion\":\"%s\"}}"),
               SettingsText(SET_MQTT_TOPIC), UpdateAndGetSessionId(), requestTopic, requestTopic, requestTopic, SettingsText(SET_MQTT_TOPIC), SettingsText(SET_PNU), SettingsText(SET_DONGHO), SettingsText(SET_CERT_ARN), ziot.version);

    ziot.timerList[TIMER_HELLO_START]();

    PublishTopicWithPostfix(payload, topic, "/hello");
}

void PublishChitChat(char *topic)
{
    char payload[600] = "";
    char requestTopic[80] = "";

    snprintf_P(requestTopic, sizeof(requestTopic), PSTR("%s%s"), topic, "/chitchat");
    snprintf_P(payload, sizeof(payload),
               PSTR("{\"clientId\":\"%s\",\"sessionId\":\"%d\",\"requestTopic\":\"%s\",\"responseTopic\":\"%s/res\",\"data\":{\"network\":{\"ap\":{\"ssid\":\"%s\",\"bssid\":\"%s\"},\"ip\":{\"address\":\"%s/%d\",\"gateway\":\"%s\"},\"misc\":{\"rssi\":\"%d\"}}}}"),
               SettingsText(SET_MQTT_TOPIC), UpdateAndGetSessionId(), requestTopic, requestTopic, wifiConfig.ssid, wifiConfig.bssid, wifiConfig.ipAddr, numberOfSetBits((uint32_t)WiFi.subnetMask()), wifiConfig.gatewayAddr, WiFi.RSSI());

    ziot.timerList[TIMER_CHITCHAT_START]();

    PublishTopicWithPostfix(payload, topic, "/chitchat");

    ziot.second = 0;
}

void SubscribeTopicWithPostfix(char *topic, char *postfix)
{
    uint8_t topicSize = strlen(topic) + strlen(postfix);
    char *fullTopic = (char *)malloc(sizeof(char) * (topicSize + 1));

    strcpy(fullTopic, topic);
    strcat(fullTopic, postfix);

    MqttSubscribe(fullTopic);
    free(fullTopic);
}

void UpdateShadow(char *topic, char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    ResponseWithVaList(format, ap);

    va_end(ap);
    MqttPublish(topic);
}

void MakeStatusesLastFormat(uint8_t numberOfArray, char **codeArray, char **valueArray, char *output)
{
    for (uint8_t i = 0; i < numberOfArray; i++)
    {
        int size = strlen(codeArray[i]) + strlen(valueArray[i] + 13);
        char *temp = (char *)malloc(size + 60);
        char timestamp[15] = "";
        GetTimestampInMillis(timestamp);
        if (i == numberOfArray - 1)
        {
            snprintf_P(temp, size + 60, "{\"code\":\"%s\",\"value\":%s,\"createdAt\":%s}", codeArray[i], valueArray[i], timestamp);
        }
        else
        {
            snprintf_P(temp, size + 60, "{\"code\":\"%s\",\"value\":%s,\"createdAt\":%s},", codeArray[i], valueArray[i], timestamp);
        }
        strcat(output, temp);
        free(temp);
    }
}

/*********************************************************************************************\
* OTA
\*********************************************************************************************/

bool SaveTargetOtaUrl(char *url)
{
    uint8_t *spi_buffer = (uint8_t *)malloc(URL_SIZE);
    if (!spi_buffer)
    {
        printf("[ZIoT] Can't allocate memory to spi_buffer!\n");
        return false;
    }

#ifdef ESP8266
    memcpy_P(spi_buffer, tls_spi_start, tls_spi_len);
    uint16_t startAddress = atoi(SettingsText(SET_ENTRY2_START));
    memcpy(spi_buffer + tls_obj_store_offset + startAddress, url, strlen(url) + 1);
    if (ESP.flashEraseSector(tls_spi_start_sector))
    {
        ESP.flashWrite(tls_spi_start_sector * SPI_FLASH_SEC_SIZE, (uint32_t *)spi_buffer, SPI_FLASH_SEC_SIZE);
    }
#elif ESP32
    if (ziot_spi_start != nullptr)
    {
        memcpy_P(spi_buffer, ziot_spi_start, URL_SIZE);
    }
    else
    {
        memset(spi_buffer, 0, URL_SIZE);
    }

    memcpy(spi_buffer, url, URL_SIZE);
    OsalSaveNvm("/target_ota_url", spi_buffer, URL_SIZE);
#endif
    free(spi_buffer);
}

#endif // FIRMWARE_ZIOT_MINIMAL

bool LoadTargetOtaUrl(void)
{
#ifdef ESP8266
    tls_dir_t tls_dir_2;
    uint16_t startAddress;
    if (strcmp(SettingsText(SET_ENTRY2_START), "") != 0)
    {
        startAddress = atoi(SettingsText(SET_ENTRY2_START));

        memcpy_P(&tls_dir_2, (uint8_t *)0x402FF000 + 0x0400, sizeof(tls_dir_2));
        char *data = (char *)(tls_spi_start + tls_obj_store_offset + startAddress);
        strcpy(TasmotaGlobal.ziot_ota_url, data);
    }
#elif ESP32
    if (ziot_spi_start == nullptr)
    {
        ziot_spi_start = (uint8_t *)malloc(URL_SIZE);
        if (ziot_spi_start == nullptr)
        {
            printf("[ZIoT] free memory is not enough");
            return false;
        }
    }

    if (!OsalLoadNvm("/target_ota_url", ziot_spi_start, URL_SIZE))
    {
        printf("[ZIoT] Target ota url file doesn't exist");
    }
    else
    {
        memcpy_P(TasmotaGlobal.ziot_ota_url, ziot_spi_start, URL_SIZE);
        printf("LoadTargetOtaUrl : %s\n", TasmotaGlobal.ziot_ota_url);
    }
#endif
}

void ZIoTButtonHandler(void)
{
    uint32_t buttonIndex = XdrvMailbox.index;
    uint16_t loopsPerSec = 1000 / Settings->button_debounce;

    if (Button.last_state[buttonIndex] == PRESSED)
    {
        if ((Button.hold_timer[buttonIndex] == loopsPerSec * Settings->param[P_HOLD_TIME] / 10))
        {
            TasmotaGlobal.restart_flag = 212;
        }
    }
}

/*********************************************************************************************\
* Handler
\*********************************************************************************************/

void ZIoTHandler(uint8_t function)
{
    if (FUNC_PRE_INIT == function)
    {
#ifdef FIRMWARE_ZIOT_MINIMAL
        LoadTargetOtaUrl();
        ziot.ready = true;
#endif // FIRMWARE_ZIOT_MINIMAL
    }
    else if (ziot.ready)
    {
        switch (function)
        {
#ifndef FIRMWARE_ZIOT_MINIMAL
        case FUNC_MQTT_INIT:
#ifndef FIRMWARE_ZIOT_UART_MODULE
            InitWifiConfig();
#endif // FIRMWARE_ZIOT_UART_MODULE
#ifdef ESP8266
            PublishHello(ziot.mainTopic);
            ziot.timerList[TIMER_HELLO_START]();
#endif
            break;
        case FUNC_MQTT_SUBSCRIBE:
            SubscribeTopicWithPostfix(ziot.mainTopic, "/hello/accepted");
            SubscribeTopicWithPostfix(ziot.mainTopic, "/hello/rejected");
            break;
        case FUNC_COMMAND:
            if (strcmp(XdrvMailbox.topic, "RES") == 0)
            {
                JsonParser parser(XdrvMailbox.data);

                JsonParserObject root = parser.getRootObject();
                String sessionId = root["sessionId"].getStr();

                JsonParserObject data = root["data"].getObject();
                JsonParserObject ota = data["ota"].getObject();
                String newVersion = ota["newVersion"].getStr();
                String targetUrl = String(URL_SCHEME) + ota["targetUrl"].getStr();
                String minimalUrl = String(URL_SCHEME) + ota["minimalUrl"].getStr();

                if (sessionId.length())
                {
                    char *sessionIdCharType = (char *)sessionId.c_str();

                    if (atoi(sessionIdCharType) == GetSessionId())
                    {
                        ziot.timerList[TIMER_CHITCHAT_CLEAR]();

                        char *newVersionCharType = (char *)newVersion.c_str();
                        char *minimalUrlCharType = (char *)minimalUrl.c_str();
                        char *targetUrlCharType = (char *)targetUrl.c_str();

                        if (((targetUrl.length() > 13) && (minimalUrl.length() > 13)) && ((strcmp(minimalUrlCharType, URL_SCHEME) != 0) && (strcmp(targetUrlCharType, URL_SCHEME) != 0)))
                        {
                            SaveTargetOtaUrl(targetUrlCharType);
                            strcpy(TasmotaGlobal.ziot_ota_url, minimalUrlCharType);

                            TasmotaGlobal.ota_state_flag = 3;
                        }
                    }
                }
            }
            break;
        case FUNC_EVERY_SECOND:
            ziot.second++;

            if (!TasmotaGlobal.global_state.mqtt_down && ziot.second == HEALTH_CHECK_PERIOD)
            {
                PublishChitChat(ziot.mainTopic);
                ziot.second = 0;
            }
#ifdef ESP32
            else if (!TasmotaGlobal.global_state.mqtt_down && ziot.needHello && ziot.second >= 40)
            {
                PublishHello(ziot.mainTopic);
                ziot.timerList[TIMER_HELLO_START]();
                ziot.needHello = false;
            }
#endif // ESP32
            break;
#endif // FIRMWARE_ZIOT_MINIMAL
        case FUNC_BUTTON_PRESSED:
            ZIoTButtonHandler();
            break;
#ifdef FIRMWARE_ZIOT_UART_MODULE
        case FUNC_LOOP:
            if (!ziot.wifiConfigured && !TasmotaGlobal.global_state.wifi_down)
            {
                InitWifiConfig();
            }
            break;
#endif // FIRMWARE_ZIOT_UART_MODULE
        }
    }
}

#endif // FIRMWARE_ZIOT