/*
  xsns_48_chirp.ino - soil moisture sensor support for Sonoff-Tasmota

  Copyright (C) 2019  Theo Arends & Christian Baars

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

  --------------------------------------------------------------------------------------------
  Version Date      Action    Description
  --------------------------------------------------------------------------------------------


  ---
  1.0.0.0 20190608  started   - further development by Christian Baars  - https://github.com/Staars/Sonoff-Tasmota
                    forked    - from arendst/tasmota                    - https://github.com/arendst/Sonoff-Tasmota
                    base      - code base from arendst and              - https://github.com/Miceuz/i2c-moisture-sensor

*/

#ifdef USE_I2C
#ifdef USE_CHIRP

/*********************************************************************************************\
 * CHIRP - Soil moisture sensor
 * 
 * I2C Address: 0x20 - standard address, is changeable
\*********************************************************************************************/

#define XSNS_48                     48
#define CHIRP_MAX_SENSOR_COUNT      3               // 127 is expectectd to be the max number

#define CHIRP_ADDR_STANDARD         0x20            // standard address

/*********************************************************************************************\
 * constants
\*********************************************************************************************/

#define D_CMND_CHIRP "CHIRP"

const char S_JSON_CHIRP_COMMAND_NVALUE[] PROGMEM = "{\"" D_CMND_CHIRP "%s\":%d}";
const char S_JSON_CHIRP_COMMAND[] PROGMEM        = "{\"" D_CMND_CHIRP "%s\"}";
const char kCHIRP_Commands[] PROGMEM             = "Select|Set|Scan|Reset|Sleep|Wake";

const char kChirpTypes[] PROGMEM = "CHIRP";

/*********************************************************************************************\
 * enumerations
\*********************************************************************************************/

enum CHIRP_Commands {                                 // commands useable in console or rules
  CMND_CHIRP_SELECT,                                  // select active sensor by I2C address, makes only sense for multiple sensors
  CMND_CHIRP_SET,                                     // set new I2C address for selected/active sensor, will reset
  CMND_CHIRP_SCAN,                                    // scan the I2C bus for one or more chirp sensors
  CMND_CHIRP_RESET,                                   // CHIRPReset, a fresh and default restart
  CMND_CHIRP_SLEEP,                                   // put sensor to sleep
  CMND_CHIRP_WAKE };                                  // wake sensor by reading firmware version


/*********************************************************************************************\
 * command defines
\*********************************************************************************************/

#define CHIRP_GET_CAPACITANCE       0x00            // 16 bit, read
#define CHIRP_SET_ADDRESS           0x01            // 8 bit,  write
#define CHIRP_GET_ADDRESS           0x02            // 8 bit,  read
#define CHIRP_MEASURE_LIGHT         0x03            // no value, write, -> initiate measurement, then wait at least 3 seconds
#define CHIRP_GET_LIGHT             0x04            // 16 bit, read, -> higher value means darker environment, noisy data, not calibrated
#define CHIRP_GET_TEMPERATURE       0x05            // 16 bit, read
#define CHIRP_RESET                 0x06            // no value, write
#define CHIRP_GET_VERSION           0x07            // 8 bit, read, -> 22 means 2.2
#define CHIRP_SLEEP                 0x08            // no value, write
#define CHIRP_GET_BUSY              0x09            // 8 bit, read, -> 1 = busy, 0 = otherwise

/*********************************************************************************************\
 * helper function
\*********************************************************************************************/

bool I2cWriteReg(uint8_t addr, uint8_t reg)
{
   return I2cWrite(addr, reg, 0, 0);
}

/********************************************************************************************/

// globals

uint8_t    chirp_current        = 0;    // current selected/active sensor
uint8_t    chirp_found_sensors  = 0;    // number of found sensors

char       chirp_name[7];
uint8_t    chirp_next_job       = 0;    //0=reset, 1=auto-wake, 2=moisture+temperature, 3=light, 4 = pause; 5 = TELE done
uint32_t   chirp_timeout_count  = 0;    //is handled every second, so value is equal to seconds (it is a slow sensor)

#pragma pack(1)
struct ChirpSensor_t{
    uint16_t   moisture = 0;      // shall hold post-processed data, if implemented
    uint16_t   light = 0;         // light level, maybe already postprocessed depending on the firmware
    int16_t    temperature= 0;    // temperature in degrees CELSIUS * 10
    uint8_t    version = 0;       // firmware-version
    uint8_t    address:7;         // we need only 7bit so...
    uint8_t    explicitSleep:1;   // there is a free bit to play with ;)
};
#pragma pack()

ChirpSensor_t chirp_sensor[CHIRP_MAX_SENSOR_COUNT];       // should be 8 bytes per sensor slot

/********************************************************************************************/

void ChirpReset(uint8_t addr) {
    I2cWriteReg(addr, CHIRP_RESET);
}

/********************************************************************************************/

void ChirpResetAll(void) {
    for (uint32_t i = 0; i < chirp_found_sensors; i++) {
      if (chirp_sensor[i].version) { 
        ChirpReset(chirp_sensor[i].address);
        }
    }
}
/********************************************************************************************/

void ChirpClockSet() { // set I2C for this slow sensor
    Wire.setClockStretchLimit(4000);
    Wire.setClock(50000);
}

/********************************************************************************************/

void ChirpSleep(uint8_t addr) {
    I2cWriteReg(addr, CHIRP_SLEEP);
}

/********************************************************************************************/

// void ChirpSleepAll(void) {
//     for (uint32_t i = 0; i < chirp_found_sensors; i++) {
//       if (chirp_sensor[i].version) { 
//         ChirpSleep(chirp_sensor[i].address);
//         }
//     }
// }

// /********************************************************************************************/

// void ChirpAutoWakeAll(void) {
//     for (uint32_t i = 0; i < chirp_found_sensors; i++) {
//       if (chirp_sensor[i].version && !chirp_sensor[i].explicitSleep) { 
//         ChirpReadVersion(chirp_sensor[i].address);
//         }
//     }
// }

/********************************************************************************************/

void ChirpSelect(uint8_t sensor) {
  if(sensor < chirp_found_sensors) { //TODO: show some infos
    chirp_current = sensor;
    AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: Sensor %u now active."), chirp_current);
  }
  if (sensor == 255) {
    AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: Sensor %u active at address 0x%x."), chirp_current, chirp_sensor[chirp_current].address);
  }
}

/********************************************************************************************/

bool ChirpMeasureLight(void) {
  for (uint32_t i = 0; i < chirp_found_sensors; i++) {
    if (chirp_sensor[i].version && !chirp_sensor[i].explicitSleep) { 
      uint8_t lightReady = I2cRead8(chirp_sensor[i].address, CHIRP_GET_BUSY);
      AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: busy status for light for sensor %u"), lightReady);
      if (lightReady == 1) {
        return false; // a measurement is still in progress, we stop everything and come back in the next loop = 1 second
      }
      AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: init measure light for sensor %u"), i);
      I2cWriteReg(chirp_sensor[i].address, CHIRP_MEASURE_LIGHT); 
      }
  }
  return true; // we could read all values (maybe at different times, but that does not really matter) and consider this job finished
}

/********************************************************************************************/

void ChirpReadCapTemp() { // no timeout needed for both measurements, so we do it at once
    for (uint32_t i = 0; i < chirp_found_sensors; i++) {
      if (chirp_sensor[i].version && !chirp_sensor[i].explicitSleep) { 
        AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: now really read CapTemp for sensor at address 0x%x"), chirp_sensor[i].address);
        chirp_sensor[i].moisture = I2cRead16(chirp_sensor[i].address, CHIRP_GET_CAPACITANCE);
        chirp_sensor[i].temperature = I2cRead16(chirp_sensor[i].address, CHIRP_GET_TEMPERATURE); 
        }
    }
}

/********************************************************************************************/

bool ChirpReadLight() {   // sophisticated calculations could be done here
  bool success = false;
  for (uint32_t i = 0; i < chirp_found_sensors; i++) {
    AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: will read light for sensor %u"), i);
    if (chirp_sensor[i].version) {
      if (I2cValidRead16(&chirp_sensor[i].light, chirp_sensor[i].address, CHIRP_GET_LIGHT)){
        AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: light read success"));
        success = true;
      }
      if(!chirp_sensor[i].explicitSleep){ success = true;} 
      }
  }
  return success;
}

/********************************************************************************************/

uint8_t ChirpReadVersion(uint8_t addr) {
  return (I2cRead8(addr, CHIRP_GET_VERSION));
}

/********************************************************************************************/

bool ChirpSet(uint8_t addr) {
  if(addr < 128){
    if (I2cWrite8(chirp_sensor[chirp_current].address, CHIRP_SET_ADDRESS, addr)){
      I2cWrite8(chirp_sensor[chirp_current].address, CHIRP_SET_ADDRESS, addr); // two calls are needed for sensor firmware version 2.6
      AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: Wrote adress %u "), addr);
      ChirpReset(chirp_sensor[chirp_current].address);
      chirp_sensor[chirp_current].address = addr;
      return true;
    }
  }
  return false;
}

/********************************************************************************************/

bool ChirpScan() {
    ChirpClockSet();
    chirp_found_sensors = 0;
    for (uint8_t address = 1; address <= 127; address++) {
      chirp_sensor[chirp_found_sensors].version = 0;
      chirp_sensor[chirp_found_sensors].version = ChirpReadVersion(address);
      delay(2);
      chirp_sensor[chirp_found_sensors].version = ChirpReadVersion(address);
      if(chirp_sensor[chirp_found_sensors].version > 0) {
        AddLog_P2(LOG_LEVEL_DEBUG, S_LOG_I2C_FOUND_AT, "CHIRP:", address);    
        if(chirp_found_sensors<CHIRP_MAX_SENSOR_COUNT){
          chirp_sensor[chirp_found_sensors].address = address; // push next sensor, as long as there is space in the array
          AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: fw %u"), chirp_sensor[chirp_found_sensors].version);
        }
        chirp_found_sensors++;
      }
    }
    AddLog_P2(LOG_LEVEL_DEBUG, PSTR("Found %u CHIRP sensor(s)."), chirp_found_sensors);
    if (chirp_found_sensors == 0) {return false;}
    else {return true;}
}

/********************************************************************************************/

void ChirpDetect(void)
{
  if (chirp_next_job > 0) {
    return;
  }
  AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: scan will start ..."));
  if (ChirpScan()) {
    uint8_t chirp_model = 0;  // TODO: ??
    GetTextIndexed(chirp_name, sizeof(chirp_name), chirp_model, kChirpTypes);
  }
}


/********************************************************************************************/

void ChirpEverySecond(void)
{
  // AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: every second"));
  if(chirp_timeout_count == 0) {    //countdown complete, now do something
      switch(chirp_next_job) {
          case 0:                   //this should only be called after driver initialization
          AddLog_P2(LOG_LEVEL_DEBUG,PSTR( "CHIRP: reset all"));
          ChirpResetAll();
          chirp_timeout_count = 1;
          chirp_next_job++;
          break;
          case 1:                   // auto-sleep-wake seems to expose a fundamental I2C-problem of the sensor and is deactivated
          // AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: auto-wake all"));
          // ChirpAutoWakeAll();       // this is only a wake-up call at the start of next read cycle
          chirp_next_job++;         // go on, next job should start in a second
          break;
          case 2:
          AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: call CapTemp twice"));
          ChirpReadCapTemp();       // it is reported to be useful, to read twice, because otherwise old values are received
          ChirpReadCapTemp();       // this is the "real" read call, we simply overwrite the existing values
          AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: call measure light"));
          ChirpMeasureLight();      // prepare the next step -> initiate light read
          chirp_timeout_count = 2;  // wait 3 seconds, no need to hurry ...
          chirp_next_job++;
          break;
          case 3:
          AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: call read light"));
          if (ChirpReadLight()){                                // now read light and if successful continue, otherwise come back in a second and try again
            // AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: auto-sleep all"));
            // ChirpSleepAll();                                    // let all sensors auto-sleep 
            chirp_next_job++;
          }          
          break;
          case 4:
          AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: paused, waiting for TELE"));
          break;
          case 5:
          if (Settings.tele_period > 9){
              chirp_timeout_count = Settings.tele_period - 10;  // sync it with the TELEPERIOD, we need about up to 10 seconds to measure, depending on the light level
              AddLog_P2(LOG_LEVEL_DEBUG, PSTR("CHIRP: timeout: %u, tele: %u"), chirp_timeout_count, Settings.tele_period);
            }
          chirp_next_job = 1;                                 // back to step 1
          break;
      }
  }
  else {
      chirp_timeout_count--;         // count down
  }
}

/********************************************************************************************/
// normaly in i18n.h

#define D_JSON_MOISTURE "Moisture"

#ifdef USE_WEBSERVER
  // {s} = <tr><th>, {m} = </th><td>, {e} = </td></tr>

 const char HTTP_SNS_MOISTURE[] PROGMEM = "{s} " D_JSON_MOISTURE ": {m}%s %{e}";
 const char HTTP_SNS_CHIRPVER[] PROGMEM = "{s} CHIRP-sensor %u at address: {m}0x%x{e}"
                                          "{s} FW-version: {m}%s {e}";                                                                                            ;
 const char HTTP_SNS_CHIRPSLEEP[] PROGMEM = "{s} {m} is sleeping ...{e}";
#endif  // USE_WEBSERVER


/********************************************************************************************/

void ChirpShow(bool json)
{
  for (uint32_t i = 0; i < chirp_found_sensors; i++) {
    if (chirp_sensor[i].version) {
      // convert double values to string
      char str_moisture[33];      
      dtostrfd(chirp_sensor[i].moisture, 0, str_moisture);
      char str_temperature[33];
      double t_temperature = ((double) chirp_sensor[i].temperature )/10.0;   
      dtostrfd(t_temperature, Settings.flag2.temperature_resolution, str_temperature);
      char str_light[33];      
      dtostrfd(chirp_sensor[i].light, 0, str_light);
      char str_version[33];      
      dtostrfd(chirp_sensor[i].version, 0, str_version);
      if (json) {
        if(!chirp_sensor[i].explicitSleep){
          ResponseAppend_P(PSTR(",\"%s%u\":{\"" D_JSON_MOISTURE "\":%s,\"" D_JSON_TEMPERATURE "\":%s,\"" D_JSON_ILLUMINANCE "\":\"%s}"),
          chirp_name, i, str_moisture, str_temperature, str_light);}
        else {
          ResponseAppend_P(PSTR(",\"%s%u\":{\"sleeping\"}"),
          chirp_name, i);
        }
  #ifdef USE_DOMOTICZ
      if (0 == tele_period) {
              DomoticzTempHumSensor(str_temperature, str_moisture);
              DomoticzSensor(DZ_ILLUMINANCE,chirp_sensor[i].light);
        }
  #endif  // USE_DOMOTICZ
  #ifdef USE_WEBSERVER
      } else {
        WSContentSend_PD(HTTP_SNS_CHIRPVER, i, chirp_sensor[i].address, str_version);
        if (chirp_sensor[i].explicitSleep){
          WSContentSend_PD(HTTP_SNS_CHIRPSLEEP);
        }
        else {
          WSContentSend_PD(HTTP_SNS_MOISTURE, str_moisture);
          WSContentSend_PD(HTTP_SNS_ILLUMINANCE, " ", chirp_sensor[i].light);
          WSContentSend_PD(HTTP_SNS_TEMP, " ",str_temperature, TempUnit());   
        }
  
  #endif  // USE_WEBSERVER
      }
    }
  }  
}

/*********************************************************************************************\
 * check the Chirp commands
\*********************************************************************************************/

bool ChirpCmd(void) {
  char command[CMDSZ];
  bool serviced = true;
  uint8_t disp_len = strlen(D_CMND_CHIRP);

  if (!strncasecmp_P(XdrvMailbox.topic, PSTR(D_CMND_CHIRP), disp_len)) {  // prefix
    int command_code = GetCommandCode(command, sizeof(command), XdrvMailbox.topic + disp_len, kCHIRP_Commands);

    switch (command_code) {
      case CMND_CHIRP_SELECT:
      case CMND_CHIRP_SET:
        if (XdrvMailbox.data_len > 0) {
          if (command_code == CMND_CHIRP_SELECT)  { ChirpSelect(XdrvMailbox.payload); }                       //select active sensor, i.e. for wake, sleep or reset
          if (command_code == CMND_CHIRP_SET)     { ChirpSet((uint8_t)XdrvMailbox.payload); }                 //set and change I2C-address of selected sensor
        Response_P(S_JSON_CHIRP_COMMAND_NVALUE, command, XdrvMailbox.payload);
        }
        else {
          if (command_code == CMND_CHIRP_SELECT)  { ChirpSelect(255); }                                       //show active sensor
        Response_P(S_JSON_CHIRP_COMMAND, command, XdrvMailbox.payload);
        }     
        break;
      case CMND_CHIRP_SCAN: 
      case CMND_CHIRP_SLEEP:
      case CMND_CHIRP_WAKE:
      case CMND_CHIRP_RESET:
        if (command_code == CMND_CHIRP_SCAN)     {  chirp_next_job = 0;
                                                    ChirpDetect(); }                                            // this will re-init the sensor array
        if (command_code == CMND_CHIRP_SLEEP)    {  chirp_sensor[chirp_current].explicitSleep = true;         // we do not touch this sensor in the read functions
                                                    ChirpSleep(chirp_sensor[chirp_current].address); }
        if (command_code == CMND_CHIRP_WAKE)     {  chirp_sensor[chirp_current].explicitSleep = false;        // back in action
                                                    ChirpReadVersion(chirp_sensor[chirp_current].address); }  // just use read version as wakeup call                                         
        if (command_code == CMND_CHIRP_RESET)    { ChirpReset(chirp_sensor[chirp_current].address); }
        Response_P(S_JSON_CHIRP_COMMAND, command, XdrvMailbox.payload);
        break;
      default:
    	  // else for Unknown command
    	  serviced = false;
    	break;
    }
  }
  return serviced;
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xsns48(uint8_t function)
{
  bool result = false;

  if (i2c_flg) {  
    switch (function) {
      case FUNC_INIT:
        ChirpDetect();         // We can call CHIRPSCAN later to re-detect
        break;
      case FUNC_EVERY_SECOND:
        if(chirp_found_sensors > 0){
          ChirpEverySecond();
        }    
        break;
      case FUNC_COMMAND:
        result = ChirpCmd();  
        break;
      case FUNC_JSON_APPEND:
        ChirpShow(1);
        chirp_next_job = 5; // TELE done, now compute time for next measure cycle
        break;
#ifdef USE_WEBSERVER
      case FUNC_WEB_SENSOR:
        ChirpShow(0);
        break;
#endif  // USE_WEBSERVER
    }
  }
  return result;
}

#endif  // USE_CHIRP
#endif  // USE_I2C