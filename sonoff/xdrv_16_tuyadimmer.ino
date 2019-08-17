/*
  xdrv_16_tuyadimmer.ino - Tuya dimmer support for Sonoff-Tasmota

  Copyright (C) 2019  digiblur, Joel Stein and Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_LIGHT
#ifdef USE_TUYA_DIMMER

#define XDRV_16                16

#ifndef TUYA_DIMMER_ID
#define TUYA_DIMMER_ID         0
#endif

#define TUYA_POWER_ID          1

#define TUYA_CMD_HEARTBEAT     0x00
#define TUYA_CMD_QUERY_PRODUCT 0x01
#define TUYA_CMD_MCU_CONF      0x02
#define TUYA_CMD_WIFI_STATE    0x03
#define TUYA_CMD_WIFI_RESET    0x04
#define TUYA_CMD_WIFI_SELECT   0x05
#define TUYA_CMD_SET_DP        0x06
#define TUYA_CMD_STATE         0x07
#define TUYA_CMD_QUERY_STATE   0x08

#define TUYA_TYPE_BOOL         0x01
#define TUYA_TYPE_VALUE        0x02

#define TUYA_BUFFER_SIZE       256

#include <TasmotaSerial.h>

TasmotaSerial *TuyaSerial = nullptr;

struct TUYA {
  uint8_t new_dim = 0;                   // Tuya dimmer value temp
  bool ignore_dim = false;               // Flag to skip serial send to prevent looping when processing inbound states from the faceplate interaction
  uint8_t cmd_status = 0;                // Current status of serial-read
  uint8_t cmd_checksum = 0;              // Checksum of tuya command
  uint8_t data_len = 0;                  // Data lenght of command
  int8_t wifi_state = -2;                // Keep MCU wifi-status in sync with WifiState()
  uint8_t heartbeat_timer = 0;           // 10 second heartbeat timer for tuya module

  char *buffer = nullptr;                // Serial receive buffer
  int byte_counter = 0;                  // Index in serial receive buffer
} Tuya;

/*********************************************************************************************\
 * Internal Functions
\*********************************************************************************************/

void TuyaSendCmd(uint8_t cmd, uint8_t payload[] = nullptr, uint16_t payload_len = 0)
{
  uint8_t checksum = (0xFF + cmd + (payload_len >> 8) + (payload_len & 0xFF));
  TuyaSerial->write(0x55);                  // Tuya header 55AA
  TuyaSerial->write(0xAA);
  TuyaSerial->write((uint8_t)0x00);         // version 00
  TuyaSerial->write(cmd);                   // Tuya command
  TuyaSerial->write(payload_len >> 8);      // following data length (Hi)
  TuyaSerial->write(payload_len & 0xFF);    // following data length (Lo)
  snprintf_P(log_data, sizeof(log_data), PSTR("TYA: Send \"55aa00%02x%02x%02x"), cmd, payload_len >> 8, payload_len & 0xFF);
  for (uint32_t i = 0; i < payload_len; ++i) {
    TuyaSerial->write(payload[i]);
    checksum += payload[i];
    snprintf_P(log_data, sizeof(log_data), PSTR("%s%02x"), log_data, payload[i]);
  }
  TuyaSerial->write(checksum);
  TuyaSerial->flush();
  snprintf_P(log_data, sizeof(log_data), PSTR("%s%02x\""), log_data, checksum);
  AddLog(LOG_LEVEL_DEBUG);
}

void TuyaSendState(uint8_t id, uint8_t type, uint8_t* value)
{
  uint16_t payload_len = 4;
  uint8_t payload_buffer[8];
  payload_buffer[0] = id;
  payload_buffer[1] = type;
  switch (type) {
    case TUYA_TYPE_BOOL:
      payload_len += 1;
      payload_buffer[2] = 0x00;
      payload_buffer[3] = 0x01;
      payload_buffer[4] = value[0];
      break;
    case TUYA_TYPE_VALUE:
      payload_len += 4;
      payload_buffer[2] = 0x00;
      payload_buffer[3] = 0x04;
      payload_buffer[4] = value[3];
      payload_buffer[5] = value[2];
      payload_buffer[6] = value[1];
      payload_buffer[7] = value[0];
      break;
  }

  TuyaSendCmd(TUYA_CMD_SET_DP, payload_buffer, payload_len);
}

void TuyaSendBool(uint8_t id, bool value)
{
  TuyaSendState(id, TUYA_TYPE_BOOL, (uint8_t*)&value);
}

void TuyaSendValue(uint8_t id, uint32_t value)
{
  TuyaSendState(id, TUYA_TYPE_VALUE, (uint8_t*)(&value));
}

bool TuyaSetPower(void)
{
  bool status = false;

  uint8_t rpower = XdrvMailbox.index;
  int16_t source = XdrvMailbox.payload;

  if (source != SRC_SWITCH && TuyaSerial) {  // ignore to prevent loop from pushing state from faceplate interaction
    TuyaSendBool(active_device, bitRead(rpower, active_device-1));
    status = true;
  }
  return status;
}

bool TuyaSetChannels(void)
{
  LightSerialDuty(((uint8_t*)XdrvMailbox.data)[0]);
  return true;
}

void LightSerialDuty(uint8_t duty)
{
  if (duty > 0 && !Tuya.ignore_dim && TuyaSerial) {
    if (Settings.flag3.tuya_dimmer_min_limit) { // Enable dimming limit SetOption69: Enabled by default
      if (duty < 25) { duty = 25; }  // dimming acts odd below 25(10%) - this mirrors the threshold set on the faceplate itself
    }

    if (Settings.flag3.tuya_show_dimmer == 0) {
      if(Settings.flag3.tuya_dimmer_range_255 == 0) {
        duty = changeUIntScale(duty, 0, 255, 0, 100);
      }
      if (Tuya.new_dim != duty) {
        AddLog_P2(LOG_LEVEL_DEBUG, PSTR("TYA: Send dim value=%d (id=%d)"), duty, Settings.param[P_TUYA_DIMMER_ID]);
        TuyaSendValue(Settings.param[P_TUYA_DIMMER_ID], duty);
      }
    }
  } else {
    Tuya.ignore_dim = false;  // reset flag
    if (Settings.flag3.tuya_show_dimmer == 0) {
      if(Settings.flag3.tuya_dimmer_range_255 == 0) {
        duty = changeUIntScale(duty, 0, 255, 0, 100);
      }
      AddLog_P2(LOG_LEVEL_DEBUG, PSTR("TYA: Send dim skipped value=%d"), duty);  // due to 0 or already set
    }
  }
}

void TuyaRequestState(void)
{
  if (TuyaSerial) {
    // Get current status of MCU
    AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: Read MCU state"));

    TuyaSendCmd(TUYA_CMD_QUERY_STATE);
  }
}

void TuyaResetWifi(void)
{
  if (!Settings.flag.button_restrict) {
    char scmnd[20];
    snprintf_P(scmnd, sizeof(scmnd), D_CMND_WIFICONFIG " %d", 2);
    ExecuteCommand(scmnd, SRC_BUTTON);
  }
}

void TuyaPacketProcess(void)
{
  char scmnd[20];

  switch (Tuya.buffer[3]) {

    case TUYA_CMD_HEARTBEAT:
      AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: Heartbeat"));
      if (Tuya.buffer[6] == 0) {
        AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: Detected MCU restart"));
        Tuya.wifi_state = -2;
      }
      break;

    case TUYA_CMD_STATE:
      if (Tuya.buffer[5] == 5) {  // on/off packet

        /*if ((power || Settings.light_dimmer > 0) && (power != Tuya.buffer[10])) {
          ExecuteCommandPower(1, Tuya.buffer[10], SRC_SWITCH);  // send SRC_SWITCH? to use as flag to prevent loop from inbound states from faceplate interaction
        }*/
        AddLog_P2(LOG_LEVEL_DEBUG, PSTR("TYA: RX Device-%d --> MCU State: %s Current State:%s"),Tuya.buffer[6],Tuya.buffer[10]?"On":"Off",bitRead(power, Tuya.buffer[6]-1)?"On":"Off");
        if ((power || Settings.light_dimmer > 0) && (Tuya.buffer[10] != bitRead(power, Tuya.buffer[6]-1))) {
          ExecuteCommandPower(Tuya.buffer[6], Tuya.buffer[10], SRC_SWITCH);  // send SRC_SWITCH? to use as flag to prevent loop from inbound states from faceplate interaction
        }
      }
      else if (Tuya.buffer[5] == 8) {  // dim packet

        AddLog_P2(LOG_LEVEL_DEBUG, PSTR("TYA: RX Dim State=%d"), Tuya.buffer[13]);
        if (Settings.flag3.tuya_show_dimmer == 0) {
          if (!Settings.param[P_TUYA_DIMMER_ID]) {
            AddLog_P2(LOG_LEVEL_DEBUG, PSTR("TYA: Autoconfiguring Dimmer ID %d"), Tuya.buffer[6]);
            Settings.param[P_TUYA_DIMMER_ID] = Tuya.buffer[6];
          }
          if (Settings.param[P_TUYA_DIMMER_ID] == Tuya.buffer[6]) {
            if(Settings.flag3.tuya_dimmer_range_255 == 0) {
              Tuya.new_dim = (uint8_t) Tuya.buffer[13];
            } else {
              Tuya.new_dim = changeUIntScale((uint8_t) Tuya.buffer[13], 0, 255, 0, 100);
            }
            if ((power || Settings.flag3.tuya_apply_o20) && (Tuya.new_dim > 0) && (abs(Tuya.new_dim - Settings.light_dimmer) > 1)) {
              Tuya.ignore_dim = true;

              snprintf_P(scmnd, sizeof(scmnd), PSTR(D_CMND_DIMMER " %d"), Tuya.new_dim );
              ExecuteCommand(scmnd, SRC_SWITCH);
            }
          }
        }
      }
      break;

    case TUYA_CMD_WIFI_RESET:
    case TUYA_CMD_WIFI_SELECT:
      AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: RX WiFi Reset"));
      TuyaResetWifi();
      break;

    case TUYA_CMD_WIFI_STATE:
      AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: RX WiFi LED set ACK"));
      Tuya.wifi_state = WifiState();
      break;

    case TUYA_CMD_MCU_CONF:
      AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: RX MCU configuration"));

      if (Tuya.buffer[5] == 2) {
        uint8_t led1_gpio = Tuya.buffer[6];
        uint8_t key1_gpio = Tuya.buffer[7];
        bool key1_set = false;
        bool led1_set = false;
        for (uint32_t i = 0; i < sizeof(Settings.my_gp); i++) {
          if (Settings.my_gp.io[i] == GPIO_LED1) led1_set = true;
          else if (Settings.my_gp.io[i] == GPIO_KEY1) key1_set = true;
        }
        if (!Settings.my_gp.io[led1_gpio] && !led1_set) {
          Settings.my_gp.io[led1_gpio] = GPIO_LED1;
          restart_flag = 2;
        }
        if (!Settings.my_gp.io[key1_gpio] && !key1_set) {
          Settings.my_gp.io[key1_gpio] = GPIO_KEY1;
          restart_flag = 2;
        }
      }
      TuyaRequestState();
      break;

    default:
      AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: RX unknown command"));
  }
}

/*********************************************************************************************\
 * API Functions
\*********************************************************************************************/

bool TuyaModuleSelected(void)
{
  if (!(pin[GPIO_TUYA_RX] < 99) || !(pin[GPIO_TUYA_TX] < 99)) {  // fallback to hardware-serial if not explicitly selected
    pin[GPIO_TUYA_TX] = 1;
    pin[GPIO_TUYA_RX] = 3;
    Settings.my_gp.io[1] = GPIO_TUYA_TX;
    Settings.my_gp.io[3] = GPIO_TUYA_RX;
    restart_flag = 2;
  }
  light_type = LT_SERIAL1;
  return true;
}

void TuyaInit(void)
{
  devices_present += Settings.param[P_TUYA_RELAYS];  // SetOption41 - Add virtual relays if present
  if (!Settings.param[P_TUYA_DIMMER_ID]) {
    Settings.param[P_TUYA_DIMMER_ID] = TUYA_DIMMER_ID;
  }
  Tuya.buffer = (char*)(malloc(TUYA_BUFFER_SIZE));
  if (Tuya.buffer != nullptr) {
    TuyaSerial = new TasmotaSerial(pin[GPIO_TUYA_RX], pin[GPIO_TUYA_TX], 2);
    if (TuyaSerial->begin(9600)) {
      if (TuyaSerial->hardwareSerial()) { ClaimSerial(); }
      // Get MCU Configuration
      AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: Request MCU configuration"));

      TuyaSendCmd(TUYA_CMD_MCU_CONF);
    }
  }
  Tuya.heartbeat_timer = 0; // init heartbeat timer when dimmer init is done
}

void TuyaSerialInput(void)
{
  while (TuyaSerial->available()) {
    yield();
    uint8_t serial_in_byte = TuyaSerial->read();

    if (serial_in_byte == 0x55) {            // Start TUYA Packet
      Tuya.cmd_status = 1;
      Tuya.buffer[Tuya.byte_counter++] = serial_in_byte;
      Tuya.cmd_checksum += serial_in_byte;
    }
    else if (Tuya.cmd_status == 1 && serial_in_byte == 0xAA) { // Only packtes with header 0x55AA are valid
      Tuya.cmd_status = 2;

      Tuya.byte_counter = 0;
      Tuya.buffer[Tuya.byte_counter++] = 0x55;
      Tuya.buffer[Tuya.byte_counter++] = 0xAA;
      Tuya.cmd_checksum = 0xFF;
    }
    else if (Tuya.cmd_status == 2) {
      if (Tuya.byte_counter == 5) { // Get length of data
        Tuya.cmd_status = 3;
        Tuya.data_len = serial_in_byte;
      }
      Tuya.cmd_checksum += serial_in_byte;
      Tuya.buffer[Tuya.byte_counter++] = serial_in_byte;
    }
    else if ((Tuya.cmd_status == 3) && (Tuya.byte_counter == (6 + Tuya.data_len)) && (Tuya.cmd_checksum == serial_in_byte)) { // Compare checksum and process packet
      Tuya.buffer[Tuya.byte_counter++] = serial_in_byte;

      snprintf_P(log_data, sizeof(log_data), PSTR("TYA: RX Packet: \""));
      for (uint32_t i = 0; i < Tuya.byte_counter; i++) {
        snprintf_P(log_data, sizeof(log_data), PSTR("%s%02x"), log_data, Tuya.buffer[i]);
      }
      snprintf_P(log_data, sizeof(log_data), PSTR("%s\""), log_data);
      AddLog(LOG_LEVEL_DEBUG);

      TuyaPacketProcess();
      Tuya.byte_counter = 0;
      Tuya.cmd_status = 0;
      Tuya.cmd_checksum = 0;
      Tuya.data_len = 0;
    }                               // read additional packets from TUYA
    else if (Tuya.byte_counter < TUYA_BUFFER_SIZE -1) {  // add char to string if it still fits
      Tuya.buffer[Tuya.byte_counter++] = serial_in_byte;
      Tuya.cmd_checksum += serial_in_byte;
    } else {
      Tuya.byte_counter = 0;
      Tuya.cmd_status = 0;
      Tuya.cmd_checksum = 0;
      Tuya.data_len = 0;
    }
  }
}

bool TuyaButtonPressed(void)
{
  if (!XdrvMailbox.index && ((PRESSED == XdrvMailbox.payload) && (NOT_PRESSED == Button.last_state[XdrvMailbox.index]))) {
    AddLog_P(LOG_LEVEL_DEBUG, PSTR("TYA: Reset GPIO triggered"));
    TuyaResetWifi();
    return true;  // Reset GPIO served here
  }
  return false;   // Don't serve other buttons
}

void TuyaSetWifiLed(void)
{
  uint8_t wifi_state = 0x02;
  switch(WifiState()){
    case WIFI_SMARTCONFIG:
      wifi_state = 0x00;
      break;
    case WIFI_MANAGER:
    case WIFI_WPSCONFIG:
      wifi_state = 0x01;
      break;
    case WIFI_RESTART:
      wifi_state =  0x03;
      break;
  }

  AddLog_P2(LOG_LEVEL_DEBUG, PSTR("TYA: Set WiFi LED %d (%d)"), wifi_state, WifiState());

  TuyaSendCmd(TUYA_CMD_WIFI_STATE, &wifi_state, 1);
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xdrv16(uint8_t function)
{
  bool result = false;

  if (TUYA_DIMMER == my_module_type) {
    switch (function) {
      case FUNC_LOOP:
        if (TuyaSerial) { TuyaSerialInput(); }
        break;
      case FUNC_MODULE_INIT:
        result = TuyaModuleSelected();
        break;
      case FUNC_INIT:
        TuyaInit();
        break;
      case FUNC_SET_DEVICE_POWER:
        result = TuyaSetPower();
        break;
      case FUNC_BUTTON_PRESSED:
        result = TuyaButtonPressed();
        break;
      case FUNC_EVERY_SECOND:
        if (TuyaSerial && Tuya.wifi_state != WifiState()) { TuyaSetWifiLed(); }
        Tuya.heartbeat_timer++;
        if (Tuya.heartbeat_timer > 10) {
          Tuya.heartbeat_timer = 0;
          TuyaSendCmd(TUYA_CMD_HEARTBEAT);
        }
        break;
      case FUNC_SET_CHANNELS:
        result = TuyaSetChannels();
        break;
    }
  }
  return result;
}

#endif  // USE_TUYA_DIMMER
#endif  // USE_LIGHT
