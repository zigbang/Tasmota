#ifndef ___HTTPS_UPDATE_H___
#define ___HTTPS_UPDATE_H___

#include <HTTPUpdate.h>
#include "HTTPSClient.h"

class HTTPSUpdate : public HTTPUpdate
{
public:
    HTTPSUpdate(void) : HTTPUpdate(){};
    HTTPSUpdate(int httpClientTimeout) : HTTPUpdate(httpClientTimeout){};
    ~HTTPSUpdate(void){};

    t_httpUpdate_return update(WiFiClientSecure &client, const String &url, const String &currentVersion = "", const char *CAcert = "");
    t_httpUpdate_return handleUpdate(HTTPClient &http, const String &currentVersion, bool spiffs = false);
};

extern HTTPSUpdate httpsUpdate;

#endif /* ___HTTPS_UPDATE_H___ */