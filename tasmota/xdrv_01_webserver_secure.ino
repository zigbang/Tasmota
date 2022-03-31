#include "FS.h"
#ifdef ESP8266
#include <WiFiClientSecure.h>
#endif
#ifdef ESP32
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
using namespace httpsserver;
SSLCert cert = SSLCert(
    serverCert, serverCert_len, serverKey, serverKey_len);
#endif

extern const br_ec_private_key *AWS_IoT_Private_Key;

class tok_entry_t {
public:
  uint32_t name;    // simple 4 letters name. Currently 'skey', 'crt ', 'crt1', 'crt2'
  uint16_t start;   // start offset
  uint16_t len;     // len of object
};                  // 8 bytes

const static uint32_t ACCESS_TOKEN = 0x206B6F74;

class tok_dir_t {
public:
  tok_entry_t entry[4];     // 4 entries max, only 4 used today, for future use
};                          // 4*8 = 64 bytes

tok_dir_t tok_dir;          // memory copy of tls_dir from flash

struct WEBSECURE {
  bool state_HTTPS = false;
  bool state_login = false;
} WebSecure;

#ifdef ESP32
void HandleConfigurationWithApp(HTTPRequest * req, HTTPResponse * res);
#endif

void StartWebserverSecure(void)
{
  if (!WebSecure.state_HTTPS) {
    if (!WebserverSecure) {
#ifdef ESP8266
      WebserverSecure = new ESP8266WebServerSecure(443);
      WebserverSecure->getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
      // WebserverSecure->getServer().setBufferSizes(1024, 1024);
      WebserverSecure->getServer().setBufferSizes(2048, 1024);
      WebserverSecure->on(F("/config"), HTTP_POST, HandleConfigurationWithApp);
#elif ESP32
      WebserverSecure = new HTTPSServer(&cert);
      ResourceNode *nodeHandleConfigurationWithApp = new ResourceNode("/config", "POST", &HandleConfigurationWithApp);
      WebserverSecure->registerNode(nodeHandleConfigurationWithApp);
#endif
    }
#ifdef ESP8266
    WebserverSecure->begin(); // Web server start
#elif ESP32
    WebserverSecure->start();
#endif
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
#ifdef ESP8266
    WebserverSecure->close();
#elif ESP32
    WebserverSecure->stop();
#endif
    WebSecure.state_HTTPS = false;
    AddLog(LOG_LEVEL_INFO, PSTR("HTTPS 웹서버 종료"));
  }
}

/*********************************************************************************************/

#ifdef ESP8266
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
#endif

/*********************************************************************************************/

#ifdef ESP8266
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
    save_result = SaveAccessToken((char*)idToken.c_str());
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
#elif ESP32
void HandleConfigurationWithApp(HTTPRequest * req, HTTPResponse * res) {
    bool save_result = false;
    byte buffer[1024];

    while (!req->requestComplete()) {
      req->readBytes(buffer, 1024);
    }

    res->setHeader("Content-Type", "application/json");

    JsonParser parser((char*)buffer);
    JsonParserObject stateObject = parser.getRootObject();

    if (stateObject.size() == 0) {
      req->discardRequestBody();
      res->printStd("{\"message\":\"Failed\", \"resason\":\"1\", \"data\":\"Server received empty request message\"}");
      return;
    }

    String idToken = stateObject["idToken"].getStr();
    String ssid = stateObject["ssid1"].getStr();
    String pwd = stateObject["pwd1"].getStr();

    if (idToken.length()) {
      save_result = SaveAccessToken((char*)idToken.c_str());
    }

    if (!save_result || !ssid.length() || !pwd.length()) {
      req->discardRequestBody();
      res->printStd("{\"message\":\"Failed\", \"resason\":\"2\", \"data\":\"Check token, ssid, and pwd\"}");
      return;
    } else {
      SettingsUpdateText(SET_ID_TOKEN, "TRUE");
      SettingsUpdateText(SET_STASSID1, (char*)ssid.c_str());
      SettingsUpdateText(SET_STAPWD1, (char*)pwd.c_str());
    }

    res->printStd("{\"message\":\"Success\"}");
    TasmotaGlobal.restart_flag = 2;
}
#endif

#ifdef ESP32
static uint8_t * tok_spi_start = nullptr;
const static size_t   tok_spi_len      = 0x0400;  // 1kb blocs
const static size_t   tok_block_offset = 0x0000;  // don't need offset in FS
#elif ESP8266
// const static uint16_t tls_spi_start_sector = EEPROM_LOCATION + 4;  // 0xXXFF
// const static uint8_t* tls_spi_start    = (uint8_t*) ((tls_spi_start_sector * SPI_FLASH_SEC_SIZE) + 0x40200000);  // 0x40XFF000
const static uint16_t tok_spi_start_sector = 0xFF;  // Force last bank of first MB
const static uint8_t* tok_spi_start    = (uint8_t*) 0x402FF000;  // 0x402FF000
const static size_t   tok_spi_len      = 0x1000;  // 4kb blocs
const static size_t   tok_block_offset = 0x0400;
#endif
const static size_t   tok_block_len    = 0x0400;   // 1kb
const static size_t   tok_obj_store_offset = tok_block_offset + sizeof(tok_dir_t);

inline void TokEraseBuffer(uint8_t *buffer) {
  memset(buffer + tok_block_offset, 0xFF, tok_block_len);
}

// static data structures for Private Key and Certificate, only the pointer
// to binary data will change to a region in SPI Flash
static br_ec_private_key TOKEN = {
	23,
	nullptr, 0
};

void loadAccessToken(void) {
#ifdef ESP32
  // We load the file in RAM and use it as if it was in Flash. The buffer is never deallocated once we loaded TLS keys
  AWS_IoT_Private_Key = nullptr;

  if (tok_spi_start == nullptr){
      tok_spi_start = (uint8_t*) malloc(tok_block_len);
      if (tok_spi_start == nullptr) {
        return;
      }
  }

  if (!OsalLoadNvm(TASM_FILE_TLSKEY, tok_spi_start, tok_block_len)) {
    free(tok_spi_start);
    return;
  }
#endif
  memcpy_P(&tok_dir, tok_spi_start + tok_block_offset, sizeof(tok_dir));

  // calculate the addresses for Key and Cert in Flash
  if ((ACCESS_TOKEN == tok_dir.entry[0].name) && (tok_dir.entry[0].len > 0)) {
    TOKEN.x = (unsigned char *)(tok_spi_start + tok_obj_store_offset + tok_dir.entry[0].start);
    TOKEN.xlen = tok_dir.entry[0].len;
    AWS_IoT_Private_Key = &TOKEN;
  } else {
    AWS_IoT_Private_Key = nullptr;
  }
}

bool SaveAccessToken(char* accessToken) {
  tok_dir_t *tok_dir_write;

  uint8_t *spi_buffer = (uint8_t*) malloc(tok_spi_len);
  if (!spi_buffer) {
    return false;
  }
  if (tok_spi_start != nullptr) {  // safeguard for ESP32
    memcpy_P(spi_buffer, tok_spi_start, tok_spi_len);
  } else {
    memset(spi_buffer, 0, tok_spi_len);   // safeguard for ESP32, removed by compiler for ESP8266
  }

  uint32_t bin_len = strlen(accessToken) + 1;
  
  uint8_t  *bin_buf = nullptr;
  if (bin_len > 0) {
    bin_buf = (uint8_t*) malloc(bin_len + 4);
    if (!bin_buf) {
      free(spi_buffer);
      return false;
    }
  }

  if (bin_len > 0) {
    memcpy(bin_buf, accessToken, bin_len);
    bin_buf[bin_len - 1] = '\0';
  }

  // address of writable tls_dir in buffer
  tok_dir_write = (tok_dir_t*) (spi_buffer + tok_block_offset);

  bool save_file = false;   // for ESP32, do we need to write file

#ifdef ESP32
  if (TfsFileExists(TASM_FILE_TLSKEY)) {
    TfsDeleteFile(TASM_FILE_TLSKEY);  // delete file
  }
#elif ESP8266
  TokEraseBuffer(spi_buffer);   // Erase any previously stored data
#endif

  if (bin_len > 0) {
    // if (bin_len != 32) {
    //   // no private key was previously stored, abort
    //   AddLog(LOG_LEVEL_INFO, PSTR("TLSKey: Certificate must be 32 bytes: %d."), bin_len);
    //   free(spi_buffer);
    //   free(bin_buf);
    //   return false;
    // }
    tok_entry_t *entry = &tok_dir_write->entry[0];
    entry->name = ACCESS_TOKEN;
    entry->start = 0;
    entry->len = bin_len;
    memcpy(spi_buffer + tok_obj_store_offset + entry->start, bin_buf, entry->len);
    save_file = true;
  } else {
    // if lenght is zero, simply erase this SPI flash area
  }

#ifdef ESP32
  if (save_file) {
    OsalSaveNvm(TASM_FILE_TLSKEY, spi_buffer, tok_spi_len);
  }
#elif ESP8266
  if (ESP.flashEraseSector(tok_spi_start_sector)) {
    ESP.flashWrite(tok_spi_start_sector * SPI_FLASH_SEC_SIZE, (uint32_t*) spi_buffer, SPI_FLASH_SEC_SIZE);
  }
#endif

  free(spi_buffer);
  free(bin_buf);

  loadAccessToken();   // reload into memory any potential change
  return true;
}