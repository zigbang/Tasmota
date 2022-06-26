/*
    xsns_89_sonoff.ino - ESP8266 based relay for ZIoT Tasmota
*/

#ifdef FIRMWARE_ZIOT_SONOFF
#define XSNS_89 89

#define HEALTH_CHECK_PERIOD 120
#define RESPONSE_TIMEOUT_US 10000000 // 5s
#define MAX_RETRY_COUNT 5

#define HELLO_RESPONSE_CHECKER 0
#define HEALTHCHECK_RESPONSE_CHECKER 1
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
#endif  // FIRMWARE_ZIOT_MINIMAL

struct ZIoTSonoff {
    bool ready = false;
    bool executedOnce = false;
    uint32_t sessionId = 0;
    char* version = "1.0.2";
#ifndef FIRMWARE_ZIOT_MINIMAL
    char mainTopic[60];
    char shadowTopic[70];
    char* schemeVersion = "v220110";
    char* vendor = "sonoff";
    char* thingType = "relay";
    uint32_t second = 0;
    int8_t lastShadow = NO_SHADOW;
    TimeoutChecker timeoutChecker[3];
#endif  // FIRMWARE_ZIOT_MINIMAL
} ziotSonoff;

#ifndef FIRMWARE_ZIOT_MINIMAL
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
                        case HEALTHCHECK_RESPONSE_CHECKER:
                            PublishHealthCheck();
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
    char payload[500] = "";
    char responseTopic[73] = "";
    char errorTopic[73] = "";

    snprintf_P(responseTopic, sizeof(responseTopic), PSTR("%s%s"), ziotSonoff.mainTopic, "/hello/accepted");
    snprintf_P(errorTopic, sizeof(errorTopic), PSTR("%s%s"), ziotSonoff.mainTopic, "/hello/rejected");

    snprintf_P(payload, sizeof(payload), \
        PSTR("{\"sessionId\":\"%d\",\"responseTopic\":\"%s\",\"errorTopic\":\"%s\",\"vendor\":\"sonoff\",\"thingName\":\"%s\",\"pnu\":\"%s\",\"dongho\":\"%s\",\"certArn\":\"%s\",\"firmwareVersion\":\"%s\"}"), \
        ++ziotSonoff.sessionId, responseTopic, errorTopic, SettingsText(SET_MQTT_TOPIC), SettingsText(SET_PNU), SettingsText(SET_DONGHO), SettingsText(SET_CERT_ARN), ziotSonoff.version);
    
    ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = true;

    PublishMainTopicWithPostfix(payload, "/hello");
}

void PublishHealthCheck(void)
{
    char payload[500] = "";
    char requestTopic[80] = "";
    char responseTopic[80] = "";

    snprintf_P(requestTopic, sizeof(requestTopic), PSTR("%s%s"), ziotSonoff.mainTopic, "/healthcheck");
    snprintf_P(responseTopic, sizeof(responseTopic), PSTR("%s%s"), ziotSonoff.mainTopic, "/healthcheck/res");

    snprintf_P(payload, sizeof(payload), \
        PSTR("{\"clientId\":\"%s\",\"sessionId\":\"%d\",\"requestTopic\":\"%s\",\"responseTopic\":\"%s\",\"data\":\"%s\"}"), \
        SettingsText(SET_MQTT_TOPIC), ++ziotSonoff.sessionId, requestTopic, responseTopic, "");

    ziotSonoff.timeoutChecker[HEALTHCHECK_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotSonoff.timeoutChecker[HEALTHCHECK_RESPONSE_CHECKER].ready = true;

    PublishMainTopicWithPostfix(payload, "/healthcheck");

    ziotSonoff.second = 0;
}

void PublishMainTopicWithPostfix(char *payload, char *postfix)
{
    uint8_t topicSize = strlen(ziotSonoff.mainTopic) + strlen(postfix);
    char *topic = (char*)malloc(sizeof(char) * (topicSize + 1));

    strcpy(topic, ziotSonoff.mainTopic);
    strcat(topic, postfix);

    MqttPublishPayload(topic, payload);
}

void SubscribeMainTopicWithPostfix(char *postfix)
{
    uint8_t topicSize = strlen(ziotSonoff.mainTopic) + strlen(postfix);
    char *topic = (char*)malloc(sizeof(char) * (topicSize + 1));

    strcpy(topic, ziotSonoff.mainTopic);
    strcat(topic, postfix);

    MqttSubscribe(topic);
}

void SubscribeShadowTopicWithPostfix(char *postfix)
{
    uint8_t topicSize = strlen(ziotSonoff.shadowTopic) + strlen(postfix);
    char *topic = (char*)malloc(sizeof(char) * (topicSize + 1));

    strcpy(topic, ziotSonoff.shadowTopic);
    strcat(topic, postfix);

    MqttSubscribe(topic);
}

void UpdateInitialShadow(void)
{
    char payload[410];
    char switch1[6];

    if (bitRead(TasmotaGlobal.power, 0) == 0) {
        strcpy(switch1, "false");
    } else {
        strcpy(switch1, "true");
    }

    snprintf_P(payload, sizeof(payload), \
        PSTR("{\"schemeVersion\":\"%s\",\"vendor\":\"%s\",\"thingType\":\"%s\",\"firmwareVersion\":\"%s\",\"status\":{\"isConnected\":true,\"batteryPercentage\":100,\"switch1\":%s,\"countdown1\":0,\"relayStatus\":\"2\",\"cycleTime\":\"\",\"switchInching\":\"\"}}"), \
        ziotSonoff.schemeVersion, ziotSonoff.vendor, ziotSonoff.thingType, ziotSonoff.version, switch1 \
    );

    ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
    ziotSonoff.lastShadow = INITIAL_SHADOW;

    UpdateShadow(payload);
}

void UpdateShadowWithDesired(bool switchValue)
{
    if (switchValue == SWITCH_OFF) {
        Response_P(S_JSON_SONOFF_SWITCH_SHADOW_WITH_DESIRED, "false", "false");
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotSonoff.lastShadow = SWITICH_OFF_SHADOW_WITH_DESIRED;
    } else {
        Response_P(S_JSON_SONOFF_SWITCH_SHADOW_WITH_DESIRED, "true", "true");
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotSonoff.lastShadow = SWITICH_ON_SHADOW_WITH_DESIRED;
    }

    MqttPublish(ziotSonoff.shadowTopic);
}

void UpdateShadowOnlyReported(bool switchValue)
{
    if (switchValue == SWITCH_OFF) {
        Response_P(S_JSON_SONOFF_SWITCH_SHADOW, "false");
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotSonoff.lastShadow = SWITICH_OFF_SHADOW_ONLY_REPORTED;
    } else {
        Response_P(S_JSON_SONOFF_SWITCH_SHADOW, "true");
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotSonoff.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotSonoff.lastShadow = SWITICH_ON_SHADOW_ONLY_REPORTED;
    }
    MqttPublish(ziotSonoff.shadowTopic);
}

void UpdateShadow(char *payload)
{
    char topic[64];
    char awsPayload[410];

    snprintf_P(topic, sizeof(topic), PSTR("$aws/things/%s/shadow/update"), SettingsText(SET_MQTT_TOPIC));
    snprintf_P(awsPayload, sizeof(awsPayload), PSTR("{\"state\":{\"reported\":%s}}"), payload);

    MqttClient.publish(topic, awsPayload, false);
}
#endif  // FIRMWARE_ZIOT_MINIMAL

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
                        SubscribeMainTopicWithPostfix("/healthcheck/res");

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
                            String version = data["version"].getStr();

                            if (sessionId.length() && version.length()) {
                                char* sessionIdCharType = (char*)sessionId.c_str();
                                
                                if (atoi(sessionIdCharType) == ziotSonoff.sessionId) {
                                    ziotSonoff.timeoutChecker[HEALTHCHECK_RESPONSE_CHECKER].ready = false;
                                    ziotSonoff.timeoutChecker[HEALTHCHECK_RESPONSE_CHECKER].count = 0;

                                    char* versionCharType = (char*)version.c_str();
                                    TasmotaGlobal.ota_state_flag = 3;
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

                if (!ziotSonoff.executedOnce && !TasmotaGlobal.global_state.mqtt_down) {
                    char payload[60];
                    ziotSonoff.executedOnce = true;

                    snprintf_P(payload, sizeof(payload), PSTR("{\"version\":\"%s\"}"), ziotSonoff.version);
                    UpdateShadow(payload);
                }

                if (!TasmotaGlobal.global_state.mqtt_down && ziotSonoff.second == HEALTH_CHECK_PERIOD) {
                    // PublishHealthCheck();
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