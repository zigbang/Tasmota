#include "HTTPUpdate.h"

extern "C" uint32_t _FS_start;
extern "C" uint32_t _FS_end;

HTTPUpdateResult HTTPUpdate::update(WiFiClient &client, const String &url, const String &currentVersion)
{
    HTTPClient http;
    http.begin(client, url);
    return handleUpdate(http, currentVersion, false);
}

HTTPUpdateResult HTTPUpdate::handleUpdate(HTTPClient &http, const String &currentVersion, bool spiffs)
{

    HTTPUpdateResult ret = HTTP_UPDATE_FAILED;

    // use HTTP/1.0 for update since the update handler not support any transfer Encoding
    http.useHTTP10(true);
    http.setTimeout(8000);
    http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
    http.setUserAgent(F("ESP8266-http-Update"));
    http.addHeader(F("x-ESP8266-Chip-ID"), String(ESP.getChipId()));
    http.addHeader(F("x-ESP8266-STA-MAC"), WiFi.macAddress());
    http.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
    http.addHeader(F("x-ESP8266-free-space"), String(ESP.getFreeSketchSpace()));
    http.addHeader(F("x-ESP8266-sketch-size"), String(ESP.getSketchSize()));
    http.addHeader(F("x-ESP8266-sketch-md5"), String(ESP.getSketchMD5()));
    http.addHeader(F("x-ESP8266-chip-size"), String(ESP.getFlashChipRealSize()));
    http.addHeader(F("x-ESP8266-sdk-version"), ESP.getSdkVersion());
    http.addHeader("Accept", "application/octet-stream");

    if (spiffs)
    {
        http.addHeader(F("x-ESP8266-mode"), F("spiffs"));
    }
    else
    {
        http.addHeader(F("x-ESP8266-mode"), F("sketch"));
    }

    if (currentVersion && currentVersion[0] != 0x00)
    {
        http.addHeader(F("x-ESP8266-version"), currentVersion);
    }

    const char *headerkeys[] = {"x-MD5"};
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);

    // track these headers
    http.collectHeaders(headerkeys, headerkeyssize);

    int code = http.GET();
    int len = http.getSize();

    if (code <= 0)
    {
        DEBUG_HTTP_UPDATE("[httpUpdate] HTTP error: %s\n", http.errorToString(code).c_str());
        _setLastError(code);
        http.end();
        return HTTP_UPDATE_FAILED;
    }

    DEBUG_HTTP_UPDATE("[httpUpdate] Header read fin.\n");
    DEBUG_HTTP_UPDATE("[httpUpdate] Server header:\n");
    DEBUG_HTTP_UPDATE("[httpUpdate]  - code: %d\n", code);
    DEBUG_HTTP_UPDATE("[httpUpdate]  - len: %d\n", len);

    if (http.hasHeader("x-MD5"))
    {
        DEBUG_HTTP_UPDATE("[httpUpdate]  - MD5: %s\n", http.header("x-MD5").c_str());
    }

    DEBUG_HTTP_UPDATE("[httpUpdate] ESP8266 info:\n");
    DEBUG_HTTP_UPDATE("[httpUpdate]  - free Space: %d\n", ESP.getFreeSketchSpace());
    DEBUG_HTTP_UPDATE("[httpUpdate]  - current Sketch Size: %d\n", ESP.getSketchSize());

    if (currentVersion && currentVersion[0] != 0x00)
    {
        DEBUG_HTTP_UPDATE("[httpUpdate]  - current version: %s\n", currentVersion.c_str());
    }

    switch (code)
    {
    case HTTP_CODE_OK: ///< OK (Start Update)
        if (len > 0)
        {
            bool startUpdate = true;
            if (spiffs)
            {
                size_t spiffsSize = ((size_t)&_FS_end - (size_t)&_FS_start);
                if (len > (int)spiffsSize)
                {
                    DEBUG_HTTP_UPDATE("[httpUpdate] spiffsSize to low (%d) needed: %d\n", spiffsSize, len);
                    startUpdate = false;
                }
            }
            else
            {
                if (len > (int)ESP.getFreeSketchSpace())
                {
                    DEBUG_HTTP_UPDATE("[httpUpdate] FreeSketchSpace to low (%d) needed: %d\n", ESP.getFreeSketchSpace(), len);
                    startUpdate = false;
                }
            }

            if (!startUpdate)
            {
                _setLastError(HTTP_UE_TOO_LESS_SPACE);
                ret = HTTP_UPDATE_FAILED;
            }
            else
            {
                // Warn main app we're starting up...
                if (_cbStart)
                {
                    _cbStart();
                }

                WiFiClient *tcp = http.getStreamPtr();

                if (_closeConnectionsOnUpdate)
                {
                    WiFiUDP::stopAll();
                    WiFiClient::stopAllExcept(tcp);
                }

                delay(100);

                int command;

                if (spiffs)
                {
                    command = U_FS;
                    DEBUG_HTTP_UPDATE("[httpUpdate] runUpdate filesystem...\n");
                }
                else
                {
                    command = U_FLASH;
                    DEBUG_HTTP_UPDATE("[httpUpdate] runUpdate flash...\n");
                }

                if (!spiffs)
                {
                    uint8_t buf[4];
                    if (tcp->peekBytes(&buf[0], 4) != 4)
                    {
                        DEBUG_HTTP_UPDATE("[httpUpdate] peekBytes magic header failed\n");
                        _setLastError(HTTP_UE_BIN_VERIFY_HEADER_FAILED);
                        http.end();
                        return HTTP_UPDATE_FAILED;
                    }

                    // check for valid first magic byte
                    if (buf[0] != 0xE9 && buf[0] != 0x1f)
                    {
                        DEBUG_HTTP_UPDATE("[httpUpdate] Magic header does not start with 0xE9\n");
                        _setLastError(HTTP_UE_BIN_VERIFY_HEADER_FAILED);
                        http.end();
                        return HTTP_UPDATE_FAILED;
                    }

                    if (buf[0] == 0xe9)
                    {
                        uint32_t bin_flash_size = ESP.magicFlashChipSize((buf[3] & 0xf0) >> 4);
                        printf("binary size : %d\n", bin_flash_size);
                        printf("flash chip real size : %d\n", ESP.getFlashChipRealSize());

                        // check if new bin fits to SPI flash
                        if (bin_flash_size > ESP.getFlashChipRealSize())
                        {
                            DEBUG_HTTP_UPDATE("[httpUpdate] New binary does not fit SPI Flash size\n");
                            _setLastError(HTTP_UE_BIN_FOR_WRONG_FLASH);
                            http.end();
                            return HTTP_UPDATE_FAILED;
                        }
                    }
                }
                if (runUpdate(*tcp, len, http.header("x-MD5"), command))
                {
                    ret = HTTP_UPDATE_OK;
                    DEBUG_HTTP_UPDATE("[httpUpdate] Update ok\n");
                    http.end();
                    // Warn main app we're all done
                    if (_cbEnd)
                    {
                        _cbEnd();
                    }

#ifdef ATOMIC_FS_UPDATE
                    if (_rebootOnUpdate)
                    {
#else
                    if (_rebootOnUpdate && !spiffs)
                    {
#endif
                        ESP.restart();
                    }
                }
                else
                {
                    ret = HTTP_UPDATE_FAILED;
                    DEBUG_HTTP_UPDATE("[httpUpdate] Update failed\n");
                }
            }
        }
        else
        {
            _setLastError(HTTP_UE_SERVER_NOT_REPORT_SIZE);
            ret = HTTP_UPDATE_FAILED;
            DEBUG_HTTP_UPDATE("[httpUpdate] Content-Length was 0 or wasn't set by Server?!\n");
        }
        break;
    case HTTP_CODE_NOT_MODIFIED:
        ///< Not Modified (No updates)
        ret = HTTP_UPDATE_NO_UPDATES;
        break;
    case HTTP_CODE_NOT_FOUND:
        _setLastError(HTTP_UE_SERVER_FILE_NOT_FOUND);
        ret = HTTP_UPDATE_FAILED;
        break;
    case HTTP_CODE_FORBIDDEN:
        _setLastError(HTTP_UE_SERVER_FORBIDDEN);
        ret = HTTP_UPDATE_FAILED;
        break;
    default:
        _setLastError(HTTP_UE_SERVER_WRONG_HTTP_CODE);
        ret = HTTP_UPDATE_FAILED;
        DEBUG_HTTP_UPDATE("[httpUpdate] HTTP Code is (%d)\n", code);
        // http.writeToStream(&Serial1);
        break;
    }

    http.end();
    return ret;
}

HTTPUpdate httpUpdate;