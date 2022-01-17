#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char fingerprint[] PROGMEM = "e8 c3 8d d8 41 15 1b 6d a9 9c 26 36 29 30 b2 14 f7 71 0d 1c";

WiFiClientSecure client;
uint16_t provisioning_counter = 0;

void HTTPSClientInit(void) {
    client.setFingerprint(fingerprint);
}

void GetCertification(void) {
    const char host[] = "p2x2wtwvsf.execute-api.ap-northeast-2.amazonaws.com";
    String url = "/dev/certification?idToken=" + String(SettingsText(SET_ID_TOKEN)) + "&deviceType=light" + "&macAddr=" + NetworkUniqueId();

    if (!client.connect(host, 443)) {
        AddLog(LOG_LEVEL_INFO, PSTR("%s에 HTTPS 연결 실패"), host);
    } else {
        AddLog(LOG_LEVEL_INFO, PSTR("%s에 HTTPS 연결 성공"), host);

        printf("url : %s\n", url.c_str());
        

        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "User-Agent: ZIoTTasmota\r\n" +
                    "Connection: close\r\n\r\n");
        
        printf("Reqeust sent\n");

        String headers = "";
        String body = "";
        bool finishedHeaders = false;
        bool currentLineIsBlank = true;
        bool gotResponse = false;

        unsigned long timeout = millis();
        while (!client.available()) {
            if (millis() - timeout > 20000) {
                printf("Client Timeout !");
                client.stop();
                provisioning_counter = 0;
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
                AddLog(LOG_LEVEL_INFO, PSTR("request 성공"));
                printf("body: %s\n", body.c_str());
                JsonParser parser((char*) body.c_str());
                JsonParserObject stateObject = parser.getRootObject();

                String cert = stateObject["cert"].getStr();
                String key = stateObject["key"].getStr();

                if (!cert.length() || !key.length()) {
                    AddLog(LOG_LEVEL_INFO, PSTR("Cert 및 Key 정보 Error"));
                    client.stop();
                    provisioning_counter = 0;
                    return;
                }

                SettingsUpdateText(SET_ID_TOKEN, "");
                TasmotaGlobal.idToken_info_flag = 0;
                char* certCharType = (char*)cert.c_str();
                char* keyCharType = (char*)key.c_str();

                memcpy(AmazonClientCert, certCharType, strlen(certCharType));
                memcpy(AmazonPrivateKey, keyCharType, strlen(keyCharType));
                printf("cert: %s\n", certCharType);
                printf("key: %s\n", keyCharType);

                url.~String();
                headers.~String();
                body.~String();
                cert.~String();
                key.~String();

                ConvertTlsFile(0);
                ConvertTlsFile(1);
                TasmotaGlobal.cert_info_flag = 1;
                TasmotaGlobal.restart_flag = 2;
            } else {
                AddLog(LOG_LEVEL_INFO, PSTR("request 실패"));
            }

            client.stop();
            provisioning_counter = 0;
            return;
        }
    }
}

void ProvisioningCheck(void) {
    if (!TasmotaGlobal.idToken_info_flag && TasmotaGlobal.cert_info_flag) {
        provisioning_counter = 0;
        return;
    } else {
        if (provisioning_counter) {
            provisioning_counter--;
        } else {
            provisioning_counter = 1000;
            GetCertification();
        }
    }
}