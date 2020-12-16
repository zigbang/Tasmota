/*
  xsns_75_prometheus.ino - Web based information for Tasmota

  Copyright (C) 2020  Theo Arends

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

#ifdef USE_PROMETHEUS
/*********************************************************************************************\
 * Prometheus support
\*********************************************************************************************/

#define XSNS_75 75

const char *UnitfromType(const char *type)
{
  if (strcmp(type, "time") == 0)
  {
    return "_seconds";
  }
  if (strcmp(type, "temperature") == 0 || strcmp(type, "dewpoint") == 0)
  {
    return "_celsius";
  }
  if (strcmp(type, "pressure") == 0)
  {
    return "_hpa";
  }
  if (strcmp(type, "voltage") == 0)
  {
    return "_volts";
  }
  if (strcmp(type, "current") == 0)
  {
    return "_amperes";
  }
  if (strcmp(type, "mass") == 0)
  {
    return "_grams";
  }
  if (strcmp(type, "carbondioxide") == 0)
  {
    return "_ppm";
  }
  if (strcmp(type, "humidity") == 0)
  {
    return "_percentage";
  }
  return "";
}

const char *FormatMetricName(const char *metric)
{
  char *formated = (char *)malloc(strlen(metric) + 1);
  uint32_t cnt = 0;
  for (cnt; cnt < strlen(metric) + 1; cnt++)
  {
    if (metric[cnt] == ' ')
    { //replace space with
      formated[cnt] = '_';
    }
    else
    {
      formated[cnt] = tolower(metric[cnt]);
    }
  }
  return formated;
}

void HandleMetrics(void)
{
  if (!HttpCheckPriviledgedAccess())
  {
    return;
  }

  AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP "Prometheus"));

  WSContentBegin(200, CT_PLAIN);

  char parameter[FLOATSZ];

  // Pseudo-metric providing metadata about the running firmware version.
  WSContentSend_P(PSTR("# TYPE tasmota_info gauge\ntasmota_info{version=\"%s\",image=\"%s\",build_timestamp=\"%s\"} 1\n"),
                  TasmotaGlobal.version, TasmotaGlobal.image_name, GetBuildDateAndTime().c_str());
  WSContentSend_P(PSTR("# TYPE tasmota_uptime_seconds gauge\ntasmota_uptime_seconds %d\n"), TasmotaGlobal.uptime);
  WSContentSend_P(PSTR("# TYPE tasmota_boot_count counter\ntasmota_boot_count %d\n"), Settings.bootcount);
  WSContentSend_P(PSTR("# TYPE tasmota_flash_writes_total counter\ntasmota_flash_writes_total %d\n"), Settings.save_flag);

  // Pseudo-metric providing metadata about the WiFi station.
  WSContentSend_P(PSTR("# TYPE tasmota_wifi_station_info gauge\ntasmota_wifi_station_info{bssid=\"%s\",ssid=\"%s\"} 1\n"), WiFi.BSSIDstr().c_str(), WiFi.SSID().c_str());

  // Wi-Fi Signal strength
  WSContentSend_P(PSTR("# TYPE tasmota_wifi_station_signal_dbm gauge\ntasmota_wifi_station_signal_dbm{mac_address=\"%s\"} %d\n"), WiFi.BSSIDstr().c_str(), WiFi.RSSI());

  if (!isnan(TasmotaGlobal.temperature_celsius))
  {
    dtostrfd(TasmotaGlobal.temperature_celsius, Settings.flag2.temperature_resolution, parameter);
    WSContentSend_P(PSTR("# TYPE tasmotaglobal_temperature_celsius gauge\ntasmotaglobal_temperature_celsius %s\n"), parameter);
  }
  if (TasmotaGlobal.humidity != 0)
  {
    dtostrfd(TasmotaGlobal.humidity, Settings.flag2.humidity_resolution, parameter);
    WSContentSend_P(PSTR("# TYPE tasmotaglobal_humidity gauge\ntasmotaglobal_humidity %s\n"), parameter);
  }
  if (TasmotaGlobal.pressure_hpa != 0)
  {
    dtostrfd(TasmotaGlobal.pressure_hpa, Settings.flag2.pressure_resolution, parameter);
    WSContentSend_P(PSTR("# TYPE tasmotaglobal_pressure_hpa gauge\ntasmotaglobal_pressure_hpa %s\n"), parameter);
  }

#ifdef USE_ENERGY_SENSOR
  dtostrfd(Energy.voltage[0], Settings.flag2.voltage_resolution, parameter);
  WSContentSend_P(PSTR("# TYPE energy_voltage_volts gauge\nenergy_voltage_volts %s\n"), parameter);
  dtostrfd(Energy.current[0], Settings.flag2.current_resolution, parameter);
  WSContentSend_P(PSTR("# TYPE energy_current_amperes gauge\nenergy_current_amperes %s\n"), parameter);
  dtostrfd(Energy.active_power[0], Settings.flag2.wattage_resolution, parameter);
  WSContentSend_P(PSTR("# TYPE energy_power_active_watts gauge\nenergy_power_active_watts %s\n"), parameter);
  dtostrfd(Energy.daily, Settings.flag2.energy_resolution, parameter);
  WSContentSend_P(PSTR("# TYPE energy_power_kilowatts_daily counter\nenergy_power_kilowatts_daily %s\n"), parameter);
  dtostrfd(Energy.total, Settings.flag2.energy_resolution, parameter);
  WSContentSend_P(PSTR("# TYPE energy_power_kilowatts_total counter\nenergy_power_kilowatts_total %s\n"), parameter);
#endif

  for (uint32_t device = 0; device < TasmotaGlobal.devices_present; device++)
  {
    power_t mask = 1 << device;
    WSContentSend_P(PSTR("# TYPE relay%d_state gauge\nrelay%d_state %d\n"), device + 1, device + 1, (TasmotaGlobal.power & mask));
  }

  ResponseClear();
  MqttShowSensor();
  char json[strlen(TasmotaGlobal.mqtt_data) + 1];
  snprintf_P(json, sizeof(json), TasmotaGlobal.mqtt_data);
  String jsonStr = json;
  JsonParser parser((char *)jsonStr.c_str());
  JsonParserObject root = parser.getRootObject();
  if (root)
  { // did JSON parsing went ok?
    for (auto key1 : root)
    {
      JsonParserToken value1 = key1.getValue();
      if (value1.isObject())
      {
        JsonParserObject Object2 = value1.getObject();
        for (auto key2 : Object2)
        {
          JsonParserToken value2 = key2.getValue();
          if (value2.isObject())
          {
            JsonParserObject Object3 = value2.getObject();
            for (auto key3 : Object3)
            {
              const char *value = key3.getValue().getStr(nullptr);
              if (value != nullptr && isdigit(value[0]))
              {
                const char *sensor = FormatMetricName(key2.getStr());                                                                                        //cleanup sensor name
                const char *type = FormatMetricName(key3.getStr());                                                                                          //cleanup sensor type
                const char *unit = UnitfromType(type);                                                                                                       //grab base unit corresponding to type
                WSContentSend_P(PSTR("# TYPE tasmota_sensors_%s%s gauge\ntasmota_sensors_%s%s{sensor=\"%s\"} %s\n"), type, unit, type, unit, sensor, value); //build metric as "# TYPE tasmota_sensors_%type%_%unit% gauge\ntasmotasensors_%type%_%unit%{sensor=%sensor%"} %value%""
              }
            }
          }
          else
          {
            const char *value = value2.getStr(nullptr);
            if (value != nullptr && isdigit(value[0]))
            {
              const char *sensor = FormatMetricName(key1.getStr());
              const char *type = FormatMetricName(key2.getStr());
              const char *unit = UnitfromType(type);
              WSContentSend_P(PSTR("# TYPE tasmota_sensors_%s%s gauge\ntasmota_sensors_%s%s{sensor=\"%s\"} %s\n"), type, unit, type, unit, sensor, value);
            }
          }
        }
      }
      else
      {
        const char *value = value1.getStr(nullptr);
        if (value != nullptr && isdigit(value[0] && strcmp(key1.getStr(), "Time") != 0))
        { //remove false 'time' metric
          const char *sensor = FormatMetricName(key1.getStr());
          WSContentSend_P(PSTR("# TYPE tasmota_sensors_%s gauge\ntasmota_sensors{sensor=\"%s\"} %s\n"), sensor, sensor, value);
        }
      }
    }
  }

  WSContentEnd();
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xsns75(uint8_t function)
{
  bool result = false;

  switch (function)
  {
  case FUNC_WEB_ADD_HANDLER:
    WebServer_on(PSTR("/metrics"), HandleMetrics);
    break;
  }
  return result;
}

#endif // USE_PROMETHEUS
