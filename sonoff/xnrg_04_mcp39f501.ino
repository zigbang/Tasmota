/*
  xnrg_04_mcp39f501.ino - MCP39F501 energy sensor support for Sonoff-Tasmota

  Copyright (C) 2018  Theo Arends

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

#ifdef USE_ENERGY_SENSOR
#ifdef USE_MCP39F501
/*********************************************************************************************\
 * MCP39F501 - Energy (Shelly 2)
 *
 * Based on datasheet from https://www.microchip.com/wwwproducts/en/MCP39F501
 * and https://github.com/OLIMEX/olimex-iot-firmware-esp8266/blob/7a7f9bb56d4b72770dba8d0f18eaa9d956dd0baf/olimex/user/modules/mod_emtr.c
\*********************************************************************************************/

#define XNRG_04                 4

#define MCP_TIMEOUT             4

#define MCP_START_FRAME         0xA5
#define MCP_ACK_FRAME           0x06
#define MCP_ERROR_NAK           0x15
#define MCP_ERROR_CRC           0x51

#define MCP_SINGLE_WIRE         0xAB

#define MCP_SET_ADDRESS         0x41

#define MCP_READ                0x4E
#define MCP_READ_16             0x52
#define MCP_READ_32             0x44

#define MCP_WRITE               0x4D
#define MCP_WRITE_16            0x57
#define MCP_WRITE_32            0x45

#define MCP_SAVE_REGISTERS      0x53

#define MCP_CALIBRATION_BASE    0x0028
#define MCP_CALIBRATION_LEN     52

#define MCP_FREQUENCY_REF_BASE  0x0094
#define MCP_FREQUENCY_GAIN_BASE 0x00AE
#define MCP_FREQUENCY_LEN       4

typedef struct mcp_calibration_registers_type {
  uint16_t gain_current_rms;
  uint16_t gain_voltage_rms;
  uint16_t gain_active_power;
  uint16_t gain_reactive_power;
  sint32_t offset_current_rms;
  sint32_t offset_active_power;
  sint32_t offset_reactive_power;
  sint16_t dc_offset_current;
  sint16_t phase_compensation;
  uint16_t apparent_power_divisor;

  uint32_t system_configuration;
  uint16_t dio_configuration;
  uint32_t range;

  uint32_t calibration_current;
  uint16_t calibration_voltage;
  uint32_t calibration_active_power;
  uint32_t calibration_reactive_power;
  uint16_t accumulation_interval;
} mcp_calibration_registers_type;
mcp_calibration_registers_type mcp_calibration_registers;

typedef struct mcp_calibration_setpoint_type {
  uint32_t calibration_current;
  uint16_t calibration_voltage;
  uint32_t calibration_active_power;
  uint32_t calibration_reactive_power;
  uint16_t line_frequency_ref;
} mcp_calibration_setpoint_type;
mcp_calibration_setpoint_type mcp_calibration_setpoint;

typedef struct mcp_frequency_registers_type {
  uint16_t line_frequency_ref;
  uint16_t gain_line_frequency;
} mcp_frequency_registers_type;
mcp_frequency_registers_type mcp_frequency_registers;

typedef struct mcp_output_registers_type {
  uint32_t current_rms;
  uint16_t voltage_rms;
  uint32_t active_power;
  uint32_t reactive_power;
  uint32_t apparent_power;
  sint16_t power_factor;
  uint16_t line_frequency;
  uint16_t thermistor_voltage;
  uint16_t event_flag;
  uint16_t system_status;
} mcp_output_registers_type;
mcp_output_registers_type mcp_output_registers;

uint32_t mcp_system_configuration = 0x03000000;
uint16_t mcp_address = 0;
uint8_t mcp_single_wire_active = 0;
uint8_t mcp_calibration_active = 0;
uint8_t mcp_init = 0;
uint8_t mcp_timeout = 0;
unsigned long mcp_kWhcounter = 0;

/*********************************************************************************************\
 * Olimex tools
 * https://github.com/OLIMEX/olimex-iot-firmware-esp8266/blob/7a7f9bb56d4b72770dba8d0f18eaa9d956dd0baf/olimex/user/modules/mod_emtr.c
\*********************************************************************************************/

uint8_t McpChecksum(uint8_t *data)
{
  uint8_t checksum = 0;
  uint8_t offset = 0;
  uint8_t len = data[1] -1;

  if (MCP_SINGLE_WIRE == data[0]) {
    offset = 3;
    len = 15;
  }
  for (byte i = offset; i < len; i++) { checksum += data[i];	}
  return (MCP_SINGLE_WIRE == data[0]) ? ~checksum : checksum;
}

unsigned long McpExtractInt(char *data, uint8_t offset, uint8_t size)
{
	unsigned long result = 0;
	unsigned long pow = 1;

	for (byte i = 0; i < size; i++) {
		result = result + (uint8_t)data[offset + i] * pow;
		pow = pow * 256;
	}
	return result;
}

void McpSetInt(unsigned long value, uint8_t *data, uint8_t offset, size_t size)
{
	for (byte i = 0; i < size; i++) {
		data[offset + i] = ((value >> (i * 8)) & 0xFF);
	}
}

void McpSend(uint8_t *data)
{
  if (mcp_timeout) { return; }
  mcp_timeout = MCP_TIMEOUT;

  data[0] = MCP_START_FRAME;
  data[data[1] -1] = McpChecksum(data);

//  AddLogSerial(LOG_LEVEL_DEBUG_MORE, data, data[1]);

  for (byte i = 0; i < data[1]; i++) {
    Serial.write(data[i]);
  }
}

uint32_t McpGetRange(uint8_t shift)
{
  return (mcp_calibration_registers.range >> shift) & 0xFF;
}

void McpSetRange(uint8_t shift, uint32_t range)
{
  uint32_t old_range = McpGetRange(shift);
  mcp_calibration_registers.range = mcp_calibration_registers.range ^ (old_range << shift);
  mcp_calibration_registers.range = mcp_calibration_registers.range | (range << shift);
}

bool McpCalibrationCalc(uint8_t range_shift)
{
  uint32_t measured;
  uint32_t expected;
  uint16_t *gain;
  uint32_t new_gain;

  if (range_shift == 0) {
    measured = mcp_output_registers.voltage_rms;
    expected = mcp_calibration_registers.calibration_voltage;
    gain = &(mcp_calibration_registers.gain_voltage_rms);
  } else if (range_shift == 8) {
    measured = mcp_output_registers.current_rms;
    expected = mcp_calibration_registers.calibration_current;
    gain = &(mcp_calibration_registers.gain_current_rms);
  } else if (range_shift == 16) {
    measured = mcp_output_registers.active_power;
    expected = mcp_calibration_registers.calibration_active_power;
    gain = &(mcp_calibration_registers.gain_active_power);
  } else {
    return false;
  }

  if (measured == 0) {
    return false;
  }

  uint32_t range = McpGetRange(range_shift);

calc:
  new_gain = (*gain) * expected / measured;

  if (new_gain < 25000) {
    range++;
    if (measured > 6) {
      measured = measured / 2;
      goto calc;
    }
  }

  if (new_gain > 55000) {
    range--;
    measured = measured * 2;
    goto calc;
  }

  *gain = new_gain;
  McpSetRange(range_shift, range);

  return true;
}

void McpCalibrationReactivePower()
{
  mcp_calibration_registers.gain_reactive_power = mcp_calibration_registers.gain_reactive_power * mcp_calibration_registers.calibration_reactive_power / mcp_output_registers.reactive_power;
}

void McpCalibrationLineFreqency()
{
  mcp_frequency_registers.gain_line_frequency = mcp_frequency_registers.gain_line_frequency * mcp_frequency_registers.line_frequency_ref / mcp_output_registers.line_frequency;
}

void McpResetSetpoints()
{
  mcp_calibration_setpoint.calibration_active_power = 0;
  mcp_calibration_setpoint.calibration_voltage = 0;
  mcp_calibration_setpoint.calibration_current = 0;
  mcp_calibration_setpoint.calibration_reactive_power = 0;
  mcp_calibration_setpoint.line_frequency_ref = 0;
}

/********************************************************************************************/

void McpGetAddress()
{
  // A5 07 41 00 26 52 65
  uint8_t data[7];

  data[1] = sizeof(data);
  data[2] = MCP_SET_ADDRESS;  // Set address pointer
  data[3] = 0x00;             // address
  data[4] = 0x26;             // address
  data[5] = MCP_READ_16;      // Read 2 bytes

  McpSend(data);

  // Receives 06 05 004D 58
}

void McpGetCalibration()
{
  if (mcp_calibration_active) { return; }
  mcp_calibration_active = 4;

  // A5 08 41 00 28 4E 34 98
  uint8_t data[8];

  data[1] = sizeof(data);
  data[2] = MCP_SET_ADDRESS;                     // Set address pointer
  data[3] = (MCP_CALIBRATION_BASE >> 8) & 0xFF;  // address
  data[4] = (MCP_CALIBRATION_BASE >> 0) & 0xFF;  // address
  data[5] = MCP_READ;                            // Read N bytes
  data[6] = MCP_CALIBRATION_LEN;

  McpSend(data);

  // Receives 06 37 C882 B6AD 0781 9273 06000000 00000000 00000000 0000 D3FF 0300 00000003 9204 120C1300 204E0000 9808 E0AB0000 D9940000 0200 24
}

void McpSetCalibration()
{
  uint8_t data[7 + MCP_CALIBRATION_LEN + 2 + 1];

  data[1] = sizeof(data);
  data[2] = MCP_SET_ADDRESS;                     // Set address pointer
  data[3] = (MCP_CALIBRATION_BASE >> 8) & 0xFF;  // address
  data[4] = (MCP_CALIBRATION_BASE >> 0) & 0xFF;  // address

  data[5] = MCP_WRITE;                           // Write N bytes
  data[6] = MCP_CALIBRATION_LEN;

  McpSetInt(mcp_calibration_registers.gain_current_rms,            data,  0+7, 2);
  McpSetInt(mcp_calibration_registers.gain_voltage_rms,            data,  2+7, 2);
  McpSetInt(mcp_calibration_registers.gain_active_power,           data,  4+7, 2);
  McpSetInt(mcp_calibration_registers.gain_reactive_power,         data,  6+7, 2);
  McpSetInt(mcp_calibration_registers.offset_current_rms,          data,  8+7, 4);
  McpSetInt(mcp_calibration_registers.offset_active_power,         data, 12+7, 4);
  McpSetInt(mcp_calibration_registers.offset_reactive_power,       data, 16+7, 4);
  McpSetInt(mcp_calibration_registers.dc_offset_current,           data, 20+7, 2);
  McpSetInt(mcp_calibration_registers.phase_compensation,          data, 22+7, 2);
  McpSetInt(mcp_calibration_registers.apparent_power_divisor,      data, 24+7, 2);

  McpSetInt(mcp_calibration_registers.system_configuration,        data, 26+7, 4);
  McpSetInt(mcp_calibration_registers.dio_configuration,           data, 30+7, 2);
  McpSetInt(mcp_calibration_registers.range,                       data, 32+7, 4);

  McpSetInt(mcp_calibration_registers.calibration_current,         data, 36+7, 4);
  McpSetInt(mcp_calibration_registers.calibration_voltage,         data, 40+7, 2);
  McpSetInt(mcp_calibration_registers.calibration_active_power,    data, 42+7, 4);
  McpSetInt(mcp_calibration_registers.calibration_reactive_power,  data, 46+7, 4);
  McpSetInt(mcp_calibration_registers.accumulation_interval,       data, 50+7, 2);

  data[MCP_CALIBRATION_LEN+7] = MCP_SAVE_REGISTERS;  // Save registers to flash
  data[MCP_CALIBRATION_LEN+8] = mcp_address;         // Device address

  McpSend(data);
}

void McpGetFrequency()
{
  if (mcp_calibration_active) { return; }
  mcp_calibration_active = 4;

  // A5 0B 41 00 94 52 41 00 AE 52 18
  uint8_t data[11];

  data[1] = sizeof(data);
  data[2] = MCP_SET_ADDRESS;                        // Set address pointer
  data[3] = (MCP_FREQUENCY_REF_BASE >> 8) & 0xFF;   // address
  data[4] = (MCP_FREQUENCY_REF_BASE >> 0) & 0xFF;   // address

  data[5] = MCP_READ_16;                            // Read register

  data[6] = MCP_SET_ADDRESS;                        // Set address pointer
  data[7] = (MCP_FREQUENCY_GAIN_BASE >> 8) & 0xFF;  // address
  data[8] = (MCP_FREQUENCY_GAIN_BASE >> 0) & 0xFF;  // address

  data[9] = MCP_READ_16;                            // Read register

  McpSend(data);
}

void McpSetFrequency()
{
  // A5 11 41 00 94 57 C3 B4 41 00 AE 57 7E 46 53 4D 03
  uint8_t data[17];

  data[ 1] = sizeof(data);
  data[ 2] = MCP_SET_ADDRESS;                     // Set address pointer
  data[ 3] = (MCP_FREQUENCY_REF_BASE >> 8) & 0xFF;   // address
  data[ 4] = (MCP_FREQUENCY_REF_BASE >> 0) & 0xFF;   // address

  data[ 5] = MCP_WRITE_16;                                // Write register
  data[ 6] = (mcp_frequency_registers.line_frequency_ref >> 8) & 0xFF;  // line_frequency_ref high
  data[ 7] = (mcp_frequency_registers.line_frequency_ref >> 0) & 0xFF;  // line_frequency_ref low

  data[ 8] = MCP_SET_ADDRESS;                             // Set address pointer
  data[ 9] = (MCP_FREQUENCY_GAIN_BASE >> 8) & 0xFF;       // address
  data[10] = (MCP_FREQUENCY_GAIN_BASE >> 0) & 0xFF;       // address

  data[11] = MCP_WRITE_16;                                // Write register
  data[12] = (mcp_frequency_registers.gain_line_frequency >> 8) & 0xFF; // gain_line_frequency high
  data[13] = (mcp_frequency_registers.gain_line_frequency >> 0) & 0xFF; // gain_line_frequency low

  data[14] = MCP_SAVE_REGISTERS;  // Save registers to flash
  data[15] = mcp_address;         // Device address

  McpSend(data);
}

void McpSetSystemConfiguration(uint16 interval)
{
  // A5 11 41 00 42 45 03 00 01 00 41 00 5A 57 00 06 7A
  uint8_t data[17];

  data[ 1] = sizeof(data);
  data[ 2] = MCP_SET_ADDRESS;     // Set address pointer
  data[ 3] = 0x00;                // address
  data[ 4] = 0x42;                // address
  data[ 5] = MCP_WRITE_32;        // Write 4 bytes
  data[ 6] = (mcp_system_configuration >> 24) & 0xFF; // system_configuration
  data[ 7] = (mcp_system_configuration >> 16) & 0xFF; // system_configuration
  data[ 8] = (mcp_system_configuration >>  8) & 0xFF; // system_configuration
  data[ 9] = (mcp_system_configuration >>  0) & 0xFF; // system_configuration
  data[10] = MCP_SET_ADDRESS;     // Set address pointer
  data[11] = 0x00;                // address
  data[12] = 0x5A;                // address
  data[13] = MCP_WRITE_16;        // Write 2 bytes
  data[14] = (interval >>  8) & 0xFF; // interval
  data[15] = (interval >>  0) & 0xFF; // interval

  McpSend(data);
}

void McpSingleWireStart()
{
  if ((mcp_system_configuration & (1 << 8)) != 0) { return; }
  mcp_system_configuration = mcp_system_configuration | (1 << 8);
	McpSetSystemConfiguration(6);  // 64
  mcp_single_wire_active = 1;
}

void McpSingleWireStop(uint8_t force)
{
  if (!force && ((mcp_system_configuration & (1 << 8)) == 0)) { return; }
  mcp_system_configuration = mcp_system_configuration & (~(1 << 8));
	McpSetSystemConfiguration(2);  // 4
  mcp_single_wire_active = 0;
}

/********************************************************************************************/

void McpAddressReceive()
{
  // 06 05 004D 58
  mcp_address = serial_in_buffer[2] * 256 + serial_in_buffer[3];
}

void McpParseCalibration()
{
  bool action = false;

  // 06 37 C882 B6AD 0781 9273 06000000 00000000 00000000 0000 D3FF 0300 00000003 9204 120C1300 204E0000 9808 E0AB0000 D9940000 0200 24
  mcp_calibration_registers.gain_current_rms           = McpExtractInt(serial_in_buffer,  2, 2);
  mcp_calibration_registers.gain_voltage_rms           = McpExtractInt(serial_in_buffer,  4, 2);
  mcp_calibration_registers.gain_active_power          = McpExtractInt(serial_in_buffer,  6, 2);
  mcp_calibration_registers.gain_reactive_power        = McpExtractInt(serial_in_buffer,  8, 2);
  mcp_calibration_registers.offset_current_rms         = McpExtractInt(serial_in_buffer, 10, 4);
  mcp_calibration_registers.offset_active_power        = McpExtractInt(serial_in_buffer, 14, 4);
  mcp_calibration_registers.offset_reactive_power      = McpExtractInt(serial_in_buffer, 18, 4);
  mcp_calibration_registers.dc_offset_current          = McpExtractInt(serial_in_buffer, 22, 2);
  mcp_calibration_registers.phase_compensation         = McpExtractInt(serial_in_buffer, 24, 2);
  mcp_calibration_registers.apparent_power_divisor     = McpExtractInt(serial_in_buffer, 26, 2);

  mcp_calibration_registers.system_configuration       = McpExtractInt(serial_in_buffer, 28, 4);
  mcp_calibration_registers.dio_configuration          = McpExtractInt(serial_in_buffer, 32, 2);
  mcp_calibration_registers.range                      = McpExtractInt(serial_in_buffer, 34, 4);

  mcp_calibration_registers.calibration_current        = McpExtractInt(serial_in_buffer, 38, 4);
  mcp_calibration_registers.calibration_voltage        = McpExtractInt(serial_in_buffer, 42, 2);
  mcp_calibration_registers.calibration_active_power   = McpExtractInt(serial_in_buffer, 44, 4);
  mcp_calibration_registers.calibration_reactive_power = McpExtractInt(serial_in_buffer, 48, 4);
  mcp_calibration_registers.accumulation_interval      = McpExtractInt(serial_in_buffer, 52, 2);

  if (mcp_calibration_setpoint.calibration_active_power) {
    mcp_calibration_registers.calibration_active_power = mcp_calibration_setpoint.calibration_active_power;
    if (McpCalibrationCalc(16)) { action = true; }
  }
  if (mcp_calibration_setpoint.calibration_voltage) {
    mcp_calibration_registers.calibration_voltage = mcp_calibration_setpoint.calibration_voltage;
    if (McpCalibrationCalc(0)) { action = true; }
  }
  if (mcp_calibration_setpoint.calibration_current) {
    mcp_calibration_registers.calibration_current = mcp_calibration_setpoint.calibration_current;
    if (McpCalibrationCalc(8)) { action = true; }
  }
  mcp_timeout = 0;
  if (action) { McpSetCalibration(); }
  McpResetSetpoints();
}

void McpParseFrequency()
{
  // 06 07 C350 8000 A0
  mcp_frequency_registers.line_frequency_ref  = serial_in_buffer[2] * 256 + serial_in_buffer[3];
  mcp_frequency_registers.gain_line_frequency = serial_in_buffer[4] * 256 + serial_in_buffer[5];

  if (mcp_calibration_setpoint.line_frequency_ref) {
    mcp_frequency_registers.line_frequency_ref = mcp_calibration_setpoint.line_frequency_ref;
    McpCalibrationLineFreqency();
    mcp_timeout = 0;
    McpSetFrequency();
  }
  McpResetSetpoints();
}

void McpParseData(uint8_t single_wire)
{
  if (single_wire) {
    //  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    // AB CD EF 51 06 00 00 B8 08 FC 0D 00 00 0A C4 11
    // Header-- Current---- Volt- Power------ Freq- Ck

    mcp_output_registers.current_rms    = McpExtractInt(serial_in_buffer,  3, 4);
    mcp_output_registers.voltage_rms    = McpExtractInt(serial_in_buffer,  7, 2);
    mcp_output_registers.active_power   = McpExtractInt(serial_in_buffer,  9, 4);
    mcp_output_registers.line_frequency = McpExtractInt(serial_in_buffer, 13, 2);
  } else {
    //  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
    // 06 19 61 06 00 00 FE 08 9B 0E 00 00 0B 00 00 00 97 0E 00 00 FF 7F 0C C6 35
    // 06 19 CE 18 00 00 F2 08 3A 38 00 00 66 00 00 00 93 38 00 00 36 7F 9A C6 B7
    // Ak Ln Current---- Volt- ActivePower ReActivePow ApparentPow Factr Frequ Ck

    mcp_output_registers.current_rms    = McpExtractInt(serial_in_buffer,  2, 4);
    mcp_output_registers.voltage_rms    = McpExtractInt(serial_in_buffer,  6, 2);
    mcp_output_registers.active_power   = McpExtractInt(serial_in_buffer,  8, 4);
    mcp_output_registers.reactive_power = McpExtractInt(serial_in_buffer, 12, 4);
    mcp_output_registers.line_frequency = McpExtractInt(serial_in_buffer, 22, 2);
  }

  if (energy_power_on) {  // Powered on
    energy_frequency = (float)mcp_output_registers.line_frequency / 1000;
    energy_voltage = (float)mcp_output_registers.voltage_rms / 10;
    energy_power = (float)mcp_output_registers.active_power / 100;
    if (0 == energy_power) {
      energy_current = 0;
    } else {
      energy_current = (float)mcp_output_registers.current_rms / 10000;
    }
  } else {  // Powered off
    energy_frequency = 0;
    energy_voltage = 0;
    energy_power = 0;
    energy_current = 0;
  }
}

bool McpSerialInput()
{
  serial_in_buffer[serial_in_byte_counter++] = serial_in_byte;
  unsigned long start = millis();
  while (millis() - start < 20) {
    yield();
    if (Serial.available()) {
      serial_in_buffer[serial_in_byte_counter++] = Serial.read();
      start = millis();
    }
  }

  AddLogSerial(LOG_LEVEL_DEBUG_MORE);

  if (1 == serial_in_byte_counter) {
    if (MCP_ERROR_CRC == serial_in_buffer[0]) {
//      AddLog_P(LOG_LEVEL_DEBUG, PSTR("MCP: Send " D_CHECKSUM_FAILURE));
      mcp_timeout = 0;
    }
    else if (MCP_ERROR_NAK == serial_in_buffer[0]) {
//      AddLog_P(LOG_LEVEL_DEBUG, PSTR("MCP: NAck"));
      mcp_timeout = 0;
    }
  }
  else if (MCP_ACK_FRAME == serial_in_buffer[0]) {
    if (serial_in_byte_counter == serial_in_buffer[1]) {

      if (McpChecksum((uint8_t *)serial_in_buffer) != serial_in_buffer[serial_in_byte_counter -1]) {
        AddLog_P(LOG_LEVEL_DEBUG, PSTR("MCP: " D_CHECKSUM_FAILURE));
      } else {
        if (5 == serial_in_buffer[1]) { McpAddressReceive(); }
        if (25 == serial_in_buffer[1]) { McpParseData(0); }
        if (MCP_CALIBRATION_LEN + 3 == serial_in_buffer[1]) { McpParseCalibration(); }
        if (MCP_FREQUENCY_LEN + 3 == serial_in_buffer[1]) { McpParseFrequency(); }
      }

    }
    mcp_timeout = 0;
  }
  else if (MCP_SINGLE_WIRE == serial_in_buffer[0]) {
    if (serial_in_byte_counter == 16) {

      if (McpChecksum((uint8_t *)serial_in_buffer) != serial_in_buffer[serial_in_byte_counter -1]) {
        AddLog_P(LOG_LEVEL_DEBUG, PSTR("MCP: " D_CHECKSUM_FAILURE));
      } else {
        McpParseData(1);
      }

    }
    mcp_timeout = 0;
  }
  return 1;
}

/********************************************************************************************/

void McpEverySecond()
{
  uint8_t get_state[] = { 0xA5, 0x08, 0x41, 0x00, 0x04, 0x4E, 0x16, 0x00 };

  if (mcp_output_registers.active_power) {
    energy_kWhtoday_delta += ((mcp_output_registers.active_power * 10) / 36);
    EnergyUpdateToday();
  }

  if (mcp_timeout) {
    mcp_timeout--;
  }
  else if (mcp_calibration_active) {
    mcp_calibration_active--;
  }
  else if (mcp_init) {
    McpSingleWireStop(1);
    mcp_init = 0;
  }
  else if (!mcp_address) {
    McpGetAddress();
  }
  else if (!mcp_single_wire_active) {
    McpSend(get_state);
  }
}

void McpSnsInit()
{
  digitalWrite(15, 1);               // GPIO15 - MCP enable
}

void McpDrvInit()
{
  if (!energy_flg) {
    if (SHELLY2 == Settings.module) {
      pinMode(15, OUTPUT);
      digitalWrite(15, 0);           // GPIO15 - MCP disable - Reset Delta Sigma ADC's
      baudrate = 4800;
      energy_calc_power_factor = 1;  // Calculate power factor from data
      mcp_timeout = 4;               // Wait for initialization
      mcp_init = 1;                  // Execute initial setup
      McpResetSetpoints();
      energy_flg = XNRG_04;
    }
  }
}

boolean McpCommand()
{
  boolean serviced = true;

  if ((CMND_POWERCAL == energy_command_code) || (CMND_VOLTAGECAL == energy_command_code) || (CMND_CURRENTCAL == energy_command_code)) {

    // MCP Debug commands - PowerCal <payload>
//    if (1 == XdrvMailbox.payload) { McpSingleWireStart(); }
//    if (2 == XdrvMailbox.payload) { McpSingleWireStop(0); }
//    if (3 == XdrvMailbox.payload) { McpGetAddress(); }

    serviced = false;
  }
  else if (CMND_POWERSET == energy_command_code) {
    if (XdrvMailbox.data_len && mcp_output_registers.active_power) {
      Settings.energy_power_calibration = (unsigned long)(CharToDouble(XdrvMailbox.data) * 100);
      mcp_calibration_setpoint.calibration_active_power = Settings.energy_power_calibration;
      McpGetCalibration();
    }
  }
  else if (CMND_VOLTAGESET == energy_command_code) {
    if (XdrvMailbox.data_len && mcp_output_registers.voltage_rms) {
      Settings.energy_voltage_calibration = (unsigned long)(CharToDouble(XdrvMailbox.data) * 10);
      mcp_calibration_setpoint.calibration_voltage = Settings.energy_voltage_calibration;
      McpGetCalibration();
    }
  }
  else if (CMND_CURRENTSET == energy_command_code) {
    if (XdrvMailbox.data_len && mcp_output_registers.current_rms) {
      Settings.energy_current_calibration = (unsigned long)(CharToDouble(XdrvMailbox.data) * 10);
      mcp_calibration_setpoint.calibration_current = Settings.energy_current_calibration;
      McpGetCalibration();
    }
  }
  else if (CMND_FREQUENCYSET == energy_command_code) {
    if (XdrvMailbox.data_len && mcp_output_registers.line_frequency) {
      Settings.energy_frequency_calibration = (unsigned long)(CharToDouble(XdrvMailbox.data) * 10);
      mcp_calibration_setpoint.line_frequency_ref = Settings.energy_frequency_calibration;
      McpGetFrequency();
    }
  }
  else serviced = false;  // Unknown command

  return serviced;
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

int Xnrg04(byte function)
{
  int result = 0;

  if (FUNC_PRE_INIT == function) {
    McpDrvInit();
  }
  else if (XNRG_04 == energy_flg) {
    switch (function) {
      case FUNC_INIT:
        McpSnsInit();
        break;
      case FUNC_EVERY_SECOND:
        McpEverySecond();
        break;
      case FUNC_COMMAND:
        result = McpCommand();
        break;
      case FUNC_SERIAL:
        result = McpSerialInput();
        break;
    }
  }
  return result;
}

#endif  // USE_MCP39F501
#endif  // USE_ENERGY_SENSOR
