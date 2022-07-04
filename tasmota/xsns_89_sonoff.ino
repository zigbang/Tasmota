/*
    xsns_89_sonoff.ino - ESP8266 based relay for ZIoT Tasmota
*/

#ifdef FIRMWARE_ZIOT_SONOFF
#define XSNS_89 89

#define HEALTH_CHECK_PERIOD 80
#define RESPONSE_TIMEOUT_US 10000000 // 5s
#define MAX_RETRY_COUNT 5

#define HELLO_RESPONSE_CHECKER 0
#define CHITCHAT_RESPONSE_CHECKER 1
#define SHADOW_RESPONSE_CHECKER 2
#define MAX_RESPONSE_CHECKER 3

#define NO_SHADOW -1
#define INITIAL_SHADOW 0
#define SWITICH_OFF_SHADOW_WITH_DESIRED 1
#define SWITICH_ON_SHADOW_WITH_DESIRED 2
#define SWITICH_OFF_SHADOW_ONLY_REPORTED 3
#define SWITICH_ON_SHADOW_ONLY_REPORTED 4

#define SWITCH_OFF 0
#define SWITCH_ON 1

#ifndef FIRMWARE_ZIOT_MINIMAL
struct TimeoutChecker {
    uint32_t startTime = 0;
    uint8_t count = 0;
    bool ready = false;
};

struct WiFiConfig {
    char* ssid;
    char* bssid;
    char* ipAddr;
    char* gatewayAddr;
};
#endif  // FIRMWARE_ZIOT_MINIMAL

struct ZIoTSonoff {
    bool ready = false;
    uint32_t sessionId = 0;
    char* version = "1.0.3";
#ifndef FIRMWARE_ZIOT_MINIMAL
    char mainTopic[60];
    char shadowTopic[70];
    char* schemeVersion = "v220530";
    char* vendor = "sonoff";
    char* thingType = "relay";
    uint32_t second = 0;
    int8_t lastShadow = NO_SHADOW;
    TimeoutChecker timeoutChecker[3];
    WiFiConfig wifiConfig;
#endif  // FIRMWARE_ZIOT_MINIMAL
} ziotSonoff;

#ifndef FIRMWARE_ZIOT_MINIMAL
int numberOfSetBits(uint32_t i)
{
     uint32_t temp = i;
     temp = temp - ((temp >> 1) & 0x55555555);        // add pairs of bits
     temp = (temp & 0x33333333) + ((temp >> 2) & 0x33333333);  // quads
     temp = (temp + (temp >> 4)) & 0x0F0F0F0F;        // groups of 8
     return (temp * 0x01010101) >> 24;          // horizontal sum of bytes
}

void MakeStatusesLastFormat(uint8_t numberOfArray, char** codeArray, char** valueArray, char* output)
{
    for (uint8_t i = 0; i < numberOfArray; i++)
    {
        int size = strlen(codeArray[i]) + strlen(valueArray[i] + 13);
        char* temp = (char*)malloc(size + 60);
        char timestamp[15] = "";
        GetTimestampInMillis(timestamp);
        if (i == numberOfArray - 1) {
            snprintf_P(temp, size + 60, "{\"code\":\"%s\",\"value\":%s,\"createdAt\":%s}", codeArray[i], valueArray[i], timestamp);
        }
        else {
            snprintf_P(temp, size + 60, "{\"code\":\"%s\",\"value\":%s,\"createdAt\":%s},", codeArray[i], valueArray[i], timestamp);
        }
        strcat(output, temp);
        free(temp);
    }
}

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

    if (savedTime > currentTime) {
        tempTime = currentTime + (0xFFFFFFFF - savedTime);
    }
    else {
        tempTime = currentTime - savedTime;
    }

    if (tempTime >= timeoutInUs) {
        result = true;
    }

    return result;
}

void CheckTimerList(void)
{
    for (uint8_t i = 0; i < MAX_RESPONSE_CHECKER; i++) {
        if (ziotSonoff.timeoutChecker[i].ready) {
            if (IsTimeout(ziotSonoff.timeoutChecker[i].startTime, RESPONSE_TIMEOUT_US)) {
                if (ziotSonoff.timeoutChecker[i].count == MAX_RETRY_COUNT) {
                    printf("%d is reached to the max retry count!!\n", i);
                    SettingsSaveAll();
                    EspRestart();
                } else {
                    ziotSonoff.timeoutChecker[i].count++;
                    switch (i) {
                        case HELLO_RESPONSE_CHECKER:
                            PublishHello();
                            break;
                        case CHITCHAT_RESPONSE_CHECKER:
                            PublishChitChat();
                            break;
                        case SHADOW_RESPONSE_CHECKER:
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
}

void PublishHello(void)
{
    char payload[530] = "";
    char requestTopic[73] = "";

    snprintf_P(requestTopic, sizeof(requestTopic), PSTR("%s%s"), ziotSonoff.mainTopic, "/hello");

    snprintf_P(payload, sizeof(payload), \
        PSTR("{\"clientId\":\"%s\",\"sessionId\":\"%d\",\"requestTopic\":\"%s\",\"responseTopic\":\"%s/accepted\",\"errorTopic\":\"%s/rejected\",\"data\":{\"vendor\":\"sonoff\",\"thingName\":\"%s\",\"pnu\":\"%s\",\"dongho\":\"%s\",\"certArn\":\"%s\",\"firmwareVersion\":\"%s\"}}"), \
        SettingsText(SET_MQTT_TOPIC), ++ziotSonoff.sessionId, requestTopic, requestTopic, requestTopic, SettingsText(SET_MQTT_TOPIC), SettingsText(SET_PNU), SettingsText(SET_DONGHO), SettingsText(SET_CERT_ARN), ziotSonoff.version);
    
    ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = true;

    PublishMainTopicWithPostfix(payload, "/hello");
}

void PublishChitChat(void)
{
    char payload[600] = "";
    char requestTopic[80] = "";

    snprintf_P(requestTopic, sizeof(requestTopic), PSTR("%s%s"), ziotSonoff.mainTopic, "/chitchat");    
    snprintf_P(payload, sizeof(payload), \
        PSTR("{\"clientId\":\"%s\",\"sessionId\":\"%d\",\"requestTopic\":\"%s\",\"responseTopic\":\"%s/res\",\"data\":{\"network\":{\"ap\":{\"ssid\":\"%s\",\"bssid\":\"%s\"},\"ip\":{\"address\":\"%s/%d\",\"gateway\":\"%s\"},\"misc\":{\"rssi\":\"%d\"}}}}"), \
        SettingsText(SET_MQTT_TOPIC), ++ziotSonoff.sessionId, requestTopic, requestTopic, ziotSonoff.wifiConfig.ssid, ziotSonoff.wifiConfig.bssid, ziotSonoff.wifiConfig.ipAddr, numberOfSetBits((uint32_t)WiFi.subnetMask()), ziotSonoff.wifiConfig.gatewayAddr, WiFi.RSSI());

    ziotSonoff.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotSonoff.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].ready = true;

    PublishMainTopicWithPostfix(payload, "/chitchat");

    ziotSonoff.second = 0;
}

void PublishMainTopicWithPostfix(char *payload, char *postfix)
{
    uint8_t topicSize = strlen(ziotSonoff.mainTopic) + strlen(postfix);
    char *topic = (char*)malloc(sizeof(char) * (topicSize + 1));

    strcpy(topic, ziotSonoff.mainTopic);
    strcat(topic, postfix);

    MqttPublishPayload(topic, payload);
    free(topic);
}

void SubscribeMainTopicWithPostfix(char *postfix)
{
    uint8_t topicSize = strlen(ziotSonoff.mainTopic) + strlen(postfix);
    char *topic = (char*)malloc(sizeof(char) * (topicSize + 1));

    strcpy(topic, ziotSonoff.mainTopic);
    strcat(topic, postfix);

    MqttSubscribe(topic);
    free(topic);
}

void SubscribeShadowTopicWithPostfix(char *postfix)
{
    uint8_t topicSize = strlen(ziotSonoff.shadowTopic) + strlen(postfix);
    char *topic = (char*)malloc(sizeof(char) * (topicSize + 1));

    strcpy(topic, ziotSonoff.shadowTopic);
    strcat(topic, postfix);

    MqttSubscribe(topic);
    free(topic);
}

void UpdateInitialShadow(void)
{
    char payload[410];
    char switch1[6];
    char statusesLast[80] = "";

    if (bitRead(TasmotaGlobal.power, 0) == 0) {
        strcpy(switch1, "false");
    } else {
        strcpy(switch1, "true");
    }

    char* code[1] = {"switch1"};
    char* value[1] = {switch1};
    MakeStatusesLastFormat(1, code, value, statusesLast);


    ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
    ziotSonoff.lastShadow = INITIAL_SHADOW;

    UpdateShadow(ziotSonoff.shadowTopic, (char*)S_JSON_SONOFF_SWITCH_SHADOW, ziotSonoff.schemeVersion, ziotSonoff.vendor, ziotSonoff.thingType, ziotSonoff.version, switch1, statusesLast);
}

void UpdateShadowWithDesired(bool switchValue)
{
    char statusesLast[80] = "";
    char* code[1] = {"switch1"};
    char* value[1] = {""};

    if (switchValue == SWITCH_OFF) {
        strcpy(value[0], "false");
        MakeStatusesLastFormat(1, code, value, statusesLast);

        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotSonoff.lastShadow = SWITICH_OFF_SHADOW_WITH_DESIRED;
    } else {
        strcpy(value[0], "true");
        MakeStatusesLastFormat(1, code, value, statusesLast);

        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotSonoff.lastShadow = SWITICH_ON_SHADOW_WITH_DESIRED;
    }
    UpdateShadow(ziotSonoff.shadowTopic, (char*)S_JSON_SONOFF_SWITCH_SHADOW_WITH_DESIRED, value[0], ziotSonoff.schemeVersion, ziotSonoff.vendor, ziotSonoff.thingType, ziotSonoff.version, value[0], statusesLast);
}

void UpdateShadowOnlyReported(bool switchValue)
{
    char statusesLast[80] = "";
    char* code[1] = {"switch1"};
    char* value[1] = {""};

    if (switchValue == SWITCH_OFF) {
        strcpy(value[0], "false");
        MakeStatusesLastFormat(1, code, value, statusesLast);
        
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotSonoff.lastShadow = SWITICH_OFF_SHADOW_ONLY_REPORTED;
    } else {
        strcpy(value[0], "true");
        MakeStatusesLastFormat(1, code, value, statusesLast);

        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotSonoff.lastShadow = SWITICH_ON_SHADOW_ONLY_REPORTED;
    }
    UpdateShadow(ziotSonoff.shadowTopic, (char*)S_JSON_SONOFF_SWITCH_SHADOW, ziotSonoff.schemeVersion, ziotSonoff.vendor, ziotSonoff.thingType, ziotSonoff.version, value[0], statusesLast);
}

void UpdateShadow(char *topic, char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    ResponseWithVaList(format, ap);

    va_end(ap);
    MqttPublish(topic);
}

bool SaveTargetOtaUrl(char* url)
{
    uint8_t *spi_buffer = (uint8_t*)malloc(tls_spi_len);
    if (!spi_buffer) {
        printf("Can't allocate memory to spi_buffer!\n");
        return false;
    }

    memcpy_P(spi_buffer, tls_spi_start, tls_spi_len);
    uint16_t startAddress = atoi(SettingsText(SET_ENTRY2_START));
    memcpy(spi_buffer + tls_obj_store_offset + startAddress, url, strlen(url) + 1);
    if (ESP.flashEraseSector(tls_spi_start_sector)) {
        ESP.flashWrite(tls_spi_start_sector * SPI_FLASH_SEC_SIZE, (uint32_t*)spi_buffer, SPI_FLASH_SEC_SIZE);
    }
    free(spi_buffer);
}

void InitWifiConfig(void)
{
    ziotSonoff.wifiConfig.ssid = (char*)malloc(30);
    strcpy(ziotSonoff.wifiConfig.ssid, SettingsText(SET_STASSID1));

    ziotSonoff.wifiConfig.bssid = (char*)malloc(WiFi.BSSIDstr().length());
    strcpy(ziotSonoff.wifiConfig.bssid, WiFi.BSSIDstr().c_str());

    uint32_t temp2 = (uint32_t)NetworkAddress();
    ziotSonoff.wifiConfig.ipAddr = (char*)malloc(16);
    snprintf_P(ziotSonoff.wifiConfig.ipAddr, 16, PSTR("%u.%u.%u.%u"), temp2 & 0xFF, (temp2 >> 8) & 0xFF, (temp2 >> 16) & 0xFF, (temp2 >> 24) & 0xFF);

    temp2 = Settings->ipv4_address[1];
    ziotSonoff.wifiConfig.gatewayAddr = (char*)malloc(16);
    snprintf_P(ziotSonoff.wifiConfig.gatewayAddr, 16, PSTR("%u.%u.%u.%u"), temp2 & 0xFF, (temp2 >> 8) & 0xFF, (temp2 >> 16) & 0xFF, (temp2 >> 24) & 0xFF);
}

void GetTimestampInMillis(char* src)
{
    String timestamp = String(UtcTime()) + String(RtcMillis());
    strcpy(src, timestamp.c_str());
}
#endif  // FIRMWARE_ZIOT_MINIMAL

bool LoadTargetOtaUrl(void)
{
    tls_dir_t tls_dir_2;
    uint16_t startAddress;
    if (strcmp(SettingsText(SET_ENTRY2_START), "") != 0) {
        startAddress = atoi(SettingsText(SET_ENTRY2_START));

        memcpy_P(&tls_dir_2, (uint8_t*)0x402FF000 + 0x0400, sizeof(tls_dir_2));
        char* data = (char *) (tls_spi_start + tls_obj_store_offset + startAddress);
        strcpy(TasmotaGlobal.sonoff_ota_url, data);
    }
}

void SonoffButtonHandler(void)
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
* Interface
\*********************************************************************************************/

bool Xsns89(uint8_t function)
{
    bool result = false;

    if (FUNC_PRE_INIT == function)
    {
        ziotSonoff.ready = true;
        printf("The version of this firmware is : %s\n", ziotSonoff.version);
#ifdef FIRMWARE_ZIOT_MINIMAL
        LoadTargetOtaUrl();
#endif  // FIRMWARE_ZIOT_MINIMAL
    }
    else if (ziotSonoff.ready)
    {
        switch (function)
        {
#ifndef FIRMWARE_ZIOT_MINIMAL
            case FUNC_INIT:
                snprintf_P(ziotSonoff.mainTopic, sizeof(ziotSonoff.mainTopic), PSTR("ziot/sonoff/basicr2/%s"), \
                        SettingsText(SET_MQTT_TOPIC));
                snprintf_P(ziotSonoff.shadowTopic, sizeof(ziotSonoff.shadowTopic), PSTR("$aws/things/%s/shadow/update"), \
                        SettingsText(SET_MQTT_TOPIC));
            break;
            case FUNC_MQTT_INIT:
            {
                InitWifiConfig();
                PublishHello();
                break;
            }
            case FUNC_MQTT_SUBSCRIBE:
                SubscribeMainTopicWithPostfix("/hello/accepted");
                SubscribeMainTopicWithPostfix("/hello/rejected");
                break;
            case FUNC_COMMAND:
                printf("topic : %s, data : %s\n", XdrvMailbox.topic, XdrvMailbox.data);
                if (ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].ready) {
                    if (strcmp(XdrvMailbox.topic, "ACCEPTED") == 0) {
                        strcpy(XdrvMailbox.topic, "");
                        ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = false;
                        ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].count = 0;
                        
                        SubscribeShadowTopicWithPostfix("/delta");
                        SubscribeShadowTopicWithPostfix("/rejected");
                        SubscribeShadowTopicWithPostfix("/accepted");
                        SubscribeMainTopicWithPostfix("/chitchat/res");

                        UpdateInitialShadow();
                    }
                    else if (strcmp(XdrvMailbox.topic, "REJECTED") == 0) {
                        MqttDisconnect();
                        ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = false;
                        ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].count = 0;
                        TasmotaGlobal.restart_flag = 212;
                    }
                }
                else {
                    if (ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready) {
                        if (strcmp(XdrvMailbox.topic, "REJECTED") == 0) {
                            strcpy(XdrvMailbox.topic, "");
                            ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].count = 0;
                            ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
                            switch (ziotSonoff.lastShadow) {
                                case NO_SHADOW:
                                    break;
                                case INITIAL_SHADOW:
                                    UpdateInitialShadow();
                                    break;
                                case SWITICH_OFF_SHADOW_ONLY_REPORTED:
                                    UpdateShadowOnlyReported(SWITCH_OFF);
                                    break;
                                case SWITICH_ON_SHADOW_ONLY_REPORTED:
                                    UpdateShadowOnlyReported(SWITCH_ON);
                                    break;
                                case SWITICH_OFF_SHADOW_WITH_DESIRED:
                                    UpdateShadowWithDesired(SWITCH_OFF);
                                    break;
                                case SWITICH_ON_SHADOW_WITH_DESIRED:
                                    UpdateShadowWithDesired(SWITCH_ON);
                                    break;
                                default:
                                    break;
                            }
                        }
                        else {
                            ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = false;
                            ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].count = 0;
                            ziotSonoff.lastShadow = NO_SHADOW;
                        }
                    }
                    else {
                        if (strcmp(XdrvMailbox.topic, "DELTA") == 0) {
                            strcpy(XdrvMailbox.topic, "");

                            JsonParser parser(XdrvMailbox.data);

                            JsonParserObject root = parser.getRootObject();
                            JsonParserObject state = root["state"].getObject();
                            JsonParserObject status = state["status"].getObject();

                            String switch1 = status["switch1"].getStr();
                            char* switch1CharType = (char*)switch1.c_str();

                            if ((strcmp(switch1CharType, "true") == 0) && bitRead(TasmotaGlobal.power, 0) == 0) {
                                SetAllPower(POWER_TOGGLE_NO_STATE, SRC_MQTT);
                                UpdateShadowOnlyReported(SWITCH_ON);
                            }
                            else if ((strcmp(switch1CharType, "false") == 0) && bitRead(TasmotaGlobal.power, 0) == 1) {
                                SetAllPower(POWER_TOGGLE_NO_STATE, SRC_MQTT);
                                UpdateShadowOnlyReported(SWITCH_OFF);
                            }
                        }
                        else if (strcmp(XdrvMailbox.topic, "RES") == 0) {
                            JsonParser parser(XdrvMailbox.data);

                            JsonParserObject root = parser.getRootObject();
                            String sessionId = root["sessionId"].getStr();

                            JsonParserObject data = root["data"].getObject();
                            JsonParserObject ota = data["ota"].getObject();
                            String newVersion = ota["newVersion"].getStr();
                            String targetUrl = ota["targetUrl"].getStr();
                            String minimalUrl = ota["minimalUrl"].getStr();

                            if (sessionId.length()) {
                                char* sessionIdCharType = (char*)sessionId.c_str();
                                
                                if (atoi(sessionIdCharType) == ziotSonoff.sessionId) {
                                    ziotSonoff.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].ready = false;
                                    ziotSonoff.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].count = 0;

                                    char* newVersionCharType = (char*)newVersion.c_str();
                                    char* minimalUrlCharType = (char*)minimalUrl.c_str();
                                    char* targetUrlCharType = (char*)targetUrl.c_str();

                                    if ((targetUrl.length() && minimalUrl.length()) && ((strcmp(minimalUrlCharType, "false") != 0) && (strcmp(targetUrlCharType, "false") != 0))) {
                                        SaveTargetOtaUrl(targetUrlCharType);
                                        strcpy(TasmotaGlobal.sonoff_ota_url, minimalUrlCharType);

                                        TasmotaGlobal.ota_state_flag = 3;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
#endif  // FIRMWARE_ZIOT_MINIMAL
            case FUNC_LOOP:
                break;
            case FUNC_EVERY_SECOND:
#ifndef FIRMWARE_ZIOT_MINIMAL
                ziotSonoff.second++;

                if (!TasmotaGlobal.global_state.mqtt_down && ziotSonoff.second == HEALTH_CHECK_PERIOD) {
                    PublishChitChat();
                }

                CheckTimerList();
#endif  // FIRMWARE_ZIOT_MINIMAL
                break;
            case FUNC_BUTTON_PRESSED:
                SonoffButtonHandler();
                break;
        }
    }

    return result;
}

#endif  // FIRMWARE_ZIOT_SONOFF