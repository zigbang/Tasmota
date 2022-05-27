// /*
//     xsns_89_sonoff.ino - ESP8266 based relay for ZIoT Tasmota
// */

// #ifdef FIRMWARE_ZIOT_SONOFF
// #define XSNS_89 89

// #include <Arduino.h> //
// #include "tasmota.h" //

// #ifdef FIRMWARE_ZIOT_SONOFF_MINIMAL
// bool initial_ota_try;
// #endif

// /*********************************************************************************************\
// * Interface
// \*********************************************************************************************/

// bool Xsns89(uint8_t function)
// {
//     bool result = false;

//     if (FUNC_PRE_INIT == function)
//     {
//         // Init
//     }
//     else if ()
//     {
//         switch (function)
//         {
//             case FUNC_LOOP:
//                 break;
//             case FUNC_EVERY_SECOND:
//                 break;
// #ifdef FIRMWARE_ZIOT_SONOFF_MINIMAL
//             case FUNC_EVERY_250_MSECOND:
//                 if (!initial_ota_try) {
//                     char command[TOPSZ + 10];
//                     snprintf_P(command, sizeof(command), PSTR(D_CMND_UPGRADE " 1"));
//                     ExecuteCommand(command, SRC_IGNORE);
//                     initial_ota_try = true;
//                 }

//                 int ota_error = ESPhttpUpdate.getLastError();

//                 printf("ota_error : %d\n", ota_error);
//                 break;
// #endif
//         }
//     }

//     return result;
// }

// #endif  // FIRMWARE_ZIOT_SONOFF