/*
    xdrv_100_uart_module.ino - ZIoT Tasmota UART module for integration with other mcu
*/

#ifdef FIRMWARE_ZIOT_UART_MODULE
#define XSNS_90 90

#define ZIOT_BUFFER_SIZE 1024
#define ZIOT_BAUDRATE 115200
#define HARDWARE_FALLBACK 2
#define ZIOT_UART_RX_PIN 14
#define ZIOT_UART_TX_PIN 12

#define RESPONSE_TIMEOUT_US 10000000 // 10s
#define MAX_RETRY_COUNT 5

#define HELLO_RESPONSE_CHECKER 0
#define CHITCHAT_RESPONSE_CHECKER 1
#define SHADOW_RESPONSE_CHECKER 2
#define UART_RESPONSE_CHECKER 3
#define MAX_RESPONSE_CHECKER 4

#define NO_SHADOW -1
#define INITIAL_SHADOW 0
#define SWITICH_OFF_SHADOW_WITH_DESIRED 1
#define SWITICH_ON_SHADOW_WITH_DESIRED 2
#define SWITICH_OFF_SHADOW_ONLY_REPORTED 3
#define SWITICH_ON_SHADOW_ONLY_REPORTED 4

#define SWITCH_OFF 0
#define SWITCH_ON 1

#define COMMAND_ERROR -1
#define COMMAND_NULL 0
#define COMMAND_REBOOT 1
#define COMMAND_RESET 2
#define COMMAND_GET_STATUS 3
#define VALUE_ERROR -1
#define VALUE_NULL 0
#define VALUE_RESET_FACTORY 1
#define VALUE_RESET_WIFI 2

#define STATUS_RESPONSE_FULL_DISCONNECTED 0
#define STATUS_RESPONSE_WIFI_CONNECTED 1
#define STATUS_RESPONSE_FULL_CONNECTED 2

#define STATUS_RESPONSE_AP_MODE 0
#define STATUS_RESPONSE_PROVISIONING_MODE 0
#define STATUS_RESPONSE_STATION_MODE 0

#define ERROR_CODE_UNKNOWN 0
#define ERROR_CODE_INTERNAL 1
#define ERROR_CODE_START_PATTERN_MISMATCH 2
#define ERROR_CODE_OVERFLOWED 3
#define ERROR_CODE_RX_TIMEOUT 4
#define ERROR_CODE_WRONG_COMMAND_OR_VALUE 5
#define ERROR_CODE_WRONG_TYPE 6
#define ERROR_CODE_WRONG_FORMAT 7
#define ERROR_CODE_WRONG_CHECKSUM 8
#define ERROR_CODE_TX_EVENT_FAILED 9

#include <TasmotaSerial.h>
TasmotaSerial *ZIoTSerial = nullptr;
static portMUX_TYPE rx_mutex;

#ifndef FIRMWARE_ZIOT_MINIMAL
const char S_JSON_UART_MODULE_SHADOW_WITH_DESIRED[] PROGMEM = "{\"state\":{\"desired\":{\"status\":{\"switch1\":%s}},\"reported\":{\"schemeVersion\":\"%s\",\"vendor\":\"%s\",\"thingType\":\"%s\",\"firmwareVersion\":\"%s\",\"status\":{\"isConnected\":true,\"batteryPercentage\":100,\"switch1\":%s,\"countdown1\":0,\"relayStatus\":\"2\",\"cycleTime\":\"\",\"switchInching\":\"\"},\"statusesLast\":[%s]}}}";
const char S_JSON_UART_MODULE_SHADOW[] PROGMEM = "{\"state\":{\"reported\":{\"schemeVersion\":\"%s\",\"vendor\":\"%s\",\"thingType\":\"%s\",\"firmwareVersion\":\"%s\",\"status\":{\"isConnected\":true,\"batteryPercentage\":100,\"switch1\":%s,\"countdown1\":0,\"relayStatus\":\"2\",\"cycleTime\":\"\",\"switchInching\":\"\"},\"statusesLast\":[%s]}}}";
const char S_JSON_UART_MODULE_RESPONSE[] PROGMEM = "{\"type\":\"resp\",\"data\":{\"cmnd\":\"%s\",\"val\":%s}}";
const char S_JSON_UART_MODULE_RESPONSE_STAT[] PROGMEM = "{\"type\":\"resp\",\"data\":{\"cmnd\":\"%s\",\"val\":{\"mode\":%c,\"con\":%c}}}";
const char S_JSON_UART_MODULE_RESPONSE_STAT_FULL[] PROGMEM = "{\"type\":\"resp\",\"data\":{\"cmnd\":\"%s\",\"val\":{\"mode\":%c,\"con\":%c,\"ssid\":\"%s\",\"bssid\":\"%s\",\"ip\":\"%s\",\"gw\":\"%s\"}}}";
const char S_JSON_UART_MODULE_RESPONSE_ERROR[] PROGMEM = "{\"type\":\"resp\",\"data\":{\"cmnd\":\"%s\",\"val\":{\"errCode\":\"%d\",\"errMsg\":\"%s\"}}}";
const char S_JSON_UART_MODULE_REQUEST[] PROGMEM = "{\"type\":\"%s\",\"data\":%s}";

void InitPacketQueue(PacketQueue *queue)
{
    queue->front = queue->rear = nullptr;
    queue->count = 0;
}

bool IsQueueEmpty(PacketQueue *queue)
{
    return queue->count == 0;
}

void Enqueue(PacketQueue *queue, uint8_t seq, uint8_t ack, char *data, int length)
{
    Packet *newPacket = (Packet *)malloc(sizeof(Packet));
    newPacket->seq = seq;
    newPacket->ack = ack;
    newPacket->data = (char *)malloc(length);
    newPacket->length = length;
    strncpy(newPacket->data, data, length);
    newPacket->next = nullptr;

    if (IsQueueEmpty(queue))
    {
        queue->front = newPacket;
    }
    else
    {
        queue->rear->next = newPacket;
    }
    queue->rear = newPacket;
    queue->count++;
}

void Dequeue(PacketQueue *queue, uint8_t *seq = nullptr, uint8_t *ack = nullptr, char *dest = nullptr, int *length = nullptr)
{
    Packet *ptr;
    if (IsQueueEmpty(queue))
    {
        printf("[ERR] PacketQueue is empty\n");
        ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_INTERNAL, "Internal error");
        return;
    }
    ptr = queue->front;
    if (seq != nullptr)
    {
        *seq = ptr->seq;
    }
    if (ack != nullptr)
    {
        *ack = ptr->ack;
    }
    if (dest != nullptr)
    {
        strncpy(dest, ptr->data, ptr->length);
    }
    if (length != nullptr)
    {
        *length = ptr->length;
    }
    queue->front = ptr->next;
    free(ptr);
    queue->count--;
}

void EmptyQueue(PacketQueue *queue)
{
    while (!IsQueueEmpty(queue))
    {
        Dequeue(queue);
    }
}

struct TimeoutChecker
{
    uint32_t startTime = 0;
    uint8_t count = 0;
    bool ready = false;
    uint8_t seq = 1;
};
#endif // FIRMWARE_ZIOT_MINIMAL

struct ZIoTUART
{
    bool ready = false;
    char *version = "1.0.10";
    char *recvBuffer = nullptr;
    int recvLength = 0;
    uint32_t rxTime = 0;
    uint8_t seq = 1;
    uint8_t ack = 1;
    bool startFlag = false;
    char *sendBuffer = nullptr;
    PacketQueue packetQueue;
#ifndef FIRMWARE_ZIOT_MINIMAL
    char mainTopic[60];
    char shadowTopic[70];
    char *schemeVersion = "v220530";
    char *vendor = "zigbang";
    char *thingType = "Switch";
    int8_t lastShadow = NO_SHADOW;
    TimeoutChecker timeoutChecker[MAX_RESPONSE_CHECKER];
    uint32_t second = 0;
    bool needUpdateInitialShadow = false;
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

// TODO: 여러개의 Response에 대한 CheckTimer 구현

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
                    printf("[Error] %d is reached to the max retry count. Reboot the device\n", i);
                    SettingsSaveAll();
                    EspRestart();
                }
                else
                {
                    ziotUart.timeoutChecker[i].count++;
                    ziotUart.timeoutChecker[i].startTime = GetUsTime();

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
                            // case SWITICH_OFF_SHADOW_ONLY_REPORTED:
                            //     UpdateShadowOnlyReported(SWITCH_OFF);
                            //     break;
                            // case SWITICH_ON_SHADOW_ONLY_REPORTED:
                            //     UpdateShadowOnlyReported(SWITCH_ON);
                            //     break;
                            // case SWITICH_OFF_SHADOW_WITH_DESIRED:
                            //     UpdateShadowWithDesired(SWITCH_OFF);
                            //     break;
                            // case SWITICH_ON_SHADOW_WITH_DESIRED:
                            //     UpdateShadowWithDesired(SWITCH_ON);
                            //     break;
                        default:
                            break;
                        }
                        break;
                    case UART_RESPONSE_CHECKER:
                        printf("[DBG] Response didn't arrive in 10s\n");
                        printf("[DBG] Send event to mcu again : %s\n", ziotUart.sendBuffer);
                        ZIoTSerial->write((const char *)ziotUart.sendBuffer);
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

#endif // FIRMWARE_ZIOT_MINIMAL

void FlushBuffer(void)
{
    ZIoTSerial->flush();
    ziotUart.recvLength = 0;
    ziotUart.startFlag = false;
    memset(ziotUart.recvBuffer, 0, ZIOT_BUFFER_SIZE);
}

void UartModuleInit(void)
{
    if (ziotUart.ready != true)
    {
        if (ziotUart.recvBuffer == nullptr)
        {
            ziotUart.recvBuffer = (char *)malloc(ZIOT_BUFFER_SIZE);
        }

        if (ziotUart.sendBuffer == nullptr)
        {
            ziotUart.sendBuffer = (char *)malloc(ZIOT_BUFFER_SIZE);
        }

        if (ziotUart.recvBuffer != nullptr && ziotUart.sendBuffer)
        {
            ZIoTSerial = new TasmotaSerial(ZIOT_UART_RX_PIN, ZIOT_UART_TX_PIN, HARDWARE_FALLBACK);
            if (ZIoTSerial->begin(ZIOT_BAUDRATE))
            {
                FlushBuffer();
                ziotUart.ready = true;
            }
        }
    }
}

void UartInputHandler(void *arg)
{
    while (true)
    {
        while (ZIoTSerial->available())
        {
            portENTER_CRITICAL(&rx_mutex);
            ziotUart.recvLength += ZIoTSerial->read(ziotUart.recvBuffer + ziotUart.recvLength, ZIOT_BUFFER_SIZE);
            portEXIT_CRITICAL(&rx_mutex);
        }
    }
}

void AnalyzePacket(void *arg)
{
    while (true)
    {
        portENTER_CRITICAL(&rx_mutex);
        int recvLength = ziotUart.recvLength;
        char startByte1 = ziotUart.recvBuffer[0];
        char startByte2 = ziotUart.recvBuffer[1];
        uint8_t seqNumber = ziotUart.recvBuffer[2];
        uint8_t ackNumber = ziotUart.recvBuffer[3];
        char checksumByte = ziotUart.recvBuffer[recvLength - 3];
        char endByte1 = ziotUart.recvBuffer[recvLength - 2];
        char endByte2 = ziotUart.recvBuffer[recvLength - 1];
        portEXIT_CRITICAL(&rx_mutex);

        if (recvLength >= 2)
        {
            if (!ziotUart.startFlag)
            {
                if (startByte1 == 'Z' && startByte2 == 'b')
                {
                    ziotUart.rxTime = GetUsTime();
                    ziotUart.startFlag = true;
                }
                else
                {
                    printf("[ERR] Start pattern mismatch!\n");
                    ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_START_PATTERN_MISMATCH, "Start pattern mismatch error");
                    printf("[DBG] Received start pattern is %x, %x\n", startByte1, startByte2);
                    portENTER_CRITICAL(&rx_mutex);
                    FlushBuffer();
                    portEXIT_CRITICAL(&rx_mutex);
                    continue;
                }
            }

            if (endByte1 == '\r' && endByte2 == '\n')
            {
                if (recvLength <= 10)
                {
                    printf("[ERR] Wrong uart protocol\n");
                    ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_WRONG_FORMAT, "Wrong json format error");
                    portENTER_CRITICAL(&rx_mutex);
                    FlushBuffer();
                    portEXIT_CRITICAL(&rx_mutex);
                    continue;
                }
                else
                {
                    char data[ZIOT_BUFFER_SIZE] = "";
                    portENTER_CRITICAL(&rx_mutex);
                    memcpy(data, ziotUart.recvBuffer + 2, recvLength - 5);
                    portEXIT_CRITICAL(&rx_mutex);

                    char expectedChecksum = CalculateChecksum(data, recvLength - 5);

                    if (expectedChecksum != checksumByte)
                    {
                        printf("[ERR] Checksum byte mismatch! expected : %x, but received : %x\n", expectedChecksum, checksumByte);
                        ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_WRONG_CHECKSUM, "Wrong checksum error");
                        portENTER_CRITICAL(&rx_mutex);
                        FlushBuffer();
                        portEXIT_CRITICAL(&rx_mutex);
                        continue;
                    }
                    else
                    {
                        Enqueue(&ziotUart.packetQueue, seqNumber, ackNumber, data + 2, recvLength - 7);
                        portENTER_CRITICAL(&rx_mutex);
                        FlushBuffer();
                        portEXIT_CRITICAL(&rx_mutex);
                        continue;
                    }
                }
            }

            if (IsTimeout(ziotUart.rxTime, 5000000))
            {
                printf("[ERR] Rx timeout\n");
                ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_RX_TIMEOUT, "Rx timeout error");
                portENTER_CRITICAL(&rx_mutex);
                FlushBuffer();
                portEXIT_CRITICAL(&rx_mutex);
                continue;
            }

            if (recvLength >= ZIOT_BUFFER_SIZE)
            {
                printf("[ERR] Buffer overflowd\n");
                ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_OVERFLOWED, "Buffer overflowed error");
                portENTER_CRITICAL(&rx_mutex);
                FlushBuffer();
                portEXIT_CRITICAL(&rx_mutex);
                continue;
            }
        }
    }
}

void ParsePacket(void)
{
    if (!IsQueueEmpty(&ziotUart.packetQueue))
    {
        uint8_t rxSeq = 1;
        uint8_t rxAck = 1;
        char data[ZIOT_BUFFER_SIZE] = "";
        char forJson[ZIOT_BUFFER_SIZE] = "";
        int length = 0;

        // TODO: PacketQueue seq 기반 sorting 구현
        Dequeue(&ziotUart.packetQueue, &rxSeq, &rxAck, data, &length);

        ziotUart.ack = rxSeq;
        memcpy(forJson, data, length);

        JsonParser parser(forJson);
        JsonParserObject root = parser.getRootObject();
        String type = root["type"].getStr();
        char *typeChar = (char *)type.c_str();

        if (strlen(typeChar) != 0)
        {
            if (strcmp(typeChar, "ctrl") == 0)
            {
                JsonParserObject data = root["data"].getObject();
                String command = data["cmnd"].getStr();
                String value = data["val"].getStr();

                UartCommandStruct commands;
                commands = DecodeCommand((char *)command.c_str(), (char *)value.c_str());

                if (!ExecuteCommand(commands.command, commands.value))
                {
                    printf("[ERR] Wrong packet command or packet value\n");
                    ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_WRONG_COMMAND_OR_VALUE, "Wrong command or value error");
                }
            }
            else if (strcmp(typeChar, "evt") == 0)
            {
                char temp[ZIOT_BUFFER_SIZE] = "";
                memcpy(temp, data + 21, length - 22);

                if (TasmotaGlobal.ziot_mode != STATION_MODE || TasmotaGlobal.global_state.wifi_down || TasmotaGlobal.global_state.mqtt_down)
                {
                    printf("[ERR] Can't send event message. Network is not connected to cloud\n");
                    ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "evt", ERROR_CODE_TX_EVENT_FAILED, "Network is not connected to cloud");
                } else {
                    HandleEventTypePacket(temp);
                }
            }
            else if (strcmp(typeChar, "resp") == 0)
            {
                if (ziotUart.timeoutChecker[UART_RESPONSE_CHECKER].ready && (ziotUart.timeoutChecker[UART_RESPONSE_CHECKER].seq == rxAck)) {
                    ziotUart.timeoutChecker[UART_RESPONSE_CHECKER].ready = false;
                    ziotUart.timeoutChecker[UART_RESPONSE_CHECKER].count = 0;
                    printf("[DBG] Received response : %s\n", data);

                    portENTER_CRITICAL(&rx_mutex);
                    memset(ziotUart.sendBuffer, 0, ZIOT_BUFFER_SIZE);
                    portEXIT_CRITICAL(&rx_mutex);
                }
            }
            else
            {
                printf("[ERR] Wrong packet type\n");
                ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_WRONG_TYPE, "Wrong type error");
            }
        }
        else
        {
            printf("[ERR] Wrong uart protocol\n");
            ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_ERROR, "null", ERROR_CODE_WRONG_FORMAT, "Wrong json format error");
        }
    }
}

UartCommandStruct DecodeCommand(char *command, char *value)
{
    UartCommandStruct result;

    if ((strlen(command) == 0) || strlen(value) == 0)
    {
        result.command = COMMAND_ERROR;
        result.value = VALUE_ERROR;
    }
    else
    {
        if (strcmp(command, "rbt") == 0)
        {
            result.command = COMMAND_REBOOT;
            result.value = VALUE_NULL;
        }
        else if (strcmp(command, "rst") == 0)
        {
            result.command = COMMAND_RESET;
            if (strcmp(value, "fty") == 0)
            {
                result.value = VALUE_RESET_FACTORY;
            }
            else if (strcmp(value, "wifi") == 0)
            {
                result.value = VALUE_RESET_WIFI;
            }
            else
            {
                result.value = VALUE_ERROR;
            }
        }
        else if (strcmp(command, "stat") == 0)
        {
            result.command = COMMAND_GET_STATUS;
            result.value = VALUE_NULL;
        }
        else
        {
            result.command = COMMAND_ERROR;
            result.value = VALUE_ERROR;
        }
    }

    return result;
}

bool ExecuteCommand(int8_t command, int8_t value)
{
    bool result = false;

    if ((command != COMMAND_ERROR) && (value != VALUE_ERROR))
    {
        switch (command)
        {
        case COMMAND_REBOOT:
            ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE, "rbt", "\"null\"");
            TasmotaGlobal.restart_flag = 2;
            result = true;
            break;
        case COMMAND_RESET:
            if (value == VALUE_RESET_FACTORY)
            {
                ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE, "rst", "\"fty\"");
                TasmotaGlobal.restart_flag = 212;
            }
            else if (value == VALUE_RESET_WIFI)
            {
                ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE, "rst", "\"wifi\"");
                SettingsUpdateText(SET_STASSID1, "");
                SettingsUpdateText(SET_STAPWD1, "");
                SettingsUpdateText(SET_STASSID2, "");
                SettingsUpdateText(SET_STAPWD2, "");
            }
            result = true;
            break;
        case COMMAND_GET_STATUS:
        {
            char mode = '0';
            char connection = '0';

            if (TasmotaGlobal.ziot_mode == AP_MODE)
            {
                mode = '0';
            }
            else if (TasmotaGlobal.ziot_mode == PROVISIONING_MODE)
            {
                mode = '1';
            }
            else
            {
                mode = '2';
            }

            if (TasmotaGlobal.global_state.wifi_down)
            {
                connection = '0';
            }
            else if (!TasmotaGlobal.isCloudConnected)
            {
                connection = '1';
            }
            else
            {
                connection = '2';
            }

            if (ziot.wifiConfigured)
            {
                ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_STAT_FULL, "stat", mode, connection, wifiConfig.ssid, wifiConfig.bssid, wifiConfig.ipAddr, wifiConfig.gatewayAddr);
            }
            else
            {
                ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE_STAT, "stat", mode, connection);
            }
        }
            result = true;
            break;
        default:
            break;
        }
    }

    return result;
}

void ResponsePacketMaker(char *format, ...)
{
    va_list arg;
    char sendBuffer[ZIOT_BUFFER_SIZE];
    uint8_t seq = 1;
    uint8_t ack = ziotUart.ack;

    portENTER_CRITICAL(&rx_mutex);
    if (ziotUart.seq == 255) {
        ziotUart.seq = 1;
    }
    seq = ziotUart.seq++;
    portEXIT_CRITICAL(&rx_mutex);

    sendBuffer[0] = 'Z';
    sendBuffer[1] = 'b';
    sendBuffer[2] = seq;
    sendBuffer[3] = ack;

    va_start(arg, format);
    vsprintf(sendBuffer + 4, format, arg);
    va_end(arg);

    int length = strlen(sendBuffer);
    char checksumByte = CalculateChecksum(sendBuffer + 2, length - 2);

    sendBuffer[length] = checksumByte;
    sendBuffer[length + 1] = '\r';
    sendBuffer[length + 2] = '\n';
    sendBuffer[length + 3] = '\0';

    printf("[DBG] Send response : %s\n", sendBuffer);
    ZIoTSerial->write((const char *)sendBuffer);
}

void RequestPacketMaker(char *format, ...)
{
    va_list arg;
    char sendBuffer[ZIOT_BUFFER_SIZE];
    uint8_t seq = 1;
    uint8_t ack = ziotUart.ack;

    portENTER_CRITICAL(&rx_mutex);
    if (ziotUart.seq == 255) {
        ziotUart.seq = 1;
    }
    seq = ziotUart.seq++;
    portEXIT_CRITICAL(&rx_mutex);

    sendBuffer[0] = 'Z';
    sendBuffer[1] = 'b';
    sendBuffer[2] = seq;
    sendBuffer[3] = ack;

    va_start(arg, format);
    vsprintf(sendBuffer + 4, format, arg);
    va_end(arg);

    int length = strlen(sendBuffer);
    char checksumByte = CalculateChecksum(sendBuffer + 2, length - 2);

    sendBuffer[length] = checksumByte;
    sendBuffer[length + 1] = '\r';
    sendBuffer[length + 2] = '\n';
    sendBuffer[length + 3] = '\0';

    printf("[DBG] Send request : %s\n", sendBuffer);

    portENTER_CRITICAL(&rx_mutex);
    ziotUart.timeoutChecker[UART_RESPONSE_CHECKER].startTime = GetUsTime();
    ziotUart.timeoutChecker[UART_RESPONSE_CHECKER].ready = true;
    ziotUart.timeoutChecker[UART_RESPONSE_CHECKER].seq = seq;

    memcpy(ziotUart.sendBuffer, sendBuffer, ZIOT_BUFFER_SIZE);
    portEXIT_CRITICAL(&rx_mutex);

    ZIoTSerial->write((const char *)sendBuffer);
}

void HandleEventTypePacket(char *data)
{
    printf("[DBG] Send event to cloud : %s\n", data);
    // TODO: ziot uart module topic and postfix 정의
    PublishTopicWithPostfix(data, "ziot-uart-module", "/event");
    ResponsePacketMaker((char *)S_JSON_UART_MODULE_RESPONSE, "evt", "null");
}

char CalculateChecksum(char *data, int length)
{
    uint32_t checksum = 0;

    for (int i = 0; i < length; i++)
    {
        checksum += data[i];
    }

    checksum %= 256;

    return (char)checksum;
}

bool Xsns90(uint8_t function)
{
    bool result = false;

    ZIoTHandler(function);

    if (FUNC_PRE_INIT == function)
    {
        InitPacketQueue(&ziotUart.packetQueue);
        UartModuleInit();
    }
    else if (ziotUart.ready)
    {
        switch (function)
        {
#ifndef FIRMWARE_ZIOT_MINIMAL
        case FUNC_INIT:
            vPortCPUInitializeMutex(&rx_mutex);
            xTaskCreate(UartInputHandler, "uart_rx_task", 1024 * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
            xTaskCreate(AnalyzePacket, "analyze_packet_task", 1024 * 4, NULL, tskIDLE_PRIORITY, NULL);

            snprintf_P(ziotUart.mainTopic, sizeof(ziotUart.mainTopic), PSTR("ziot/sonoff/basicr2/%s"),
                       SettingsText(SET_MQTT_TOPIC));
            snprintf_P(ziotUart.shadowTopic, sizeof(ziotUart.shadowTopic), PSTR("$aws/things/%s/shadow/update"),
                       SettingsText(SET_MQTT_TOPIC));
            InitZIoT(ziotUart.version, ziotUart.schemeVersion, ziotUart.vendor, ziotUart.thingType, ziotUart.mainTopic, ziotUart.shadowTopic, StartTimerHello, StartTimerChitChat, ClearTimerChitChat);
            break;
        case FUNC_COMMAND:
            // printf("topic : %s\ndata : %s\n", XdrvMailbox.topic, XdrvMailbox.data);
            if (ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].ready)
            {
                if (strcmp(XdrvMailbox.topic, "ACCEPTED") == 0)
                {
                    strcpy(XdrvMailbox.topic, "");
                    ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].ready = false;
                    ziotUart.timeoutChecker[HELLO_RESPONSE_CHECKER].count = 0;

                    ziotUart.needUpdateInitialShadow = true;
                    printf("[DBG] Successfully registered this moudle as cloud thing\n");
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
                        if (ziotUart.lastShadow == INITIAL_SHADOW) {
                            TasmotaGlobal.isCloudConnected = true;
                            printf("[DBG] Successfully updated the initial shadow\n");
                        }

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
                    else if (strcmp(XdrvMailbox.topic, "DATA") == 0)
                    {
                        // TODO: event에 대한 토픽 이름 정의
                        printf("[DBG] Send event to mcu : %s\n", XdrvMailbox.data);
                        RequestPacketMaker((char *)S_JSON_UART_MODULE_REQUEST, "evt", XdrvMailbox.data);
                    }
                }
            }
            break;
        case FUNC_LOOP:
            if (ZIoTSerial)
            {
                ParsePacket();
            }
            break;
        case FUNC_EVERY_SECOND:
            if (!TasmotaGlobal.global_state.mqtt_down && ziotUart.needUpdateInitialShadow) {
                ziotUart.second++;
                if (ziotUart.second >= 10) {
                    UpdateInitialShadow();

                    ziotUart.needUpdateInitialShadow = false;
                    ziotUart.second = 0;
                }
            }

            if (TasmotaGlobal.mqtt_reconnected) {
                TasmotaGlobal.mqtt_reconnected = false;
                ziotUart.needUpdateInitialShadow = true;
                ziotUart.second = 9;
            }

            CheckTimerList();
            break;
#endif // FIRMWARE_ZIOT_MINIMAL
        }
    }

    return result;
}

#endif // FIRMWARE_ZIOT_UART_MODULE