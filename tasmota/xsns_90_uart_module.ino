/*
    xdrv_100_uart_module.ino - ZIoT Tasmota UART module for integration with other mcu
*/

#ifdef FIRMWARE_ZIOT_UART_MODULE
#define XSNS_90 90

#define ZIOT_BUFFER_SIZE 256
#define ZIOT_BAUDRATE 115200
#define HARDWARE_FALLBACK 2
#define ZIOT_UART_RX_PIN 14
#define ZIOT_UART_TX_PIN 12

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

#include <TasmotaSerial.h>
TasmotaSerial *ZIoTSerial = nullptr;

#ifndef FIRMWARE_ZIOT_MINIMAL
const char S_JSON_UART_MODULE_SHADOW_WITH_DESIRED[] PROGMEM = "{\"state\":{\"desired\":{\"status\":{\"switch1\":%s}},\"reported\":{\"schemeVersion\":\"%s\",\"vendor\":\"%s\",\"thingType\":\"%s\",\"firmwareVersion\":\"%s\",\"status\":{\"isConnected\":true,\"batteryPercentage\":100,\"switch1\":%s,\"countdown1\":0,\"relayStatus\":\"2\",\"cycleTime\":\"\",\"switchInching\":\"\"},\"statusesLast\":[%s]}}}";
const char S_JSON_UART_MODULE_SHADOW[] PROGMEM = "{\"state\":{\"reported\":{\"schemeVersion\":\"%s\",\"vendor\":\"%s\",\"thingType\":\"%s\",\"firmwareVersion\":\"%s\",\"status\":{\"isConnected\":true,\"batteryPercentage\":100,\"switch1\":%s,\"countdown1\":0,\"relayStatus\":\"2\",\"cycleTime\":\"\",\"switchInching\":\"\"},\"statusesLast\":[%s]}}}";

struct TimeoutChecker
{
    uint32_t startTime = 0;
    uint8_t count = 0;
    bool ready = false;
};
#endif // FIRMWARE_ZIOT_MINIMAL

struct ZIoTUART
{
    bool ready = false;
    char *version = "1.0.8";
    char *buffer = nullptr;
#ifndef FIRMWARE_ZIOT_MINIMAL
    char mainTopic[60];
    char shadowTopic[70];
    char *schemeVersion = "v220530";
    char *vendor = "sonoff";
    char *thingType = "relay";
    int8_t lastShadow = NO_SHADOW;
    TimeoutChecker timeoutChecker[3];
    uint32_t second = 0;
    bool needUpdateInitialShadow = true;
#endif // FIRMWARE_ZIOT_MINIMAL
} ziotUart;

#ifndef FIRMWARE_ZIOT_MINIMAL
void StartTimerHello(void)
{
    ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = true;
}

void StartTimerChitChat(void)
{
    ziotUart.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotUart.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].ready = true;
}

void ClearTimerChitChat(void)
{
    ziotUart.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].ready = false;
    ziotUart.timeoutChecker[CHITCHAT_RESPONSE_CHECKER].count = 0;
}

void CheckTimerList(void)
{
    for (uint8_t i = 0; i < MAX_RESPONSE_CHECKER; i++)
    {
        if (ziotUart.timeoutChecker[i].ready)
        {
            if (IsTimeout(ziotUart.timeoutChecker[i].startTime, RESPONSE_TIMEOUT_US))
            {
                if (ziotUart.timeoutChecker[i].count == MAX_RETRY_COUNT)
                {
                    printf("%d is reached to the max retry count!!\n", i);
                    SettingsSaveAll();
                    EspRestart();
                }
                else
                {
                    ziotUart.timeoutChecker[i].count++;
                    switch (i)
                    {
                    case HELLO_RESPONSE_CHECKER:
                        PublishHello(ziotUart.mainTopic);
                        break;
                    case CHITCHAT_RESPONSE_CHECKER:
                        PublishChitChat(ziotUart.mainTopic);
                        break;
                    case SHADOW_RESPONSE_CHECKER:
                        switch (ziotUart.lastShadow)
                        {
                        case NO_SHADOW:
                            ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = false;
                            ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].count = 0;
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

    if (bitRead(TasmotaGlobal.power, 0) == 0)
    {
        strcpy(switch1, "false");
    }
    else
    {
        strcpy(switch1, "true");
    }

    char *code[1] = {"switch1"};
    char *value[1] = {switch1};
    MakeStatusesLastFormat(1, code, value, statusesLast);

    ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
    ziotUart.lastShadow = INITIAL_SHADOW;

    UpdateShadow(ziotUart.shadowTopic, (char *)S_JSON_UART_MODULE_SHADOW, ziotUart.schemeVersion, ziotUart.vendor, ziotUart.thingType, ziotUart.version, switch1, statusesLast);
}

void UpdateShadowWithDesired(bool switchValue)
{
    char statusesLast[80] = "";
    char *code[1] = {"switch1"};
    char *value[1] = {""};

    if (switchValue == SWITCH_OFF)
    {
        strcpy(value[0], "false");
        MakeStatusesLastFormat(1, code, value, statusesLast);

        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotUart.lastShadow = SWITICH_OFF_SHADOW_WITH_DESIRED;
    }
    else
    {
        strcpy(value[0], "true");
        MakeStatusesLastFormat(1, code, value, statusesLast);

        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotUart.lastShadow = SWITICH_ON_SHADOW_WITH_DESIRED;
    }
    UpdateShadow(ziotUart.shadowTopic, (char *)S_JSON_UART_MODULE_SHADOW_WITH_DESIRED, value[0], ziotUart.schemeVersion, ziotUart.vendor, ziotUart.thingType, ziotUart.version, value[0], statusesLast);
}

void UpdateShadowOnlyReported(bool switchValue)
{
    char statusesLast[80] = "";
    char *code[1] = {"switch1"};
    char *value[1] = {""};

    if (switchValue == SWITCH_OFF)
    {
        strcpy(value[0], "false");
        MakeStatusesLastFormat(1, code, value, statusesLast);

        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotUart.lastShadow = SWITICH_OFF_SHADOW_ONLY_REPORTED;
    }
    else
    {
        strcpy(value[0], "true");
        MakeStatusesLastFormat(1, code, value, statusesLast);

        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = true;
        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
        ziotUart.lastShadow = SWITICH_ON_SHADOW_ONLY_REPORTED;
    }
    UpdateShadow(ziotUart.shadowTopic, (char *)S_JSON_UART_MODULE_SHADOW, ziotUart.schemeVersion, ziotUart.vendor, ziotUart.thingType, ziotUart.version, value[0], statusesLast);
}

#endif  // FIRMWARE_ZIOT_MINIMAL

void UartModuleInit(void)
{
    ziotUart.buffer = (char *)(malloc(ZIOT_BUFFER_SIZE));
    if (ziotUart.buffer != nullptr)
    {
        ZIoTSerial = new TasmotaSerial(ZIOT_UART_RX_PIN, ZIOT_UART_TX_PIN, HARDWARE_FALLBACK);
        if (ZIoTSerial->begin(ZIOT_BAUDRATE))
        {
            ZIoTSerial->flush();
            ziotUart.ready = true;
        }
    }
}

void UartModuleInputHandler(void)
{
    while (ZIoTSerial->available())
    {
        yield();
        uint8_t serial_in_byte = ZIoTSerial->read();
        if (serial_in_byte)
        {
            ZIoTSerial->write(serial_in_byte);
        }
    }
}

void DecodeUartCommand(void)
{
}

void ParseUartProtocol(void)
{
}

void BypassJsonData(void)
{
}

bool Xsns90(uint8_t function)
{
    bool result = false;

    ZIoTHandler(function);

    if (FUNC_PRE_INIT == function)
    {
        UartModuleInit();
    }
    else if (ziotUart.ready)
    {
        switch (function)
        {
#ifndef FIRMWARE_ZIOT_MINIMAL
        case FUNC_INIT:
            snprintf_P(ziotUart.mainTopic, sizeof(ziotUart.mainTopic), PSTR("ziot/sonoff/basicr2/%s"),
                       SettingsText(SET_MQTT_TOPIC));
            snprintf_P(ziotUart.shadowTopic, sizeof(ziotUart.shadowTopic), PSTR("$aws/things/%s/shadow/update"),
                       SettingsText(SET_MQTT_TOPIC));
            InitZIoT(ziotUart.version, ziotUart.schemeVersion, ziotUart.vendor, ziotUart.thingType, ziotUart.mainTopic, StartTimerHello, StartTimerChitChat, ClearTimerChitChat);
            break;
        case FUNC_COMMAND:
            printf("topic : %s\ndata : %s\n", XdrvMailbox.topic, XdrvMailbox.data);
            if (ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].ready)
            {
                if (strcmp(XdrvMailbox.topic, "ACCEPTED") == 0)
                {
                    strcpy(XdrvMailbox.topic, "");
                    ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = false;
                    ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].count = 0;

                    SubscribeTopicWithPostfix(ziotUart.shadowTopic, "/delta");
                    SubscribeTopicWithPostfix(ziotUart.shadowTopic, "/rejected");
                    SubscribeTopicWithPostfix(ziotUart.shadowTopic, "/accepted");
                    SubscribeTopicWithPostfix(ziotUart.mainTopic, "/chitchat/res");
                }
                else if (strcmp(XdrvMailbox.topic, "REJECTED") == 0)
                {
                    MqttDisconnect();
                    ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = false;
                    ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].count = 0;
                    TasmotaGlobal.restart_flag = 212;
                }
            }
            else
            {
                if (ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready)
                {
                    if (strcmp(XdrvMailbox.topic, "REJECTED") == 0)
                    {
                        strcpy(XdrvMailbox.topic, "");
                        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].count = 0;
                        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].startTime = GetUsTime();
                        switch (ziotUart.lastShadow)
                        {
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
                    else
                    {
                        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].ready = false;
                        ziotUart.timeoutChecker[SHADOW_RESPONSE_CHECKER].count = 0;
                        ziotUart.lastShadow = NO_SHADOW;
                    }
                }
                else
                {
                    if (strcmp(XdrvMailbox.topic, "DELTA") == 0)
                    {
                        strcpy(XdrvMailbox.topic, "");

                        JsonParser parser(XdrvMailbox.data);

                        JsonParserObject root = parser.getRootObject();
                        JsonParserObject state = root["state"].getObject();
                        JsonParserObject status = state["status"].getObject();

                        String switch1 = status["switch1"].getStr();
                        char *switch1CharType = (char *)switch1.c_str();

                        if ((strcmp(switch1CharType, "true") == 0) && bitRead(TasmotaGlobal.power, 0) == 0)
                        {
                            SetAllPower(POWER_TOGGLE_NO_STATE, SRC_MQTT);
                            UpdateShadowOnlyReported(SWITCH_ON);
                        }
                        else if ((strcmp(switch1CharType, "false") == 0) && bitRead(TasmotaGlobal.power, 0) == 1)
                        {
                            SetAllPower(POWER_TOGGLE_NO_STATE, SRC_MQTT);
                            UpdateShadowOnlyReported(SWITCH_OFF);
                        }
                    }
                }
            }
            break;
        case FUNC_LOOP:
            if (ZIoTSerial)
            {
                UartModuleInputHandler();
            }
            break;
        case FUNC_EVERY_SECOND:
            ziotUart.second++;
            if (ziotUart.second > 80) {
                if (ziotUart.needUpdateInitialShadow) {
                    UpdateInitialShadow();
                    ziotUart.needUpdateInitialShadow = false;
                }
                ziotUart.second = 0;
            }
            CheckTimerList();
            break;
#endif // FIRMWARE_ZIOT_MINIMAL
        }
    }

    return result;
}

#endif // FIRMWARE_ZIOT_UART_MODULE