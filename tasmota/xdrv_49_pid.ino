/*
  xdrv_49_pid.ino - PID algorithm plugin for Sonoff-Tasmota
  Copyright (C) 2018 Colin Law and Thomas Herrmann
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

/**
 * Code to
 *
 * Usage:
 * Place this file in the sonoff folder.
 * Clone the library https://github.com/colinl/process-control.git from Github
 * into a subfolder of lib.
 * If you want to use a time proportioned relay output with this then also get
 * xdrv_49_timeprop.ino
 * In user_config.h or user_config_override.h include code as follows:

 #define USE_PID         // include the pid feature (+4.3k)
   #define PID_SETPOINT                  19.5    // Setpoint value. This is the process value that the process is
                                                 // aiming for.
                                                 // May be adjusted via MQTT using cmnd PidSp

   #define PID_PROPBAND                  5       // Proportional band in process units (eg degrees). This controls
                                                 // the gain of the loop and is the range of process value over which
                                                 // the power output will go from 0 to full power. The units are that
                                                 // of the process and setpoint, so for example in a heating
                                                 // application it might be set to 1.5 degrees.
                                                 // May be adjusted via MQTT using cmnd PidPb

   #define PID_INTEGRAL_TIME             1800    // Integral time seconds. This is a setting for the integral time,
                                                 // in seconds. It represents the time constant of the integration
                                                 // effect. The larger the value the slower the integral effect will be.
                                                 // Obviously the slower the process is the larger this should be. For
                                                 // example for a domestic room heated by convection radiators a setting
                                                 // of one hour might be appropriate (in seconds). To disable the
                                                 // integral effect set this to a large number.
                                                 // May be adjusted via MQTT using cmnd PidTi

   #define PID_DERIVATIVE_TIME           15      // Derivative time seconds. This is a setting for the derivative time,
                                                 // in seconds. It represents the time constant of the derivative effect.
                                                 // The larger the value the greater will be the derivative effect.
                                                 // Typically this will be set to somewhat less than 25% of the integral
                                                 // setting, once the integral has been adjusted to the optimum value. To
                                                 // disable the derivative effect set this to 0. When initially tuning a
                                                 // loop it is often sensible to start with derivative zero and wind it in
                                                 // once other parameters have been setup.
                                                 // May be adjusted via MQTT using cmnd PidTd

   #define PID_INITIAL_INT               0.5     // Initial integral value (0:1). This is an initial value which is used
                                                 // to preset the integrated error value when the flow is deployed in
                                                 // order to assist in homing in on the setpoint the first time. It should
                                                 // be set to an estimate of what the power requirement might be in order
                                                 // to maintain the process at the setpoint. For example for a domestic
                                                 // room heating application it might be set to 0.2 indicating that 20% of
                                                 // the available power might be required to maintain the setpoint. The
                                                 // value is of no consequence apart from device restart.

   #define PID_MAX_INTERVAL              300     // This is the maximum time in seconds that is expected between samples.
                                                 // It is provided to cope with unusual situations such as a faulty sensor
                                                 // that might prevent the node from being supplied with a process value.
                                                 // If no new process value is received for this time then the power is set
                                                 // to the value defined for PID_MANUAL_POWER.
                                                 // May be adjusted via MQTT using cmnd PidMaxInterval

   #define PID_DERIV_SMOOTH_FACTOR       3       // In situations where the process sensor has limited resolution (such as
                                                 // the DS18B20), the use of deriviative can be problematic as when the
                                                 // process is changing only slowly the steps in the value cause spikes in
                                                 // the derivative. To reduce the effect of these this parameter can be
                                                 // set to apply a filter to the derivative term. I have found that with
                                                 // the DS18B20 that a value of 3 here can be beneficial, providing
                                                 // effectively a low pass filter on the derivative at 1/3 of the derivative
                                                 // time. This feature may also be useful if the process value is particularly
                                                 // noisy. The smaller the value the greater the filtering effect but the
                                                 // more it will reduce the effectiveness of the derivative. A value of zero
                                                 // disables this feature.
                                                 // May be adjusted via MQTT using cmnd PidDSmooth

   #define PID_AUTO                      1       // Auto mode 1 or 0 (for manual). This can be used to enable or disable
                                                 // the control (1=enable, auto mode, 0=disabled, manual mode). When in
                                                 // manual mode the output is set the value definded for PID_MANUAL_POWER
                                                 // May be adjusted via MQTT using cmnd PidAuto

   #define PID_MANUAL_POWER              0       // Power output when in manual mode or fallback mode if too long elapses
                                                 // between process values
                                                 // May be adjusted via MQTT using cmnd PidManualPower

   #define PID_UPDATE_SECS               0       // How often to run the pid algorithm (integer secs) or 0 to run the algorithm
                                                 // each time a new pv value is received, for most applictions specify 0.
                                                 // Otherwise set this to a time
                                                 // that is short compared to the response of the process.  For example,
                                                 // something like 15 seconds may well be appropriate for a domestic room
                                                 // heating application.
                                                 // May be adjusted via MQTT using cmnd PidUpdateSecs

   #define PID_USE_TIMPROP               1       // To use an internal relay for a time proportioned output to drive the
                                                 // process, set this to indicate which timeprop output to use. For a device
                                                 // with just one relay then this will be 1.
                                                 // It is then also necessary to define USE_TIMEPROP and set the output up as
                                                 // explained in xdrv_49_timeprop.ino
                                                 // To disable this feature leave this undefined (undefined, not defined to nothing).

   #define PID_USE_LOCAL_SENSOR                  // If defined then the local sensor will be used for pv. Leave undefined if
                                                 // this is not required.  The rate that the sensor is read is defined by TELE_PERIOD
                                                 // If not using the sensor then you can supply process values via MQTT using
                                                 // cmnd PidPv

   #define PID_SHUTTER                   1     // if using the PID to control a 3-way valve, create Tasmota Shutter and define the 
                                                 // number of the shutter here. Otherwise leave this commented out

   #define PID_REPORT_MORE_SETTINGS                   // If defined, the SENSOR output will provide more extensive json
                                                 // output in the PID section

 * Help with using the PID algorithm and with loop tuning can be found at
 * http://blog.clanlaw.org.uk/2018/01/09/PID-tuning-with-node-red-contrib-pid.html
 * This is directed towards using the algorithm in the node-red node node-red-contrib-pid but the algorithm here is based on
 * the code there and the tuning techique described there should work just the same.

 *
**/


#ifdef USE_PID

#include "PID.h"

/* This might need to go to i18n.h */
#define D_PRFX_PID "Pid"
#define D_CMND_PID_SETPV "Pv"
#define D_CMND_PID_SETSETPOINT "Sp"
#define D_CMND_PID_SETPROPBAND "Pb"
#define D_CMND_PID_SETINTEGRAL_TIME "Ti"
#define D_CMND_PID_SETDERIVATIVE_TIME "Td"
#define D_CMND_PID_SETINITIAL_INT "Initint"
#define D_CMND_PID_SETDERIV_SMOOTH_FACTOR "DSmooth"
#define D_CMND_PID_SETAUTO "Auto"
#define D_CMND_PID_SETMANUAL_POWER "ManualPower"
#define D_CMND_PID_SETMAX_INTERVAL "MaxInterval"
#define D_CMND_PID_SETUPDATE_SECS "UpdateSecs"

const char kPIDCommands[] PROGMEM = D_PRFX_PID "|" // Prefix
  D_CMND_PID_SETPV "|"
  D_CMND_PID_SETSETPOINT "|"
  D_CMND_PID_SETPROPBAND "|"
  D_CMND_PID_SETINTEGRAL_TIME "|"
  D_CMND_PID_SETDERIVATIVE_TIME "|"
  D_CMND_PID_SETINITIAL_INT "|"
  D_CMND_PID_SETDERIV_SMOOTH_FACTOR "|"
  D_CMND_PID_SETAUTO "|"
  D_CMND_PID_SETMANUAL_POWER "|"
  D_CMND_PID_SETMAX_INTERVAL "|"
  D_CMND_PID_SETUPDATE_SECS;
  ;

void (* const PIDCommand[])(void) PROGMEM = {
  &CmndSetPv, 
  &CmndSetSp,
  &CmndSetPb,
  &CmndSetTi,
  &cmndsetTd,
  &CmndSetInitialInt,
  &CmndSetDSmooth,
  &CmndSetAuto,
  &CmndSetManualPower,
  &CmndSetMaxInterval,
  &CmndSetUpdateSecs
  };

static PID pid;
static int update_secs = PID_UPDATE_SECS <= 0  ?  0  :  PID_UPDATE_SECS;   // how often (secs) the pid alogorithm is run
static int max_interval = PID_MAX_INTERVAL;
static unsigned long last_pv_update_secs = 0;
static bool run_pid_now = false;     // tells PID_Every_Second to run the pid algorithm

static long pid_current_time_secs = 0;  // a counter that counts seconds since initialisation

void PID_Init()
{
  pid.initialise( PID_SETPOINT, PID_PROPBAND, PID_INTEGRAL_TIME, PID_DERIVATIVE_TIME, PID_INITIAL_INT,
    PID_MAX_INTERVAL, PID_DERIV_SMOOTH_FACTOR, PID_AUTO, PID_MANUAL_POWER );
}

void PID_Every_Second() {
  static int sec_counter = 0;
  pid_current_time_secs++;    // increment time
  // run the pid algorithm if run_pid_now is true or if the right number of seconds has passed or if too long has
  // elapsed since last pv update. If too long has elapsed the the algorithm will deal with that.
  if (run_pid_now  ||  pid_current_time_secs - last_pv_update_secs > max_interval  ||  (update_secs != 0 && sec_counter++ % update_secs  ==  0)) {
    run_pid();
    run_pid_now = false;
  }
}

void PID_Show_Sensor() {
  // Called each time new sensor data available, data in mqtt data in same format
  // as published in tele/SENSOR
  // Update period is specified in TELE_PERIOD
  if (!isnan(TasmotaGlobal.temperature_celsius)) {
    const float temperature = TasmotaGlobal.temperature_celsius;

    // pass the value to the pid alogorithm to use as current pv
    last_pv_update_secs = pid_current_time_secs;
    pid.setPv(temperature, last_pv_update_secs);
    // also trigger running the pid algorithm if we have been told to run it each pv sample
    if (update_secs == 0) {
      // this runs it at the next second
      run_pid_now = true;
    } 
  } else {
    AddLog_P(LOG_LEVEL_ERROR, PSTR("PID: No local temperature sensor found"));
  }
}

/* struct XDRVMAILBOX { */
/*   uint16_t      valid; */
/*   uint16_t      index; */
/*   uint16_t      data_len; */
/*   int16_t       payload; */
/*   char         *topic; */
/*   char         *data; */
/* } XdrvMailbox; */

void CmndSetPv(void) {
  last_pv_update_secs = pid_current_time_secs;
  pid.setPv(atof(XdrvMailbox.data), last_pv_update_secs);
  // also trigger running the pid algorithm if we have been told to run it each pv sample
  if (update_secs == 0) {
    // this runs it at the next second
    run_pid_now = true;
  }
}

void CmndSetSp(void) {
  pid.setSp(atof(XdrvMailbox.data));
  ResponseCmndNumber(atof(XdrvMailbox.data));
}

void CmndSetPb(void) {
  pid.setPb(atof(XdrvMailbox.data));
  ResponseCmndNumber(atof(XdrvMailbox.data));
}

void CmndSetTi(void) {
  pid.setTi(atof(XdrvMailbox.data));
  ResponseCmndNumber(atof(XdrvMailbox.data));
}

void cmndsetTd(void) {
  pid.setTd(atof(XdrvMailbox.data));
  ResponseCmndNumber(atof(XdrvMailbox.data));
}

void CmndSetInitialInt(void) {
  pid.setInitialInt(atof(XdrvMailbox.data));
  ResponseCmndNumber(atof(XdrvMailbox.data));
}

void CmndSetDSmooth(void) {
  pid.setDSmooth(atof(XdrvMailbox.data));
  ResponseCmndNumber(atof(XdrvMailbox.data));
}

void CmndSetAuto(void) {
  pid.setAuto(atoi(XdrvMailbox.data));
  ResponseCmndNumber(atoi(XdrvMailbox.data));
}

void CmndSetManualPower(void) {
  pid.setManualPower(atof(XdrvMailbox.data));
  ResponseCmndNumber(atof(XdrvMailbox.data));
}

void CmndSetMaxInterval(void) {
  pid.setMaxInterval(atoi(XdrvMailbox.data));
  ResponseCmndNumber(atoi(XdrvMailbox.data));
}

// case CMND_PID_SETUPDATE_SECS:
//   update_secs = atoi(XdrvMailbox.data) ;
//   if (update_secs < 0)
//     update_secs = 0;
void CmndSetUpdateSecs(void) {
  update_secs = (atoi(XdrvMailbox.data));
  if (update_secs < 0)
    update_secs = 0;
  ResponseCmndNumber(update_secs);
}

void PIDShowValues(void) {
  char str_buf[FLOATSZ];
  char chr_buf;
  int i_buf;
  double d_buf;
  ResponseAppend_P(PSTR(",\"PID\":{"));

// #define D_CMND_PID_SETPV "Pv"
  d_buf = pid.getPv();
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidPv\":%s,"), str_buf);
// #define D_CMND_PID_SETSETPOINT "Sp"
  d_buf = pid.getSp();
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidSp\":%s,"), str_buf);

#ifdef PID_REPORT_MORE_SETTINGS
// #define D_CMND_PID_SETPROPBAND "Pb"
  d_buf = pid.getPb();
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidPb\":%s,"), str_buf);
// #define D_CMND_PID_SETINTEGRAL_TIME "Ti"
  d_buf = pid.getTi();
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidTi\":%s,"), str_buf);
// #define D_CMND_PID_SETDERIVATIVE_TIME "Td"
  d_buf = pid.getTd();
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidTd\":%s,"), str_buf);
// #define D_CMND_PID_SETINITIAL_INT "Initint"
  d_buf = pid.getInitialInt();
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidInitialInt\":%s,"), str_buf);
// #define D_CMND_PID_SETDERIV_SMOOTH_FACTOR "DSmooth"
  d_buf = pid.getDSmooth();
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidDSmooth\":%s,"), str_buf);
// #define D_CMND_PID_SETAUTO "Auto"
  chr_buf = pid.getAuto();
  ResponseAppend_P(PSTR("\"PidAuto\":%d,"), chr_buf);
// #define D_CMND_PID_SETMANUAL_POWER "ManualPower"
  d_buf = pid.getManualPower();
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidManualPower\":%s,"), str_buf);
// #define D_CMND_PID_SETMAX_INTERVAL "MaxInterval"
  i_buf = pid.getMaxInterval();
  ResponseAppend_P(PSTR("\"PidMaxInterval\":%d,"), i_buf);

// #define D_CMND_PID_SETUPDATE_SECS "UpdateSecs"
  ResponseAppend_P(PSTR("\"PidUpdateSecs\":%d,"), update_secs);
#endif // PID_REPORT_MORE_SETTINGS

// The actual power value
  d_buf = pid.tick(pid_current_time_secs);
  dtostrfd(d_buf, 2, str_buf);
  ResponseAppend_P(PSTR("\"PidPower\":%s"), str_buf);

  ResponseAppend_P(PSTR("}"));
}

static void run_pid()
{
  double power = pid.tick(pid_current_time_secs);
#ifdef PID_DONT_USE_PID_TOPIC
  // This part is left inside to regularly publish the PID Power via
  // `%topic%/PID {"power":"0.000"}`
  char str_buf[FLOATSZ];
  dtostrfd(power, 3, str_buf);
  snprintf_P(TasmotaGlobal.mqtt_data, sizeof(TasmotaGlobal.mqtt_data), PSTR("{\"%s\":\"%s\"}"), "power", str_buf);
  MqttPublishPrefixTopic_P(TELE, "PID", false);
#endif // PID_DONT_USE_PID_TOPIC

#if defined PID_SHUTTER
    // send output as a position from 0-100 to defined shutter
    int pos = power * 100;
    ShutterSetPosition(PID_SHUTTER, pos);
#endif //PID_SHUTTER

#if defined PID_USE_TIMPROP
    // send power to appropriate timeprop output
    Timeprop_Set_Power( PID_USE_TIMPROP-1, power );
#endif // PID_USE_TIMPROP
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

#define XDRV_49       49

bool Xdrv49(byte function)
{
  bool result = false;

  switch (function) {
  case FUNC_INIT:
    PID_Init();
    break;
  case FUNC_EVERY_SECOND:
    PID_Every_Second();
    break;
  case FUNC_SHOW_SENSOR:
    // only use this if the pid loop is to use the local sensor for pv
    #if defined PID_USE_LOCAL_SENSOR
      PID_Show_Sensor();
    #endif // PID_USE_LOCAL_SENSOR
    break;
  case FUNC_COMMAND:
    result = DecodeCommand(kPIDCommands, PIDCommand);
    break;
  case FUNC_JSON_APPEND:
    PIDShowValues();
    break;
  }
  return result;
}
#endif // USE_PID
