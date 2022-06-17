/*
    xsns_89_sonoff.ino - ESP8266 based relay for ZIoT Tasmota
*/

#ifdef FIRMWARE_ZIOT_SONOFF
#define XSNS_89 89

struct ZIoTSonoff {
    bool ready = false;
    bool executedOnce = false;
    bool isInitialStage = true;
    bool waitShadowResponse = false;
    uint32_t sessionId = 0;
    char* version = "1.0.2";
    char mainTopic[60];
    char shadowTopic[70];
    char* schemeVersion = "v220110";
    char* vendor = "sonoff";
    char* thingType = "relay";
} ziotSonoff;

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

void UpdateShadow(char *payload)
{
    char topic[64];
    char awsPayload[410];

    snprintf_P(topic, sizeof(topic), PSTR("$aws/things/%s/shadow/update"), SettingsText(SET_MQTT_TOPIC));
    snprintf_P(awsPayload, sizeof(awsPayload), PSTR("{\"state\":{\"reported\":%s}}"), payload);

    ziotSonoff.waitShadowResponse = true;
    MqttClient.publish(topic, awsPayload, false);
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
        
        ziotSonoff.waitShadowResponse = true;
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
                char payload[500] = "";
                char responseTopic[73] = "";
                char errorTopic[73] = "";

                snprintf_P(responseTopic, sizeof(responseTopic), PSTR("%s%s"), ziotSonoff.mainTopic, "/hello/accepted");
                snprintf_P(errorTopic, sizeof(errorTopic), PSTR("%s%s"), ziotSonoff.mainTopic, "/hello/rejected");

                snprintf_P(payload, sizeof(payload), \
                    PSTR("{\"sessionId\":\"%d\",\"responseTopic\":\"%s\",\"errorTopic\":\"%s\",\"vendor\":\"sonoff\",\"thingName\":\"%s\",\"pnu\":\"%s\",\"dongho\":\"%s\",\"certArn\":\"%s\",\"firmwareVersion\":\"%s\"}"), \
                    ++ziotSonoff.sessionId, responseTopic, errorTopic, SettingsText(SET_MQTT_TOPIC), SettingsText(SET_PNU), SettingsText(SET_DONGHO), SettingsText(SET_CERT_ARN), ziotSonoff.version);

                PublishMainTopicWithPostfix(payload, "/hello");
                break;
            }
            case FUNC_MQTT_SUBSCRIBE:
                SubscribeMainTopicWithPostfix("/hello/accepted");
                SubscribeMainTopicWithPostfix("/hello/rejected");
                break;
            case FUNC_COMMAND:
                if (ziotSonoff.isInitialStage) {
                    if (strcmp(XdrvMailbox.topic, "ACCEPT") == 0) {
                        strcpy(XdrvMailbox.topic, "");
                        ziotSonoff.isInitialStage = false;
                        
                        SubscribeShadowTopicWithPostfix("/delta");
                        SubscribeShadowTopicWithPostfix("/rejected");
                        SubscribeShadowTopicWithPostfix("/accepted");
                        SubscribeMainTopicWithPostfix("/healthcheck/res");

                        char payload[410];

                        snprintf_P(payload, sizeof(payload), \
                            PSTR("{\"schemeVersion\":\"%s\",\"vendor\":\"%s\",\"thingType\":\"%s\",\"firmwareVersion\":\"%s\",\"status\":{\"isConnected\":true,\"batteryPercentage\":100,\"switch1\":%s,\"countdown1\":0,\"relayStatus\":\"2\",\"cycleTime\":\"\",\"switchInching\":\"\"}}"), \
                            ziotSonoff.schemeVersion, ziotSonoff.vendor, ziotSonoff.thingType, ziotSonoff.version, "true" \
                        );

                        UpdateShadow(payload);
                    }
                    else {
                        MqttDisconnect();
                        TasmotaGlobal.restart_flag = 212;
                    }
                }
                else {
                    if (ziotSonoff.waitShadowResponse) {
                        if (strcmp(XdrvMailbox.topic, "REJECTED") == 0) {
                            printf("something is rejected!!!\n");
                            strcpy(XdrvMailbox.topic, "");
                            ziotSonoff.waitShadowResponse = true;
                            MqttPublish(ziotSonoff.shadowTopic);
                        }
                        else {
                            ziotSonoff.waitShadowResponse = false;
                        }
                    }
                    else {
                        if (strcmp(XdrvMailbox.topic, "DELTA") == 0) {
                            strcpy(XdrvMailbox.topic, "");
                            bool executeCommand = false;

                            JsonParser parser(XdrvMailbox.data);
                            JsonParserObject root = parser.getRootObject();
                            JsonParserObject state = root["state"].getObject();
                            JsonParserObject status = state["status"].getObject();
                            String switch1 = status["switch1"].getStr();
                            char* switch1CharType = (char*)switch1.c_str();

                            if ((strcmp(switch1CharType, "true") == 0) && bitRead(TasmotaGlobal.power, 0) == 0) {
                                SetAllPower(POWER_TOGGLE_NO_STATE, SRC_MQTT);
                                Response_P(S_JSON_SONOFF_SWITCH_SHADOW, "true");
                                MqttPublish(ziotSonoff.shadowTopic);
                            }
                            else if ((strcmp(switch1CharType, "false") == 0) && bitRead(TasmotaGlobal.power, 0) == 1) {
                                SetAllPower(POWER_TOGGLE_NO_STATE, SRC_MQTT);
                                Response_P(S_JSON_SONOFF_SWITCH_SHADOW, "false");
                                MqttPublish(ziotSonoff.shadowTopic);
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
                if (!ziotSonoff.executedOnce && !TasmotaGlobal.global_state.mqtt_down) {
                    char payload[60];
                    ziotSonoff.executedOnce = true;

                    snprintf_P(payload, sizeof(payload), PSTR("{\"version\":\"%s\"}"), ziotSonoff.version);
                    UpdateShadow(payload);
                }
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