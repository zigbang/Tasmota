#include "FS.h"

struct WEBSECURE {
  bool state_HTTPS = false;
  bool state_login = false;
} WebSecure;

void StartWebserverSecure(void)
{
  if (!WebSecure.state_HTTPS) {
    if (!WebserverSecure) {
      WebserverSecure = new ESP8266WebServerSecure(443);
      WebserverSecure->getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
      // WebserverSecure->getServer().setBufferSizes(1024, 1024);
      WebserverSecure->getServer().setBufferSizes(2048, 1024);
      WebserverSecure->on(F("/config"), HTTP_POST, HandleConfigurationWithApp);
    }

    WebserverSecure->begin(); // Web server start
  }
  if (!WebSecure.state_HTTPS) {
    AddLog(LOG_LEVEL_INFO, PSTR(D_LOG_HTTP "HTTPS" D_WEBSERVER_ACTIVE_ON " %s%s " D_WITH_IP_ADDRESS " %_I"),
      NetworkHostname(), (Mdns.begun) ? PSTR(".local") : "", (uint32_t)WiFi.localIP());
    WebSecure.state_HTTPS = true;
  }
}

// TODO: 메모리 회수 코드 추가
void StopWebserverSecure(void)
{
  if (WebSecure.state_HTTPS) {
    WebserverSecure->close();
    WebSecure.state_HTTPS = false;
    AddLog(LOG_LEVEL_INFO, PSTR("HTTPS 웹서버 종료"));
  }
}

/*********************************************************************************************/

void HttpHeaderCorsSecure(void)
{
  if (strlen(SettingsText(SET_CORS))) {
    WebserverSecure->sendHeader(F("Access-Control-Allow-Origin"), SettingsText(SET_CORS));
  }
}

void WSHeaderSendSecure(void)
{
  char server[32];
  // TODO: ZIoT 버전 변경
  snprintf_P(server, sizeof(server), PSTR("Tasmota/%s (%s)"), TasmotaGlobal.version, GetDeviceHardware().c_str());
  WebserverSecure->sendHeader(F("Server"), server);
  WebserverSecure->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
  WebserverSecure->sendHeader(F("Pragma"), F("no-cache"));
  WebserverSecure->sendHeader(F("Expires"), F("-1"));
  HttpHeaderCorsSecure();
}

/**********************************************************************************************
* HTTP Content Page handler
**********************************************************************************************/

void WSSendSecure(int code, int ctype, const String& content)
{
  char ct[25];  // strlen("application/octet-stream") +1 = Longest Content type string
  WebserverSecure->send(code, GetTextIndexed(ct, sizeof(ct), ctype, kContentTypes), content);
}

/**********************************************************************************************
* HTTP Content Chunk handler
**********************************************************************************************/

void WSContentBeginSecure(int code, int ctype) {
  WebserverSecure->client().flush();
  WSHeaderSendSecure();
  WebserverSecure->setContentLength(CONTENT_LENGTH_UNKNOWN);
  WSSendSecure(code, ctype, "");                         // Signal start of chunked content
  Web.chunk_buffer = "";
}

void _WSContentSendSecure(const char* content, size_t size) {  // Lowest level sendContent for all core versions
  WebserverSecure->sendContent(content, size);

#ifdef USE_DEBUG_DRIVER
  ShowFreeMem(PSTR("WSContentSend"));
#endif
  DEBUG_CORE_LOG(PSTR("WEB: Chunk size %d"), size);
}

void _WSContentSendSecure(const String& content) {       // Low level sendContent for all core versions
  _WSContentSendSecure(content.c_str(), content.length());
}

void WSContentFlushSecure(void) {
  if (Web.chunk_buffer.length() > 0) {
    _WSContentSendSecure(Web.chunk_buffer);              // Flush chunk buffer
    Web.chunk_buffer = "";
  }
}

void WSContentSendSecure(const char* content, size_t size) {
  WSContentFlushSecure();
  _WSContentSendSecure(content, size);
}

void _WSContentSendBufferSecure(bool decimal, const char * formatP, va_list arg) {
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
    WSContentFlushSecure();                              // Send chunk buffer before possible content oversize
  }
  if (strlen(content) >= CHUNKED_BUFFER_SIZE) {    // Content is oversize
    _WSContentSendSecure(content);                       // Send content
  }

  free(content);
}

void WSContentSend_PSecure(const char* formatP, ...) {   // Content send snprintf_P char data
  // This uses char strings. Be aware of sending %% if % is needed
  va_list arg;
  va_start(arg, formatP);
  _WSContentSendBufferSecure(false, formatP, arg);
  va_end(arg);
}

void WSContentSend_PDSecure(const char* formatP, ...) {  // Content send snprintf_P char data checked for decimal separator
  // This uses char strings. Be aware of sending %% if % is needed
  va_list arg;
  va_start(arg, formatP);
  _WSContentSendBufferSecure(true, formatP, arg);
  va_end(arg);
}

void WSContentStart_PSecure(const char* title, bool auth)
{
  WSContentBeginSecure(200, CT_HTML);

  if (title != nullptr) {
    WSContentSend_PSecure(HTTP_HEADER1, PSTR(D_HTML_LANGUAGE), SettingsText(SET_DEVICENAME), title);
  }
}

void WSContentStart_PSecure(const char* title)
{
  WSContentStart_PSecure(title, true);
}

void WSContentSendStyle_PSecure(const char* formatP, ...)
{
  if ( WifiIsInManagerMode() && (!Web.initial_config) ) {
    if (WifiConfigCounter()) {
      WSContentSend_PSecure(HTTP_SCRIPT_COUNTER);
    }
  }
  //WSContentSend_P(HTTP_SCRIPT_LOADER);
  WSContentSend_PSecure(HTTP_HEAD_LAST_SCRIPT);

  WSContentSend_PSecure(HTTP_HEAD_STYLE1, WebColor(COL_FORM), WebColor(COL_INPUT), WebColor(COL_INPUT_TEXT), WebColor(COL_INPUT),
                  WebColor(COL_INPUT_TEXT), PSTR(""), PSTR(""), WebColor(COL_BACKGROUND));
  WSContentSend_PSecure(HTTP_HEAD_STYLE2, WebColor(COL_BUTTON), WebColor(COL_BUTTON_TEXT), WebColor(COL_BUTTON_HOVER),
                  WebColor(COL_BUTTON_RESET), WebColor(COL_BUTTON_RESET_HOVER), WebColor(COL_BUTTON_SAVE), WebColor(COL_BUTTON_SAVE_HOVER),
                  WebColor(COL_BUTTON));
#ifdef USE_ZIGBEE
  WSContentSend_PSecure(HTTP_HEAD_STYLE_ZIGBEE);
#endif // USE_ZIGBEE
  if (formatP != nullptr) {
    // This uses char strings. Be aware of sending %% if % is needed
    va_list arg;
    va_start(arg, formatP);
    _WSContentSendBufferSecure(false, formatP, arg);
    va_end(arg);
  }
  WSContentSend_PSecure(HTTP_HEAD_STYLE_LOADER);
  WSContentSend_PSecure(HTTP_HEAD_STYLE3, WebColor(COL_TEXT),
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
    WSContentSend_PSecure(PSTR("<h4>%s%s (%s%s%s)</h4>"),    // tasmota.local (192.168.2.12, 192.168.4.1)
      NetworkHostname(),
      (Mdns.begun) ? PSTR(".local") : "",
      (lip) ? WiFi.localIP().toString().c_str() : "",
      (lip && sip) ? ", " : "",
      (sip) ? WiFi.softAPIP().toString().c_str() : "");
  }
  WSContentSend_PSecure(PSTR("</div>"));
}

void WSContentSendStyleSecure(void)
{
  WSContentSendStyle_PSecure(nullptr);
}

void WSContentTextCenterStartSecure(uint32_t color) {
  WSContentSend_PSecure(PSTR("<div style='text-align:center;color:#%06x;'>"), color);
}

void WSContentButtonSecure(uint32_t title_index, bool show=true)
{
  char action[4];
  char title[100];  // Large to accomodate UTF-16 as used by Russian

  WSContentSend_PSecure(PSTR("<p><form id=but%d style=\"display: %s;\" action='%s' method='get'"),
    title_index,
    show ? "block":"none",
    GetTextIndexed(action, sizeof(action), title_index, kButtonAction));
  if (title_index <= BUTTON_FACTORY_RESET) {
    char confirm[100];
    WSContentSend_PSecure(PSTR(" onsubmit='return confirm(\"%s\");'><button name='%s' class='button bred'>%s</button></form></p>"),
      GetTextIndexed(confirm, sizeof(confirm), title_index, kButtonConfirm),
      (!title_index) ? PSTR("rst") : PSTR("non"),
      GetTextIndexed(title, sizeof(title), title_index, kButtonTitle));
  } else {
    WSContentSend_PSecure(PSTR("><button>%s</button></form></p>"),
      GetTextIndexed(title, sizeof(title), title_index, kButtonTitle));
  }
}

void WSContentSpaceButtonSecure(uint32_t title_index, bool show=true)
{
  WSContentSend_PSecure(PSTR("<div id=but%dd style=\"display: %s;\"></div>"),title_index, show ? "block":"none");            // 5px padding
  WSContentButtonSecure(title_index, show);
}

void WSContentSend_TempSecure(const char *types, float f_temperature) {
  WSContentSend_PDSecure(HTTP_SNS_F_TEMP, types, Settings->flag2.temperature_resolution, &f_temperature, TempUnit());
}

void WSContentSend_VoltageSecure(const char *types, float f_voltage) {
  WSContentSend_PDSecure(HTTP_SNS_F_VOLTAGE, types, Settings->flag2.voltage_resolution, &f_voltage);
}

void WSContentSend_CurrentMASecure(const char *types, float f_current) {
  WSContentSend_PDSecure(HTTP_SNS_F_CURRENT_MA, types, Settings->flag2.current_resolution, &f_current);
}

void WSContentSend_THDSecure(const char *types, float f_temperature, float f_humidity)
{
  WSContentSend_TempSecure(types, f_temperature);

  char parameter[FLOATSZ];
  dtostrfd(f_humidity, Settings->flag2.humidity_resolution, parameter);
  WSContentSend_PDSecure(HTTP_SNS_HUM, types, parameter);
  dtostrfd(CalcTempHumToDew(f_temperature, f_humidity), Settings->flag2.temperature_resolution, parameter);
  WSContentSend_PDSecure(HTTP_SNS_DEW, types, parameter, TempUnit());
}

void WSContentEndSecure(void)
{
  WSContentSendSecure("", 0);                            // Signal end of chunked content
  WebserverSecure->client().stop();
}

void WSContentStopSecure(void)
{
  if ( WifiIsInManagerMode() && (!Web.initial_config) ) {
    if (WifiConfigCounter()) {
      WSContentSend_PSecure(HTTP_COUNTER);
    }
  }
  // TODO: ZIoT 버전 변경
  WSContentSend_PSecure(HTTP_END, WiFi.macAddress().c_str(), TasmotaGlobal.version);
  WSContentEndSecure();
}

/*********************************************************************************************/

void HandleConfigurationWithApp(void) {
  bool save_result = false;
  if(!WebserverSecure->hasArg(F("plain"))) {
    WSContentBeginSecure(500, CT_APP_JSON);
    WSContentSend_PSecure(PSTR("{\"message\":\"Failed\" \"resason\":\"1\" \"data\":\"Server received empty request message\"}"));
    WSContentEndSecure();
    return;
  }

  JsonParser parser((char*) WebserverSecure->arg("plain").c_str());
  JsonParserObject stateObject = parser.getRootObject();

  String idToken = stateObject["idToken"].getStr();
  String ssid = stateObject["ssid1"].getStr();
  String pwd = stateObject["pwd1"].getStr();

  if (idToken.length()) {
    char* temp = (char*)idToken.c_str();
    save_result = TfsSaveFile("/idToken.txt", (uint8_t*)temp, 1024);
  }

  if (!save_result || !ssid.length() || !pwd.length()) {
    WSContentBeginSecure(400, CT_APP_JSON);
    WSContentSend_PSecure(PSTR("{\"message\":\"Failed\" \"resason\":\"2\" \"data\":\"Check token, ssid, and pwd\"}"));
    WSContentEndSecure();
    return;
  } else {
    SettingsUpdateText(SET_ID_TOKEN, "TRUE");
    SettingsUpdateText(SET_STASSID1, (char*)ssid.c_str());
    SettingsUpdateText(SET_STAPWD1, (char*)pwd.c_str());
  }

  WSContentBeginSecure(200, CT_APP_JSON);
  WSContentSend_PSecure(PSTR("{\"message\":\"Success\"}"));
  WSContentEndSecure();

  TasmotaGlobal.restart_flag = 2;
}