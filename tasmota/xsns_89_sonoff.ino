/*
    xsns_89_sonoff.ino - ESP8266 based relay for ZIoT Tasmota
*/

#ifdef FIRMWARE_ZIOT_SONOFF
#define XSNS_89 89

struct ZIoTSonoff {
    bool ready = false;
    bool executedOnce = false;
    char* version = "1.0.0";
} ziotSonoff;

void UpdateShadow(char *payload)
{
    char topic[64];
    char awsPayload[60];

    snprintf_P(topic, sizeof(topic), PSTR("$aws/things/%s/shadow/update"), SettingsText(SET_MQTT_TOPIC));
    snprintf_P(awsPayload, sizeof(awsPayload), PSTR("{\"state\":{\"reported\":%s}}"), payload);

    MqttClient.publish(topic, awsPayload, false);
}

/*********************************************************************************************\
* Interface
\*********************************************************************************************/

bool Xsns89(uint8_t function)
{
    bool result = false;

    if (FUNC_PRE_INIT == function)
    {
        ziotSonoff.ready = true;
        printf("The version of this firmware is : %s\n", ziotSonoff.version);
    }
    else if (ziotSonoff.ready)
    {
        switch (function)
        {
            case FUNC_LOOP:
                break;
            case FUNC_EVERY_SECOND:
                break;
        }
    }

    return result;
}

#endif  // FIRMWARE_ZIOT_SONOFF