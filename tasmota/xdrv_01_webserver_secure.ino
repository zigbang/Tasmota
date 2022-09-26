#include "FS.h"
#ifdef ESP8266
#include <WiFiClientSecure.h>
#endif

struct WEBSECURE {
  bool state_HTTPS = false;
  bool state_login = false;
} WebSecure;

bool SaveAccessToken(char* accessToken) {
  uint8_t *spi_buffer = (uint8_t*)malloc(tls_spi_len);
  if (!spi_buffer)
  {
    printf("[ZIoT] Can't allocate memory to spi_buffer!\n");
    return false;
  }

#ifdef ESP8266
    memcpy_P(spi_buffer, tls_spi_start, tls_spi_len);
    uint16_t startAddress = 1000;
    memcpy(spi_buffer + tls_obj_store_offset + startAddress, accessToken, strlen(accessToken) + 1);
    if (ESP.flashEraseSector(tls_spi_start_sector))
    {
        ESP.flashWrite(tls_spi_start_sector * SPI_FLASH_SEC_SIZE, (uint32_t *)spi_buffer, SPI_FLASH_SEC_SIZE);
    }
#elif ESP32
    if (ziot_spi_start != nullptr)
    {
        memcpy_P(spi_buffer, ziot_spi_start, strlen(accessToken));
    }
    else
    {
        memset(spi_buffer, 0, strlen(accessToken));
    }

    memcpy(spi_buffer, accessToken, strlen(accessToken));
    OsalSaveNvm("/access_token", spi_buffer, strlen(accessToken));
#endif
    free(spi_buffer);
    return true;
}

void loadAccessToken(void)
{
#ifdef ESP8266
    tls_dir_t tls_dir_2;
    uint16_t startAddress;
    // if (strcmp(SettingsText(SET_ENTRY2_START), "") != 0)
    // {
        startAddress = 1000;

        memcpy_P(&tls_dir_2, (uint8_t *)0x402FF000 + 0x0400, sizeof(tls_dir_2));
        char *data = (char *)(tls_spi_start + tls_obj_store_offset + startAddress);
        strncpy(TasmotaGlobal.ziot_access_token, data, 311);
    // }
#elif ESP32
    if (ziot_spi_start == nullptr)
    {
        ziot_spi_start = (uint8_t *)malloc(311);
        if (ziot_spi_start == nullptr)
        {
            printf("[ZIoT] free memory is not enough");
            return;
        }
    }

    if (!OsalLoadNvm("/access_token", ziot_spi_start, 311))
    {
        printf("[ZIoT] access_token file doesn't exist");
    }
    else
    {
        memcpy_P(TasmotaGlobal.ziot_access_token, ziot_spi_start, 311);
        printf("Access Token : %s\n", TasmotaGlobal.ziot_access_token);
    }
#endif
    return;
}