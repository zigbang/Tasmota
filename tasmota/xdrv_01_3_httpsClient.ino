#ifndef FIRMWARE_ZIOT_MINIMAL
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

WiFiClientSecure client;
uint16_t provisioning_counter = 0;

void HTTPSClientInit(void) {
    if (strlen(SettingsText(SET_ID_TOKEN)) && strlen(SettingsText(SET_STASSID1)) && strlen(SettingsText(SET_STAPWD1))) {
#ifdef ESP8266
        if (strcmp(SettingsText(SET_ENV), "dev") == 0) {
            client.setFingerprint(AWS_FINGERPRINT_DEV);
        }
        else {
            client.setFingerprint(AWS_FINGERPRINT_PROD);
        }
#elif ESP32
        client.setCACert(rootCA1);
#endif
        loadAccessToken();
    }
}

char* trimRight(char* s) {
    int maxSize = 4096;
    char t[maxSize];
    char *end;
    if (maxSize < strlen(s)) {
      return s;
    }
    strcpy(t, s);
    end = t + strlen(t) - 1;
    while (end != t && *end != '}')
    {
        end--;
    }
    *(end + 1) = '\0';
    s = t;
    return s;
}

void GetCertification(void) {
    char* host = (char*)malloc(sizeof(char) * 100);
    String url;

    if (strcmp(SettingsText(SET_ENV), "dev") == 0) {
        strcpy(host, API_HOST_DEV);
        url = LAMBDA_CERT_URL_DEV;
    }
    else {
        strcpy(host, API_HOST_PROD);
        url = LAMBDA_CERT_URL_PROD;
    }

    if (!client.connect(host, 443)) {
        AddLog(LOG_LEVEL_INFO, PSTR("%s에 연결 실패"), host);
    } else {
        AddLog(LOG_LEVEL_INFO, PSTR("%s에 연결 성공"), host);

        String payload = "GET "+ url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "User-Agent: Zigbang\r\n" +
                    "Authorization: " + TasmotaGlobal.ziot_access_token + "\r\n" +
                    "Connection: close\r\n\r\n";

        free(host);
#ifdef ESP8266
        client.write(payload.c_str());
#elif ESP32
        client.write((const uint8_t*)payload.c_str(), payload.length());
#endif  // ESP8266
        payload.~String();

        String headers = "";
        String body = "";
        bool finishedHeaders = false;
        bool currentLineIsBlank = true;
        bool gotResponse = false;

        unsigned long timeout = millis();
        while (!client.available()) {
            if (millis() - timeout > 20000) {
                printf_P(PSTR("시간초과!\n"));
                client.stop();
                return;
            }
        }

        while (client.available()) {
            char c = client.read();

            if (finishedHeaders) {
                body = body + c;
            } else {
                if (currentLineIsBlank && c == '\n') {
                    finishedHeaders = true;
                } else {
                    headers = headers + c;
                }
            }

            if (c == '\n') {
                currentLineIsBlank = true;
            }
            else if (c != '\r') {
                currentLineIsBlank = false;
            }

            gotResponse = true;
        }
        if (gotResponse) {
            if (headers.startsWith("HTTP/1.1 200")) {
                AddLog(LOG_LEVEL_INFO, PSTR("요청 성공"));
#ifdef ESP32
                char* temp = trimRight((char*) body.c_str());
                JsonParser parser(temp);
#elif ESP8266
                JsonParser parser((char*) body.c_str());
#endif
                JsonParserObject stateObject = parser.getRootObject();

                String cert = stateObject["cert"].getStr();
                String key = stateObject["key"].getStr();
                String arn = stateObject["arn"].getStr();
                if (!cert.length() || !key.length() || !arn.length()) {
                    AddLog(LOG_LEVEL_INFO, PSTR("Cert 정보 Error"));
                    client.stop();
                    return;
                }

                char* certCharType = (char*)cert.c_str();
                char* keyCharType = (char*)key.c_str();
                char* arnCharType = (char*)arn.c_str();

                memcpy(AmazonClientCert, certCharType, strlen(certCharType));
                memcpy(AmazonPrivateKey, keyCharType, strlen(keyCharType));

                SettingsUpdateText(SET_CERT_ARN, arnCharType);

                url.~String();
                headers.~String();
                body.~String();
                cert.~String();
                key.~String();

                if (!ConvertTlsFile(0) || !ConvertTlsFile(1)) {
                    AddLog(LOG_LEVEL_INFO, PSTR("Cert 저장 실패"));
                    client.stop();
                    return;
                }

                SettingsUpdateText(SET_ID_TOKEN, "");
                TasmotaGlobal.idToken_info_flag = 0;
                TasmotaGlobal.cert_info_flag = 1;
                TasmotaGlobal.restart_flag = 2;
                AddLog(LOG_LEVEL_INFO, PSTR("Cert 저장 성공"));
            } else {
                AddLog(LOG_LEVEL_INFO, PSTR("요청 실패"));
                client.stop();
                return;
            }

            client.stop();
            provisioning_counter = 0;
            return;
        }
    }
}

void ProvisioningCheck(void) {
    if (!TasmotaGlobal.idToken_info_flag && TasmotaGlobal.cert_info_flag) {
        return;
    } else {
        provisioning_counter++;
        if (provisioning_counter == 5) {
            SettingsUpdateText(SET_STASSID1, "");
            SettingsUpdateText(SET_STAPWD1, "");
            TasmotaGlobal.restart_flag = 2;
        }
        GetCertification();
    }
}
#endif  // FIRMWARE_ZIOT_MINIMAL