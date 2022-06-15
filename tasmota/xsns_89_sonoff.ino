/*
    xsns_89_sonoff.ino - ESP8266 based relay for ZIoT Tasmota
*/

#ifdef FIRMWARE_ZIOT_SONOFF
#define XSNS_89 89

struct ZIoTSonoff {
    bool ready = false;
    bool executedOnce = false;
    bool isInitialStage = true;
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
                char payload[200];

                snprintf_P(payload, sizeof(payload), \
                    PSTR("{\"vendor\":\"sonoff\",\"thingName\":\"%s\",\"pnu\":\"%s\",\"dongho\":\"%s\",\"firmwareVersion\":\"%s\"}"), \
                    SettingsText(SET_MQTT_TOPIC), SettingsText(SET_PNU), SettingsText(SET_DONGHO), ziotSonoff.version);

                PublishMainTopicWithPostfix(payload, "/hello");
                break;
            case FUNC_MQTT_SUBSCRIBE:
                SubscribeMainTopicWithPostfix("/hello/accepted");
                SubscribeMainTopicWithPostfix("/hello/rejected");
                break;
            case FUNC_COMMAND:
                if (ziotSonoff.isInitialStage) {
                    if (strcmp(XdrvMailbox.topic, "ACCEPT") == 0) {
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