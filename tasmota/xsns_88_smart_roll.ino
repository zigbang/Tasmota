/*
    xsns_88_smart_roll.ino - ESP32 Temperature and Hall Effect sensor for Tasmota
*/

#define XSNS_88 88

#ifdef FIRMWARE_SMART_ROLL
#include <Arduino.h> //
#include "tasmota.h" //
#include <Ticker.h>

#define MOTOR_DRIVER_PIN1 27
#define MOTOR_DRIVER_PIN2 26
#define MOTOR_DRIVER_EN_PIN 28

#define BUTTON_UP 0x00
#define BUTTON_DOWN 0x01
#define BUTTON_FUNC 0x02
#define BUTTON_RST 0x03

#define DEFAULT_VALUE 0

#define NUMBER_OF_ENTRIES 5

#define D_CMND_SMART_ROLL_CONTROL "CONTROL"
#define D_CMND_SMART_ROLL_REMEMBER "REMEMBER"
#define D_JSON_SUCCESS "SUCCESS"

Ticker TickerSmartRoll;

struct HeightMemory
{
    uint8_t type1;
    uint8_t type2;
    uint8_t type3;
    uint8_t type4;
};

struct SmartRoll
{
    uint8_t id = 0x00;
    uint8_t realPosition = 0;
    uint8_t targetPosition = 0;
    HeightMemory heightMemory = {
        0,
    };
    uint8_t calibTop = 0;
    uint8_t calibBottom = 255;
    uint8_t battery = 100;
    char* version = "1.0.0";
    bool directionUp = false;
    bool ready = false;
    bool moving = false;
} smartRoll;

struct SmartRollEntry
{
    uint32_t name;
    uint16_t start;
    uint16_t len;
};

struct SmartRollDir
{
    SmartRollEntry entry[NUMBER_OF_ENTRIES];
};
SmartRollDir smartRollDir;

enum SmartRollButtonStates
{
    SMART_ROLL_NOT_PRESSED,
    SMART_ROLL_PRESSED_DOUBLE,
    SMART_ROLL_PRESSED_CALIB,
    SMART_ROLL_PRESSED_IMMEDIATE,
    SMART_ROLL_PRESSED_STOP,
    SMART_ROLL_PRESSED_RST,
};

const static uint32_t SMART_ROLL_NAME_ID = 0x20646975;        // 'uid ' little endian
const static uint32_t SMART_ROLL_NAME_REALPOS = 0x20736f70;   // 'pos ' little endian
const static uint32_t SMART_ROLL_NAME_HEIGHTMEM = 0x206d656d; // 'mem ' little endian
const static uint32_t SMART_ROLL_NAME_CALIBTOP = 0x20706f74; // 'top ' little endian
const static uint32_t SMART_ROLL_NAME_CALIBBOTTOM = 0x20746f62; // 'bot ' little endian

const static uint32_t SmartRollEntryNames[NUMBER_OF_ENTRIES] = { SMART_ROLL_NAME_ID, SMART_ROLL_NAME_REALPOS, \
                                                                SMART_ROLL_NAME_HEIGHTMEM, SMART_ROLL_NAME_CALIBTOP, \
                                                                SMART_ROLL_NAME_CALIBBOTTOM };

static uint8_t *smartRollSpiStart = nullptr;
const static size_t smartRollSpiLen = 0x0400;      // 1kb
const static size_t smartRollBlockLen = 0x0400;    // 1kb
const static size_t smartRollBlockOffset = 0x0000; // don't need offset in FS
const static size_t smartRollObjStoreOffset = smartRollBlockOffset + sizeof(SmartRollDir);

const char JSON_SMART_ROLL_TELE[] PROGMEM = "\"" D_PRFX_SMART_ROLL "\":{\"Version\":%d,\"Position\":%d%,\"Battery\":%d%}";

uint8_t ConvertPercentToRealUnit(uint8_t percent)
{
    float temp = percent / 100.0f;
    uint8_t realValue = ((float)(smartRoll.calibBottom - smartRoll.calibTop) * temp) + smartRoll.calibTop;
    return realValue;
}

uint8_t ConvertRealToPercentUnit(uint8_t realValue)
{
}

void CommandFullUp(void)
{
    smartRoll.directionUp = true;
    smartRoll.targetPosition = smartRoll.calibTop;
}

void CommandFullDown(void)
{
    smartRoll.directionUp = false;
    smartRoll.targetPosition = smartRoll.calibBottom;
}

void CommandUp(uint8_t value = DEFAULT_VALUE)
{
    smartRoll.directionUp = true;
    if ((int)(smartRoll.targetPosition - value) > 0)
    {
        (value == DEFAULT_VALUE) ? smartRoll.targetPosition-- : smartRoll.targetPosition -= value;
    }
    else 
    {
        smartRoll.targetPosition = 0;
    }
}

void CommandDown(uint8_t value = DEFAULT_VALUE)
{
    smartRoll.directionUp = false;
    if ((int)(smartRoll.targetPosition + value) < 255)
    {
        (value == DEFAULT_VALUE) ? smartRoll.targetPosition++ : smartRoll.targetPosition += value;
    }
    else
    {
        smartRoll.targetPosition = 255;
    }
}

void CommandStop(void)
{
    smartRoll.targetPosition = smartRoll.realPosition;
}

bool LoadConfigFromFlash(void)
{
    if (smartRollSpiStart == nullptr)
    {
        smartRollSpiStart = (uint8_t *)malloc(smartRollBlockLen);
        if (smartRollSpiStart == nullptr)
        {
            printf("[SmartRoll] free memory is not enough");
            return false;
        }
    }

    if (!OsalLoadNvm("/smart_roll_config", smartRollSpiStart, smartRollBlockLen))
    {
        printf("[SmartRoll] The config file doesn't exist");
    }
    else
    {
        memcpy_P(&smartRollDir, smartRollSpiStart + smartRollBlockOffset, sizeof(smartRollDir));

        if ((smartRollDir.entry[0].name == SMART_ROLL_NAME_ID) && (smartRollDir.entry[0].len > 0))
        {
            memcpy_P(&smartRoll.id, (smartRollSpiStart + smartRollObjStoreOffset + smartRollDir.entry[0].start), sizeof(uint8_t));
        }
        if ((smartRollDir.entry[1].name == SMART_ROLL_NAME_REALPOS) && (smartRollDir.entry[1].len > 0))
        {
            memcpy_P(&smartRoll.realPosition, (smartRollSpiStart + smartRollObjStoreOffset + smartRollDir.entry[1].start), sizeof(uint8_t));
        }
        if ((smartRollDir.entry[2].name == SMART_ROLL_NAME_HEIGHTMEM) && (smartRollDir.entry[2].len > 0))
        {
            memcpy_P(&smartRoll.heightMemory, (smartRollSpiStart + smartRollObjStoreOffset + smartRollDir.entry[2].start), sizeof(HeightMemory));
        }
        if ((smartRollDir.entry[3].name == SMART_ROLL_NAME_CALIBTOP) && (smartRollDir.entry[3].len > 0))
        {
            memcpy_P(&smartRoll.calibTop, (smartRollSpiStart + smartRollObjStoreOffset + smartRollDir.entry[3].start), sizeof(uint8_t));
        }
        if ((smartRollDir.entry[4].name == SMART_ROLL_NAME_CALIBBOTTOM) && (smartRollDir.entry[4].len > 0))
        {
            memcpy_P(&smartRoll.calibBottom, (smartRollSpiStart + smartRollObjStoreOffset + smartRollDir.entry[4].start), sizeof(uint8_t));
        }
    }
    free(smartRollSpiStart);

    return true;
}

bool SaveConfigToFlash(void)
{
    bool result = false;
    uint8_t buffer[5] = {smartRoll.id, smartRoll.realPosition, 0, smartRoll.calibTop, smartRoll.calibBottom};

    uint8_t *spi_buffer = (uint8_t *)malloc(smartRollSpiLen);
    if (!spi_buffer)
    {
        printf("[SmartRoll] free memory is not enough!\n");
        result = false;
    }
    else
    {
        if (smartRollSpiStart != nullptr)
        {
            memcpy_P(spi_buffer, smartRollSpiStart, smartRollSpiLen);
        }
        else
        {
            memset(spi_buffer, 0, smartRollSpiLen);
        }

        SmartRollDir *smart_roll_dir_write;
        smart_roll_dir_write = (SmartRollDir *)(spi_buffer + smartRollBlockOffset);

        for (int i = 0; i < NUMBER_OF_ENTRIES; i++) {
            if (i == 0)
            {
                SmartRollEntry *entry = &smart_roll_dir_write->entry[i];
                entry->name = SmartRollEntryNames[i];
                entry->start = i;
                entry->len = sizeof(uint8_t);
                uint8_t temp = buffer[i];
                memcpy(spi_buffer + smartRollObjStoreOffset + entry->start, &temp, entry->len);
            }
            else if (i == 2)
            {
                SmartRollEntry *entry = &smart_roll_dir_write->entry[i];
                entry->name = SmartRollEntryNames[i];
                entry->start = (smart_roll_dir_write->entry[i - 1].start + smart_roll_dir_write->entry[i - 1].len + 3) & ~0x03;
                entry->len = sizeof(HeightMemory);
                HeightMemory temp = smartRoll.heightMemory;
                memcpy(spi_buffer + smartRollObjStoreOffset + entry->start, &temp, entry->len);
            }
            else
            {
                SmartRollEntry *entry = &smart_roll_dir_write->entry[i];
                entry->name = SmartRollEntryNames[i];
                entry->start = (smart_roll_dir_write->entry[i - 1].start + smart_roll_dir_write->entry[i - 1].len + 3) & ~0x03;
                entry->len = sizeof(uint8_t);
                uint8_t temp = buffer[i];
                memcpy(spi_buffer + smartRollObjStoreOffset + entry->start, &temp, entry->len);
            }
        }

        OsalSaveNvm("/smart_roll_config", spi_buffer, smartRollSpiLen);
        free(spi_buffer);
        result = true;
    }

    return result;
}

void SmartRollInit(void)
{
    smartRoll.ready = false;

    if (!LoadConfigFromFlash())
    {
        printf("[SmartRoll] Failed to load the config from the flash\n");
    }
    else
    {
        printf("[SmartRoll] Successfully loaded the config from the flash\nid: %d, realPosition: %d \
    , heightMemory.type1: %d, heightMemory.type2: %d \
    , heightMemory.type3: %d, heightMemory.type4: %d \
    , calibTop: %d, calibBottom: %d\n",
               smartRoll.id, smartRoll.realPosition, smartRoll.heightMemory.type1, \
               smartRoll.heightMemory.type2, smartRoll.heightMemory.type3, \
               smartRoll.heightMemory.type4, smartRoll.calibTop, smartRoll.calibBottom);

        smartRoll.targetPosition = smartRoll.realPosition;
        
        RotaryInit();

        // TODO: IrInit

        // 모터 드라이버용 핀 out 설정
        pinMode(MOTOR_DRIVER_PIN1, OUTPUT);
        pinMode(MOTOR_DRIVER_PIN2, OUTPUT);
        pinMode(MOTOR_DRIVER_EN_PIN, OUTPUT);

        TickerSmartRoll.attach_ms(50, SmartRollRtc50ms);

        smartRoll.ready = true;
    }

    if (smartRoll.ready)
    {
        printf("[SmartRoll] Successfully initialized Smart Roll\n");
    }
    else
    {
        printf("[SmartRoll] Failed to initialize Smart Roll\n");
    }
}

// Position 계산 및 변환 API

void SmartRollRtc50ms(void) // 타이머 인러텁트 서비스 루틴
{
    if (smartRoll.realPosition != smartRoll.targetPosition)
    {
        smartRoll.moving = true;
        if (smartRoll.realPosition < smartRoll.targetPosition)
        {
            // 모터 제어
            smartRoll.realPosition++; // TODO: 실제로 realPosition 변경은 SmartRollUpdatePosition에서 구현
            printf("Move Down! realPos : %d, targetPos : %d\n", smartRoll.realPosition, smartRoll.targetPosition);
        }
        else if (smartRoll.realPosition > smartRoll.targetPosition)
        {
            // 모터 제어
            smartRoll.realPosition--;
            printf("Move Up! realPos : %d, targetPos : %d\n", smartRoll.realPosition, smartRoll.targetPosition);
        }
    }
    else
    {
        smartRoll.moving = false;
    }
}

void SmartRollButtonHandler(void) // 물리 버튼 핸들러 API
{
    uint8_t buttonState = SMART_ROLL_NOT_PRESSED;
    uint8_t button = XdrvMailbox.payload;
    uint32_t buttonIndex = XdrvMailbox.index;
    uint8_t smartRollIndex = buttonIndex;
    uint16_t loopsPerSec = 1000 / Settings->button_debounce;

    if ((button == PRESSED) && (Button.last_state[buttonIndex] == NOT_PRESSED)) // 버튼이 최초로 눌렸을 때
    {
        if (buttonIndex == BUTTON_FUNC)
        {
            Button.window_timer[BUTTON_FUNC] = loopsPerSec;
        }
        else
        {
            if (Button.window_timer[buttonIndex] == 0)
            {
                if (Button.window_timer[BUTTON_FUNC])
                {
                    buttonState = SMART_ROLL_PRESSED_CALIB;
                    Button.window_timer[BUTTON_FUNC] = 0;
                }
                else
                {
                    buttonState = SMART_ROLL_PRESSED_IMMEDIATE;
                    Button.window_timer[buttonIndex] = (loopsPerSec >> 1);
                }
            }
            else
            {
                buttonState = SMART_ROLL_PRESSED_DOUBLE;
                Button.window_timer[buttonIndex] = 0;
            }
        }
    }

    if (button == NOT_PRESSED) // 버튼이 안눌렸을때
    {
        Button.hold_timer[buttonIndex] = 0;
    }
    else if (Button.last_state[buttonIndex] == PRESSED) // 버튼이 연속적으로 눌리고 있을때
    {
        Button.hold_timer[buttonIndex]++;

        if (smartRollIndex == BUTTON_RST)
        {
            if ((Button.hold_timer[buttonIndex] == loopsPerSec * Settings->param[P_HOLD_TIME] / 10))
            {
                buttonState = SMART_ROLL_PRESSED_RST;
            }
        }
        else
        {
            buttonState = SMART_ROLL_PRESSED_IMMEDIATE;
        }
    }

    if (Button.window_timer[buttonIndex])
    {
        Button.window_timer[buttonIndex]--;
    }

    if (buttonState != SMART_ROLL_NOT_PRESSED)
    {
        switch (buttonState)
        {
        case SMART_ROLL_PRESSED_RST:
            char cmd[20];
            snprintf_P(cmd, sizeof(cmd), PSTR(D_CMND_FACTORY_RESET));
            if (TfsFileExists("/smart_roll_config"))
            {
                TfsDeleteFile("/smart_roll_config");
            }
            ExecuteCommand(cmd, SRC_BUTTON);
            break;
        case SMART_ROLL_PRESSED_DOUBLE:
            if (smartRollIndex == BUTTON_UP)
            {
                CommandFullUp();
            }
            else if (smartRollIndex == BUTTON_DOWN)
            {
                CommandFullDown();
            }
            break;
        case SMART_ROLL_PRESSED_CALIB:
            if (smartRollIndex == BUTTON_UP)
            {
                CommandCalibration(BUTTON_UP);
            }
            else if (smartRollIndex == BUTTON_DOWN)
            {
                CommandCalibration(BUTTON_DOWN);
            }
            break;
        case SMART_ROLL_PRESSED_IMMEDIATE:
            if (smartRollIndex == BUTTON_UP)
            {
                if (smartRoll.moving && !smartRoll.directionUp)
                {
                    CommandStop();
                }
                else
                {
                    CommandUp();
                }
            }
            else if (smartRollIndex == BUTTON_DOWN)
            {
                if (smartRoll.moving && smartRoll.directionUp)
                {
                    CommandStop();
                }
                else
                {
                    CommandDown();
                }
            }
            break;
        }
    }

    Button.last_state[buttonIndex] = button;
}

void SmartRollUpdatePosition(void)
{
}

// 전류 센서 값 Read API

// 엔코더 Wrapping API

// Calibration API
void CommandCalibration(uint8_t direction)
{
    printf("Start the calibration for direction : %d to value : %d\n", direction, smartRoll.realPosition);

    if (direction == BUTTON_UP)
    {
        smartRoll.calibTop = smartRoll.realPosition;
    }
    else
    {
        smartRoll.calibBottom = smartRoll.realPosition;
    }

    SaveConfigToFlash();
}

bool CmndSmartRollControll(void)
{
  char buffer[4];
  bool result = false;

  if (XdrvMailbox.data_len > 0)
  {
    uint8_t value = 0;
    uint8_t converted = 0;

    memcpy_P(buffer, XdrvMailbox.data, sizeof(buffer));

    if (buffer[0] == '-')
    {
        value = atoi(&buffer[1]);
        converted = ConvertPercentToRealUnit(value);
        if (converted != DEFAULT_VALUE)
        {
            CommandUp(converted);
        }
    }
    else if (buffer[0] == 'M')
    {
        switch(buffer[1])
        {
            case '1':
                value = smartRoll.heightMemory.type1;
            break;
            case '2':
                value = smartRoll.heightMemory.type2;
            break;
            case '3':
                value = smartRoll.heightMemory.type3;
            break;
            case '4':
                value = smartRoll.heightMemory.type4;
            break;
            default:
                return result;
        }
        
        if (smartRoll.realPosition > value)
        {
            CommandUp(smartRoll.realPosition - value);
        }
        else if (smartRoll.realPosition < value)
        {
            CommandDown(value - smartRoll.realPosition);
        }
    }
    else
    {
        value = atoi(&buffer[0]);
        converted = ConvertPercentToRealUnit(value);
        if (converted != DEFAULT_VALUE)
        {
            CommandDown(converted);
        }
    }

    result = true;
  }

  return result;
}

bool CmndSmartRollRemember(void)
{
  char buffer[4];
  bool result = false;

  if (XdrvMailbox.data_len > 0)
  {
    memcpy_P(buffer, XdrvMailbox.data, sizeof(buffer));

    switch(buffer[1])
    {
        case '1':
            smartRoll.heightMemory.type1 = smartRoll.realPosition;
        break;
        case '2':
            smartRoll.heightMemory.type2 = smartRoll.realPosition;
        break;
        case '3':
            smartRoll.heightMemory.type3 = smartRoll.realPosition;
        break;
        case '4':
            smartRoll.heightMemory.type4 = smartRoll.realPosition;
        break;
        default:
            return result;
    }

    if (SaveConfigToFlash())
    {
        result = true;
    }
  }

  return result;
}

// MQTT 명령 파싱 API
bool SmartRollMQTTCommand(void)
{
    bool result = false;

    if (strcmp(XdrvMailbox.topic, D_CMND_SMART_ROLL_CONTROL) == 0)
    {
        if (CmndSmartRollControll())
        {
            Response_P(PSTR("{\"" D_CMND_SMART_ROLL_CONTROL "\":\"" D_JSON_SUCCESS "\"}"));
            result = true;
        }
    }
    else if (strcmp(XdrvMailbox.topic, D_CMND_SMART_ROLL_REMEMBER) == 0)
    {
        if (CmndSmartRollRemember())
        {
            Response_P(PSTR("{\"" D_CMND_SMART_ROLL_REMEMBER "\":\"" D_JSON_SUCCESS "\"}"));
            result = true;
        }
    }

    return result;
}

// IR 메시지 파싱 API

// 채널 등록 API

/*********************************************************************************************\
* Interface
\*********************************************************************************************/

bool Xsns88(uint8_t function)
{
    bool result = false;

    if (FUNC_PRE_INIT == function)
    {
        SmartRollInit();
    }
    else if (smartRoll.ready)
    {
        switch (function)
        {
        case FUNC_EVERY_50_MSECOND: // 현재 Position 계산 및 변환, 전류 센서 센싱
            SmartRollUpdatePosition();
            break;
        case FUNC_JSON_APPEND:
            ResponseAppend_P(",");
            // TODO: telemetry 전송 시, version 정보 정확하게 전송되도록 데이터 변환
            ResponseAppend_P(JSON_SMART_ROLL_TELE, smartRoll.version, smartRoll.realPosition, smartRoll.battery);
            break;
        case FUNC_COMMAND: // MQTT 명령 파싱
            result = SmartRollMQTTCommand();
            break;
        case FUNC_BUTTON_PRESSED:
            SmartRollButtonHandler();
            result = true;
            break;
        }
    }
    return result;
}

#endif // FIRMWARE_SMART_ROLL