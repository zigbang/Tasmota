/*
  xdrv_01_webserver.ino - webserver for Tasmota

  Copyright (C) 2021  Theo Arends

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

#ifdef USE_WEBSERVER
/*********************************************************************************************\
 * Web server and WiFi Manager
 *
 * Enables configuration and reconfiguration of WiFi credentials using a Captive Portal
 * Based on source by AlexT (https://github.com/tzapu)
\*********************************************************************************************/

#define XDRV_01                                   1

// Enable below demo feature only if defines USE_UNISHOX_COMPRESSION and USE_SCRIPT_WEB_DISPLAY are disabled
//#define USE_WEB_SSE

#ifndef WIFI_SOFT_AP_CHANNEL
#define WIFI_SOFT_AP_CHANNEL                      1      // Soft Access Point Channel number between 1 and 11 as used by WifiManager web GUI
#endif

#ifndef MAX_WIFI_NETWORKS_TO_SHOW
#define MAX_WIFI_NETWORKS_TO_SHOW                 3      // Maximum number of Wifi Networks to show in the Wifi Configuration Menu BEFORE clicking on Show More Networks.
#endif

#ifndef RESTART_AFTER_INITIAL_WIFI_CONFIG
#define RESTART_AFTER_INITIAL_WIFI_CONFIG         true   // Restart Tasmota after initial Wifi Config of a blank device
#endif                                                   //   If disabled, Tasmota will keep both the wifi AP and the wifi connection to the router
                                                         //   but only until next restart.
#ifndef AFTER_INITIAL_WIFI_CONFIG_GO_TO_NEW_IP           // If RESTART_AFTER_INITIAL_WIFI_CONFIG and AFTER_INITIAL_WIFI_CONFIG_GO_TO_NEW_IP are true,
#define AFTER_INITIAL_WIFI_CONFIG_GO_TO_NEW_IP    true   //   the user will be redirected to the new IP of Tasmota (in the new Network).
#endif                                                   //   If the first is true, but this is false, the device will restart but the user will see
                                                         //   a window telling that the WiFi Configuration was Ok and that the window can be closed.

const uint16_t CHUNKED_BUFFER_SIZE = 500;                // Chunk buffer size

const uint16_t HTTP_REFRESH_TIME = 5000;                 // milliseconds
const uint16_t HTTP_RESTART_RECONNECT_TIME = 15000;      // milliseconds - Allow time for restart and wifi reconnect
#ifdef ESP8266
const uint16_t HTTP_OTA_RESTART_RECONNECT_TIME = 24000;  // milliseconds - Allow time for uploading binary, unzip/write to final destination and wifi reconnect
#endif  // ESP8266
#ifdef ESP32
const uint16_t HTTP_OTA_RESTART_RECONNECT_TIME = 10000;  // milliseconds - Allow time for restart and wifi reconnect
#endif  // ESP32

#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266WebServerSecure.h>

const char HTTP_SCRIPT_ROOT2[] PROGMEM =
  "var rfsh=1,ft;"
  "function la(p){"
    "a=p||'';"
    "clearTimeout(ft);clearTimeout(lt);"
    "if(x!=null){x.abort();}"             // Abort if no response within 2 seconds (happens on restart 1)
    "x=new XMLHttpRequest();"
    "x.onreadystatechange=function(){"
      "if(x.readyState==4&&x.status==200){"
        "var s=x.responseText.replace(/{t}/g,\"<table style='width:100%%'>\")"
                            ".replace(/{s}/g,\"<tr><th>\")"
//                            ".replace(/{m}/g,\"</th><td>\")"
                            ".replace(/{m}/g,\"</th><td style='width:20px;white-space:nowrap'>\")"  // I want a right justified column with left justified text
                            ".replace(/{e}/g,\"</td></tr>\");"
        "eb('l1').innerHTML=s;"
        "if(s.indexOf(\"{loader}\") != -1){"
        "eb('loader').style.display=\"\";"
        "} else {eb('loader').style.display=\"none\";}"
        "clearTimeout(ft);clearTimeout(lt);"
        "if(rfsh){"
          "lt=setTimeout(la,%d);"               // Settings.web_refresh
        "}"
      "}"
    "};"
    "if(rfsh){"
      "x.open('GET','.?m=1'+a,true);"       // ?m related to Webserver->hasArg("m")
      "x.send();"
      "ft=setTimeout(la,20000);"               // 20s failure timeout
    "}"
  "}"
  "function seva(par,ivar){"
    "la('&sv='+ivar+'_'+par);"
  "}"
  "function siva(par,ivar){"
    "rfsh=1;"
    "la('&sv='+ivar+'_'+par);"
    "rfsh=0;"
  "}"
  "function pr(f){"
    "if(f){"
      "clearTimeout(lt);clearTimeout(ft);"
      "lt=setTimeout(la,%d);"
      "rfsh=1;"
    "}else{"
      "clearTimeout(lt);clearTimeout(ft);"
      "rfsh=0;"
    "}"
  "}"
  ;


#ifdef USE_UNISHOX_COMPRESSION
  #ifdef USE_JAVASCRIPT_ES6
    #include "./html_compressed/HTTP_HEADER1_ES6.h"
  #else
    #include "./html_compressed/HTTP_HEADER1_NOES6.h"
  #endif
#else
  #ifdef USE_JAVASCRIPT_ES6
    #include "./html_uncompressed/HTTP_HEADER1_ES6.h"
  #else
    #include "./html_uncompressed/HTTP_HEADER1_NOES6.h"
  #endif
#endif

const char HTTP_SCRIPT_COUNTER[] PROGMEM =
  "var cn=180;"                           // seconds
  "function u(){"
    "if(cn>=0){"
      "eb('t').innerHTML='" D_RESTART_IN " '+cn+' " D_SECONDS "';"
      "cn--;"
      "setTimeout(u,1000);"
    "}"
  "}"
  "wl(u);";

#ifdef USE_UNISHOX_COMPRESSION
  #ifdef USE_SCRIPT_WEB_DISPLAY
    #include "./html_compressed/HTTP_SCRIPT_ROOT_WEB_DISPLAY.h"
  #else
    #include "./html_compressed/HTTP_SCRIPT_ROOT_NO_WEB_DISPLAY.h"
  #endif
  #include "./html_compressed/HTTP_SCRIPT_ROOT_PART2.h"
#else
  #ifdef USE_SCRIPT_WEB_DISPLAY
    #include "./html_uncompressed/HTTP_SCRIPT_ROOT_WEB_DISPLAY.h"
  #else
    #ifdef USE_WEB_SSE
      #include "./html_uncompressed/HTTP_SCRIPT_ROOT_SSE_NO_WEB_DISPLAY.h"
    #else
      #include "./html_uncompressed/HTTP_SCRIPT_ROOT_NO_WEB_DISPLAY.h"
    #endif  // USE_WEB_SSE
  #endif
  #include "./html_uncompressed/HTTP_SCRIPT_ROOT_PART2.h"
#endif

const char HTTP_SCRIPT_WIFI[] PROGMEM =
  "function c(l){"
    "eb('s1').value=l.innerText||l.textContent;"
    "eb('p1').focus();"
  "}";

const char HTTP_SCRIPT_HIDE[] PROGMEM =
  "function hidBtns() {"
    "eb('butmo').style.display='none';"
    "eb('butmod').style.display='none';"
    "eb('but0').style.display='block';"
    "eb('but1').style.display='block';"
    "eb('but13').style.display='block';"
    "eb('but0d').style.display='block';"
    "eb('but13d').style.display='block';"
  "}";

const char HTTP_SCRIPT_RELOAD_TIME[] PROGMEM =
  "setTimeout(function(){location.href='.';},%d);";

#ifdef USE_UNISHOX_COMPRESSION
  #include "./html_compressed/HTTP_SCRIPT_CONSOL.h"
#else
  #include "./html_uncompressed/HTTP_SCRIPT_CONSOL.h"
#endif

const char HTTP_SCRIPT_INFO_BEGIN[] PROGMEM =
  "function i(){"
    "var s,o=\"";
const char HTTP_SCRIPT_INFO_END[] PROGMEM =
    "\";"                                 // "}1" and "}2" means do not use "}x" in Information text
    "s=o.replace(/}1/g,\"</td></tr><tr><th>\").replace(/}2/g,\"</th><td>\");"
    "eb('i').innerHTML=s;"
  "}"
  "wl(i);";

#ifdef USE_UNISHOX_COMPRESSION
  #include "./html_compressed/HTTP_HEAD_LAST_SCRIPT.h"
  #include "./html_compressed/HTTP_HEAD_STYLE1.h"
  #include "./html_compressed/HTTP_HEAD_STYLE2.h"
#else
  #include "./html_uncompressed/HTTP_HEAD_LAST_SCRIPT.h"
  #include "./html_uncompressed/HTTP_HEAD_STYLE1.h"
  #include "./html_uncompressed/HTTP_HEAD_STYLE2.h"
#endif

#ifdef USE_ZIGBEE
// Styles used for Zigbee Web UI
// Battery icon from https://css.gg/battery
//
  #ifdef USE_UNISHOX_COMPRESSION
    #include "./html_compressed/HTTP_HEAD_STYLE_ZIGBEE.h"
  #else
    #include "./html_uncompressed/HTTP_HEAD_STYLE_ZIGBEE.h"
  #endif
#endif // USE_ZIGBEE

const char HTTP_HEAD_STYLE_SSI[] PROGMEM =
  // Signal Strength Indicator
  ".si{display:inline-flex;align-items:flex-end;height:15px;padding:0}"
  ".si i{width:3px;margin-right:1px;border-radius:3px;background-color:#%06x}"
  ".si .b0{height:25%%}.si .b1{height:50%%}.si .b2{height:75%%}.si .b3{height:100%%}.o30{opacity:.3}";

const char HTTP_HEAD_STYLE_LOADER[] PROGMEM =
"#loader, #loader:before, #loader:after {border-radius: 50%%;width: 1.5em;height: 1.5em;-webkit-animation-fill-mode: both;"
 "animation-fill-mode: both;-webkit-animation: load7 1.8s infinite ease-in-out;animation: load7 1.5s infinite ease-in-out;}"
"#loader {color: #ffa400;font-size: 10px;margin: auto;margin-bottom:2rem;position: relative;text-indent: -9999em;"
"-webkit-transform: translateZ(0);-ms-transform: translateZ(0);transform: translateZ(0);-webkit-animation-delay: -0.16s;animation-delay: -0.16s;}"
"#loader:before,#loader:after {content: '';position: absolute;top: 0;}"
"#loader:before {left: -3.5em;-webkit-animation-delay: -0.32s;animation-delay: -0.32s;}"
"#loader:after {left: 3.5em;}"
"@-webkit-keyframes load7 {0%%,80%%,100%% {box-shadow: 0 2.5em 0 -1.3em;}40%% {box-shadow: 0 2.5em 0 0;}}"
"@keyframes load7 {0%%,80%%,100%% {box-shadow: 0 2.5em 0 -1.3em;}40%% {box-shadow: 0 2.5em 0 0;}}";

const char HTTP_HEAD_STYLE3[] PROGMEM =
  "</style>"

  "</head>"
  "<body>"
  "<div style='text-align:left;display:inline-block;color:#%06x;min-width:340px;'>"  // COLOR_TEXT
#ifdef FIRMWARE_MINIMAL
  "<div style='text-align:center;color:#%06x;'><h3>" D_MINIMAL_FIRMWARE_PLEASE_UPGRADE "</h3></div>"  // COLOR_TEXT_WARNING
#endif
  "<div style='text-align:center;color:#%06x;'><noscript>" D_NOSCRIPT "<br></noscript>" // COLOR_TITLE
/*
#ifdef LANGUAGE_MODULE_NAME
  "<h3>" D_MODULE " %s</h3>"
#else
  "<h3>%s " D_MODULE "</h3>"
#endif
*/
  "<h3>%s</h3>"    // Module name
  "<h2>%s</h2>";   // Device name

const char HTTP_MSG_SLIDER_GRADIENT[] PROGMEM =
  "<div id='%s' class='r' style='background-image:linear-gradient(to right,%s,%s);'>"
  "<input id='sl%d' type='range' min='%d' max='%d' value='%d' onchange='lc(\"%c\",%d,value)'>"
  "</div>";
const char HTTP_MSG_SLIDER_SHUTTER[] PROGMEM =
  "<div><span class='p'>" D_CLOSE "</span><span class='q'>" D_OPEN "</span></div>"
  "<div><input type='range' min='0' max='100' value='%d' onchange='lc(\"u\",%d,value)'></div>";

const char HTTP_MSG_RSTRT[] PROGMEM =
  "<br><div style='text-align:center;'>" D_DEVICE_WILL_RESTART "</div><br>";

const char HTTP_FORM_LOGIN[] PROGMEM =
  "<fieldset>"
  "<form method='post' action='/'>"
  "<p><b>" D_USER "</b><br><input name='USER1' placeholder='" D_USER "'></p>"
  "<p><b>" D_PASSWORD "</b><br><input name='PASS1' type='password' placeholder='" D_PASSWORD "'></p>"
  "<br>"
  "<button>" D_OK "</button>"
  "</form></fieldset>";

const char HTTP_FORM_WIFI_PART1[] PROGMEM =
  "<fieldset><legend><b>&nbsp;" D_WIFI_PARAMETERS "&nbsp;</b></legend>"
  "<form method='get' action='wi'>"
  "<p><b>" D_AP1_SSID "</b>%s<br><input id='s1' placeholder=\"" D_AP1_SSID_HELP "\" value=\"%s\"></p>"  // Need \" instead of ' to be able to use ' in text (#8489)
  "<p><label><b>" D_AP_PASSWORD "</b><input type='checkbox' onclick='sp(\"p1\")'></label><br><input id='p1' type='password' placeholder=\"" D_AP_PASSWORD_HELP "\"";

const char HTTP_FORM_WIFI_PART2[] PROGMEM =
  " value=\"" D_ASTERISK_PWD "\"></p>"
  "<p><b>" D_AP2_SSID "</b> (" STA_SSID2 ")<br><input id='s2' placeholder=\"" D_AP2_SSID_HELP "\" value=\"%s\"></p>"
  "<p><label><b>" D_AP_PASSWORD "</b><input type='checkbox' onclick='sp(\"p2\")'></label><br><input id='p2' type='password' placeholder=\"" D_AP_PASSWORD_HELP "\" value=\"" D_ASTERISK_PWD "\"></p>"
  "<p><b>" D_HOSTNAME "</b> (%s)<br><input id='h' placeholder=\"%s\" value=\"%s\"></p>"
  "<p><b>" D_CORS_DOMAIN "</b><input id='c' placeholder=\"" CORS_DOMAIN "\" value=\"%s\"></p>";

const char HTTP_FORM_END[] PROGMEM =
  "<br>"
  "<button name='save' type='submit' class='button bgrn'>" D_SAVE "</button>"
  "</form></fieldset>";

const char HTTP_FORM_RST[] PROGMEM =
  "<div id='f1' style='display:block;'>"
  "<fieldset><legend><b>&nbsp;" D_RESTORE_CONFIGURATION "&nbsp;</b></legend>";
const char HTTP_FORM_UPG[] PROGMEM =
  "<div id='f1' style='display:block;'>"
  "<fieldset><legend><b>&nbsp;" D_UPGRADE_BY_WEBSERVER "&nbsp;</b></legend>"
  "<form method='get' action='u1'>"
  "<br><b>" D_OTA_URL "</b><br><input id='o' placeholder=\"OTA_URL\" value=\"%s\"><br>"
  "<br><button type='submit'>" D_START_UPGRADE "</button></form>"
  "</fieldset><br><br>"
  "<fieldset><legend><b>&nbsp;" D_UPGRADE_BY_FILE_UPLOAD "&nbsp;</b></legend>";
const char HTTP_FORM_RST_UPG[] PROGMEM =
  "<form method='post' action='u2' enctype='multipart/form-data'>"
  "<br><input type='file' name='u2'><br>"
  "<br><button type='submit' onclick='eb(\"f1\").style.display=\"none\";eb(\"f2\").style.display=\"block\";this.form.submit();'>" D_START " %s</button></form>"
  "</fieldset>"
  "</div>"
  "<div id='f2' style='display:none;text-align:center;'><b>" D_UPLOAD_STARTED " ...</b></div>";

const char HTTP_FORM_CMND[] PROGMEM =
  "<br><textarea readonly id='t1' cols='340' wrap='off'></textarea><br><br>"
  "<form method='get' onsubmit='return l(1);'>"
  "<input id='c1' placeholder='" D_ENTER_COMMAND "' autofocus><br>"
  //  "<br><button type='submit'>Send command</button>"
  "</form>";

const char HTTP_TABLE100[] PROGMEM =
  "<table style='width:100%%'>";

const char HTTP_COUNTER[] PROGMEM =
  "<br><div id='t' style='text-align:center;'></div>";
// TODO: 최종 리포지토리로 링크 변경
const char HTTP_END[] PROGMEM =
  "<table style='100%%; font-size:11px; display:block'><tbody style='display:block'><tr style='display:block'><th class=\"p\" style='color:#aaa'>%s</th><td class=\"q\" style='text-align:right'><a href='https://github.com/seojinwoo/ZiotTasmota' target='_blank' style='color:#aaa;'>ZIoT %s " D_BY " (주)직방</a></td></tr></tbody></table>"
  "</body>"
  "</html>";

const char HTTP_DEVICE_CONTROL[] PROGMEM = "<td style='width:%d%%'><button onclick='la(\"&o=%d\");'>%s%s</button></td>";  // ?o is related to WebGetArg("o", tmp, sizeof(tmp));
const char HTTP_DEVICE_STATE[] PROGMEM = "<td style='width:%d%%;text-align:center;font-weight:%s;font-size:%dpx'>%s</td>";

enum ButtonTitle {
  BUTTON_RESTART, BUTTON_RESET_CONFIGURATION, BUTTON_FACTORY_RESET,
  BUTTON_MAIN, BUTTON_COGNITO_LOGIN, BUTTON_CONFIGURATION, BUTTON_INFORMATION, BUTTON_FIRMWARE_UPGRADE, BUTTON_WIFI, BUTTON_BACKUP, BUTTON_RESTORE };
const char kButtonTitle[] PROGMEM =
  D_RESTART "|" D_RESET_CONFIGURATION "|" "공장 초기화" "|"
  D_MAIN_MENU "|" "로그인" "|" D_CONFIGURATION "|" D_INFORMATION "|" D_FIRMWARE_UPGRADE "|"
  D_CONFIGURE_WIFI "|" D_BACKUP_CONFIGURATION "|" D_RESTORE_CONFIGURATION "|";
const char kButtonAction[] PROGMEM =
  ".|rt|frt|"
  ".|lo|cn|in|up|"
  "wi|dl|rs|";

const char kButtonConfirm[] PROGMEM = D_CONFIRM_RESTART "|" D_CONFIRM_RESET_CONFIGURATION "|" "공장 초기화 확인";

enum CTypes { CT_HTML, CT_PLAIN, CT_XML, CT_STREAM, CT_APP_JSON, CT_APP_STREAM };
const char kContentTypes[] PROGMEM = "text/html|text/plain|text/xml|text/event-stream|application/json|application/octet-stream";

const char kEmulationOptions[] PROGMEM = D_NONE "|" D_BELKIN_WEMO "|" D_HUE_BRIDGE;

const char kUploadErrors[] PROGMEM =
  D_UPLOAD_ERR_1 "|" D_UPLOAD_ERR_2 "|" D_UPLOAD_ERR_3 "|" D_UPLOAD_ERR_4 "|" D_UPLOAD_ERR_5 "|" D_UPLOAD_ERR_6 "|" D_UPLOAD_ERR_7 "|" D_UPLOAD_ERR_8 "|" D_UPLOAD_ERR_9;

const uint16_t DNS_PORT = 53;
enum HttpOptions {HTTP_OFF, HTTP_USER, HTTP_ADMIN, HTTP_MANAGER, HTTP_MANAGER_RESET_ONLY};
enum WifiTestOptions {WIFI_NOT_TESTING, WIFI_TESTING, WIFI_TEST_FINISHED_SUCCESSFUL, WIFI_TEST_FINISHED_BAD};

DNSServer *DnsServer;
ESP8266WebServer *Webserver;
BearSSL::ESP8266WebServerSecure *WebserverSecure;

struct WEB {
  String chunk_buffer = "";                         // Could be max 2 * CHUNKED_BUFFER_SIZE
  uint16_t upload_error = 0;
  uint8_t state = HTTP_OFF;
  uint8_t upload_file_type;
  uint8_t config_block_count = 0;
  bool upload_services_stopped = false;
  bool initial_config = false;
  bool state_HTTPS = false;
  uint8_t wifiTest = WIFI_NOT_TESTING;
  uint8_t wifi_test_counter = 0;
  uint16_t save_data_counter = 0;
  uint8_t old_wificonfig = MAX_WIFI_OPTION; // means "nothing yet saved here"
} Web;

// Helper function to avoid code duplication (saves 4k Flash)
// arg can be in PROGMEM
static void WebGetArg(const char* arg, char* out, size_t max)
{
  String s = Webserver->arg((const __FlashStringHelper *)arg);
  strlcpy(out, s.c_str(), max);
//  out[max-1] = '\0';  // Ensure terminating NUL
}

String AddWebCommand(const char* command, const char* arg, const char* dflt) {
/*
  // OK but fixed max argument
  char param[200];                             // Allow parameter with lenght up to 199 characters
  WebGetArg(arg, param, sizeof(param));
  uint32_t len = strlen(param);
  char cmnd[232];
  snprintf_P(cmnd, sizeof(cmnd), PSTR(";%s %s"), command, (0 == len) ? dflt : (StrCaseStr_P(command, PSTR("Password")) && (len < 5)) ? "" : param);
  return String(cmnd);
*/
/*
  // Any argument size (within stack space) +48 bytes
  String param = Webserver->arg((const __FlashStringHelper *)arg);
  uint32_t len = param.length();
//  char cmnd[len + strlen_P(command) + strlen_P(dflt) + 4];
  char cmnd[64 + len];
  snprintf_P(cmnd, sizeof(cmnd), PSTR(";%s %s"), command, (0 == len) ? dflt : (StrCaseStr_P(command, PSTR("Password")) && (len < 5)) ? "" : param.c_str());
  return String(cmnd);
*/
  // Any argument size (within heap space) +24 bytes
  // Exception (3) if not first moved from flash to stack
  // Exception (3) if not using __FlashStringHelper
  // Exception (3) if not FPSTR()
//  char rcommand[strlen_P(command) +1];
//  snprintf_P(rcommand, sizeof(rcommand), command);
//  char rdflt[strlen_P(dflt) +1];
//  snprintf_P(rdflt, sizeof(rdflt), dflt);
  String result = F(";");
//  result += rcommand;
//  result += (const __FlashStringHelper *)command;
  result += FPSTR(command);
  result += F(" ");
  String param = Webserver->arg(FPSTR(arg));
  uint32_t len = param.length();
  if (0 == len) {
//    result += rdflt;
//    result += (const __FlashStringHelper *)dflt;
    result += FPSTR(dflt);
  }
  else if (!(StrCaseStr_P(command, PSTR("Password")) && (len < 5))) {
    result += param;
  }
  return result;
}

static bool WifiIsInManagerMode(){
  return (HTTP_MANAGER == Web.state || HTTP_MANAGER_RESET_ONLY == Web.state);
}

void ShowWebSource(uint32_t source)
{
  if ((source > 0) && (source < SRC_MAX)) {
    char stemp1[20];
    AddLog(LOG_LEVEL_DEBUG, PSTR("SRC: %s from %_I"), GetTextIndexed(stemp1, sizeof(stemp1), source, kCommandSource), (uint32_t)Webserver->client().remoteIP());
  }
}

void ExecuteWebCommand(char* svalue, uint32_t source) {
  ShowWebSource(source);
  TasmotaGlobal.last_source = source;
  ExecuteCommand(svalue, SRC_IGNORE);
}

void ExecuteWebCommand(char* svalue) {
  ExecuteWebCommand(svalue, SRC_WEBGUI);
}

void StartWebserverSecure(void)
{
  if (!Web.state_HTTPS) {
    if (!WebserverSecure) {
      WebserverSecure = new ESP8266WebServerSecure(443);
      WebserverSecure->getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
      WebserverSecure->getServer().setBufferSizes(1024, 1024);
      WebserverSecure->on(F("/lc"), HTTP_GET, HandleCognitoLoginCode);
    }

    WebserverSecure->begin(); // Web server start
  }
  if (!Web.state_HTTPS) {
    AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_HTTP D_WEBSERVER_ACTIVE_ON " %s%s " D_WITH_IP_ADDRESS " %_I"),
      NetworkHostname(), (Mdns.begun) ? PSTR(".local") : "", (uint32_t)WiFi.localIP());
    Web.state_HTTPS = true;
  }
}

// replace the series of `Webserver->on()` with a table in PROGMEM
typedef struct WebServerDispatch_t {
  char uri[3];   // the prefix "/" is added automatically
  uint8_t method;
  void (*handler)(void);
} WebServerDispatch_t;

const WebServerDispatch_t WebServerDispatch[] PROGMEM = {
  { "",   HTTP_ANY, HandleRoot },
  { "u2", HTTP_OPTIONS, HandlePreflightRequest },
  { "cs", HTTP_OPTIONS, HandlePreflightRequest },
  { "cm", HTTP_ANY, HandleHttpCommand },
#ifndef FIRMWARE_MINIMAL
  { "cn", HTTP_ANY, HandleConfiguration },
  { "wi", HTTP_ANY, HandleWifiConfiguration },
  { "rt", HTTP_ANY, HandleResetConfiguration },
  { "in", HTTP_ANY, HandleInformation },
#endif  // Not FIRMWARE_MINIMAL
};

void WebServer_on(const char * prefix, void (*func)(void), uint8_t method = HTTP_ANY) {
  if (Webserver == nullptr) { return; }
#ifdef ESP8266
  Webserver->on((const __FlashStringHelper *) prefix, (HTTPMethod) method, func);
#endif  // ESP8266
#ifdef ESP32
  Webserver->on(prefix, (HTTPMethod) method, func);
#endif  // ESP32
}

void StartWebserver(int type, IPAddress ipweb)
{
  Settings->web_refresh = HTTP_REFRESH_TIME;
  if (!Web.state) {
    if (!Webserver) {
      Webserver = new ESP8266WebServer((HTTP_MANAGER == type || HTTP_MANAGER_RESET_ONLY == type) ? 80 : WEB_PORT);
      // call `Webserver->on()` on each entry
      for (uint32_t i=0; i<nitems(WebServerDispatch); i++) {
        const WebServerDispatch_t & line = WebServerDispatch[i];
        // copy uri in RAM and prefix with '/'
        char uri[4];
        uri[0] = '/';
        uri[1] = pgm_read_byte(&line.uri[0]);
        uri[2] = pgm_read_byte(&line.uri[1]);
        uri[3] = '\0';
        // register
        WebServer_on(uri, line.handler, pgm_read_byte(&line.method));
      }
      Webserver->on(F("/info"), HTTP_GET, HandleDeviceInfo);
      Webserver->on(F("/certs"), HTTP_GET, HandleCertsInfo);
      Webserver->on(F("/certs"), HTTP_POST, HandleCertsConfiguration);
      Webserver->on(F("/frt"), HTTP_GET, HandleFactoryResetConfiguration);
      Webserver->on(F("/lo"), HTTP_GET, HandleCognitoLogin);
      Webserver->onNotFound(HandleNotFound);
//      Webserver->on(F("/u2"), HTTP_POST, HandleUploadDone, HandleUploadLoop);  // this call requires 2 functions so we keep a direct call
#ifndef FIRMWARE_MINIMAL
      XdrvCall(FUNC_WEB_ADD_HANDLER);
      XsnsCall(FUNC_WEB_ADD_HANDLER);
#endif  // Not FIRMWARE_MINIMAL
    }

    Webserver->begin(); // Web server start
  }
  if (Web.state != type) {
#if LWIP_IPV6
    String ipv6_addr = WifiGetIPv6();
    if (ipv6_addr!="") {
      AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_HTTP D_WEBSERVER_ACTIVE_ON " %s%s " D_WITH_IP_ADDRESS " %_I and IPv6 global address %s "),
        NetworkHostname(), (Mdns.begun) ? PSTR(".local") : "", (uint32_t)ipweb, ipv6_addr.c_str());
    } else {
      AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_HTTP D_WEBSERVER_ACTIVE_ON " %s%s " D_WITH_IP_ADDRESS " %_I"),
        NetworkHostname(), (Mdns.begun) ? PSTR(".local") : "", (uint32_t)ipweb);
    }
#else
    AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_HTTP D_WEBSERVER_ACTIVE_ON " %s%s " D_WITH_IP_ADDRESS " %_I"),
      NetworkHostname(), (Mdns.begun) ? PSTR(".local") : "", (uint32_t)ipweb);
#endif // LWIP_IPV6 = 1
    TasmotaGlobal.rules_flag.http_init = 1;
    Web.state = type;
  }

  StartWebserverSecure();
}

void StopWebserver(void)
{
  if (Web.state) {
    Webserver->close();
    Web.state = HTTP_OFF;
    AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_HTTP D_WEBSERVER_STOPPED));
  }
}

void WifiManagerBegin(bool reset_only)
{
  // setup AP
  if (!Web.initial_config) { AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_WIFI D_WCFG_2_WIFIMANAGER " " D_ACTIVE_FOR_3_MINUTES)); }
  if (!TasmotaGlobal.global_state.wifi_down) {
//    WiFi.mode(WIFI_AP_STA);
    WifiSetMode(WIFI_AP_STA);
    AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_WIFI D_WIFIMANAGER_SET_ACCESSPOINT_AND_STATION));
  } else {
//    WiFi.mode(WIFI_AP);
    WifiSetMode(WIFI_AP);
    AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_WIFI D_WIFIMANAGER_SET_ACCESSPOINT));
  }

  //StopWebserver();

  DnsServer = new DNSServer();

  int channel = WIFI_SOFT_AP_CHANNEL;
  if ((channel < 1) || (channel > 13)) { channel = 1; }

  // bool softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4);
  WiFi.softAP(TasmotaGlobal.hostname, WIFI_AP_PASSPHRASE, channel, 0, 1);
  delay(500); // Without delay I've seen the IP address blank
  /* Setup the DNS server redirecting all the domains to the apIP */
  DnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  DnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

  StartWebserver((reset_only ? HTTP_MANAGER_RESET_ONLY : HTTP_MANAGER), WiFi.softAPIP());
}

void PollDnsWebserver(void)
{
  if (DnsServer) { DnsServer->processNextRequest(); }
  if (Webserver) { Webserver->handleClient(); }
  if (WebserverSecure) { WebserverSecure->handleClient(); }
}

/*********************************************************************************************/

void HttpHeaderCors(void)
{
  if (strlen(SettingsText(SET_CORS))) {
    Webserver->sendHeader(F("Access-Control-Allow-Origin"), SettingsText(SET_CORS));
  }
}

void WSHeaderSend(void)
{
  char server[32];
  // TODO: ZIoT 버전 변경
  snprintf_P(server, sizeof(server), PSTR("Tasmota/%s (%s)"), TasmotaGlobal.version, GetDeviceHardware().c_str());
  Webserver->sendHeader(F("Server"), server);
  Webserver->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
  Webserver->sendHeader(F("Pragma"), F("no-cache"));
  Webserver->sendHeader(F("Expires"), F("-1"));
  HttpHeaderCors();
}

/**********************************************************************************************
* HTTP Content Page handler
**********************************************************************************************/

void WSSend(int code, int ctype, const String& content)
{
  char ct[25];  // strlen("application/octet-stream") +1 = Longest Content type string
  Webserver->send(code, GetTextIndexed(ct, sizeof(ct), ctype, kContentTypes), content);
}

/**********************************************************************************************
* HTTP Content Chunk handler
**********************************************************************************************/

void WSContentBegin(int code, int ctype) {
  Webserver->client().flush();
  WSHeaderSend();
  Webserver->setContentLength(CONTENT_LENGTH_UNKNOWN);
  WSSend(code, ctype, "");                         // Signal start of chunked content
  Web.chunk_buffer = "";
}

void _WSContentSend(const char* content, size_t size) {  // Lowest level sendContent for all core versions
  Webserver->sendContent(content, size);

#ifdef USE_DEBUG_DRIVER
  ShowFreeMem(PSTR("WSContentSend"));
#endif
  DEBUG_CORE_LOG(PSTR("WEB: Chunk size %d"), size);
}

void _WSContentSend(const String& content) {       // Low level sendContent for all core versions
  _WSContentSend(content.c_str(), content.length());
}

void WSContentFlush(void) {
  if (Web.chunk_buffer.length() > 0) {
    _WSContentSend(Web.chunk_buffer);              // Flush chunk buffer
    Web.chunk_buffer = "";
  }
}

void WSContentSend(const char* content, size_t size) {
  WSContentFlush();
  _WSContentSend(content, size);
}

void _WSContentSendBuffer(bool decimal, const char * formatP, va_list arg) {
  char* content = ext_vsnprintf_malloc_P(formatP, arg);
  if (content == nullptr) { return; }              // Avoid crash

  int len = strlen(content);
  if (0 == len) { return; }                        // No content

  if (decimal && (D_DECIMAL_SEPARATOR[0] != '.')) {
    for (uint32_t i = 0; i < len; i++) {
      if ('.' == content[i]) {
        content[i] = D_DECIMAL_SEPARATOR[0];
      }
    }
  }

  if (len < CHUNKED_BUFFER_SIZE) {                 // Append chunk buffer with small content
    Web.chunk_buffer += content;
    len = Web.chunk_buffer.length();
  }

  if (len >= CHUNKED_BUFFER_SIZE) {                // Either content or chunk buffer is oversize
    WSContentFlush();                              // Send chunk buffer before possible content oversize
  }
  if (strlen(content) >= CHUNKED_BUFFER_SIZE) {    // Content is oversize
    _WSContentSend(content);                       // Send content
  }

  free(content);
}

void WSContentSend_P(const char* formatP, ...) {   // Content send snprintf_P char data
  // This uses char strings. Be aware of sending %% if % is needed
  va_list arg;
  va_start(arg, formatP);
  _WSContentSendBuffer(false, formatP, arg);
  va_end(arg);
}

void WSContentSend_PD(const char* formatP, ...) {  // Content send snprintf_P char data checked for decimal separator
  // This uses char strings. Be aware of sending %% if % is needed
  va_list arg;
  va_start(arg, formatP);
  _WSContentSendBuffer(true, formatP, arg);
  va_end(arg);
}

void WSContentStart_P(const char* title, bool auth)
{
  WSContentBegin(200, CT_HTML);

  if (title != nullptr) {
    WSContentSend_P(HTTP_HEADER1, PSTR(D_HTML_LANGUAGE), SettingsText(SET_DEVICENAME), title);
  }
}

void WSContentStart_P(const char* title)
{
  WSContentStart_P(title, true);
}

void WSContentSendStyle_P(const char* formatP, ...)
{
  if ( WifiIsInManagerMode() && (!Web.initial_config) ) {
    if (WifiConfigCounter()) {
      WSContentSend_P(HTTP_SCRIPT_COUNTER);
    }
  }
  //WSContentSend_P(HTTP_SCRIPT_LOADER);
  WSContentSend_P(HTTP_HEAD_LAST_SCRIPT);

  WSContentSend_P(HTTP_HEAD_STYLE1, WebColor(COL_FORM), WebColor(COL_INPUT), WebColor(COL_INPUT_TEXT), WebColor(COL_INPUT),
                  WebColor(COL_INPUT_TEXT), PSTR(""), PSTR(""), WebColor(COL_BACKGROUND));
  WSContentSend_P(HTTP_HEAD_STYLE2, WebColor(COL_BUTTON), WebColor(COL_BUTTON_TEXT), WebColor(COL_BUTTON_HOVER),
                  WebColor(COL_BUTTON_RESET), WebColor(COL_BUTTON_RESET_HOVER), WebColor(COL_BUTTON_SAVE), WebColor(COL_BUTTON_SAVE_HOVER),
                  WebColor(COL_BUTTON));
#ifdef USE_ZIGBEE
  WSContentSend_P(HTTP_HEAD_STYLE_ZIGBEE);
#endif // USE_ZIGBEE
  if (formatP != nullptr) {
    // This uses char strings. Be aware of sending %% if % is needed
    va_list arg;
    va_start(arg, formatP);
    _WSContentSendBuffer(false, formatP, arg);
    va_end(arg);
  }
  WSContentSend_P(HTTP_HEAD_STYLE_LOADER);
  WSContentSend_P(HTTP_HEAD_STYLE3, WebColor(COL_TEXT),
#ifdef FIRMWARE_MINIMAL
  WebColor(COL_TEXT_WARNING),
#endif
  WebColor(COL_TITLE),
  "", SettingsText(SET_DEVICENAME));

  // SetOption53 - Show hostname and IP address in GUI main menu
#if (RESTART_AFTER_INITIAL_WIFI_CONFIG)
  if (Settings->flag3.gui_hostname_ip) {
#else
  if ( Settings->flag3.gui_hostname_ip || ( (WiFi.getMode() == WIFI_AP_STA) && (!Web.initial_config) )  ) {
#endif
    bool lip = (static_cast<uint32_t>(WiFi.localIP()) != 0);
    bool sip = (static_cast<uint32_t>(WiFi.softAPIP()) != 0);
    WSContentSend_P(PSTR("<h4>%s%s (%s%s%s)</h4>"),    // tasmota.local (192.168.2.12, 192.168.4.1)
      NetworkHostname(),
      (Mdns.begun) ? PSTR(".local") : "",
      (lip) ? WiFi.localIP().toString().c_str() : "",
      (lip && sip) ? ", " : "",
      (sip) ? WiFi.softAPIP().toString().c_str() : "");
  }
  WSContentSend_P(PSTR("</div>"));
}

void WSContentSendStyle(void)
{
  WSContentSendStyle_P(nullptr);
}

void WSContentTextCenterStart(uint32_t color) {
  WSContentSend_P(PSTR("<div style='text-align:center;color:#%06x;'>"), color);
}

void WSContentButton(uint32_t title_index, bool show=true)
{
  char action[4];
  char title[100];  // Large to accomodate UTF-16 as used by Russian

  WSContentSend_P(PSTR("<p><form id=but%d style=\"display: %s;\" action='%s' method='get'"),
    title_index,
    show ? "block":"none",
    GetTextIndexed(action, sizeof(action), title_index, kButtonAction));
  if (title_index <= BUTTON_FACTORY_RESET) {
    char confirm[100];
    WSContentSend_P(PSTR(" onsubmit='return confirm(\"%s\");'><button name='%s' class='button bred'>%s</button></form></p>"),
      GetTextIndexed(confirm, sizeof(confirm), title_index, kButtonConfirm),
      (!title_index) ? PSTR("rst") : PSTR("non"),
      GetTextIndexed(title, sizeof(title), title_index, kButtonTitle));
  } else {
    WSContentSend_P(PSTR("><button>%s</button></form></p>"),
      GetTextIndexed(title, sizeof(title), title_index, kButtonTitle));
  }
}

void WSContentSpaceButton(uint32_t title_index, bool show=true)
{
  WSContentSend_P(PSTR("<div id=but%dd style=\"display: %s;\"></div>"),title_index, show ? "block":"none");            // 5px padding
  WSContentButton(title_index, show);
}

void WSContentSend_Temp(const char *types, float f_temperature) {
  WSContentSend_PD(HTTP_SNS_F_TEMP, types, Settings->flag2.temperature_resolution, &f_temperature, TempUnit());
}

void WSContentSend_Voltage(const char *types, float f_voltage) {
  WSContentSend_PD(HTTP_SNS_F_VOLTAGE, types, Settings->flag2.voltage_resolution, &f_voltage);
}

void WSContentSend_CurrentMA(const char *types, float f_current) {
  WSContentSend_PD(HTTP_SNS_F_CURRENT_MA, types, Settings->flag2.current_resolution, &f_current);
}

void WSContentSend_THD(const char *types, float f_temperature, float f_humidity)
{
  WSContentSend_Temp(types, f_temperature);

  char parameter[FLOATSZ];
  dtostrfd(f_humidity, Settings->flag2.humidity_resolution, parameter);
  WSContentSend_PD(HTTP_SNS_HUM, types, parameter);
  dtostrfd(CalcTempHumToDew(f_temperature, f_humidity), Settings->flag2.temperature_resolution, parameter);
  WSContentSend_PD(HTTP_SNS_DEW, types, parameter, TempUnit());
}

void WSContentEnd(void)
{
  WSContentSend("", 0);                            // Signal end of chunked content
  Webserver->client().stop();
}

void WSContentStop(void)
{
  if ( WifiIsInManagerMode() && (!Web.initial_config) ) {
    if (WifiConfigCounter()) {
      WSContentSend_P(HTTP_COUNTER);
    }
  }
  // TODO: ZIoT 버전 변경
  WSContentSend_P(HTTP_END, WiFi.macAddress().c_str(), TasmotaGlobal.version);
  WSContentEnd();
}

/*********************************************************************************************/

void WebRestart(uint32_t type)
{
  // type 0 = restart
  // type 1 = restart after config change
  // type 2 = Checking WiFi Connection - no restart, only refresh page.
  // type 3 = restart after WiFi Connection Test Successful
  bool reset_only = (HTTP_MANAGER_RESET_ONLY == Web.state);

  WSContentStart_P((type) ? PSTR(D_SAVE_CONFIGURATION) : PSTR(D_RESTART), !reset_only);
#if ((RESTART_AFTER_INITIAL_WIFI_CONFIG) && (AFTER_INITIAL_WIFI_CONFIG_GO_TO_NEW_IP))
  // In case of type 3 (New network has been configured) go to the new device's IP in the new Network
  if (3 == type) {
    WSContentSend_P("setTimeout(function(){location.href='http://%_I';},%d);",
      (uint32_t)WiFi.localIP(),
      HTTP_RESTART_RECONNECT_TIME
    );
  } else {
    WSContentSend_P(HTTP_SCRIPT_RELOAD_TIME, HTTP_RESTART_RECONNECT_TIME);
  }
#else
  // In case of type 3 (New network has been configured) do not refresh the page. Just halt.
  // The IP of the device while was in AP mode, won't be the new IP of the newly configured Network.
  if (!(3 == type)) { WSContentSend_P(HTTP_SCRIPT_RELOAD_TIME, HTTP_RESTART_RECONNECT_TIME); }
#endif
  WSContentSendStyle();
  if (type) {
    if (!(3 == type)) {
      WSContentSend_P(PSTR("<div style='text-align:center;'><b>%s</b><br><br></div>"), (type==2) ? PSTR(D_TRYING_TO_CONNECT) : PSTR(D_CONFIGURATION_SAVED) );
    } else {
#if (AFTER_INITIAL_WIFI_CONFIG_GO_TO_NEW_IP)
      WSContentTextCenterStart(WebColor(COL_TEXT_SUCCESS));
      WSContentSend_P(PSTR(D_SUCCESSFUL_WIFI_CONNECTION "<br><br></div><div style='text-align:center;'>" D_REDIRECTING_TO_NEW_IP "<br><br><a href='http://%_I'>%_I</a><br></div>"),(uint32_t)WiFi.localIP(),(uint32_t)WiFi.localIP());
      WSContentSend_P(PSTR("<div id='t' style='text-align:center'></div>"));
      WSContentSend_P(PSTR("<script>var cn=30; function counter(){if(cn>=0){eb('t').innerHTML='" D_RESTART_IN " '+cn+' " D_SECONDS " 안에 이동합니다.';cn--;setTimeout(counter, 1000);}}wl(counter);</script><br>"));
#else
      WSContentTextCenterStart(WebColor(COL_TEXT_SUCCESS));
      WSContentSend_P(PSTR(D_SUCCESSFUL_WIFI_CONNECTION "<br><br></div><div style='text-align:center;'>" D_NOW_YOU_CAN_CLOSE_THIS_WINDOW "<br><br></div>"));
#endif
    }
  }
  if (type<2) {
    WSContentSend_P(HTTP_MSG_RSTRT);
    if (HTTP_MANAGER == Web.state || reset_only) {
      Web.state = HTTP_ADMIN;
    } else {
      WSContentSpaceButton(BUTTON_MAIN);
    }
  }
  WSContentStop();

  if (!(2 == type)) {
    AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_RESTART));
    ShowWebSource(SRC_WEBGUI);
    TasmotaGlobal.restart_flag = 2;
  }
}

/*********************************************************************************************/

void HandleCognitoLogin(void)
{
  Webserver->sendHeader(F("Location"), String(F("https://ziot-sonoff-auth.auth.ap-northeast-2.amazoncognito.com/login?client_id=3ambmcokjea85jv4ff2hmkb0un&response_type=code&scope=openid&redirect_uri=https://nb0pw9tdj5.execute-api.ap-northeast-2.amazonaws.com/2021-08-16/login")), true);
  WSSend(302, CT_PLAIN, "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
}

void HandleCognitoLoginCode(void)
{
  WebserverSecure->send(200, "text/plain", "Hello from esp8266 over HTTPS!");  
}

void HandleWifiLogin(void)
{
  WSContentStart_P(PSTR(D_CONFIGURE_WIFI), false);  // false means show page no matter if the client has or has not credentials
  WSContentSendStyle();
  WSContentSend_P(HTTP_FORM_LOGIN);

  if (HTTP_MANAGER_RESET_ONLY == Web.state) {
    WSContentSpaceButton(BUTTON_RESTART);
#ifndef FIRMWARE_MINIMAL
    WSContentSpaceButton(BUTTON_RESET_CONFIGURATION);
#endif  // FIRMWARE_MINIMAL
  }

  WSContentStop();
}

uint32_t WebDeviceColumns(void) {
  const uint32_t max_columns = 8;

  uint32_t rows = TasmotaGlobal.devices_present / max_columns;
  if (TasmotaGlobal.devices_present % max_columns) { rows++; }
  uint32_t cols = TasmotaGlobal.devices_present / rows;
  if (TasmotaGlobal.devices_present % rows) { cols++; }
  return cols;
}

#ifdef USE_LIGHT
void WebSliderColdWarm(void)
{
  WSContentSend_P(HTTP_MSG_SLIDER_GRADIENT,  // Cold Warm
    PSTR("a"),             // a - Unique HTML id
    PSTR("#eff"), PSTR("#f81"),  // 6500k in RGB (White) to 2500k in RGB (Warm Yellow)
    1,               // sl1
    153, 500,        // Range color temperature
    LightGetColorTemp(),
    't', 0);         // t0 - Value id releated to lc("t0", value) and WebGetArg("t0", tmp, sizeof(tmp));
}
#endif  // USE_LIGHT

void HandleRoot(void)
{
#ifndef NO_CAPTIVE_PORTAL
  if (CaptivePortal()) { return; }  // If captive portal redirect instead of displaying the page.
#endif  // NO_CAPTIVE_PORTAL

  if (Webserver->hasArg(F("rst"))) {
    WebRestart(0);
    return;
  }

  if (WifiIsInManagerMode()) {
#ifndef FIRMWARE_MINIMAL
      if (!strlen(SettingsText(SET_WEBPWD)) || (((Webserver->arg(F("USER1")) == WEB_USERNAME ) && (Webserver->arg(F("PASS1")) == SettingsText(SET_WEBPWD) )) || HTTP_MANAGER_RESET_ONLY == Web.state)) {
        if (!Web.initial_config) {
          Web.initial_config = !strlen(SettingsText(SET_STASSID1));
          if (Web.initial_config) { AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP "Blank Device - Initial Configuration")); }
        }
        HandleWifiConfiguration();
      }
#endif  // Not FIRMWARE_MINIMAL
    return;
  }

  if (HandleRootStatusRefresh()) {
    return;
  }

  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_MAIN_MENU));

  char stemp[33];

  WSContentStart_P(PSTR(D_MAIN_MENU));
#ifdef USE_SCRIPT_WEB_DISPLAY
  WSContentSend_P(HTTP_SCRIPT_ROOT2, Settings->web_refresh, Settings->web_refresh);
#else
  WSContentSend_P(HTTP_SCRIPT_ROOT2, Settings->web_refresh);
#endif
  WSContentSend_P(HTTP_SCRIPT_ROOT_PART2);

  WSContentSendStyle();
  WSContentSend_P(PSTR("<div id=\"loader\" style='padding:0'></div>"));
  WSContentSend_P(PSTR("</br>"));

  WSContentSend_P(PSTR("<div style='padding:0;' id='l1' name='l1'></div>"));

#ifndef FIRMWARE_MINIMAL
  XdrvCall(FUNC_WEB_ADD_MAIN_BUTTON);
  XsnsCall(FUNC_WEB_ADD_MAIN_BUTTON);
#endif  // Not FIRMWARE_MINIMAL

  if (HTTP_ADMIN == Web.state) {
    WSContentSpaceButton(BUTTON_COGNITO_LOGIN);
    WSContentSpaceButton(BUTTON_CONFIGURATION);
    WSContentButton(BUTTON_INFORMATION);
    WSContentButton(BUTTON_RESTART);
  }
  WSContentStop();
}

bool HandleRootStatusRefresh(void)
{
  if (!Webserver->hasArg("m")) {     // Status refresh requested
    return false;
  }

  #ifdef USE_SCRIPT_WEB_DISPLAY
    Script_Check_HTML_Setvars();
  #endif

  WSContentSend_P(PSTR("{t}<tr>"));
  WSContentSend_P(HTTP_DEVICE_STATE, 40, PSTR("normal"), 17, TasmotaGlobal.mqtt_connected ? "홈 IoT 서비스가 동작 중입니다.":"홈 IoT 서비스에 연결중입니다..");
  WSContentSend_P(PSTR("</tr></table></br>"));
  WSContentSend_P(PSTR("\n\n"));  // Prep for SSE
  if (!TasmotaGlobal.mqtt_connected) {
    WSContentSend_P(PSTR("<div style='display:none'>{loader}</div>"));
  }
  WSContentEnd();

  return true;
}

#ifdef USE_SHUTTER
int32_t IsShutterWebButton(uint32_t idx) {
  /* 0: Not a shutter, 1..4: shutter up idx, -1..-4: shutter down idx */
  int32_t ShutterWebButton = 0;
  if (Settings->flag3.shutter_mode) {  // SetOption80 - Enable shutter support
    for (uint32_t i = 0; i < MAX_SHUTTERS; i++) {
      if (Settings->shutter_startrelay[i] && ((Settings->shutter_startrelay[i] == idx) || (Settings->shutter_startrelay[i] == (idx-1)))) {
        ShutterWebButton = (Settings->shutter_startrelay[i] == idx) ? (i+1): (-1-i);
        break;
      }
    }
  }
  return ShutterWebButton;
}
#endif // USE_SHUTTER

/*-------------------------------------------------------------------------------------------*/

#ifndef FIRMWARE_MINIMAL

void HandleConfiguration(void)
{
  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_CONFIGURATION));

  WSContentStart_P(PSTR(D_CONFIGURATION));
  WSContentSendStyle();

  WSContentButton(BUTTON_WIFI);

  WSContentSpaceButton(BUTTON_RESET_CONFIGURATION);
  WSContentSpaceButton(BUTTON_FACTORY_RESET);

  WSContentSpaceButton(BUTTON_MAIN);
  WSContentStop();
}

/*-------------------------------------------------------------------------------------------*/

void HandleTest(void) {
  Webserver->send(200, "text/plain", "Hello from esp8266 over HTTPS!");
}

/*-------------------------------------------------------------------------------------------*/

const char kUnescapeCode[] = "&><\"\'\\";
const char kEscapeCode[] PROGMEM = "&amp;|&gt;|&lt;|&quot;|&apos;|&#92;";

String HtmlEscape(const String unescaped) {
  char escaped[10];
  size_t ulen = unescaped.length();
  String result = "";
  for (size_t i = 0; i < ulen; i++) {
    char c = unescaped[i];
    char *p = strchr(kUnescapeCode, c);
    if (p != nullptr) {
      result += GetTextIndexed(escaped, sizeof(escaped), p - kUnescapeCode, kEscapeCode);
    } else {
      result += c;
    }
  }
  return result;
}

void HandleWifiConfiguration(void) {
  char tmp[TOPSZ];  // Max length is currently 150

  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_CONFIGURE_WIFI));

  if (Webserver->hasArg(F("save")) && HTTP_MANAGER_RESET_ONLY != Web.state) {
    if ( WifiIsInManagerMode() ) {
      // Test WIFI Connection to Router
      // As Tasmota is in this case in AP mode, a STA connection can be established too at the same time
      Web.wifi_test_counter = 9;   // seconds to test user's proposed AP
      Web.wifiTest = WIFI_TESTING;

      Web.save_data_counter = TasmotaGlobal.save_data_counter;
      TasmotaGlobal.save_data_counter = 0;               // Stop auto saving data - Updating Settings
      Settings->save_data = 0;

      if (MAX_WIFI_OPTION == Web.old_wificonfig) { Web.old_wificonfig = Settings->sta_config; }
      TasmotaGlobal.wifi_state_flag = Settings->sta_config = WIFI_MANAGER;

      TasmotaGlobal.sleep = 0;                           // Disable sleep
      TasmotaGlobal.restart_flag = 0;                    // No restart
      TasmotaGlobal.ota_state_flag = 0;                  // No OTA
//      TasmotaGlobal.blinks = 0;                          // Disable blinks initiated by WifiManager

      WebGetArg(PSTR("s1"), tmp, sizeof(tmp));   // SSID1
      SettingsUpdateText(SET_STASSID1, tmp);
      WebGetArg(PSTR("p1"), tmp, sizeof(tmp));   // PASSWORD1
      SettingsUpdateText(SET_STAPWD1, tmp);
      WebGetArg(PSTR("d"), tmp, sizeof(tmp));   // DeviceName
      SettingsUpdateText(SET_DEVICENAME, tmp);
      SettingsUpdateText(SET_FRIENDLYNAME1, tmp);

      AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_WIFI D_CONNECTING_TO_AP " %s " D_AS " %s ..."),
        SettingsText(SET_STASSID1), TasmotaGlobal.hostname);

      WiFi.begin(SettingsText(SET_STASSID1), SettingsText(SET_STAPWD1));

      WebRestart(2);
    } else {
      // STATION MODE or MIXED
      // Save the config and restart
      WifiSaveSettings();
      WebRestart(1);
    }
    return;
  }

  if ( WIFI_TEST_FINISHED_SUCCESSFUL == Web.wifiTest ) {
    Web.wifiTest = WIFI_NOT_TESTING;
#if (RESTART_AFTER_INITIAL_WIFI_CONFIG)
    WebRestart(3);
#else
    HandleRoot();
#endif
  }

  WSContentStart_P(PSTR(D_CONFIGURE_WIFI), !WifiIsInManagerMode());
  WSContentSend_P(HTTP_SCRIPT_WIFI);
  if (WifiIsInManagerMode()) { WSContentSend_P(HTTP_SCRIPT_HIDE); }
  if (WIFI_TESTING == Web.wifiTest) { WSContentSend_P(HTTP_SCRIPT_RELOAD_TIME, HTTP_RESTART_RECONNECT_TIME); }
#ifdef USE_ENHANCED_GUI_WIFI_SCAN
  WSContentSendStyle_P(HTTP_HEAD_STYLE_SSI, WebColor(COL_TEXT));
#else
  WSContentSendStyle();
#endif  // USE_ENHANCED_GUI_WIFI_SCAN

  bool limitScannedNetworks = true;
  if (HTTP_MANAGER_RESET_ONLY != Web.state) {
    if (WIFI_TESTING == Web.wifiTest) {
      limitScannedNetworks = false;
    } else {
      if (Webserver->hasArg(F("scan"))) { limitScannedNetworks = false; }

      AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_WIFI "Scanning..."));
#ifdef USE_EMULATION
      UdpDisconnect();
#endif  // USE_EMULATION
      int n = WiFi.scanNetworks();
      AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_WIFI D_SCAN_DONE));

      if (0 == n) {
        AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_WIFI D_NO_NETWORKS_FOUND));
        WSContentSend_P(PSTR(D_NO_NETWORKS_FOUND));
        limitScannedNetworks = false; // in order to show D_SCAN_FOR_WIFI_NETWORKS
      } else {
        //sort networks
        int indices[n];
        for (uint32_t i = 0; i < n; i++) {
          indices[i] = i;
        }

        // RSSI SORT
        for (uint32_t i = 0; i < n; i++) {
          for (uint32_t j = i + 1; j < n; j++) {
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
              std::swap(indices[i], indices[j]);
            }
          }
        }

        uint32_t networksToShow = n;
        if ((limitScannedNetworks) && (networksToShow > MAX_WIFI_NETWORKS_TO_SHOW)) { networksToShow = MAX_WIFI_NETWORKS_TO_SHOW; }

        if (WifiIsInManagerMode()) {
          WSContentTextCenterStart(WebColor(COL_TEXT));
          WSContentSend_P(PSTR(D_SELECT_YOUR_WIFI_NETWORK "</div><br>"));
        }

#ifdef USE_ENHANCED_GUI_WIFI_SCAN
        //display networks in page
        bool skipduplicated;
        int ssid_showed = 0;
        for (uint32_t i = 0; i < networksToShow; i++) {
          if (indices[i] < n) {
            int32_t rssi = WiFi.RSSI(indices[i]);
            String ssid = WiFi.SSID(indices[i]);
            DEBUG_CORE_LOG(PSTR(D_LOG_WIFI D_SSID " %s, " D_BSSID " %s, " D_CHANNEL " %d, " D_RSSI " %d"),
              ssid.c_str(), WiFi.BSSIDstr(indices[i]).c_str(), WiFi.channel(indices[i]), rssi);

            String ssid_copy = ssid;
            if (!ssid_copy.length()) { ssid_copy = F("no_name"); }
            // Print SSID
            if (!limitScannedNetworks) {
              WSContentSend_P(PSTR("<div><a href='#p' onclick='c(this)'>%s</a><br>"), HtmlEscape(ssid_copy).c_str());
            }
            skipduplicated = false;
            String nextSSID = "";
            // Handle all APs with the same SSID
            for (uint32_t j = 0; j < n; j++) {
              if ((indices[j] < n) && ((nextSSID = WiFi.SSID(indices[j])) == ssid)) {
                if (!skipduplicated) {
                  // Update RSSI / quality
                  rssi = WiFi.RSSI(indices[j]);
                  uint32_t rssi_as_quality = WifiGetRssiAsQuality(rssi);
                  uint32_t num_bars = changeUIntScale(rssi_as_quality, 0, 100, 0, 4);

                  WSContentSend_P(PSTR("<div title='%d dBm (%d%%)'>"), rssi, rssi_as_quality);
                  if (limitScannedNetworks) {
                    // Print SSID and item
                    WSContentSend_P(PSTR("<a href='#p' onclick='c(this)'>%s</a><span class='q'><div class='si'>"), HtmlEscape(ssid_copy).c_str());
                    ssid_showed++;
                    skipduplicated = true; // For the simplified page, just show 1 SSID if there are many Networks with the same
                  } else {
                    // Print item
                    WSContentSend_P(PSTR("%s<span class='q'>(%d) <div class='si'>"), WiFi.BSSIDstr(indices[j]).c_str(), WiFi.channel(indices[j])
                    );
                  }
                  // Print signal strength indicator
                  for (uint32_t k = 0; k < 4; ++k) {
                    WSContentSend_P(PSTR("<i class='b%d%s'></i>"), k, (num_bars < k) ? PSTR(" o30") : PSTR(""));
                  }
                  WSContentSend_P(PSTR("</span></div></div>"));
                } else {
                  if (ssid_showed <= networksToShow ) { networksToShow++; }
                }
                indices[j] = n;
              }
              delay(0);
            }
            if (!limitScannedNetworks) {
              WSContentSend_P(PSTR("</div>"));
            }
          }
        }
#else  // No USE_ENHANCED_GUI_WIFI_SCAN
        // remove duplicates ( must be RSSI sorted )
        for (uint32_t i = 0; i < n; i++) {
          if (-1 == indices[i]) { continue; }
          String cssid = WiFi.SSID(indices[i]);
          uint32_t cschn = WiFi.channel(indices[i]);
          for (uint32_t j = i + 1; j < n; j++) {
            if ((cssid == WiFi.SSID(indices[j])) && (cschn == WiFi.channel(indices[j]))) {
              DEBUG_CORE_LOG(PSTR(D_LOG_WIFI D_DUPLICATE_ACCESSPOINT " %s"), WiFi.SSID(indices[j]).c_str());
              indices[j] = -1;  // set dup aps to index -1
            }
          }
        }

        //display networks in page
        for (uint32_t i = 0; i < networksToShow; i++) {
          if (-1 == indices[i]) { continue; }  // skip dups
          int32_t rssi = WiFi.RSSI(indices[i]);
          DEBUG_CORE_LOG(PSTR(D_LOG_WIFI D_SSID " %s, " D_BSSID " %s, " D_CHANNEL " %d, " D_RSSI " %d"),
            WiFi.SSID(indices[i]).c_str(), WiFi.BSSIDstr(indices[i]).c_str(), WiFi.channel(indices[i]), rssi);
          int quality = WifiGetRssiAsQuality(rssi);
          String ssid_copy = WiFi.SSID(indices[i]);
          if (!ssid_copy.length()) { ssid_copy = F("no_name"); }
          WSContentSend_P(PSTR("<div><a href='#p' onclick='c(this)'>%s</a>&nbsp;(%d)&nbsp<span class='q'>%d%% (%d dBm)</span></div>"),
            HtmlEscape(ssid_copy).c_str(),
            WiFi.channel(indices[i]),
            quality, rssi
          );

          delay(0);
        }
#endif  // USE_ENHANCED_GUI_WIFI_SCAN

        WSContentSend_P(PSTR("<br>"));
      }
    }

    WSContentSend_P(PSTR("<div><a href='/wi?scan='>%s</a></div><br>"), (limitScannedNetworks) ? PSTR(D_SHOW_MORE_WIFI_NETWORKS) : PSTR(D_SCAN_FOR_WIFI_NETWORKS));
    WSContentSend_P(HTTP_FORM_WIFI_PART1, (WifiIsInManagerMode()) ? "" : PSTR(" (" STA_SSID1 ")"), SettingsText(SET_STASSID1));
    if (WifiIsInManagerMode()) {
      // As WIFI_HOSTNAME may contain %s-%04d it cannot be part of HTTP_FORM_WIFI where it will exception
      WSContentSend_P(PSTR("></p><p><b> 기기 별명 </b><br><input id='d' placeholder=\" 기기 별명을 입력해주세요 \"></p>"));
    } else {
      WSContentSend_P(HTTP_FORM_WIFI_PART2, SettingsText(SET_STASSID2), WIFI_HOSTNAME, WIFI_HOSTNAME, SettingsText(SET_HOSTNAME), SettingsText(SET_CORS));
    }

    WSContentSend_P(HTTP_FORM_END);
  }

  if (WifiIsInManagerMode()) {
#ifndef FIRMWARE_MINIMAL
    WSContentTextCenterStart(WebColor(COL_TEXT_WARNING));
    WSContentSend_P(PSTR("<h3>"));

    if (WIFI_TESTING == Web.wifiTest) {
      WSContentSend_P(PSTR(D_TRYING_TO_CONNECT "<br>%s</h3></div>"), SettingsText(SET_STASSID1));
    } else if (WIFI_TEST_FINISHED_BAD == Web.wifiTest) {
      WSContentSend_P(PSTR(D_CONNECT_FAILED_TO " %s<br>" D_CHECK_CREDENTIALS "</h3></div>"), SettingsText(SET_STASSID1));
    }
    WSContentSend_P(PSTR("<button style=\"display:block\" onclick=\"document.getElementById('s1').value='%s';document.getElementById('p1').value='%s';\">Test용 WiFi</button><p></p>"), DEFAULT_SSID, DEFAULT_PASS);
    // More Options Button
    WSContentSend_P(PSTR("<div id=butmod style=\"display:%s;\"></div><p><form id=butmo style=\"display:%s;\"><button type='button' onclick='hidBtns()'>" D_SHOW_MORE_OPTIONS "</button></form></p>"),
      (WIFI_TEST_FINISHED_BAD == Web.wifiTest) ? "none" : Web.initial_config ? "block" : "none", Web.initial_config ? "block" : "none"
    );
    WSContentSpaceButton(BUTTON_RESTORE, !Web.initial_config);
    WSContentButton(BUTTON_RESET_CONFIGURATION, !Web.initial_config);
#endif  // FIRMWARE_MINIMAL
    WSContentSpaceButton(BUTTON_RESTART, !Web.initial_config);
  } else {
    WSContentSpaceButton(BUTTON_CONFIGURATION);
  }
  WSContentStop();
}

void WifiSaveSettings(void) {
  String cmnd = F(D_CMND_BACKLOG "0 ");
  cmnd += AddWebCommand(PSTR(D_CMND_HOSTNAME), PSTR("h"), PSTR("1"));
  cmnd += AddWebCommand(PSTR(D_CMND_CORS), PSTR("c"), PSTR("1"));
  cmnd += AddWebCommand(PSTR(D_CMND_SSID "1"), PSTR("s1"), PSTR("1"));
  cmnd += AddWebCommand(PSTR(D_CMND_SSID "2"), PSTR("s2"), PSTR("1"));
  cmnd += AddWebCommand(PSTR(D_CMND_PASSWORD "3"), PSTR("p1"), PSTR("\""));
  cmnd += AddWebCommand(PSTR(D_CMND_PASSWORD "4"), PSTR("p2"), PSTR("\""));
  ExecuteWebCommand((char*)cmnd.c_str());
}

/*-------------------------------------------------------------------------------------------*/

void HandleBackupConfiguration(void)
{
  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_BACKUP_CONFIGURATION));

  uint32_t config_len = SettingsConfigBackup();
  if (!config_len) { return; }

  WiFiClient myClient = Webserver->client();
  Webserver->setContentLength(config_len);

  char attachment[TOPSZ];
  snprintf_P(attachment, sizeof(attachment), PSTR("attachment; filename=%s"), SettingsConfigFilename().c_str());
  Webserver->sendHeader(F("Content-Disposition"), attachment);

  WSSend(200, CT_APP_STREAM, "");
  myClient.write((const char*)settings_buffer, config_len);

  SettingsBufferFree();
}

/*-------------------------------------------------------------------------------------------*/

void HandleResetConfiguration(void)
{
  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_RESET_CONFIGURATION));

  WSContentStart_P(PSTR(D_RESET_CONFIGURATION), !WifiIsInManagerMode());
  WSContentSendStyle();
  WSContentSend_P(PSTR("<div style='text-align:center;'>" D_CONFIGURATION_RESET "</div>"));
  WSContentSend_P(HTTP_MSG_RSTRT);
  WSContentSpaceButton(BUTTON_MAIN);
  WSContentStop();

  char command[CMDSZ];
  snprintf_P(command, sizeof(command), PSTR(D_CMND_RESET " 1"));
  ExecuteWebCommand(command);
}

void HandleFactoryResetConfiguration(void)
{
  AddLog(LOG_LEVEL_DEBUG, PSTR("공장 초기화 중..."));

  WSContentStart_P(PSTR("공장 초기화"), !WifiIsInManagerMode());
  WSContentSendStyle();
  WSContentSend_P(PSTR("<div style='text-align:center;'>" D_CONFIGURATION_RESET "</div>"));
  WSContentSend_P(HTTP_MSG_RSTRT);
  WSContentSpaceButton(BUTTON_MAIN);
  WSContentStop();

  char command[CMDSZ];
  snprintf_P(command, sizeof(command), PSTR(D_CMND_RESET " 2"));
  ExecuteWebCommand(command);
}

void HandleRestoreConfiguration(void)
{
  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_RESTORE_CONFIGURATION));

  WSContentStart_P(PSTR(D_RESTORE_CONFIGURATION));
  WSContentSendStyle();
  WSContentSend_P(HTTP_FORM_RST);
  WSContentSend_P(HTTP_FORM_RST_UPG, PSTR(D_RESTORE));
  if (WifiIsInManagerMode()) {
    WSContentSpaceButton(BUTTON_MAIN);
  } else {
    WSContentSpaceButton(BUTTON_CONFIGURATION);
  }
  WSContentStop();

  Web.upload_file_type = UPL_SETTINGS;
}

/*-------------------------------------------------------------------------------------------*/

void HandleInformation(void)
{
  float freemem = ((float)ESP_getFreeHeap()) / 1024;
  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_INFORMATION));

  char stopic[TOPSZ];

  WSContentStart_P(PSTR(D_INFORMATION));
  // Save 1k of code space replacing table html with javascript replace codes
  // }1 = </td></tr><tr><th>
  // }2 = </th><td>
  WSContentSend_P(HTTP_SCRIPT_INFO_BEGIN);
  WSContentSend_P(PSTR("<table style='width:100%%'><tr><th>"));
  // TODO: ZIoT 버전 변경
  WSContentSend_P(PSTR(D_PROGRAM_VERSION "}2%s%s"), TasmotaGlobal.version, TasmotaGlobal.image_name);
  WSContentSend_P(PSTR("}1" D_UPTIME "}2%s"), GetUptime().c_str());
  WSContentSend_P(PSTR("}1}2&nbsp;"));  // Empty line
#ifdef ESP32
#ifdef USE_ETHERNET
  if (static_cast<uint32_t>(EthernetLocalIP()) != 0) {
    WSContentSend_P(PSTR("}1" D_HOSTNAME "}2%s%s"), EthernetHostname(), (Mdns.begun) ? PSTR(".local") : "");
    WSContentSend_P(PSTR("}1" D_MAC_ADDRESS "}2%s"), EthernetMacAddress().c_str());
    WSContentSend_P(PSTR("}1" D_IP_ADDRESS " (eth)}2%_I"), (uint32_t)EthernetLocalIP());
    WSContentSend_P(PSTR("}1}2&nbsp;"));  // Empty line
  }
#endif
#endif
  if (Settings->flag4.network_wifi) {
    int32_t rssi = WiFi.RSSI();
    WSContentSend_P(PSTR("}1" D_AP "%d " D_SSID " (" D_RSSI ")}2%s (%d%%, %d dBm) 11%c"), Settings->sta_active +1, HtmlEscape(SettingsText(SET_STASSID1 + Settings->sta_active)).c_str(), WifiGetRssiAsQuality(rssi), rssi, pgm_read_byte(&kWifiPhyMode[WiFi.getPhyMode() & 0x3]) );
    WSContentSend_P(PSTR("}1" D_HOSTNAME "}2%s%s"), TasmotaGlobal.hostname, (Mdns.begun) ? PSTR(".local") : "");
#if LWIP_IPV6
    String ipv6_addr = WifiGetIPv6();
    if (ipv6_addr != "") {
      WSContentSend_P(PSTR("}1 IPv6 Address }2%s"), ipv6_addr.c_str());
    }
#endif
    if (static_cast<uint32_t>(WiFi.localIP()) != 0) {
      WSContentSend_P(PSTR("}1" D_MAC_ADDRESS "}2%s"), WiFi.macAddress().c_str());
      WSContentSend_P(PSTR("}1" D_IP_ADDRESS " (wifi)}2%_I"), (uint32_t)WiFi.localIP());
      WSContentSend_P(PSTR("}1}2&nbsp;"));  // Empty line
    }
  }
  if (!TasmotaGlobal.global_state.network_down) {
    WSContentSend_P(PSTR("}1" D_GATEWAY "}2%_I"), Settings->ipv4_address[1]);
    WSContentSend_P(PSTR("}1" D_SUBNET_MASK "}2%_I"), Settings->ipv4_address[2]);
    WSContentSend_P(PSTR("}1" D_DNS_SERVER "}2%_I"), Settings->ipv4_address[3]);
  }
  if ((WiFi.getMode() >= WIFI_AP) && (static_cast<uint32_t>(WiFi.softAPIP()) != 0)) {
    WSContentSend_P(PSTR("}1}2&nbsp;"));  // Empty line
    WSContentSend_P(PSTR("}1" D_MAC_ADDRESS "}2%s"), WiFi.softAPmacAddress().c_str());
    WSContentSend_P(PSTR("}1" D_IP_ADDRESS " (AP)}2%_I"), (uint32_t)WiFi.softAPIP());
    WSContentSend_P(PSTR("}1" D_GATEWAY "}2%_I"), (uint32_t)WiFi.softAPIP());
  }
  WSContentSend_P(PSTR("}1}2&nbsp;"));  // Empty line
  WSContentSend_P(PSTR("</td></tr></table>"));

  WSContentSend_P(HTTP_SCRIPT_INFO_END);
  WSContentSendStyle();
  // WSContentSend_P(PSTR("<fieldset><legend><b>&nbsp;Information&nbsp;</b></legend>"));
  WSContentSend_P(PSTR("<style>td{padding:0px 5px;}</style>"
                       "<div id='i' name='i'></div>"));
  //   WSContentSend_P(PSTR("</fieldset>"));

  // Show more
  WSContentSend_P(PSTR("<div id=\"but2d\" style=\"display:block;\"></div><p></p><form id=\"but2\" style=\"display:block;\"><button onclick=\"if(document.getElementById('hide_context').style.display != ''){document.getElementById('hide_context').style.display = '';this.innerText = '숨기기';}else{document.getElementById('hide_context').style.display = 'none'; this.innerText = '더보기';}\" type=\"button\"/>더보기</button></form>"));
  WSContentSend_P(PSTR("<div id=\"hide_context\" style=\"display: none;\">"));
  WSContentSend_P("<script>function hide(){var s,o=\"");
  WSContentSend_P(PSTR("<table style='width:100%%'><tr><th>"));
  WSContentSend_P(PSTR(D_BUILD_DATE_AND_TIME "}2%s"), GetBuildDateAndTime().c_str());
  WSContentSend_P(PSTR("}1" D_CORE_AND_SDK_VERSION "}2" ARDUINO_CORE_RELEASE "/%s"), ESP.getSdkVersion());
  #ifdef ESP8266
  WSContentSend_P(PSTR("}1" D_FLASH_WRITE_COUNT "}2%d at 0x%X"), Settings->save_flag, GetSettingsAddress());
#endif  // ESP8266
#ifdef ESP32
  WSContentSend_P(PSTR("}1" D_FLASH_WRITE_COUNT "}2%d"), Settings->save_flag);
#endif  // ESP32
  WSContentSend_P(PSTR("}1" D_BOOT_COUNT "}2%d"), Settings->bootcount);
  WSContentSend_P(PSTR("}1" D_RESTART_REASON "}2%s"), GetResetReason().c_str());
  WSContentSend_P(PSTR("}1}2&nbsp;"));  // Empty line
  if (Settings->flag.mqtt_enabled) {  // SetOption3 - Enable MQTT
    WSContentSend_P(PSTR("}1" D_MQTT_HOST "}2%s"), SettingsText(SET_MQTT_HOST));
    WSContentSend_P(PSTR("}1" D_MQTT_PORT "}2%d"), Settings->mqtt_port);
#ifdef USE_MQTT_TLS
    WSContentSend_P(PSTR("}1" D_MQTT_TLS_ENABLE "}2%s"), Settings->flag4.mqtt_tls ? PSTR(D_ENABLED) : PSTR(D_DISABLED));
#endif  // USE_MQTT_TLS
    WSContentSend_P(PSTR("}1" D_MQTT_USER "}2%s"), SettingsText(SET_MQTT_USER));
    WSContentSend_P(PSTR("}1" D_MQTT_CLIENT "}2%s"), TasmotaGlobal.mqtt_client);
    WSContentSend_P(PSTR("}1" D_MQTT_TOPIC "}2%s"), SettingsText(SET_MQTT_TOPIC));
    uint32_t real_index = SET_MQTT_GRP_TOPIC;
    for (uint32_t i = 0; i < MAX_GROUP_TOPICS; i++) {
      if (1 == i) { real_index = SET_MQTT_GRP_TOPIC2 -1; }
      if (strlen(SettingsText(real_index +i))) {
        WSContentSend_P(PSTR("}1" D_MQTT_GROUP_TOPIC " %d}2%s"), 1 +i, GetGroupTopic_P(stopic, "", real_index +i));
      }
    }
    WSContentSend_P(PSTR("}1" D_MQTT_FULL_TOPIC "}2%s"), GetTopic_P(stopic, CMND, TasmotaGlobal.mqtt_topic, ""));
    WSContentSend_P(PSTR("}1" D_MQTT " " D_FALLBACK_TOPIC "}2%s"), GetFallbackTopic_P(stopic, ""));
    WSContentSend_P(PSTR("}1" D_MQTT_NO_RETAIN "}2%s"), Settings->flag4.mqtt_no_retain ? PSTR(D_ENABLED) : PSTR(D_DISABLED));
  } else {
    WSContentSend_P(PSTR("}1" D_MQTT "}2" D_DISABLED));
  }

  WSContentSend_P(PSTR("}1}2&nbsp;"));  // Empty line
#ifdef USE_EMULATION
  WSContentSend_P(PSTR("}1" D_EMULATION "}2%s"), GetTextIndexed(stopic, sizeof(stopic), Settings->flag2.emulation, kEmulationOptions));
#endif  // USE_EMULATION
  WSContentSend_P(PSTR("}1" D_MDNS_DISCOVERY "}2%s"), (Settings->flag3.mdns_enabled) ? D_ENABLED : D_DISABLED);  // SetOption55 - Control mDNS service
  if (Settings->flag3.mdns_enabled) {  // SetOption55 - Control mDNS service
    WSContentSend_P(PSTR("}1" D_MDNS_ADVERTISE "}2" D_WEB_SERVER));
  }
  else {
    WSContentSend_P(PSTR("}1" D_MDNS_ADVERTISE "}2" D_DISABLED));
  }

  WSContentSend_P(PSTR("}1}2&nbsp;"));  // Empty line
  WSContentSend_P(PSTR("}1" D_ESP_CHIP_ID "}2%d (%s)"), ESP_getChipId(), GetDeviceHardware().c_str());
#ifdef ESP8266
  WSContentSend_P(PSTR("}1" D_FLASH_CHIP_ID "}20x%06X"), ESP.getFlashChipId());
#endif
  WSContentSend_P(PSTR("}1" D_FLASH_CHIP_SIZE "}2%d kB"), ESP.getFlashChipRealSize() / 1024);
  WSContentSend_P(PSTR("}1" D_PROGRAM_FLASH_SIZE "}2%d kB"), ESP.getFlashChipSize() / 1024);
  WSContentSend_P(PSTR("}1" D_PROGRAM_SIZE "}2%d kB"), ESP_getSketchSize() / 1024);
  WSContentSend_P(PSTR("}1" D_FREE_PROGRAM_SPACE "}2%d kB"), ESP.getFreeSketchSpace() / 1024);
#ifdef ESP32
  int32_t freeMaxMem = 100 - (int32_t)(ESP_getMaxAllocHeap() * 100 / ESP_getFreeHeap());
  WSContentSend_PD(PSTR("}1" D_FREE_MEMORY "}2%1_f kB (" D_FRAGMENTATION " %d%%)"), &freemem, freeMaxMem);
  if (psramFound()) {
    WSContentSend_P(PSTR("}1" D_PSR_MAX_MEMORY "}2%d kB"), ESP.getPsramSize() / 1024);
    WSContentSend_P(PSTR("}1" D_PSR_FREE_MEMORY "}2%d kB"), ESP.getFreePsram() / 1024);
  }
#else // ESP32
  WSContentSend_PD(PSTR("}1" D_FREE_MEMORY "}2%1_f kB"), &freemem);
#endif // ESP32
  WSContentSend_P(PSTR("</td></tr></table>"));
  WSContentSend_P(PSTR("\";s=o.replace(/}1/g,\"</td></tr><tr><th>\").replace(/}2/g,\"</th><td>\");eb('hide').innerHTML=s;}wl(hide);</script>"));
  WSContentSend_P(PSTR("<div id='hide' name='hide'></div>"));
  WSContentSend_P(PSTR("</div>"));

  WSContentSpaceButton(BUTTON_MAIN);
  WSContentStop();
}
#endif  // Not FIRMWARE_MINIMAL

/*-------------------------------------------------------------------------------------------*/

#if defined(USE_ZIGBEE_EZSP) || defined(USE_TASMOTA_CLIENT) || defined(SHELLY_FW_UPGRADE) || defined(USE_RF_FLASH) || defined(USE_CCLOADER)
#define USE_WEB_FW_UPGRADE
#endif

#ifdef USE_WEB_FW_UPGRADE

struct {
  size_t spi_hex_size;
  size_t spi_sector_counter;
  size_t spi_sector_cursor;
  bool active;
  bool ready;
} BUpload;

void BUploadInit(uint32_t file_type) {
  Web.upload_file_type = file_type;
  BUpload.spi_hex_size = 0;
  BUpload.spi_sector_counter = FlashWriteStartSector();
  BUpload.spi_sector_cursor = 0;
  BUpload.active = true;
  BUpload.ready = false;
}

uint32_t BUploadWriteBuffer(uint8_t *buf, size_t size) {
  if (0 == BUpload.spi_sector_cursor) { // Starting a new sector write so we need to erase it first
    if (!ESP.flashEraseSector(BUpload.spi_sector_counter)) {
      return 7;  // Upload aborted - flash failed
    }
  }
  BUpload.spi_sector_cursor++;
  if (!ESP.flashWrite((BUpload.spi_sector_counter * SPI_FLASH_SEC_SIZE) + ((BUpload.spi_sector_cursor -1) * HTTP_UPLOAD_BUFLEN), (uint32_t*)buf, size)) {
    return 7;  // Upload aborted - flash failed
  }
  BUpload.spi_hex_size += size;
  if (2 == BUpload.spi_sector_cursor) {  // The web upload sends 2048 bytes at a time so keep track of the cursor position to reset it for the next flash sector erase
    BUpload.spi_sector_cursor = 0;
    BUpload.spi_sector_counter++;
    if (BUpload.spi_sector_counter > FlashWriteMaxSector()) {
      return 9;  // File too large - Not enough free space
    }
  }
  return 0;
}

#endif  // USE_WEB_FW_UPGRADE

void HandleUpgradeFirmware(void) {
  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_FIRMWARE_UPGRADE));

  WSContentStart_P(PSTR(D_FIRMWARE_UPGRADE));
  WSContentSendStyle();
  WSContentSend_P(HTTP_FORM_UPG, SettingsText(SET_OTAURL));
  WSContentSend_P(HTTP_FORM_RST_UPG, PSTR(D_UPGRADE));
  WSContentSpaceButton(BUTTON_MAIN);
  WSContentStop();

  Web.upload_file_type = UPL_TASMOTA;
}

void HandleUpgradeFirmwareStart(void) {
  char command[TOPSZ + 10];  // OtaUrl

  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_UPGRADE_STARTED));
  WifiConfigCounter();

  char otaurl[TOPSZ];
  WebGetArg(PSTR("o"), otaurl, sizeof(otaurl));
  if (strlen(otaurl)) {
    snprintf_P(command, sizeof(command), PSTR(D_CMND_OTAURL " %s"), otaurl);
    ExecuteWebCommand(command);
  }

  WSContentStart_P(PSTR(D_INFORMATION));
  WSContentSend_P(HTTP_SCRIPT_RELOAD_TIME, HTTP_OTA_RESTART_RECONNECT_TIME);
  WSContentSendStyle();
  WSContentSend_P(PSTR("<div style='text-align:center;'><b>" D_UPGRADE_STARTED " ...</b></div>"));
  WSContentSend_P(HTTP_MSG_RSTRT);
  WSContentSpaceButton(BUTTON_MAIN);
  WSContentStop();

  snprintf_P(command, sizeof(command), PSTR(D_CMND_UPGRADE " 1"));
  ExecuteWebCommand(command);
}

void HandleUploadDone(void) {
#if defined(USE_ZIGBEE_EZSP)
  if ((UPL_EFR32 == Web.upload_file_type) && !Web.upload_error && BUpload.ready) {
    BUpload.ready = false;  //  Make sure not to follow thru again
    // GUI xmodem
    ZigbeeUploadStep1Done(FlashWriteStartSector(), BUpload.spi_hex_size);
    HandleZigbeeXfer();
    return;
  }
#endif  // USE_ZIGBEE_EZSP

  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_UPLOAD_DONE));

  WifiConfigCounter();
  UploadServices(1);

  WSContentStart_P(PSTR(D_INFORMATION));
  if (!Web.upload_error) {
    WSContentSend_P(HTTP_SCRIPT_RELOAD_TIME, (UPL_TASMOTA == Web.upload_file_type) ? HTTP_OTA_RESTART_RECONNECT_TIME : HTTP_RESTART_RECONNECT_TIME);  // Refesh main web ui after OTA upgrade
  }
  WSContentSendStyle();
  WSContentSend_P(PSTR("<div style='text-align:center;'><b>" D_UPLOAD " <font color='#"));
  if (Web.upload_error) {
    WSContentSend_P(PSTR("%06x'>" D_FAILED "</font></b><br><br>"), WebColor(COL_TEXT_WARNING));
    char error[100];
    if (Web.upload_error < 10) {
      GetTextIndexed(error, sizeof(error), Web.upload_error -1, kUploadErrors);
    } else {
      snprintf_P(error, sizeof(error), PSTR(D_UPLOAD_ERROR_CODE " %d"), Web.upload_error);
    }
    WSContentSend_P(error);
    DEBUG_CORE_LOG(PSTR("UPL: %s"), error);
    TasmotaGlobal.stop_flash_rotate = Settings->flag.stop_flash_rotate;  // SetOption12 - Switch between dynamic or fixed slot flash save location
    Web.upload_error = 0;
  } else {
    WSContentSend_P(PSTR("%06x'>" D_SUCCESSFUL "</font></b><br>"), WebColor(COL_TEXT_SUCCESS));
    TasmotaGlobal.restart_flag = 2;  // Always restart to re-enable disabled features during update
    WSContentSend_P(HTTP_MSG_RSTRT);
    ShowWebSource(SRC_WEBGUI);
  }
  SettingsBufferFree();
  WSContentSend_P(PSTR("</div><br>"));
  WSContentSpaceButton(BUTTON_MAIN);
  WSContentStop();
}

#ifdef USE_BLE_ESP32
  // declare the fn
  int ExtStopBLE();
#endif

void UploadServices(uint32_t start_service) {
  if (Web.upload_services_stopped != start_service) { return; }
  Web.upload_services_stopped = !start_service;

  if (start_service) {
//    AddLog(LOG_LEVEL_DEBUG, PSTR("UPL: Services enabled"));

/*
    MqttRetryCounter(0);
*/
#ifdef USE_ARILUX_RF
    AriluxRfInit();
#endif  // USE_ARILUX_RF
#ifdef USE_COUNTER
    CounterInterruptDisable(false);
#endif  // USE_COUNTER
#ifdef USE_EMULATION
    UdpConnect();
#endif  // USE_EMULATION

  } else {
//    AddLog(LOG_LEVEL_DEBUG, PSTR("UPL: Services disabled"));

#ifdef USE_BLE_ESP32
    ExtStopBLE();
#endif
#ifdef USE_EMULATION
    UdpDisconnect();
#endif  // USE_EMULATION
#ifdef USE_COUNTER
    CounterInterruptDisable(true);     // Prevent OTA failures on 100Hz counter interrupts
#endif  // USE_COUNTER
#ifdef USE_ARILUX_RF
    AriluxRfDisable();                 // Prevent restart exception on Arilux Interrupt routine
#endif  // USE_ARILUX_RF
/*
    MqttRetryCounter(60);
    if (Settings->flag.mqtt_enabled) {  // SetOption3 - Enable MQTT
      MqttDisconnect();
    }
*/
  }
}

void HandleUploadLoop(void) {
  // Based on ESP8266HTTPUpdateServer.cpp uses ESP8266WebServer Parsing.cpp and Cores Updater.cpp (Update)
  static uint32_t upload_size;
  static bool upload_error_signalled;

  if (HTTP_USER == Web.state) { return; }

  if (Web.upload_error) {
    if (!upload_error_signalled) {
      if (UPL_TASMOTA == Web.upload_file_type) { Update.end(); }
      UploadServices(1);

//      AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_UPLOAD "Upload error %d"), Web.upload_error);

      upload_error_signalled = true;
    }
    return;
  }

  HTTPUpload& upload = Webserver->upload();

  // ***** Step1: Start upload file
  if (UPLOAD_FILE_START == upload.status) {
    Web.upload_error = 0;
    upload_error_signalled = false;
    upload_size = 0;

    UploadServices(0);

    if (0 == upload.filename.c_str()[0]) {
      Web.upload_error = 1;  // No file selected
      return;
    }
    SettingsSave(1);  // Free flash for upload

    AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_UPLOAD D_FILE " %s"), upload.filename.c_str());

    if (UPL_SETTINGS == Web.upload_file_type) {
      if (!SettingsBufferAlloc()) {
        Web.upload_error = 2;  // Not enough space
        return;
      }
    }
#ifdef USE_UFILESYS
    else if (UPL_UFSFILE == Web.upload_file_type) {
      if (!UfsUploadFileOpen(upload.filename.c_str())) {
        Web.upload_error = 2;
        return;
      }
    }
#endif  // USE_UFILESYS
  }

  // ***** Step2: Write upload file
  else if (UPLOAD_FILE_WRITE == upload.status) {
    if (0 == upload.totalSize) {  // First block received
      if (UPL_SETTINGS == Web.upload_file_type) {
        Web.config_block_count = 0;
      }
#ifdef USE_WEB_FW_UPGRADE
#ifdef USE_RF_FLASH
      else if ((SONOFF_BRIDGE == TasmotaGlobal.module_type) && (':' == upload.buf[0])) {  // Check if this is a RF bridge FW file
        BUploadInit(UPL_EFM8BB1);
      }
#endif  // USE_RF_FLASH
#ifdef USE_TASMOTA_CLIENT
      else if (TasmotaClient_Available() && (':' == upload.buf[0])) {  // Check if this is a ARDUINO CLIENT hex file
        BUploadInit(UPL_TASMOTACLIENT);
      }
#endif  // USE_TASMOTA_CLIENT
#ifdef SHELLY_FW_UPGRADE
      else if (ShdPresent() && (0x00 == upload.buf[0]) && ((0x10 == upload.buf[1]) || (0x20 == upload.buf[1]))) {
        BUploadInit(UPL_SHD);
      }
#endif  // SHELLY_FW_UPGRADE
#ifdef USE_CCLOADER
      else if (CCLChipFound() && 0x02 == upload.buf[0]) { // the 0x02 is only an assumption!!
        BUploadInit(UPL_CCL);
      }
#endif  // USE_CCLOADER
#ifdef USE_ZIGBEE_EZSP
#ifdef ESP8266
      else if ((SONOFF_ZB_BRIDGE == TasmotaGlobal.module_type) && (0xEB == upload.buf[0])) {  // Check if this is a Zigbee bridge FW file
#endif  // ESP8266
#ifdef ESP32
      else if (PinUsed(GPIO_ZIGBEE_RX) && PinUsed(GPIO_ZIGBEE_TX) && (0xEB == upload.buf[0])) {  // Check if this is a Zigbee bridge FW file
#endif  // ESP32
        // Read complete file into ESP8266 flash
        // Current files are about 200k
        Web.upload_error = ZigbeeUploadStep1Init();  // 1
        if (Web.upload_error != 0) { return; }
        BUploadInit(UPL_EFR32);
      }
#endif  // USE_ZIGBEE_EZSP
#endif  // USE_WEB_FW_UPGRADE
      else if (UPL_TASMOTA == Web.upload_file_type) {
        if ((upload.buf[0] != 0xE9) && (upload.buf[0] != 0x1F)) {  // 0x1F is gzipped 0xE9
          Web.upload_error = 3;      // Invalid file signature - Magic byte is not 0xE9
          return;
        }
        if (0xE9 == upload.buf[0]) {
          uint32_t bin_flash_size = ESP.magicFlashChipSize((upload.buf[3] & 0xf0) >> 4);
          if (bin_flash_size > ESP.getFlashChipRealSize()) {
            Web.upload_error = 4;  // Program flash size is larger than real flash size
            return;
          }
  //            upload.buf[2] = 3;  // Force DOUT - ESP8285
        }
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) {         //start with max available size
          Web.upload_error = 2;  // Not enough space
          return;
        }
      }
      AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_UPLOAD "File type %d"), Web.upload_file_type);
    }  // First block received

    if (UPL_SETTINGS == Web.upload_file_type) {
      if (upload.currentSize > (sizeof(TSettings) - (Web.config_block_count * HTTP_UPLOAD_BUFLEN))) {
        Web.upload_error = 9;  // File too large
        return;
      }
      memcpy(settings_buffer + (Web.config_block_count * HTTP_UPLOAD_BUFLEN), upload.buf, upload.currentSize);
      Web.config_block_count++;
    }
#ifdef USE_UFILESYS
    else if (UPL_UFSFILE == Web.upload_file_type) {
      if (!UfsUploadFileWrite(upload.buf, upload.currentSize)) {
        Web.upload_error = 9;  // File too large
        return;
      }
    }
#endif  // USE_UFILESYS
#ifdef USE_WEB_FW_UPGRADE
    else if (BUpload.active) {
      // Write a block
//      AddLog(LOG_LEVEL_DEBUG, PSTR("DBG: Size %d"), upload.currentSize);
//      AddLogBuffer(LOG_LEVEL_DEBUG, upload.buf, 32);
      Web.upload_error = BUploadWriteBuffer(upload.buf, upload.currentSize);
      if (Web.upload_error != 0) { return; }
    }
#endif  // USE_WEB_FW_UPGRADE
    else if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Web.upload_error = 5;  // Upload buffer miscompare
      return;
    }
    if (upload.totalSize && !(upload.totalSize % 102400)) {
      AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_UPLOAD "Progress %d kB"), upload.totalSize / 1024);
    }
  }

  // ***** Step3: Finish upload file
  else if (UPLOAD_FILE_END == upload.status) {
    UploadServices(1);
    if (UPL_SETTINGS == Web.upload_file_type) {
      if (!SettingsConfigRestore()) {
        Web.upload_error = 8;  // File invalid
        return;
      }
    }
#ifdef USE_UFILESYS
    else if (UPL_UFSFILE == Web.upload_file_type) {
      UfsUploadFileClose();
    }
#endif  // USE_UFILESYS
#ifdef USE_WEB_FW_UPGRADE
    else if (BUpload.active) {
      // Done writing the data to SPI flash
      BUpload.active = false;

      AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_UPLOAD "Transfer %u bytes"), upload.totalSize);

      uint8_t* data = FlashDirectAccess();

//      uint32_t* values = (uint32_t*)(data);  // Only 4-byte access allowed
//      AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_UPLOAD "Head 0x%08X"), values[0]);

      uint32_t error = 0;
#ifdef USE_RF_FLASH
      if (UPL_EFM8BB1 == Web.upload_file_type) {
        error = SnfBrUpdateFirmware(data, BUpload.spi_hex_size);
      }
#endif  // USE_RF_FLASH
#ifdef USE_TASMOTA_CLIENT
      if (UPL_TASMOTACLIENT == Web.upload_file_type) {
        error = TasmotaClient_Flash(data, BUpload.spi_hex_size);
      }
#endif  // USE_TASMOTA_CLIENT
#ifdef SHELLY_FW_UPGRADE
      if (UPL_SHD == Web.upload_file_type) {
        error = ShdFlash(data, BUpload.spi_hex_size);
      }
#endif  // SHELLY_FW_UPGRADE
#ifdef USE_CCLOADER
      if (UPL_CCL == Web.upload_file_type) {
        error = CLLFlashFirmware(data, BUpload.spi_hex_size);
      }
#endif  // SHELLY_FW_UPGRADE
#ifdef USE_ZIGBEE_EZSP
      if (UPL_EFR32 == Web.upload_file_type) {
        BUpload.ready = true;  // So we know on upload success page if it needs to flash hex or do a normal restart
      }
#endif  // USE_ZIGBEE_EZSP
      if (error != 0) {
//        AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_UPLOAD "Transfer error %d"), error);
        Web.upload_error = error + (100 * (Web.upload_file_type -1));  // Add offset to discriminate transfer errors
        return;
      }
    }
#endif  // USE_WEB_FW_UPGRADE
    else if (!Update.end(true)) { // true to set the size to the current progress
      Web.upload_error = 6;  // Upload failed. Enable logging 3
      return;
    }
    AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_UPLOAD D_SUCCESSFUL " %u bytes"), upload.totalSize);
  }

  // ***** Step4: Abort upload file
  else {
    UploadServices(1);
    Web.upload_error = 7;  // Upload aborted
    if (UPL_TASMOTA == Web.upload_file_type) { Update.end(); }
  }
  // do actually wait a little to allow ESP32 tasks to tick
  // fixes task timeout in ESP32Solo1 style unicore code.
  delay(10);
  OsWatchLoop();
//  Scheduler();          // Feed OsWatch timer to prevent restart on long uploads
}

/*-------------------------------------------------------------------------------------------*/

void HandlePreflightRequest(void)
{
  HttpHeaderCors();
  Webserver->sendHeader(F("Access-Control-Allow-Methods"), F("GET, POST"));
  Webserver->sendHeader(F("Access-Control-Allow-Headers"), F("authorization"));
  WSSend(200, CT_HTML, "");
}

/*-------------------------------------------------------------------------------------------*/

void HandleHttpCommand(void)
{
  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_COMMAND));

  WSContentBegin(200, CT_APP_JSON);
  String svalue = Webserver->arg(F("cmnd"));
  if (svalue.length() && (svalue.length() < MQTT_MAX_PACKET_SIZE)) {
    uint32_t curridx = TasmotaGlobal.log_buffer_pointer;
    TasmotaGlobal.templog_level = LOG_LEVEL_INFO;
    ExecuteWebCommand((char*)svalue.c_str(), SRC_WEBCOMMAND);
    WSContentSend_P(PSTR("{"));
    bool cflg = false;
    uint32_t index = curridx;
    char* line;
    size_t len;
    while (GetLog(TasmotaGlobal.templog_level, &index, &line, &len)) {
      // [14:49:36.123 MQTT: stat/wemos5/RESULT = {"POWER":"OFF"}] > [{"POWER":"OFF"}]
      char* JSON = (char*)memchr(line, '{', len);
      if (JSON) {  // Is it a JSON message (and not only [15:26:08 MQT: stat/wemos5/POWER = O])
        if (cflg) { WSContentSend_P(PSTR(",")); }
        uint32_t JSONlen = len - (JSON - line) -3;
        WSContentSend(JSON +1, JSONlen);
        cflg = true;
      }
    }
    WSContentSend_P(PSTR("}"));
    TasmotaGlobal.templog_level = 0;
  } else {
    WSContentSend_P(PSTR("{\"" D_RSLT_WARNING "\":\"" D_ENTER_COMMAND " cmnd=\"}"));
  }
  WSContentEnd();
}

/********************************************************************************************/

void HandleNotFound(void)
{
//  AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP "Not found (%s)"), Webserver->uri().c_str());
#ifndef NO_CAPTIVE_PORTAL
  if (CaptivePortal()) { return; }  // If captive portal redirect instead of displaying the error page.
#endif  // NO_CAPTIVE_PORTAL

#ifdef USE_EMULATION
#ifdef USE_EMULATION_HUE
  String path = Webserver->uri();
  if ((EMUL_HUE == Settings->flag2.emulation) && (path.startsWith(F("/api")))) {
    HandleHueApi(&path);
  } else
#endif  // USE_EMULATION_HUE
#endif  // USE_EMULATION
  {
    WSContentBegin(404, CT_PLAIN);
    WSContentSend_P(PSTR(D_FILE_NOT_FOUND "\n\nURI: %s\nMethod: %s\nArguments: %d\n"), Webserver->uri().c_str(), (Webserver->method() == HTTP_GET) ? PSTR("GET") : PSTR("POST"), Webserver->args());
    for (uint32_t i = 0; i < Webserver->args(); i++) {
      WSContentSend_P(PSTR(" %s: %s\n"), Webserver->argName(i).c_str(), Webserver->arg(i).c_str());
    }
    WSContentEnd();
  }
}

#ifndef NO_CAPTIVE_PORTAL
/* Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
bool CaptivePortal(void)
{
  // Possible hostHeader: connectivitycheck.gstatic.com or 192.168.4.1
  if ((WifiIsInManagerMode()) && !ValidIpAddress(Webserver->hostHeader().c_str())) {
    AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_HTTP D_REDIRECTED));

    Webserver->sendHeader(F("Location"), String(F("http://")) + Webserver->client().localIP().toString(), true);
    WSSend(302, CT_PLAIN, "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
    Webserver->client().stop();  // Stop is needed because we sent no content length
    return true;
  }
  return false;
}
#endif  // NO_CAPTIVE_PORTAL

/*********************************************************************************************/

void HandleDeviceInfo(void) {
  WSContentBegin(200, CT_APP_JSON);
  WSContentSend_P(PSTR("{\"message\":\"Success\", \"data\":{\"nickname\":\"%s\", \"mac\":\"%s\", \"type\":\"%s\"}}"), SettingsText(SET_FRIENDLYNAME1), WiFi.macAddress().c_str(), DEVICE_TYPE);
  WSContentEnd();
}

void HandleCertsInfo(void) {
  if ((strlen(AmazonClientCert) == 0) || strlen(AmazonPrivateKey) == 0) {
    WSContentBegin(500, CT_APP_JSON);
    WSContentSend_P(PSTR("{\"message\":\"Fail\"}"));
    WSContentEnd();
    return;
  }

  WSContentBegin(200, CT_APP_JSON);
  WSContentSend_P(PSTR("{\"message\":\"Success\", \"data\":{\"cert\":\"%s\", \"key\":\"%s\"}}"), AmazonClientCert, AmazonPrivateKey);
  WSContentEnd();
}

void HandleCertsConfiguration(void) {
  if(!Webserver->hasArg(F("plain"))) {
    WSContentBegin(500, CT_APP_JSON);
    WSContentSend_P(PSTR("{\"message\":\"Fail\"}"));
    WSContentEnd();
    return;
  }

  JsonParser parser((char*) Webserver->arg("plain").c_str());
  JsonParserObject stateObject = parser.getRootObject();
  String cert = stateObject["cert"].getStr();
  String key = stateObject["key"].getStr();
  char* certCharType = (char*)cert.c_str();
  char* keyCharType = (char*)key.c_str();

/* TODO: 인증서 사이즈 체크 예외코드 작성
  if(cert.length() != 256 || key.length() < 10) {
    WSContentBegin(500, CT_APP_JSON);
    WSContentSend_P(PSTR("{\"message\":\"Fail\"}"));
    WSContentEnd();
    return;
  }
*/
  memcpy(AmazonClientCert, certCharType, strlen(certCharType));
  memcpy(AmazonPrivateKey, keyCharType, strlen(keyCharType));
  MqttDisconnect();
  ConvertTlsFile(0);
  ConvertTlsFile(1);

  WSContentBegin(200, CT_APP_JSON);
  WSContentSend_P(PSTR("{\"message\":\"Success\"}"));
  WSContentEnd();
}

int WebSend(char *buffer)
{
  // [tasmota] POWER1 ON                                               --> Sends http://tasmota/cm?cmnd=POWER1 ON
  // [192.168.178.86:80,admin:joker] POWER1 ON                        --> Sends http://hostname:80/cm?user=admin&password=joker&cmnd=POWER1 ON
  // [tasmota] /any/link/starting/with/a/slash.php?log=123             --> Sends http://tasmota/any/link/starting/with/a/slash.php?log=123
  // [tasmota,admin:joker] /any/link/starting/with/a/slash.php?log=123 --> Sends http://tasmota/any/link/starting/with/a/slash.php?log=123

  char *host;
  char *user;
  char *password;
  char *command;
  int status = 1;                             // Wrong parameters

                                              // buffer = |  [  192.168.178.86  :  80  ,  admin  :  joker  ]    POWER1 ON   |
  host = strtok_r(buffer, "]", &command);     // host = |  [  192.168.178.86  :  80  ,  admin  :  joker  |, command = |    POWER1 ON   |
  if (host && command) {
    RemoveSpace(host);                        // host = |[192.168.178.86:80,admin:joker|
    host++;                                   // host = |192.168.178.86:80,admin:joker| - Skip [
    host = strtok_r(host, ",", &user);        // host = |192.168.178.86:80|, user = |admin:joker|
    String url = F("http://");                // url = |http://|
    url += host;                              // url = |http://192.168.178.86:80|

    command = Trim(command);                  // command = |POWER1 ON| or |/any/link/starting/with/a/slash.php?log=123|
    if (command[0] != '/') {
      url += F("/cm?");                       // url = |http://192.168.178.86/cm?|
      if (user) {
        user = strtok_r(user, ":", &password);  // user = |admin|, password = |joker|
        if (user && password) {
          char userpass[200];
          snprintf_P(userpass, sizeof(userpass), PSTR("user=%s&password=%s&"), user, password);
          url += userpass;                    // url = |http://192.168.178.86/cm?user=admin&password=joker&|
        }
      }
      url += F("cmnd=");                      // url = |http://192.168.178.86/cm?cmnd=| or |http://192.168.178.86/cm?user=admin&password=joker&cmnd=|
    }
    url += command;                           // url = |http://192.168.178.86/cm?cmnd=POWER1 ON|

    DEBUG_CORE_LOG(PSTR("WEB: Uri |%s|"), url.c_str());

    WiFiClient http_client;
    HTTPClient http;
    if (http.begin(http_client, UrlEncode(url))) {  // UrlEncode(url) = |http://192.168.178.86/cm?cmnd=POWER1%20ON|
      int http_code = http.GET();             // Start connection and send HTTP header
      if (http_code > 0) {                    // http_code will be negative on error
        if (http_code == HTTP_CODE_OK || http_code == HTTP_CODE_MOVED_PERMANENTLY) {
#ifdef USE_WEBSEND_RESPONSE
          // Return received data to the user - Adds 900+ bytes to the code
          const char* read = http.getString().c_str();  // File found at server - may need lot of ram or trigger out of memory!
          ResponseClear();
          char text[2] = { 0 };
          text[0] = '.';
          while (text[0] != '\0') {
            text[0] = *read++;
            if (text[0] > 31) {               // Remove control characters like linefeed
              if (ResponseAppend_P(text) == ResponseSize()) { break; };
            }
          }
#ifdef USE_SCRIPT
          extern uint8_t tasm_cmd_activ;
          // recursive call must be possible in this case
          tasm_cmd_activ = 0;
#endif  // USE_SCRIPT
          MqttPublishPrefixTopicRulesProcess_P(RESULT_OR_STAT, PSTR(D_CMND_WEBSEND));
#endif  // USE_WEBSEND_RESPONSE
        }
        status = 0;                           // No error - Done
      } else {
        status = 2;                           // Connection failed
      }
      http.end();                             // Clean up connection data
    } else {
      status = 3;                             // Host not found or connection error
    }
  }
  return status;
}

bool JsonWebColor(const char* dataBuf)
{
  // Default (Dark theme)
  // {"WebColor":["#eaeaea","#252525","#4f4f4f","#000","#ddd","#65c115","#1f1f1f","#ff5661","#008000","#faffff","#1fa3ec","#0e70a4","#d43535","#931f1f","#47c266","#5aaf6f","#faffff","#999","#eaeaea"]}
  // Default pre v7 (Light theme)
  // {"WebColor":["#000","#fff","#f2f2f2","#000","#fff","#000","#fff","#f00","#008000","#fff","#1fa3ec","#0e70a4","#d43535","#931f1f","#47c266","#5aaf6f","#fff","#999","#000"]}	  // {"WebColor":["#000000","#ffffff","#f2f2f2","#000000","#ffffff","#000000","#ffffff","#ff0000","#008000","#ffffff","#1fa3ec","#0e70a4","#d43535","#931f1f","#47c266","#5aaf6f","#ffffff","#999999","#000000"]}

  JsonParser parser((char*) dataBuf);
  JsonParserObject root = parser.getRootObject();
  JsonParserArray arr = root[PSTR(D_CMND_WEBCOLOR)].getArray();
  if (arr) {  // if arr is valid, i.e. json is valid, the key D_CMND_WEBCOLOR was found and the token is an arra
    uint32_t i = 0;
    for (auto color : arr) {
      if (i < COL_LAST) {
        WebHexCode(i, color.getStr());
      } else {
        break;
      }
      i++;
    }
  }
  return true;
}

const char kWebSendStatus[] PROGMEM = D_JSON_DONE "|" D_JSON_WRONG_PARAMETERS "|" D_JSON_CONNECT_FAILED "|" D_JSON_HOST_NOT_FOUND "|" D_JSON_MEMORY_ERROR;

const char kWebCommands[] PROGMEM = "|"  // No prefix
#ifdef USE_EMULATION
  D_CMND_EMULATION "|"
#endif
#if defined(USE_SENDMAIL) || defined(USE_ESP32MAIL)
  D_CMND_SENDMAIL "|"
#endif
  D_CMND_WEBSERVER "|" D_CMND_WEBREFRESH "|" D_CMND_WEBSEND "|"
  D_CMND_WEBSENSOR "|" D_CMND_WEBBUTTON "|" D_CMND_CORS;

void (* const WebCommand[])(void) PROGMEM = {
#ifdef USE_EMULATION
  &CmndEmulation,
#endif
#if defined(USE_SENDMAIL) || defined(USE_ESP32MAIL)
  &CmndSendmail,
#endif
  &CmndWebServer, &CmndWebRefresh, &CmndWebSend,
  &CmndWebSensor, &CmndWebButton, &CmndCors };

/*********************************************************************************************\
 * Commands
\*********************************************************************************************/

#ifdef USE_EMULATION
void CmndEmulation(void)
{
#if defined(USE_EMULATION_WEMO) || defined(USE_EMULATION_HUE)
#if defined(USE_EMULATION_WEMO) && defined(USE_EMULATION_HUE)
  if ((XdrvMailbox.payload >= EMUL_NONE) && (XdrvMailbox.payload < EMUL_MAX)) {
#else
#ifndef USE_EMULATION_WEMO
  if ((EMUL_NONE == XdrvMailbox.payload) || (EMUL_HUE == XdrvMailbox.payload)) {
#endif
#ifndef USE_EMULATION_HUE
  if ((EMUL_NONE == XdrvMailbox.payload) || (EMUL_WEMO == XdrvMailbox.payload)) {
#endif
#endif
    Settings->flag2.emulation = XdrvMailbox.payload;
    TasmotaGlobal.restart_flag = 2;
  }
#endif
  ResponseCmndNumber(Settings->flag2.emulation);
}
#endif  // USE_EMULATION

#if defined(USE_SENDMAIL) || defined(USE_ESP32MAIL)
void CmndSendmail(void)
{
  if (XdrvMailbox.data_len > 0) {
    uint8_t result = SendMail(XdrvMailbox.data);
    char stemp1[20];
    ResponseCmndChar(GetTextIndexed(stemp1, sizeof(stemp1), result, kWebSendStatus));
  }
}
#endif  // USE_SENDMAIL


void CmndWebServer(void)
{
  if ((XdrvMailbox.payload >= 0) && (XdrvMailbox.payload <= 2)) {
    Settings->webserver = XdrvMailbox.payload;
  }
  if (Settings->webserver) {
    Response_P(PSTR("{\"" D_CMND_WEBSERVER "\":\"" D_JSON_ACTIVE_FOR " %s " D_JSON_ON_DEVICE " %s " D_JSON_WITH_IP_ADDRESS " %_I\"}"),
      (2 == Settings->webserver) ? PSTR(D_ADMIN) : PSTR(D_USER), NetworkHostname(), (uint32_t)NetworkAddress());
  } else {
    ResponseCmndStateText(0);
  }
}

void CmndWebRefresh(void)
{
  if ((XdrvMailbox.payload > 999) && (XdrvMailbox.payload <= 65000)) {
    Settings->web_refresh = XdrvMailbox.payload;
  }
  ResponseCmndNumber(Settings->web_refresh);
}

void CmndWebSend(void)
{
  if (XdrvMailbox.data_len > 0) {
    uint32_t result = WebSend(XdrvMailbox.data);
    char stemp1[20];
    ResponseCmndChar(GetTextIndexed(stemp1, sizeof(stemp1), result, kWebSendStatus));
  }
}

void CmndWebSensor(void)
{
  if (XdrvMailbox.index < MAX_XSNS_DRIVERS) {
    if (XdrvMailbox.payload >= 0) {
      bitWrite(Settings->sensors[XdrvMailbox.index / 32], XdrvMailbox.index % 32, XdrvMailbox.payload &1);
    }
  }
  Response_P(PSTR("{\"" D_CMND_WEBSENSOR "\":"));
  XsnsSensorState();
  ResponseJsonEnd();
}

void CmndWebButton(void)
{
  if ((XdrvMailbox.index > 0) && (XdrvMailbox.index <= MAX_BUTTON_TEXT)) {
    if (!XdrvMailbox.usridx) {
      ResponseCmndAll(SET_BUTTON1, MAX_BUTTON_TEXT);
    } else {
      if (XdrvMailbox.data_len > 0) {
        SettingsUpdateText(SET_BUTTON1 + XdrvMailbox.index -1, ('"' == XdrvMailbox.data[0]) ? "" : XdrvMailbox.data);
      }
      ResponseCmndIdxChar(SettingsText(SET_BUTTON1 + XdrvMailbox.index -1));
    }
  }
}

void CmndCors(void)
{
  if (XdrvMailbox.data_len > 0) {
    SettingsUpdateText(SET_CORS, (SC_CLEAR == Shortcut()) ? "" : (SC_DEFAULT == Shortcut()) ? CORS_DOMAIN : XdrvMailbox.data);
  }
  ResponseCmndChar(SettingsText(SET_CORS));
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xdrv01(uint8_t function)
{
  bool result = false;

  switch (function) {
    case FUNC_LOOP:
      PollDnsWebserver();
#ifdef USE_EMULATION
      if (Settings->flag2.emulation) { PollUdp(); }
#endif  // USE_EMULATION
      break;
    case FUNC_EVERY_SECOND:
      if (Web.initial_config) {
        Wifi.config_counter = 200;    // Do not restart the device if it has SSId Blank
      }
      if (Web.wifi_test_counter) {
        Web.wifi_test_counter--;
        AddLog(LOG_LEVEL_DEBUG, PSTR(D_LOG_WIFI D_TRYING_TO_CONNECT " %s"), SettingsText(SET_STASSID1));
        if ( WifiCheck_hasIP(WiFi.localIP()) ) {  // Got IP - Connection Established
          Web.wifi_test_counter = 0;
          Web.wifiTest = WIFI_TEST_FINISHED_SUCCESSFUL;
          AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_WIFI D_CMND_SSID "1 %s: " D_CONNECTED " - " D_IP_ADDRESS " %_I"), SettingsText(SET_STASSID1), (uint32_t)WiFi.localIP());
//          TasmotaGlobal.blinks = 255;                    // Signal wifi connection with blinks
          if (MAX_WIFI_OPTION != Web.old_wificonfig) {
            TasmotaGlobal.wifi_state_flag = Settings->sta_config = Web.old_wificonfig;
          }
          TasmotaGlobal.save_data_counter = Web.save_data_counter;
          Settings->save_data = Web.save_data_counter;
          SettingsSaveAll();
#if (!RESTART_AFTER_INITIAL_WIFI_CONFIG)
          Web.initial_config = false;
          Web.state = HTTP_ADMIN;
#endif
        } else if (!Web.wifi_test_counter) { // Test TimeOut
          Web.wifi_test_counter = 0;
          Web.wifiTest = WIFI_TEST_FINISHED_BAD;
          switch (WiFi.status()) {
            case WL_CONNECTED:
              AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_WIFI D_CONNECT_FAILED_NO_IP_ADDRESS));
              break;
            case WL_NO_SSID_AVAIL:
              AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_WIFI D_CONNECT_FAILED_AP_NOT_REACHED));
              break;
            case WL_CONNECT_FAILED:
              AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_WIFI D_CONNECT_FAILED_WRONG_PASSWORD));
              break;
            default:  // WL_IDLE_STATUS and WL_DISCONNECTED
              AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_WIFI D_CONNECT_FAILED_AP_TIMEOUT));
          }
          int n = WiFi.scanNetworks(); // restart scan
        }
      }
      break;
    case FUNC_COMMAND:
      result = DecodeCommand(kWebCommands, WebCommand);
      break;
  }
  return result;
}
#endif  // USE_WEBSERVER
