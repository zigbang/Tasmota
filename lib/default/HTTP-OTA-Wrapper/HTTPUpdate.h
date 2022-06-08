#ifndef HTTP_UPDATE_H_
#define HTTP_UPDATE_H_

#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>

class HTTPUpdate : public ESP8266HTTPUpdate
{
public:
    HTTPUpdate(void) : ESP8266HTTPUpdate(){};
    HTTPUpdate(int httpClientTimeout) : ESP8266HTTPUpdate(httpClientTimeout){};
    ~HTTPUpdate(void){};

    t_httpUpdate_return update(WiFiClient &client, const String &url, const String &currentVersion = "");
    t_httpUpdate_return handleUpdate(HTTPClient &http, const String &currentVersion, bool spiffs = false);

private:
    int _httpClientTimeout;
    followRedirects_t _followRedirects;

    // Callbacks
    HTTPUpdateStartCB    _cbStart;
    HTTPUpdateEndCB      _cbEnd;
    HTTPUpdateErrorCB    _cbError;
    HTTPUpdateProgressCB _cbProgress;

    int _ledPin;
    uint8_t _ledOn;
};

extern HTTPUpdate httpUpdate;

#endif /* HTTP_UPDATE_H_ */