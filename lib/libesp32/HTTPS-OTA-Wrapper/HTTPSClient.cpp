#include "HTTPSClient.h"

#ifdef HTTPCLIENT_1_1_COMPATIBLE
class TransportTraits
{
public:
    virtual ~TransportTraits()
    {
    }

    virtual std::unique_ptr<WiFiClient> create()
    {
        return std::unique_ptr<WiFiClient>(new WiFiClient());
    }

    virtual bool verify(WiFiClient &client, const char *host)
    {
        return true;
    }
};

class TLSTraits : public TransportTraits
{
public:
    TLSTraits(const char *CAcert, const char *clicert = nullptr, const char *clikey = nullptr) : _cacert(CAcert), _clicert(clicert), _clikey(clikey)
    {
    }

    std::unique_ptr<WiFiClient> create() override
    {
        return std::unique_ptr<WiFiClient>(new WiFiClientSecure());
    }

    bool verify(WiFiClient &client, const char *host) override
    {
        WiFiClientSecure &wcs = static_cast<WiFiClientSecure &>(client);
        if (_cacert == nullptr)
        {
            wcs.setInsecure();
        }
        else
        {
            wcs.setCACert(_cacert);
            wcs.setCertificate(_clicert);
            wcs.setPrivateKey(_clikey);
        }
        return true;
    }

protected:
    const char *_cacert;
    const char *_clicert;
    const char *_clikey;
};
#endif // HTTPCLIENT_1_1_COMPATIBLE

bool HTTPSClient::begin(WiFiClientSecure &client, String url, const char *CAcert)
{
#ifdef HTTPCLIENT_1_1_COMPATIBLE
    if (_tcpDeprecated)
    {
        log_d("mix up of new and deprecated api");
        _canReuse = false;
        end();
    }
#endif

    _client = &client;

    // check for : (http: or https:)
    int index = url.indexOf(':');
    if (index < 0)
    {
        log_d("failed to parse protocol");
        return false;
    }

    String protocol = url.substring(0, index);
    if (protocol != "http" && protocol != "https")
    {
        log_d("unknown protocol '%s'", protocol.c_str());
        return false;
    }

    _port = (protocol == "https" ? 443 : 80);
    return beginInternal(url, protocol.c_str());

    _secure = true;
    _transportTraits = TransportTraitsPtr(new TLSTraits(CAcert));
    if (!_transportTraits)
    {
        log_e("could not create transport traits");
        return false;
    }
}