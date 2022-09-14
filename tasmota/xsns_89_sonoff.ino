/*
    xsns_89_sonoff.ino - ESP8266 based relay for ZIoT Tasmota
*/

#ifdef FIRMWARE_ZIOT_SONOFF
#define XSNS_89 89

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
const char S_JSON_SONOFF_SWITCH_SHADOW_WITH_DESIRED[] PROGMEM = "{\"state\":{\"desired\":{\"status\":{\"switch1\":%s}},\"reported\":{\"schemeVersion\":\"%s\",\"vendor\":\"%s\",\"thingType\":\"%s\",\"firmwareVersion\":\"%s\",\"status\":{\"isConnected\":true,\"batteryPercentage\":100,\"switch1\":%s,\"countdown1\":0,\"relayStatus\":\"2\",\"cycleTime\":\"\",\"switchInching\":\"\"},\"statusesLast\":[%s]}}}";
const char S_JSON_SONOFF_SWITCH_SHADOW[] PROGMEM = "{\"state\":{\"reported\":{\"schemeVersion\":\"%s\",\"vendor\":\"%s\",\"thingType\":\"%s\",\"firmwareVersion\":\"%s\",\"status\":{\"isConnected\":true,\"batteryPercentage\":100,\"switch1\":%s,\"countdown1\":0,\"relayStatus\":\"2\",\"cycleTime\":\"\",\"switchInching\":\"\"},\"statusesLast\":[%s]}}}";

struct TimeoutChecker {
    uint32_t startTime = 0;
    uint8_t count = 0;
    bool ready = false;
};
#endif  // FIRMWARE_ZIOT_MINIMAL

struct ZIoTSonoff {
    bool ready = false;
    char* version = "1.0.11";
#ifndef FIRMWARE_ZIOT_MINIMAL
    char mainTopic[60];
    char shadowTopic[70];
    char* schemeVersion = "v220530";
    char* vendor = "zigbang";
    char* thingType = "Switch";
    int8_t lastShadow = NO_SHADOW;
    TimeoutChecker timeoutChecker[3];
#endif  // FIRMWARE_ZIOT_MINIMAL
} ziotSonoff;

#ifndef FIRMWARE_ZIOT_MINIMAL

void StartTimerHello(void)
{
    ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = true;
}

void StartTimerChitChat(void)
{
    ziotSonoff.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotSonoff.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].ready = true;
}

void ClearTimerChitChat(void)
{
    ziotSonoff.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].ready = false;
    ziotSonoff.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].count = 0;
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
                            PublishHello(ziotSonoff.mainTopic);
                            break;
                        case CHITCHAT_RESPONSE_CHECKER:
                            PublishChitChat(ziotSonoff.mainTopic);
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

#endif  // FIRMWARE_ZIOT_MINIMAL

/*********************************************************************************************\
* Interface
\*********************************************************************************************/

bool Xsns89(uint8_t function)
{
    bool result = false;

    ZIoTHandler(function);

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
                InitZIoT(ziotSonoff.version, ziotSonoff.schemeVersion, ziotSonoff.vendor, ziotSonoff.thingType, ziotSonoff.mainTopic, ziotSonoff.shadowTopic, StartTimerHello, StartTimerChitChat, ClearTimerChitChat);
            break;
            case FUNC_COMMAND:
                // printf("topic : %s, data : %s\n", XdrvMailbox.topic, XdrvMailbox.data);
                if (ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].ready) {
                    if (strcmp(XdrvMailbox.topic, "ACCEPTED") == 0) {
                        strcpy(XdrvMailbox.topic, "");
                        ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = false;
                        ziotSonoff.timeoutChecker[HELLO_RESPONSE_CHECKER].count = 0;

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
                    }
                }
                break;
            case FUNC_EVERY_SECOND:
                if (TasmotaGlobal.mqtt_reconnected) {
                    UpdateInitialShadow();
                    TasmotaGlobal.mqtt_reconnected = false;
                }

                CheckTimerList();
                break;
#endif  // FIRMWARE_ZIOT_MINIMAL
        }
    }

    return result;
}

#endif  // FIRMWARE_ZIOT_SONOFF