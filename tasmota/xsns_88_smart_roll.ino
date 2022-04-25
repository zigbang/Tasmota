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
#define BUTTON_RST 0x02

Ticker TickerSmartRoll;

struct HeightMemory
{
    int8_t type1;
    int8_t type2;
    int8_t type3;
    int8_t type4;
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
    uint8_t calibBottom = 0;
    bool directionUp = false;
    bool ready = false;
} smartRoll;

struct SmartRollEntry
{
    uint32_t name;
    uint16_t start;
    uint16_t len;
};

struct SmartRollDir
{
    SmartRollEntry entry[3];
};
SmartRollDir smartRollDir;

enum SmartRollButtonStates
{
    SMART_ROLL_NOT_PRESSED,
    SMART_ROLL_PRESSED_MULTI,
    SMART_ROLL_PRESSED_IMMEDIATE,
    SMART_ROLL_PRESSED_STOP,
    SMART_ROLL_PRESSED_RST,
};

const static uint32_t SMART_ROLL_NAME_ID = 0x20646975;        // 'uid ' little endian
const static uint32_t SMART_ROLL_NAME_REALPOS = 0x20736f70;   // 'pos ' little endian
const static uint32_t SMART_ROLL_NAME_HEIGHTMEM = 0x206d656d; // 'mem ' little endian

static uint8_t *smartRollSpiStart = nullptr;
const static size_t smartRollSpiLen = 0x0400;      // 1kb
const static size_t smartRollBlockLen = 0x0400;    // 1kb
const static size_t smartRollBlockOffset = 0x0000; // don't need offset in FS
const static size_t smartRollObjStoreOffset = smartRollBlockOffset + sizeof(SmartRollDir);

int8_t ConvertPercentToRealUnit(int8_t percent)
{
}

int8_t ConvertRealToPercentUnit(int8_t realValue)
{
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
    }
    free(smartRollSpiStart);

    return true;
}

// TODO: API Interface 변경 -> SmartRoll 구조체 받을 수 있도록
bool SaveConfigToFlash(void)
{
    bool result = false;

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

        SmartRollEntry *entry = &smart_roll_dir_write->entry[0];
        entry->name = SMART_ROLL_NAME_ID;
        entry->start = 0;
        entry->len = sizeof(uint8_t);
        uint8_t temp = (uint8_t)0x5A;
        memcpy(spi_buffer + smartRollObjStoreOffset + entry->start, &temp, entry->len);
        OsalSaveNvm("/smart_roll_config", spi_buffer, smartRollSpiLen);
        free(spi_buffer);
        result = true;
    }

    return result;
}

void SmartRollInit(void)
{
    smartRoll.ready = false;

    if (!LoadConfigFromFlash()) // realPosition, id, heightMemory 플래시로부터 불러오고 저장
    {
        printf("[SmartRoll] Failed to load the config from the flash\n");
    }
    else
    {
        printf("[SmartRoll] Successfully loaded the config from the flash\nid: %d, realPosition: %d \
    , heightMemory.type1: %d, heightMemory.type2: %d \
    , heightMemory.type3: %d, heightMemory.type4: %d\n",
               smartRoll.id, smartRoll.realPosition, smartRoll.heightMemory.type1, smartRoll.heightMemory.type2, smartRoll.heightMemory.type3, smartRoll.heightMemory.type4);

        RotaryInit(); // 로터리 엔코더 Init

        // TODO: IrInit

        // 모터 드라이버용 핀 out 설정
        pinMode(MOTOR_DRIVER_PIN1, OUTPUT);
        pinMode(MOTOR_DRIVER_PIN2, OUTPUT);
        pinMode(MOTOR_DRIVER_EN_PIN, OUTPUT);

        TickerSmartRoll.attach_ms(50, SmartRollRtc50ms); // -> 타이머 인터럽트 등록

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
}

void SmartRollButtonHandler(void) // 물리 버튼 핸들러 API
{
    uint8_t buttonState = SMART_ROLL_NOT_PRESSED;                          // SmartRoll 컴포넌트에서 관리되는 Button State (SMART_ROLL_NOT_PRESSED, SMART_ROLL_PRESSED_MULTI, ...)
    uint8_t button = XdrvMailbox.payload;                                  // support_button으로부터 전달받은 Button State (PRESSED, NOT_PRESSED)
    uint32_t buttonIndex = XdrvMailbox.index;                              // support_button에서 관리되는 Button ID (Default : 8개, Max : 28개)
    uint8_t smartRollIndex = buttonIndex;                                  // SmartRoll 컴포넌트에서 관리되는 Button ID (Max : 4개)
    uint16_t loopsPerSec = 1000 / Settings->button_debounce;               // Software debouncing을 위한 loop마다의 비교 시간

    if ((button == PRESSED) && (Button.last_state[buttonIndex] == NOT_PRESSED)) // 버튼이 최초로 눌렸을 때
    {
        if (Button.window_timer[buttonIndex] == 0) // 일정 시간 내에 버튼이 눌린 횟수가 0 일 때
        {
            buttonState = SMART_ROLL_PRESSED_IMMEDIATE;
            Button.window_timer[buttonIndex] = (loopsPerSec >> 2);
        }
        else // 일정 시간 내에 버튼이 눌린 횟수가 1 이상일 때 (double click)
        {
            buttonState = SMART_ROLL_PRESSED_MULTI;
            Button.window_timer[buttonIndex] = 0;
        }
    }

    if (button == NOT_PRESSED) // 버튼이 안눌렸을때
    {
        if (smartRollIndex == BUTTON_RST)
        {
            Button.hold_timer[buttonIndex] = 0; // Reset 버튼에 대해서만 hold 시간 초기화
        }
    }
    else if (Button.last_state[buttonIndex] == PRESSED) // 버튼이 연속적으로 눌리고 있을때
    {
        if (smartRollIndex == BUTTON_RST)
        {
            Button.hold_timer[buttonIndex]++;                                                        // Reset 버튼에 대해서만 hold 시간 증가
            if ((Button.hold_timer[buttonIndex] == loopsPerSec * Settings->param[P_HOLD_TIME] / 10)) // 기준 hold 시간에 도달했을 때
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
        Button.window_timer[buttonIndex]--; // double click 인지를 위한 window_timer는 루프마다 계속 감소
    }

    if (buttonState != SMART_ROLL_NOT_PRESSED)
    {
        switch (buttonState)
        {
        case SMART_ROLL_PRESSED_RST:
            char cmd[20];
            snprintf_P(cmd, sizeof(cmd), PSTR(D_CMND_FACTORY_RESET));
            ExecuteCommand(cmd, SRC_BUTTON);
            break;
        case SMART_ROLL_PRESSED_MULTI:
            if (smartRollIndex == BUTTON_UP)
            {
                CommandFullUp();
            }
            else if (smartRollIndex == BUTTON_DOWN)
            {
                CommandFullDown();
            }
            break;
        case SMART_ROLL_PRESSED_IMMEDIATE:
            if (smartRollIndex == BUTTON_UP)
            {
                CommandUp();
            }
            else if (smartRollIndex == BUTTON_DOWN)
            {
                CommandDown();
            }
            break;
        }
    }

    Button.last_state[buttonIndex] = button;
}

void CommandFullUp(void)
{
    printf("Full Up!\n");
}

void CommandFullDown(void)
{
    printf("Full Down!\n");
}

void CommandUp(void)
{
    printf("Move Up!\n");
}

void CommandDown(void)
{
    printf("Move Down!\n");
}

void SmartRollUpdatePosition(void)
{
}

// 전류 센서 값 Read API

// 엔코더 Wrapping API

// IR 메시지 파싱 API

// 채널 등록 API

// Calibration API

// (MQTT 명령 파싱 API)

// MQTT 메시지 조립 API

/*********************************************************************************************\
* Interface
\*********************************************************************************************/

bool Xsns88(uint8_t function)
{
    bool result = false;

    if (FUNC_PRE_INIT == function)
    {
        SmartRollInit(); // Init - 플래시에서 현재 높이값 (엔코더), 기기 식별자 (채널), 높이 설정값 불러오기, 타이머 인터럽트 등록 (모터 제어용)
    }
    else if (smartRoll.ready)
    {
        switch (function)
        {
        case FUNC_EVERY_50_MSECOND: // 현재 Position 계산 및 변환, 전류 센서 센싱
            SmartRollUpdatePosition();
            break;
        case FUNC_JSON_APPEND: // Telemetry용 메시지 조립
            break;
        case FUNC_COMMAND: // MQTT 명령 파싱
            // result = DecodeCommand(kAs608Commands, As608Commands);
            break;
        case FUNC_BUTTON_PRESSED: // 물리 버튼 센싱
            SmartRollButtonHandler();
            result = true;
            break;
        }
    }
    return result;
}

#endif // FIRMWARE_SMART_ROLL