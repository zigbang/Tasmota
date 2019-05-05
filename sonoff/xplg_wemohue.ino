/*
  xplg_wemohue.ino - wemo and hue support for Sonoff-Tasmota

  Copyright (C) 2019  Heiko Krupp and Theo Arends

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

#if defined(USE_WEBSERVER) && defined(USE_EMULATION)
/*********************************************************************************************\
 * Belkin WeMo and Philips Hue bridge emulation
\*********************************************************************************************/

#define UDP_BUFFER_SIZE         200      // Max UDP buffer size needed for M-SEARCH message
#define UDP_MSEARCH_SEND_DELAY  1500     // Delay in ms before M-Search response is send

#include <Ticker.h>
Ticker TickerMSearch;

IPAddress udp_remote_ip;                 // M-Search remote IP address
uint16_t udp_remote_port;                // M-Search remote port

bool udp_connected = false;
bool udp_response_mutex = false;         // M-Search response mutex to control re-entry

/*********************************************************************************************\
 * UPNP search targets
\*********************************************************************************************/

const char URN_BELKIN_DEVICE[] PROGMEM = "urn:belkin:device:**";
const char UPNP_ROOTDEVICE[] PROGMEM = "upnp:rootdevice";
const char SSDPSEARCH_ALL[] PROGMEM = "ssdpsearch:all";
const char SSDP_ALL[] PROGMEM = "ssdp:all";

/*********************************************************************************************\
 * WeMo UPNP support routines
\*********************************************************************************************/

const char WEMO_MSEARCH[] PROGMEM =
  "HTTP/1.1 200 OK\r\n"
  "CACHE-CONTROL: max-age=86400\r\n"
  "DATE: Fri, 15 Apr 2016 04:56:29 GMT\r\n"
  "EXT:\r\n"
  "LOCATION: http://%s:80/setup.xml\r\n"
  "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
  "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
  "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
  "ST: %s\r\n"                // type1 = urn:Belkin:device:**, type2 = upnp:rootdevice
  "USN: uuid:%s::%s\r\n"      // type1 = urn:Belkin:device:**, type2 = upnp:rootdevice
  "X-User-Agent: redsonic\r\n"
  "\r\n";

String WemoSerialnumber(void)
{
  char serial[16];

  snprintf_P(serial, sizeof(serial), PSTR("201612K%08X"), ESP.getChipId());
  return String(serial);
}

String WemoUuid(void)
{
  char uuid[27];

  snprintf_P(uuid, sizeof(uuid), PSTR("Socket-1_0-%s"), WemoSerialnumber().c_str());
  return String(uuid);
}

void WemoRespondToMSearch(int echo_type)
{
  char message[TOPSZ];

  TickerMSearch.detach();
  if (PortUdp.beginPacket(udp_remote_ip, udp_remote_port)) {
    char type[24];
    if (1 == echo_type) {              // type1 echo 1g & dot 2g
      strcpy_P(type, URN_BELKIN_DEVICE);
    } else {                           // type2 echo 2g (echo, plus, show)
      strcpy_P(type, UPNP_ROOTDEVICE);
    }
    char response[400];
    snprintf_P(response, sizeof(response), WEMO_MSEARCH, WiFi.localIP().toString().c_str(), type, WemoUuid().c_str(), type);
    PortUdp.write(response);
    PortUdp.endPacket();
    snprintf_P(message, sizeof(message), PSTR(D_RESPONSE_SENT));
  } else {
    snprintf_P(message, sizeof(message), PSTR(D_FAILED_TO_SEND_RESPONSE));
  }
  AddLog_P2(LOG_LEVEL_DEBUG, PSTR(D_LOG_UPNP D_WEMO " " D_JSON_TYPE " %d, %s " D_TO " %s:%d"),
    echo_type, message, udp_remote_ip.toString().c_str(), udp_remote_port);

  udp_response_mutex = false;
}

/*********************************************************************************************\
 * Hue Bridge UPNP support routines
 * Need to send 3 response packets with varying ST and USN
 *
 * Using Espressif Inc Mac Address of 5C:CF:7F:00:00:00
 * Philips Lighting is 00:17:88:00:00:00
\*********************************************************************************************/

const char HUE_RESPONSE[] PROGMEM =
  "HTTP/1.1 200 OK\r\n"
  "HOST: 239.255.255.250:1900\r\n"
  "CACHE-CONTROL: max-age=100\r\n"
  "EXT:\r\n"
  "LOCATION: http://%s:80/description.xml\r\n"
  "SERVER: Linux/3.14.0 UPnP/1.0 IpBridge/1.17.0\r\n"
  "hue-bridgeid: %s\r\n";
const char HUE_ST1[] PROGMEM =
  "ST: upnp:rootdevice\r\n"
  "USN: uuid:%s::upnp:rootdevice\r\n"
  "\r\n";
const char HUE_ST2[] PROGMEM =
  "ST: uuid:%s\r\n"
  "USN: uuid:%s\r\n"
  "\r\n";
const char HUE_ST3[] PROGMEM =
  "ST: urn:schemas-upnp-org:device:basic:1\r\n"
  "USN: uuid:%s\r\n"
  "\r\n";

String HueBridgeId(void)
{
  String temp = WiFi.macAddress();
  temp.replace(":", "");
  String bridgeid = temp.substring(0, 6) + "FFFE" + temp.substring(6);
  return bridgeid;  // 5CCF7FFFFE139F3D
}

String HueSerialnumber(void)
{
  String serial = WiFi.macAddress();
  serial.replace(":", "");
  serial.toLowerCase();
  return serial;  // 5ccf7f139f3d
}

String HueUuid(void)
{
  String uuid = F("f6543a06-da50-11ba-8d8f-");
  uuid += HueSerialnumber();
  return uuid;  // f6543a06-da50-11ba-8d8f-5ccf7f139f3d
}

void HueRespondToMSearch(void)
{
  char message[TOPSZ];

  TickerMSearch.detach();
  if (PortUdp.beginPacket(udp_remote_ip, udp_remote_port)) {
    char response[320];
    snprintf_P(response, sizeof(response), HUE_RESPONSE, WiFi.localIP().toString().c_str(), HueBridgeId().c_str());
    int len = strlen(response);

    snprintf_P(response + len, sizeof(response) - len, HUE_ST1, HueUuid().c_str());
    PortUdp.write(response);
    PortUdp.endPacket();

    snprintf_P(response + len, sizeof(response) - len, HUE_ST2, HueUuid().c_str(), HueUuid().c_str());
    PortUdp.write(response);
    PortUdp.endPacket();

    snprintf_P(response + len, sizeof(response) - len, HUE_ST3, HueUuid().c_str());
    PortUdp.write(response);
    PortUdp.endPacket();

    snprintf_P(message, sizeof(message), PSTR(D_3_RESPONSE_PACKETS_SENT));
  } else {
    snprintf_P(message, sizeof(message), PSTR(D_FAILED_TO_SEND_RESPONSE));
  }
  AddLog_P2(LOG_LEVEL_DEBUG, PSTR(D_LOG_UPNP D_HUE " %s " D_TO " %s:%d"),
    message, udp_remote_ip.toString().c_str(), udp_remote_port);

  udp_response_mutex = false;
}

/*********************************************************************************************\
 * Belkin WeMo and Philips Hue bridge UDP multicast support
\*********************************************************************************************/

bool UdpDisconnect(void)
{
  if (udp_connected) {
    PortUdp.flush();
    WiFiUDP::stopAll();
    AddLog_P(LOG_LEVEL_DEBUG, PSTR(D_LOG_UPNP D_MULTICAST_DISABLED));
    udp_connected = false;
  }
  return udp_connected;
}

bool UdpConnect(void)
{
  if (!udp_connected) {
    // Simple Service Discovery Protocol (SSDP)
    if (PortUdp.beginMulticast(WiFi.localIP(), IPAddress(239,255,255,250), 1900)) {
      AddLog_P(LOG_LEVEL_INFO, PSTR(D_LOG_UPNP D_MULTICAST_REJOINED));
      udp_response_mutex = false;
      udp_connected = true;
    } else {
      AddLog_P(LOG_LEVEL_INFO, PSTR(D_LOG_UPNP D_MULTICAST_JOIN_FAILED));
      udp_connected = false;
    }
  }
  return udp_connected;
}

void PollUdp(void)
{
  if (udp_connected) {
    if (PortUdp.parsePacket()) {
      char packet_buffer[UDP_BUFFER_SIZE];     // buffer to hold incoming UDP/SSDP packet

      int len = PortUdp.read(packet_buffer, UDP_BUFFER_SIZE -1);
      packet_buffer[len] = 0;

      AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("UDP: Packet (%d)"), len);
//      AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("\n%s"), packet_buffer);

      if (devices_present && !udp_response_mutex && (strstr_P(packet_buffer, PSTR("M-SEARCH")) != nullptr)) {
        udp_response_mutex = true;

        udp_remote_ip = PortUdp.remoteIP();
        udp_remote_port = PortUdp.remotePort();

//        AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("UDP: M-SEARCH Packet from %s:%d\n%s"),
//          udp_remote_ip.toString().c_str(), udp_remote_port, packet_buffer);

        uint32_t response_delay = UDP_MSEARCH_SEND_DELAY + ((millis() &0x7) * 100);  // 1500 - 2200 msec

        LowerCase(packet_buffer, packet_buffer);
        RemoveSpace(packet_buffer);
        if (EMUL_WEMO == Settings.flag2.emulation) {
          if (strstr_P(packet_buffer, URN_BELKIN_DEVICE) != nullptr) {     // type1 echo dot 2g, echo 1g's
            TickerMSearch.attach_ms(response_delay, WemoRespondToMSearch, 1);
            return;
          }
          else if ((strstr_P(packet_buffer, UPNP_ROOTDEVICE) != nullptr) ||  // type2 Echo 2g (echo & echo plus)
                   (strstr_P(packet_buffer, SSDPSEARCH_ALL) != nullptr) ||
                   (strstr_P(packet_buffer, SSDP_ALL) != nullptr)) {
            TickerMSearch.attach_ms(response_delay, WemoRespondToMSearch, 2);
            return;
          }
        } else {
          if ((strstr_P(packet_buffer, PSTR(":device:basic:1")) != nullptr) ||
              (strstr_P(packet_buffer, UPNP_ROOTDEVICE) != nullptr) ||
              (strstr_P(packet_buffer, SSDPSEARCH_ALL) != nullptr) ||
              (strstr_P(packet_buffer, SSDP_ALL) != nullptr)) {
            TickerMSearch.attach_ms(response_delay, HueRespondToMSearch);
            return;
          }
        }
        udp_response_mutex = false;
      }
    }
    delay(1);
  }
}

/*********************************************************************************************\
 * Wemo web server additions
\*********************************************************************************************/

const char WEMO_EVENTSERVICE_XML[] PROGMEM =
  "<scpd xmlns=\"urn:Belkin:service-1-0\">"
    "<actionList>"
      "<action>"
        "<name>SetBinaryState</name>"
        "<argumentList>"
          "<argument>"
            "<retval/>"
            "<name>BinaryState</name>"
            "<relatedStateVariable>BinaryState</relatedStateVariable>"
            "<direction>in</direction>"
          "</argument>"
        "</argumentList>"
      "</action>"
      "<action>"
        "<name>GetBinaryState</name>"
        "<argumentList>"
          "<argument>"
            "<retval/>"
            "<name>BinaryState</name>"
            "<relatedStateVariable>BinaryState</relatedStateVariable>"
            "<direction>out</direction>"
          "</argument>"
        "</argumentList>"
      "</action>"
    "</actionList>"
    "<serviceStateTable>"
      "<stateVariable sendEvents=\"yes\">"
        "<name>BinaryState</name>"
        "<dataType>bool</dataType>"
        "<defaultValue>0</defaultValue>"
      "</stateVariable>"
      "<stateVariable sendEvents=\"yes\">"
        "<name>level</name>"
        "<dataType>string</dataType>"
        "<defaultValue>0</defaultValue>"
      "</stateVariable>"
    "</serviceStateTable>"
  "</scpd>\r\n\r\n";

const char WEMO_METASERVICE_XML[] PROGMEM =
  "<scpd xmlns=\"urn:Belkin:service-1-0\">"
    "<specVersion>"
      "<major>1</major>"
      "<minor>0</minor>"
    "</specVersion>"
    "<actionList>"
      "<action>"
        "<name>GetMetaInfo</name>"
        "<argumentList>"
          "<retval />"
          "<name>GetMetaInfo</name>"
          "<relatedStateVariable>MetaInfo</relatedStateVariable>"
          "<direction>in</direction>"
        "</argumentList>"
      "</action>"
    "</actionList>"
    "<serviceStateTable>"
      "<stateVariable sendEvents=\"yes\">"
        "<name>MetaInfo</name>"
        "<dataType>string</dataType>"
        "<defaultValue>0</defaultValue>"
      "</stateVariable>"
    "</serviceStateTable>"
  "</scpd>\r\n\r\n";

const char WEMO_RESPONSE_STATE_SOAP[] PROGMEM =
  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
    "<s:Body>"
      "<u:%cetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">"
        "<BinaryState>%d</BinaryState>"
      "</u:%cetBinaryStateResponse>"
    "</s:Body>"
  "</s:Envelope>\r\n";

const char WEMO_SETUP_XML[] PROGMEM =
  "<?xml version=\"1.0\"?>"
  "<root xmlns=\"urn:Belkin:device-1-0\">"
    "<device>"
      "<deviceType>urn:Belkin:device:controllee:1</deviceType>"
      "<friendlyName>{x1</friendlyName>"
      "<manufacturer>Belkin International Inc.</manufacturer>"
      "<modelName>Socket</modelName>"
      "<modelNumber>3.1415</modelNumber>"
      "<UDN>uuid:{x2</UDN>"
      "<serialNumber>{x3</serialNumber>"
      "<binaryState>0</binaryState>"
      "<serviceList>"
        "<service>"
          "<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
          "<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
          "<controlURL>/upnp/control/basicevent1</controlURL>"
          "<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
          "<SCPDURL>/eventservice.xml</SCPDURL>"
        "</service>"
        "<service>"
          "<serviceType>urn:Belkin:service:metainfo:1</serviceType>"
          "<serviceId>urn:Belkin:serviceId:metainfo1</serviceId>"
          "<controlURL>/upnp/control/metainfo1</controlURL>"
          "<eventSubURL>/upnp/event/metainfo1</eventSubURL>"
          "<SCPDURL>/metainfoservice.xml</SCPDURL>"
        "</service>"
      "</serviceList>"
    "</device>"
  "</root>\r\n";

/********************************************************************************************/

void HandleUpnpEvent(void)
{
  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, PSTR(D_WEMO_BASIC_EVENT));

  char event[500];
  strlcpy(event, WebServer->arg(0).c_str(), sizeof(event));

//  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("\n%s"), event);

  //differentiate get and set state
  char state = 'G';
  if (strstr_P(event, PSTR("SetBinaryState")) != nullptr) {
    state = 'S';
    uint8_t power = POWER_TOGGLE;
    if (strstr_P(event, PSTR("State>1</Binary")) != nullptr) {
      power = POWER_ON;
    }
    else if (strstr_P(event, PSTR("State>0</Binary")) != nullptr) {
      power = POWER_OFF;
    }
    if (power != POWER_TOGGLE) {
      uint8_t device = (light_type) ? devices_present : 1;  // Select either a configured light or relay1
      ExecuteCommandPower(device, power, SRC_WEMO);
    }
  }

  snprintf_P(event, sizeof(event), WEMO_RESPONSE_STATE_SOAP, state, bitRead(power, devices_present -1), state);
  WSSend(200, CT_XML, event);
}

void HandleUpnpService(void)
{
  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, PSTR(D_WEMO_EVENT_SERVICE));

  WSSend(200, CT_PLAIN, FPSTR(WEMO_EVENTSERVICE_XML));
}

void HandleUpnpMetaService(void)
{
  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, PSTR(D_WEMO_META_SERVICE));

  WSSend(200, CT_PLAIN, FPSTR(WEMO_METASERVICE_XML));
}

void HandleUpnpSetupWemo(void)
{
  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, PSTR(D_WEMO_SETUP));

  String setup_xml = FPSTR(WEMO_SETUP_XML);
  setup_xml.replace("{x1", Settings.friendlyname[0]);
  setup_xml.replace("{x2", WemoUuid());
  setup_xml.replace("{x3", WemoSerialnumber());
  WSSend(200, CT_XML, setup_xml);
}

/*********************************************************************************************\
 * Hue web server additions
\*********************************************************************************************/

const char HUE_DESCRIPTION_XML[] PROGMEM =
  "<?xml version=\"1.0\"?>"
  "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
  "<specVersion>"
    "<major>1</major>"
    "<minor>0</minor>"
  "</specVersion>"
//  "<URLBase>http://{x1/</URLBase>"
  "<URLBase>http://{x1:80/</URLBase>"
  "<device>"
    "<deviceType>urn:schemas-upnp-org:device:Basic:1</deviceType>"
    "<friendlyName>Amazon-Echo-HA-Bridge ({x1)</friendlyName>"
//    "<friendlyName>Philips hue ({x1)</friendlyName>"
    "<manufacturer>Royal Philips Electronics</manufacturer>"
    "<modelDescription>Philips hue Personal Wireless Lighting</modelDescription>"
    "<modelName>Philips hue bridge 2012</modelName>"
    "<modelNumber>929000226503</modelNumber>"
    "<serialNumber>{x3</serialNumber>"
    "<UDN>uuid:{x2</UDN>"
  "</device>"
  "</root>\r\n"
  "\r\n";
const char HUE_LIGHTS_STATUS_JSON1[] PROGMEM =
  "{\"on\":{state},"
  "{light_status}"
  "\"alert\":\"none\","
  "\"effect\":\"none\","
  "\"reachable\":true}";
const char HUE_LIGHTS_STATUS_JSON2[] PROGMEM =
  ",\"type\":\"Extended color light\","
  "\"name\":\"{j1\","
  "\"modelid\":\"LCT007\","
  "\"uniqueid\":\"{j2\","
  "\"swversion\":\"5.50.1.19085\"}";
const char HUE_GROUP0_STATUS_JSON[] PROGMEM =
  "{\"name\":\"Group 0\","
   "\"lights\":[{l1],"
   "\"type\":\"LightGroup\","
   "\"action\":";
//     "\"scene\":\"none\",";
const char HueConfigResponse_JSON[] PROGMEM =
  "{\"name\":\"Philips hue\","
   "\"mac\":\"{ma\","
   "\"dhcp\":true,"
   "\"ipaddress\":\"{ip\","
   "\"netmask\":\"{ms\","
   "\"gateway\":\"{gw\","
   "\"proxyaddress\":\"none\","
   "\"proxyport\":0,"
   "\"bridgeid\":\"{br\","
   "\"UTC\":\"{dt\","
   "\"whitelist\":{\"{id\":{"
     "\"last use date\":\"{dt\","
     "\"create date\":\"{dt\","
     "\"name\":\"Remote\"}},"
   "\"swversion\":\"01041302\","
   "\"apiversion\":\"1.17.0\","
   "\"swupdate\":{\"updatestate\":0,\"url\":\"\",\"text\":\"\",\"notify\": false},"
   "\"linkbutton\":false,"
   "\"portalservices\":false"
  "}";
const char HUE_LIGHT_RESPONSE_JSON[] PROGMEM =
  "{\"success\":{\"/lights/{id/state/{cm\":{re}}";
const char HUE_ERROR_JSON[] PROGMEM =
  "[{\"error\":{\"type\":901,\"address\":\"/\",\"description\":\"Internal Error\"}}]";

/********************************************************************************************/

String GetHueDeviceId(uint8_t id)
{
  String deviceid = WiFi.macAddress() + F(":00:11-") + String(id);
  deviceid.toLowerCase();
  return deviceid;  // 5c:cf:7f:13:9f:3d:00:11-1
}

String GetHueUserId(void)
{
  char userid[7];

  snprintf_P(userid, sizeof(userid), PSTR("%03x"), ESP.getChipId());
  return String(userid);
}

void HandleUpnpSetupHue(void)
{
  AddLog_P(LOG_LEVEL_DEBUG, S_LOG_HTTP, PSTR(D_HUE_BRIDGE_SETUP));
  String description_xml = FPSTR(HUE_DESCRIPTION_XML);
  description_xml.replace("{x1", WiFi.localIP().toString());
  description_xml.replace("{x2", HueUuid());
  description_xml.replace("{x3", HueSerialnumber());
  WSSend(200, CT_XML, description_xml);
}

void HueNotImplemented(String *path)
{
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_HTTP D_HUE_API_NOT_IMPLEMENTED " (%s)"), path->c_str());

  WSSend(200, CT_JSON, "{}");
}

void HueConfigResponse(String *response)
{
  *response += FPSTR(HueConfigResponse_JSON);
  response->replace("{ma", WiFi.macAddress());
  response->replace("{ip", WiFi.localIP().toString());
  response->replace("{ms", WiFi.subnetMask().toString());
  response->replace("{gw", WiFi.gatewayIP().toString());
  response->replace("{br", HueBridgeId());
  response->replace("{dt", GetDateAndTime(DT_UTC));
  response->replace("{id", GetHueUserId());
}

void HueConfig(String *path)
{
  String response = "";
  HueConfigResponse(&response);
  WSSend(200, CT_JSON, response);
}

// device is forced to CT mode instead of HSB
// only makes sense for LST_COLDWARM, LST_RGBW and LST_RGBWC
bool g_gotct = false;

// store previously set values from the Alexa app
// it allows to correct slight deviations from value set by the app
// The Alexa app is very sensitive to exact values
uint16_t prev_hue = 0;
uint8_t  prev_sat = 0;
uint8_t  prev_bri = 254;
uint16_t prev_ct  = 254;
char     prev_x_str[24] = "\0"; // store previously set xy by Alexa app
char     prev_y_str[24] = "\0";

void HueLightStatus1(uint8_t device, String *response)
{
  uint16_t ct = 0;
  uint8_t  color_mode;
  String light_status = "";
  uint16_t hue = 0;
  uint8_t  sat = 0;
  uint8_t  bri = 254;


  if (light_type) {
    light_state.getHSB(&hue, &sat, nullptr);
    bri = light_state.getBri();

    if (bri > 254)  bri = 254;    // Philips Hue bri is between 1 and 254
    if (bri < 1)    bri = 1;
    if ((bri > prev_bri ? bri - prev_bri : prev_bri - bri) < 1)
      bri = prev_bri;

    if (sat > 254)  sat = 254;  // Philips Hue only accepts 254 as max hue
    if ((sat > prev_sat ? sat - prev_sat : prev_sat - sat) < 1) {
      sat = prev_sat;
    } else {  // if sat was changed outside of Alexa, reset xy
      prev_x_str[0] = prev_y_str[0] = 0;
    }

    hue = changeUIntScale(hue, 0, 359, 0, 65535);
    if ((hue > prev_hue ? hue - prev_hue : prev_hue - hue) < 400) {
      hue = prev_hue;
    } else {  // if hue was changed outside of Alexa, reset xy
      prev_x_str[0] = prev_y_str[0] = 0;
    }

    color_mode = light_state.getColorMode();
    ct = light_state.getCT();
    if (LCM_RGB == color_mode) { g_gotct = false; }
    if (LCM_CT  == color_mode) { g_gotct = true; }
    // If LCM_BOTH == color_mode, leave g_gotct unchanged

    // re-adjust ct if close to command value
    if ((ct > prev_ct ? ct - prev_ct : prev_ct - ct) < 1)
      ct = prev_ct;

    //AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("HueLightStatus1 HSB (%d, %d, %d) Prev_HSB (%d, %d, %d)"),
    //  hue, sat, bri, prev_hue, prev_sat, prev_bri);
  }

  *response += FPSTR(HUE_LIGHTS_STATUS_JSON1);
  response->replace("{state}", (power & (1 << (device-1))) ? "true" : "false");
  // Brightness for all devices with PWM
  //if (LST_SINGLE <= light_subtype) {
  light_status += "\"bri\":";
  light_status += String(bri);
  light_status += ",";
  //}
  if (LST_COLDWARM <= light_subtype) {
    light_status += F("\"colormode\":\"");
    light_status += (g_gotct ? "ct" : "hs");
    light_status += "\",";
  }
  if (LST_RGB <= light_subtype) {  // colors
    if (prev_x_str[0] && prev_y_str[0]) {
      light_status += "\"xy\":[";
      light_status += prev_x_str;
      light_status += ",";
      light_status += prev_y_str;
      light_status += "],";
    } else {
      float x, y;
        light_state.getXY(&x, &y);
      light_status += "\"xy\":[";
      light_status += String(x, 5);
      light_status += ",";
      light_status += String(y, 5);
      light_status += "],";
    }
    light_status += "\"hue\":";
    light_status += String(hue);
    light_status += ",";

    light_status += "\"sat\":";
    light_status += String(sat);
    light_status += ",";
  }
  if (LST_COLDWARM == light_subtype || LST_RGBW <= light_subtype) {  // white temp
    light_status += "\"ct\":";
    light_status += String(ct > 0 ? ct : 284);
    light_status += ",";  // if no ct, default to medium white
  }
  response->replace("{light_status}", light_status);
}

void HueLightStatus2(uint8_t device, String *response)
{
  *response += FPSTR(HUE_LIGHTS_STATUS_JSON2);
  response->replace("{j1", Settings.friendlyname[device-1]);
  response->replace("{j2", GetHueDeviceId(device));
}

void HueGlobalConfig(String *path)
{
  String response;
  uint8_t maxhue = (devices_present > MAX_FRIENDLYNAMES) ? MAX_FRIENDLYNAMES : devices_present;

  path->remove(0,1);                                 // cut leading / to get <id>
  response = F("{\"lights\":{\"");
  for (uint8_t i = 1; i <= maxhue; i++) {
    response += i;
    response += F("\":{\"state\":");
    HueLightStatus1(i, &response);
    HueLightStatus2(i, &response);
    if (i < maxhue) {
      response += ",\"";
    }
  }
  response += F("},\"groups\":{},\"schedules\":{},\"config\":");
  HueConfigResponse(&response);
  response += "}";
  WSSend(200, CT_JSON, response);
}

void HueAuthentication(String *path)
{
  char response[38];

  snprintf_P(response, sizeof(response), PSTR("[{\"success\":{\"username\":\"%s\"}}]"), GetHueUserId().c_str());
  WSSend(200, CT_JSON, response);
}

void HueLights(String *path)
{
/*
 * http://sonoff/api/username/lights/1/state?1={"on":true,"hue":56100,"sat":254,"bri":254,"alert":"none","transitiontime":40}
 */
  String response;
  int code = 200;
  uint16_t tmp = 0;
  uint16_t hue = 0;
  uint8_t  sat = 0;
  uint8_t  bri = 254;
  uint16_t ct = 0;
  bool resp = false;  // is the response non null (add comma between parameters)
  bool on = false;
  bool change = false;  // need to change a parameter to the light
  uint8_t device = 1;
  uint8_t maxhue = (devices_present > MAX_FRIENDLYNAMES) ? MAX_FRIENDLYNAMES : devices_present;

  path->remove(0,path->indexOf("/lights"));          // Remove until /lights
  if (path->endsWith("/lights")) {                   // Got /lights
    response = "{\"";
    for (uint8_t i = 1; i <= maxhue; i++) {
      response += i;
      response += F("\":{\"state\":");
      HueLightStatus1(i, &response);
      HueLightStatus2(i, &response);
      if (i < maxhue) {
        response += ",\"";
      }
    }
    response += "}";
  }
  else if (path->endsWith("/state")) {               // Got ID/state
    path->remove(0,8);                               // Remove /lights/
    path->remove(path->indexOf("/state"));           // Remove /state
    device = atoi(path->c_str());
    if ((device < 1) || (device > maxhue)) {
      device = 1;
    }
    if (WebServer->args()) {
      response = "[";

      StaticJsonBuffer<400> jsonBuffer;
      JsonObject &hue_json = jsonBuffer.parseObject(WebServer->arg((WebServer->args())-1));
      if (hue_json.containsKey("on")) {

        response += FPSTR(HUE_LIGHT_RESPONSE_JSON);
        response.replace("{id", String(device));
        response.replace("{cm", "on");

        on = hue_json["on"];
        switch(on)
        {
          case false : ExecuteCommandPower(device, POWER_OFF, SRC_HUE);
                       response.replace("{re", "false");
                       break;
          case true  : ExecuteCommandPower(device, POWER_ON, SRC_HUE);
                       response.replace("{re", "true");
                       break;
          default    : response.replace("{re", (power & (1 << (device-1))) ? "true" : "false");
                       break;
        }
        resp = true;
      }

      if (light_type) {
        light_state.getHSB(&hue, &sat, nullptr);
        bri = light_state.getBri();   // get the combined bri for CT and RGB, not only the RGB one
        ct = light_state.getCT();
        uint8_t color_mode = light_state.getColorMode();
        if (LCM_RGB == color_mode) { g_gotct = false; }
        if (LCM_CT  == color_mode) { g_gotct = true; }
        // If LCM_BOTH == color_mode, leave g_gotct unchanged
      }
      prev_x_str[0] = prev_y_str[0] = 0;  // reset xy string

      if (hue_json.containsKey("bri")) {             // Brightness is a scale from 1 (the minimum the light is capable of) to 254 (the maximum). Note: a brightness of 1 is not off.
        tmp = hue_json["bri"];
        prev_bri = bri = tmp;   // store command value
        // extend bri value if set to max
        if (254 <= bri) { bri = 255; }
        if (resp) { response += ","; }
        response += FPSTR(HUE_LIGHT_RESPONSE_JSON);
        response.replace("{id", String(device));
        response.replace("{cm", "bri");
        response.replace("{re", String(tmp));
        if (LST_SINGLE <= light_subtype) {
          change = true;
        }
        resp = true;
      }
      // handle xy before Hue/Sat
      // If the request contains both XY and HS, we wan't to give priority to HS
      if (hue_json.containsKey("xy")) {             // Saturation of the light. 254 is the most saturated (colored) and 0 is the least saturated (white).
        float x, y;
        x = hue_json["xy"][0];
        y = hue_json["xy"][1];
        const String &x_str = hue_json["xy"][0];
        const String &y_str = hue_json["xy"][1];
        x_str.toCharArray(prev_x_str, sizeof(prev_x_str));
        y_str.toCharArray(prev_y_str, sizeof(prev_y_str));
        //AddLog_P2(LOG_LEVEL_DEBUG_MORE, "XY (%s %s)", String(prev_x,5).c_str(), String(prev_y,5).c_str());
        uint8_t rr,gg,bb;
        LightStateClass::XyToRgb(x, y, &rr, &gg, &bb);
        LightStateClass::RgbToHsb(rr, gg, bb, &hue, &sat, nullptr);
        prev_hue = changeUIntScale(hue, 0, 359, 0, 65535);  // calculate back prev_hue
        prev_sat = (sat > 254 ? 254 : sat);
        //AddLog_P2(LOG_LEVEL_DEBUG_MORE, "XY RGB (%d %d %d) HS (%d %d)", rr,gg,bb,hue,sat);
        if (resp) { response += ","; }
        response += FPSTR(HUE_LIGHT_RESPONSE_JSON);
        response.replace("{id", String(device));
        response.replace("{cm", "xy");
        response.replace("{re", "[" + x_str + "," + y_str + "]");
        g_gotct = false;
        resp = true;
        change = true;
      }
      if (hue_json.containsKey("hue")) {             // The hue value is a wrapping value between 0 and 65535. Both 0 and 65535 are red, 25500 is green and 46920 is blue.
        tmp = hue_json["hue"];
        prev_hue = tmp;
        // change range from 0..65535 to 0..359
        hue = changeUIntScale(tmp, 0, 65535, 0, 359);
        if (resp) { response += ","; }
        response += FPSTR(HUE_LIGHT_RESPONSE_JSON);
        response.replace("{id", String(device));
        response.replace("{cm", "hue");
        response.replace("{re", String(tmp));
        if (LST_RGB <= light_subtype) {
          g_gotct = false;
          change = true;
        }
        resp = true;
      }
      if (hue_json.containsKey("sat")) {             // Saturation of the light. 254 is the most saturated (colored) and 0 is the least saturated (white).
        tmp = hue_json["sat"];
        prev_sat = sat = tmp;   // store command value
        // extend sat value if set to max
        if (254 <= sat) { sat = 255; }
        if (resp) { response += ","; }
        response += FPSTR(HUE_LIGHT_RESPONSE_JSON);
        response.replace("{id", String(device));
        response.replace("{cm", "sat");
        response.replace("{re", String(tmp));
        if (LST_RGB <= light_subtype) {
          g_gotct = false;
          change = true;
        }
        resp = true;
      }
      if (hue_json.containsKey("ct")) {  // Color temperature 153 (Cold) to 500 (Warm)
        ct = hue_json["ct"];
        prev_ct = ct;   // store commande value
        if (resp) { response += ","; }
        response += FPSTR(HUE_LIGHT_RESPONSE_JSON);
        response.replace("{id", String(device));
        response.replace("{cm", "ct");
        response.replace("{re", String(ct));
        if ((LST_COLDWARM == light_subtype) || (LST_RGBW <= light_subtype)) {
          g_gotct = true;
          change = true;
        }
        resp = true;
      }
      if (change) {
        if (light_type) {
          if (g_gotct) {
            light_controller.changeCTB(ct, bri);
          } else {
            light_controller.changeHSB(hue, sat, bri);
          }
          LightPreparePower();
          if (LST_COLDWARM <= light_subtype) {
            MqttPublishPrefixTopic_P(RESULT_OR_STAT, PSTR(D_CMND_COLOR));
          } else {
            MqttPublishPrefixTopic_P(RESULT_OR_STAT, PSTR(D_CMND_DIMMER));
          }
        }
        change = false;
      }
      response += "]";
      if (2 == response.length()) {
        response = FPSTR(HUE_ERROR_JSON);
      }
    }
    else {
      response = FPSTR(HUE_ERROR_JSON);
    }
  }
  else if(path->indexOf("/lights/") >= 0) {          // Got /lights/ID
    path->remove(0,8);                               // Remove /lights/
    device = atoi(path->c_str());
    if ((device < 1) || (device > maxhue)) {
      device = 1;
    }
    response += F("{\"state\":");
    HueLightStatus1(device, &response);
    HueLightStatus2(device, &response);
  }
  else {
    response = "{}";
    code = 406;
  }
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_HTTP D_HUE " Result (%s)"), response.c_str());
  WSSend(code, CT_JSON, response);
}

void HueGroups(String *path)
{
/*
 * http://sonoff/api/username/groups?1={"name":"Woonkamer","lights":[],"type":"Room","class":"Living room"})
 */
  String response = "{}";
  uint8_t maxhue = (devices_present > MAX_FRIENDLYNAMES) ? MAX_FRIENDLYNAMES : devices_present;

  if (path->endsWith("/0")) {
    response = FPSTR(HUE_GROUP0_STATUS_JSON);
    String lights = F("\"1\"");
    for (uint8_t i = 2; i <= maxhue; i++) {
      lights += ",\"";
      lights += String(i);
      lights += "\"";
    }
    response.replace("{l1", lights);
    HueLightStatus1(1, &response);
    response += F("}");
  }

  WSSend(200, CT_JSON, response);
}

void HandleHueApi(String *path)
{
  /* HUE API uses /api/<userid>/<command> syntax. The userid is created by the echo device and
   * on original HUE the pressed button allows for creation of this user. We simply ignore the
   * user part and allow every caller as with Web or WeMo.
   *
   * (c) Heiko Krupp, 2017
   *
   * Hue URL
   * http://sonoff/api/username/lights/1/state with post data {"on":true,"hue":56100,"sat":254,"bri":254,"alert":"none","transitiontime":40}
   * is converted by webserver to
   * http://sonoff/api/username/lights/1/state with arg plain={"on":true,"hue":56100,"sat":254,"bri":254,"alert":"none","transitiontime":40}
   */

  uint8_t args = 0;

  path->remove(0, 4);                                // remove /api
  uint16_t apilen = path->length();
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_HTTP D_HUE_API " (%s)"), path->c_str());         // HTP: Hue API (//lights/1/state
  for (args = 0; args < WebServer->args(); args++) {
    String json = WebServer->arg(args);
    AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR(D_LOG_HTTP D_HUE_POST_ARGS " (%s)"), json.c_str());  // HTP: Hue POST args ({"on":false})
  }

  if (path->endsWith("/invalid/")) {}                // Just ignore
  else if (!apilen) HueAuthentication(path);                  // New HUE App setup
  else if (path->endsWith("/")) HueAuthentication(path);      // New HUE App setup
  else if (path->endsWith("/config")) HueConfig(path);
  else if (path->indexOf("/lights") >= 0) HueLights(path);
  else if (path->indexOf("/groups") >= 0) HueGroups(path);
  else if (path->endsWith("/schedules")) HueNotImplemented(path);
  else if (path->endsWith("/sensors")) HueNotImplemented(path);
  else if (path->endsWith("/scenes")) HueNotImplemented(path);
  else if (path->endsWith("/rules")) HueNotImplemented(path);
  else HueGlobalConfig(path);
}

void HueWemoAddHandlers(void)
{
  if (devices_present) {
    if (EMUL_WEMO == Settings.flag2.emulation) {
      WebServer->on("/upnp/control/basicevent1", HTTP_POST, HandleUpnpEvent);
      WebServer->on("/eventservice.xml", HandleUpnpService);
      WebServer->on("/metainfoservice.xml", HandleUpnpMetaService);
      WebServer->on("/setup.xml", HandleUpnpSetupWemo);
    }
    if (EMUL_HUE == Settings.flag2.emulation) {
      WebServer->on("/description.xml", HandleUpnpSetupHue);
    }
  }
}

#endif  // USE_WEBSERVER && USE_EMULATION
