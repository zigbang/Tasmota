#if defined(USE_TLS) && defined(USE_MQTT_TLS_CA_CERT)

char AmazonClientCert[857];
char AmazonPrivateKey[45];

#ifdef ESP32
static const char rootCA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

unsigned char serverCert[] = {
    0x30, 0x82, 0x02, 0x18, 0x30, 0x82, 0x01, 0x81, 0x02, 0x01, 0x02, 0x30,
    0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05,
    0x05, 0x00, 0x30, 0x54, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04,
    0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
    0x04, 0x08, 0x0c, 0x02, 0x42, 0x45, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03,
    0x55, 0x04, 0x07, 0x0c, 0x06, 0x42, 0x65, 0x72, 0x6c, 0x69, 0x6e, 0x31,
    0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x4d, 0x79,
    0x43, 0x6f, 0x6d, 0x70, 0x61, 0x6e, 0x79, 0x31, 0x13, 0x30, 0x11, 0x06,
    0x03, 0x55, 0x04, 0x03, 0x0c, 0x0a, 0x6d, 0x79, 0x63, 0x61, 0x2e, 0x6c,
    0x6f, 0x63, 0x61, 0x6c, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x32, 0x30, 0x33,
    0x32, 0x39, 0x30, 0x32, 0x34, 0x34, 0x33, 0x33, 0x5a, 0x17, 0x0d, 0x33,
    0x32, 0x30, 0x33, 0x32, 0x36, 0x30, 0x32, 0x34, 0x34, 0x33, 0x33, 0x5a,
    0x30, 0x55, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
    0x02, 0x44, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08,
    0x0c, 0x02, 0x42, 0x45, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04,
    0x07, 0x0c, 0x06, 0x42, 0x65, 0x72, 0x6c, 0x69, 0x6e, 0x31, 0x12, 0x30,
    0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x4d, 0x79, 0x43, 0x6f,
    0x6d, 0x70, 0x61, 0x6e, 0x79, 0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55,
    0x04, 0x03, 0x0c, 0x0b, 0x65, 0x73, 0x70, 0x33, 0x32, 0x2e, 0x6c, 0x6f,
    0x63, 0x61, 0x6c, 0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
    0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8d,
    0x00, 0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xa2, 0x99, 0x20, 0x91,
    0x4f, 0x5d, 0x82, 0x1b, 0x67, 0x73, 0x67, 0xe1, 0xce, 0x0e, 0x92, 0x5d,
    0x00, 0x58, 0x9d, 0xeb, 0xcf, 0x08, 0x56, 0x9e, 0xf6, 0xbf, 0x0c, 0x2a,
    0x73, 0x2d, 0xa0, 0xda, 0xe7, 0xb4, 0xf4, 0xa7, 0x69, 0xe6, 0xec, 0x75,
    0x7a, 0x9d, 0x83, 0xca, 0x18, 0x59, 0x58, 0xf3, 0xe3, 0xef, 0x1c, 0x96,
    0xb5, 0xb8, 0x27, 0x48, 0xb3, 0xe7, 0x8f, 0xf8, 0xda, 0xd5, 0xed, 0x79,
    0x77, 0x16, 0xce, 0x01, 0xae, 0x8c, 0xb9, 0xdc, 0xa3, 0x0b, 0x5f, 0x6d,
    0xc8, 0x8d, 0xc3, 0x5e, 0x2b, 0xf2, 0x88, 0x02, 0x64, 0x09, 0x29, 0x9f,
    0x5c, 0x9d, 0x5c, 0x04, 0x0d, 0x85, 0x40, 0x1b, 0x49, 0xe6, 0x3f, 0x8a,
    0x60, 0x45, 0x0a, 0xfb, 0x43, 0x5f, 0x00, 0xe6, 0xc4, 0x54, 0xb5, 0xe6,
    0xb3, 0xec, 0xd3, 0xc5, 0x75, 0x26, 0x2d, 0x7c, 0xfa, 0xa2, 0x31, 0x8f,
    0xb2, 0x8e, 0xe0, 0xa9, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30, 0x0d, 0x06,
    0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00,
    0x03, 0x81, 0x81, 0x00, 0x57, 0x20, 0x36, 0x44, 0xa6, 0xce, 0x84, 0xbd,
    0x7f, 0x65, 0x63, 0x28, 0xd6, 0xef, 0x4a, 0x58, 0xb5, 0x04, 0x17, 0x23,
    0x33, 0x1a, 0xac, 0x4c, 0x8f, 0x31, 0xe1, 0xca, 0xcc, 0xc3, 0xfa, 0xdc,
    0x2d, 0x63, 0x3c, 0x54, 0xd1, 0xd1, 0x53, 0x26, 0x65, 0x38, 0xff, 0x23,
    0x03, 0x88, 0x57, 0x7d, 0xa7, 0x2b, 0xf0, 0x03, 0xa5, 0x6a, 0x6b, 0xff,
    0xa9, 0x22, 0xc9, 0x40, 0xf2, 0x7a, 0xea, 0xba, 0x4f, 0x54, 0xdd, 0xd5,
    0x0c, 0xdc, 0xb6, 0x03, 0x25, 0xeb, 0xb5, 0x5f, 0x3d, 0x89, 0x48, 0x88,
    0xad, 0xb7, 0xa6, 0x89, 0x16, 0x01, 0x10, 0x93, 0x64, 0x78, 0x77, 0xb6,
    0xab, 0x8c, 0x5d, 0xfe, 0x38, 0x6d, 0x1e, 0x19, 0x63, 0x66, 0x36, 0x86,
    0x0c, 0xc5, 0x0f, 0xfd, 0x7e, 0x21, 0xbc, 0x4d, 0xb2, 0x17, 0xdd, 0xcc,
    0xc3, 0xa1, 0xd2, 0xd2, 0x6d, 0x2f, 0x44, 0x00, 0x01, 0xa1, 0x1c, 0xa9};
unsigned int serverCert_len = 540;
unsigned char serverKey[] = {
    0x30, 0x82, 0x02, 0x5c, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xa2,
    0x99, 0x20, 0x91, 0x4f, 0x5d, 0x82, 0x1b, 0x67, 0x73, 0x67, 0xe1, 0xce,
    0x0e, 0x92, 0x5d, 0x00, 0x58, 0x9d, 0xeb, 0xcf, 0x08, 0x56, 0x9e, 0xf6,
    0xbf, 0x0c, 0x2a, 0x73, 0x2d, 0xa0, 0xda, 0xe7, 0xb4, 0xf4, 0xa7, 0x69,
    0xe6, 0xec, 0x75, 0x7a, 0x9d, 0x83, 0xca, 0x18, 0x59, 0x58, 0xf3, 0xe3,
    0xef, 0x1c, 0x96, 0xb5, 0xb8, 0x27, 0x48, 0xb3, 0xe7, 0x8f, 0xf8, 0xda,
    0xd5, 0xed, 0x79, 0x77, 0x16, 0xce, 0x01, 0xae, 0x8c, 0xb9, 0xdc, 0xa3,
    0x0b, 0x5f, 0x6d, 0xc8, 0x8d, 0xc3, 0x5e, 0x2b, 0xf2, 0x88, 0x02, 0x64,
    0x09, 0x29, 0x9f, 0x5c, 0x9d, 0x5c, 0x04, 0x0d, 0x85, 0x40, 0x1b, 0x49,
    0xe6, 0x3f, 0x8a, 0x60, 0x45, 0x0a, 0xfb, 0x43, 0x5f, 0x00, 0xe6, 0xc4,
    0x54, 0xb5, 0xe6, 0xb3, 0xec, 0xd3, 0xc5, 0x75, 0x26, 0x2d, 0x7c, 0xfa,
    0xa2, 0x31, 0x8f, 0xb2, 0x8e, 0xe0, 0xa9, 0x02, 0x03, 0x01, 0x00, 0x01,
    0x02, 0x81, 0x80, 0x31, 0x82, 0xd7, 0x3b, 0xe8, 0x22, 0xdd, 0x1f, 0x63,
    0x1c, 0xed, 0x21, 0x01, 0x11, 0xc6, 0xd7, 0xb2, 0xe7, 0x49, 0x0f, 0x28,
    0xf7, 0xad, 0x08, 0xb2, 0xb1, 0xf2, 0x0e, 0x6b, 0x0c, 0x15, 0xd3, 0x12,
    0x83, 0x33, 0x8c, 0x56, 0xdf, 0x0e, 0x59, 0xa7, 0x80, 0x97, 0x44, 0xce,
    0xad, 0x46, 0x3c, 0xdd, 0xc7, 0x4d, 0xb9, 0x46, 0x94, 0x50, 0xc1, 0xfe,
    0xa6, 0x20, 0x5c, 0xf2, 0xa5, 0xf9, 0xad, 0x6a, 0x84, 0x4c, 0x45, 0xe3,
    0x81, 0xe5, 0xcf, 0xd6, 0x75, 0x93, 0xb7, 0xf7, 0x15, 0x1b, 0x21, 0xf0,
    0xf9, 0xf8, 0xe9, 0xa1, 0x4a, 0x7a, 0x2d, 0xca, 0x2b, 0x57, 0xca, 0xbb,
    0x61, 0xe7, 0xa0, 0x09, 0xc8, 0x17, 0xcd, 0x91, 0x96, 0x8b, 0x9e, 0x94,
    0x4b, 0x84, 0xdf, 0x85, 0x5b, 0x62, 0x15, 0xbb, 0x46, 0x8d, 0x9a, 0xb6,
    0xdd, 0x33, 0x99, 0xe7, 0x18, 0x72, 0x17, 0x21, 0x64, 0xc6, 0x01, 0x02,
    0x41, 0x00, 0xd6, 0x24, 0xbb, 0xee, 0x06, 0x8c, 0xbb, 0x4d, 0xaa, 0x2b,
    0x1e, 0x40, 0xb2, 0xb0, 0x52, 0x18, 0x2a, 0x6a, 0x35, 0x81, 0x3e, 0xe5,
    0xf8, 0x7c, 0x09, 0x33, 0x02, 0x71, 0x26, 0x2f, 0x64, 0xaf, 0x62, 0x2f,
    0xc7, 0xc5, 0x89, 0xa0, 0x9e, 0x88, 0xe1, 0xed, 0x48, 0x92, 0x68, 0xd4,
    0xcc, 0xcd, 0xdb, 0x0f, 0x6a, 0x8f, 0xd2, 0x07, 0xab, 0xfd, 0x2e, 0x41,
    0x02, 0x32, 0x83, 0x83, 0xe5, 0xd1, 0x02, 0x41, 0x00, 0xc2, 0x61, 0x2d,
    0xb4, 0xd5, 0x35, 0x99, 0x73, 0x28, 0x38, 0x3a, 0x6f, 0x31, 0x19, 0x31,
    0x8e, 0x19, 0x64, 0xe4, 0x64, 0x61, 0xaf, 0x6f, 0xd6, 0xc3, 0x36, 0x30,
    0xdb, 0x29, 0xac, 0xbe, 0x18, 0xcf, 0xbf, 0x7a, 0x84, 0xbb, 0x15, 0xf6,
    0xab, 0x56, 0xcb, 0xa8, 0x13, 0x5d, 0xdf, 0xc5, 0x29, 0xea, 0xb7, 0xd6,
    0xb1, 0xe9, 0x36, 0xb1, 0xa4, 0x66, 0xb2, 0xdc, 0x23, 0xd2, 0x94, 0x0b,
    0x59, 0x02, 0x40, 0x2c, 0x01, 0xc8, 0x8d, 0x15, 0xd3, 0x7d, 0xfa, 0x6b,
    0xea, 0x08, 0x81, 0x8b, 0x37, 0x28, 0xe7, 0xc6, 0x6f, 0xa5, 0x27, 0x36,
    0x61, 0xd4, 0x3a, 0xc9, 0x39, 0x2e, 0x5b, 0x4a, 0x59, 0x9a, 0xfb, 0x5f,
    0xd6, 0x29, 0xdb, 0xb2, 0x78, 0xcb, 0x9b, 0x9d, 0xb2, 0x41, 0xa3, 0xb4,
    0xdf, 0x66, 0x67, 0x37, 0x89, 0x67, 0x80, 0xbe, 0xcc, 0xcc, 0xcf, 0x6e,
    0xdd, 0xf5, 0x31, 0xa4, 0x4d, 0x4a, 0xc1, 0x02, 0x41, 0x00, 0xa1, 0xdf,
    0x6d, 0xc1, 0xc7, 0x40, 0xa0, 0xae, 0x8e, 0xd2, 0xec, 0x8e, 0xc6, 0x93,
    0x95, 0x7a, 0x21, 0xd9, 0xac, 0x7d, 0x90, 0x00, 0x1a, 0xa1, 0xfd, 0xe5,
    0x76, 0x20, 0x3d, 0x7f, 0x76, 0xbb, 0x90, 0xde, 0x83, 0xb8, 0x5f, 0x58,
    0xb6, 0x18, 0x0f, 0xea, 0xff, 0xe8, 0x48, 0xe7, 0xdd, 0xf8, 0xbf, 0x58,
    0x23, 0x79, 0xfb, 0x9e, 0x29, 0xa7, 0xa0, 0x42, 0xd9, 0x23, 0x17, 0xed,
    0x63, 0xd9, 0x02, 0x40, 0x73, 0x8a, 0xf3, 0xc9, 0xd8, 0x2a, 0x54, 0x2c,
    0x6a, 0x99, 0x07, 0xb6, 0xa4, 0xde, 0x25, 0xf9, 0xe9, 0x7f, 0xd8, 0x57,
    0x5d, 0xb2, 0xe2, 0xf3, 0xe7, 0x96, 0xe6, 0x03, 0x96, 0xbe, 0x65, 0x90,
    0x21, 0x7e, 0x0d, 0x24, 0x2c, 0x52, 0x3e, 0x25, 0x7b, 0x66, 0xfa, 0x9e,
    0xfa, 0x00, 0x05, 0x3e, 0xb6, 0x2e, 0xca, 0x15, 0xb2, 0x15, 0x0e, 0xb5,
    0x57, 0x49, 0x61, 0x28, 0xcc, 0x1d, 0x94, 0x73};
unsigned int serverKey_len = 608;
#endif

#endif // defined(USE_TLS) && defined(USE_MQTT_TLS_CA_CERT)