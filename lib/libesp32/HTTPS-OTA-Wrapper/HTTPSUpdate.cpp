#include "HTTPSUpdate.h"
#include <esp_partition.h>
#include <esp_ota_ops.h> // get running partition

#define log_d(format, ...) printf(format, ##__VA_ARGS__)
#define log_w(format, ...) printf(format, ##__VA_ARGS__)
#define log_e(format, ...) printf(format, ##__VA_ARGS__)
#define log_v(format, ...) printf(format, ##__VA_ARGS__)

namespace HTTPSOTA
{
    String getSketchSHA256()
    {
        const size_t HASH_LEN = 32; // SHA-256 digest length

        uint8_t sha_256[HASH_LEN] = {0};

        // get sha256 digest for running partition
        if (esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256) == 0)
        {
            char buffer[2 * HASH_LEN + 1];

            for (size_t index = 0; index < HASH_LEN; index++)
            {
                uint8_t nibble = (sha_256[index] & 0xf0) >> 4;
                buffer[2 * index] = nibble < 10 ? char(nibble + '0') : char(nibble - 10 + 'A');

                nibble = sha_256[index] & 0x0f;
                buffer[2 * index + 1] = nibble < 10 ? char(nibble + '0') : char(nibble - 10 + 'A');
            }

            buffer[2 * HASH_LEN] = '\0';

            return String(buffer);
        }
        else
        {

            return String();
        }
    }
}

HTTPUpdateResult HTTPSUpdate::update(WiFiClientSecure &client, const String &url, const String &currentVersion, const char *CAcert)
{
    HTTPSClient http;
    if (!http.begin(client, url, CAcert))
    {
        return HTTP_UPDATE_FAILED;
    }
    return handleUpdate(http, currentVersion, false);
}

HTTPUpdateResult HTTPSUpdate::handleUpdate(HTTPClient &http, const String &currentVersion, bool spiffs)
{

    HTTPUpdateResult ret = HTTP_UPDATE_FAILED;

    // use HTTP/1.0 for update since the update handler not support any transfer Encoding
    http.useHTTP10(false);
    http.setTimeout(8000);
    http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
    http.setUserAgent("ESP32-http-Update");
    http.addHeader("Cache-Control", "no-cache");
    http.addHeader("x-ESP32-STA-MAC", WiFi.macAddress());
    http.addHeader("x-ESP32-AP-MAC", WiFi.softAPmacAddress());
    http.addHeader("x-ESP32-free-space", String(ESP.getFreeSketchSpace()));
    http.addHeader("x-ESP32-sketch-size", String(ESP.getSketchSize()));
    String sketchMD5 = ESP.getSketchMD5();
    if (sketchMD5.length() != 0)
    {
        http.addHeader("x-ESP32-sketch-md5", sketchMD5);
    }
    // Add also a SHA256
    String sketchSHA256 = HTTPSOTA::getSketchSHA256();
    if (sketchSHA256.length() != 0)
    {
        http.addHeader("x-ESP32-sketch-sha256", sketchSHA256);
    }
    http.addHeader("x-ESP32-chip-size", String(ESP.getFlashChipSize()));
    http.addHeader("x-ESP32-sdk-version", ESP.getSdkVersion());

    if (spiffs)
    {
        http.addHeader("x-ESP32-mode", "spiffs");
    }
    else
    {
        http.addHeader("x-ESP32-mode", "sketch");
    }

    if (currentVersion && currentVersion[0] != 0x00)
    {
        http.addHeader("x-ESP32-version", currentVersion);
    }

    http.addHeader("Accept", "application/octet-stream");

    const char *headerkeys[] = {"x-MD5"};
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char *);

    // track these headers
    http.collectHeaders(headerkeys, headerkeyssize);

    int code = http.GET();
    int len = http.getSize();

    if (code <= 0)
    {
        log_e("HTTP error: %s\n", http.errorToString(code).c_str());
        _lastError = code;
        http.end();
        return HTTP_UPDATE_FAILED;
    }

    log_d("Header read fin.\n");
    log_d("Server header:\n");
    log_d(" - code: %d\n", code);
    log_d(" - len: %d\n", len);

    if (http.hasHeader("x-MD5"))
    {
        log_d(" - MD5: %s\n", http.header("x-MD5").c_str());
    }

    log_d("ESP32 info:\n");
    log_d(" - free Space: %d\n", ESP.getFreeSketchSpace());
    log_d(" - current Sketch Size: %d\n", ESP.getSketchSize());

    if (currentVersion && currentVersion[0] != 0x00)
    {
        log_d(" - current version: %s\n", currentVersion.c_str());
    }

    switch (code)
    {
    case HTTP_CODE_OK: ///< OK (Start Update)
        if (len > 0)
        {
            bool startUpdate = true;
            if (spiffs)
            {
                const esp_partition_t *_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
                if (!_partition)
                {
                    _lastError = HTTP_UE_NO_PARTITION;
                    return HTTP_UPDATE_FAILED;
                }

                if (len > _partition->size)
                {
                    log_e("spiffsSize to low (%d) needed: %d\n", _partition->size, len);
                    startUpdate = false;
                }
            }
            else
            {
                int sketchFreeSpace = ESP.getFreeSketchSpace();
                if (!sketchFreeSpace)
                {
                    _lastError = HTTP_UE_NO_PARTITION;
                    return HTTP_UPDATE_FAILED;
                }

                if (len > sketchFreeSpace)
                {
                    log_e("FreeSketchSpace to low (%d) needed: %d\n", sketchFreeSpace, len);
                    startUpdate = false;
                }
            }

            if (!startUpdate)
            {
                _lastError = HTTP_UE_TOO_LESS_SPACE;
                ret = HTTP_UPDATE_FAILED;
            }
            else
            {

                WiFiClient *tcp = http.getStreamPtr();

                // To do?                WiFiUDP::stopAll();
                // To do?                WiFiClient::stopAllExcept(tcp);

                delay(100);

                int command;

                if (spiffs)
                {
                    command = U_SPIFFS;
                    log_d("runUpdate spiffs...\n");
                }
                else
                {
                    command = U_FLASH;
                    log_d("runUpdate flash...\n");
                }

                if (!spiffs)
                {
                    /* To do
                                        uint8_t buf[4];
                                        if(tcp->peekBytes(&buf[0], 4) != 4) {
                                            log_e("peekBytes magic header failed\n");
                                            _lastError = HTTP_UE_BIN_VERIFY_HEADER_FAILED;
                                            http.end();
                                            return HTTP_UPDATE_FAILED;
                                        }
                    */

                    // check for valid first magic byte
                    //                    if(buf[0] != 0xE9) {
                    if (tcp->peek() != 0xE9)
                    {
                        log_e("Magic header does not start with 0xE9\n");
                        _lastError = HTTP_UE_BIN_VERIFY_HEADER_FAILED;
                        http.end();
                        return HTTP_UPDATE_FAILED;
                    }
                    /* To do
                                        uint32_t bin_flash_size = ESP.magicFlashChipSize((buf[3] & 0xf0) >> 4);

                                        // check if new bin fits to SPI flash
                                        if(bin_flash_size > ESP.getFlashChipRealSize()) {
                                            log_e("New binary does not fit SPI Flash size\n");
                                            _lastError = HTTP_UE_BIN_FOR_WRONG_FLASH;
                                            http.end();
                                            return HTTP_UPDATE_FAILED;
                                        }
                    */
                }
                if (runUpdate(*tcp, len, http.header("x-MD5"), command))
                {
                    ret = HTTP_UPDATE_OK;
                    log_d("Update ok\n");
                    http.end();

                    if (_rebootOnUpdate && !spiffs)
                    {
                        ESP.restart();
                    }
                }
                else
                {
                    ret = HTTP_UPDATE_FAILED;
                    log_e("Update failed\n");
                }
            }
        }
        else
        {
            _lastError = HTTP_UE_SERVER_NOT_REPORT_SIZE;
            ret = HTTP_UPDATE_FAILED;
            log_e("Content-Length was 0 or wasn't set by Server?!\n");
        }
        break;
    case HTTP_CODE_NOT_MODIFIED:
        ///< Not Modified (No updates)
        ret = HTTP_UPDATE_NO_UPDATES;
        break;
    case HTTP_CODE_NOT_FOUND:
        _lastError = HTTP_UE_SERVER_FILE_NOT_FOUND;
        ret = HTTP_UPDATE_FAILED;
        break;
    case HTTP_CODE_FORBIDDEN:
        _lastError = HTTP_UE_SERVER_FORBIDDEN;
        ret = HTTP_UPDATE_FAILED;
        break;
    default:
        _lastError = HTTP_UE_SERVER_WRONG_HTTP_CODE;
        ret = HTTP_UPDATE_FAILED;
        log_e("HTTP Code is (%d)\n", code);
        break;
    }

    http.end();
    return ret;
}

HTTPSUpdate httpsUpdate;