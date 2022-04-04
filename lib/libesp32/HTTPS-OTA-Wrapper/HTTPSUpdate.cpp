#include "HTTPSUpdate.h"

HTTPUpdateResult HTTPSUpdate::update(WiFiClientSecure& client, const String& url, const String& currentVersion, const char* CAcert)
{
    HTTPSClient http;
    if(!http.begin(client, url, CAcert))
    {
        return HTTP_UPDATE_FAILED;
    }
    return handleUpdate(http, currentVersion, false);
}

HTTPSUpdate httpsUpdate;