#ifndef HTTPSClient_H_
#define HTTPSClient_H_

#include <HTTPClient.h>

class HTTPSClient : public HTTPClient
{
public:
    HTTPSClient() : HTTPClient(){};
    ~HTTPSClient(){};

    bool begin(WiFiClientSecure &client, String url, const char *CAcert);
};

#endif /* HTTPSClient_H_ */
