//
// Compat with ESP32
//
#include <HTTPUpdate.h>
#include <HTTPSUpdate.h>
#define ESPhttpUpdate httpUpdate
#define ESPhttpsUpdate httpsUpdate

inline HTTPUpdateResult ESPhttpUpdate_update(const String& url, const String& currentVersion = "")
{
	return HTTP_UPDATE_OK;
}
