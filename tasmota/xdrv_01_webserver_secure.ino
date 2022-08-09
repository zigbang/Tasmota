#include "FS.h"
#ifdef ESP8266
#include <WiFiClientSecure.h>
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