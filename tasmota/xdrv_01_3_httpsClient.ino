#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

WiFiClientSecure client;
uint16_t provisioning_counter = 0;

extern const br_ec_private_key *AWS_IoT_Private_Key;

void HTTPSClientInit(void) {
    if (strlen(SettingsText(SET_ID_TOKEN)) && strlen(SettingsText(SET_STASSID1)) && strlen(SettingsText(SET_STAPWD1))) {
#ifdef ESP8266
        client.setFingerprint(AWS_FINGERPRINT);
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
    const char host[] = API_HOST;
    String url = LAMBDA_CERT_URL + String(SettingsText(SET_MQTT_TOPIC));

    if ((nullptr == AWS_IoT_Private_Key) || !client.connect(host, 443)) {
        AddLog(LOG_LEVEL_INFO, PSTR("%s에 연결 실패"), host);
    } else {
        AddLog(LOG_LEVEL_INFO, PSTR("%s에 연결 성공"), host);

        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "User-Agent: Zigbang\r\n" +
                    "Authorization: " + (char*)AWS_IoT_Private_Key->x +"\r\n" +
                    "Connection: close\r\n\r\n");

        String headers = "";
        String body = "";
        bool finishedHeaders = false;
        bool currentLineIsBlank = true;
        bool gotResponse = false;

        unsigned long timeout = millis();
        while (!client.available()) {
            if (millis() - timeout > 20000) {
                printf("시간초과!\n");
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
                printf("body : %s", (char*) body.c_str());
#ifdef ESP32
                char* temp = trimRight((char*) body.c_str());
                JsonParser parser(temp);
#elif ESP8266
                JsonParser parser((char*) body.c_str());
#endif
                JsonParserObject stateObject = parser.getRootObject();

                String cert = stateObject["cert"].getStr();
                String key = stateObject["key"].getStr();
                if (!cert.length() || !key.length()) {
                    AddLog(LOG_LEVEL_INFO, PSTR("Cert 정보 Error"));
                    client.stop();
                    return;
                }

                char* certCharType = (char*)cert.c_str();
                char* keyCharType = (char*)key.c_str();

                memcpy(AmazonClientCert, certCharType, strlen(certCharType));
                memcpy(AmazonPrivateKey, keyCharType, strlen(keyCharType));

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