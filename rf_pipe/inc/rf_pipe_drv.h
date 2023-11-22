#ifndef _RADIO_COMMON_H_
#define _RADIO_COMMON_H_

#define CHANNEL_NUM     1

#include <stdint.h>

typedef union {
    struct {
        uint8_t svio : 1;               //(Sticky) IO Voltage Large Enough (not Brownout)
        uint8_t sbevmodem : 1;          //(Sticky) Modem Domain Voltage Brownout Error (Inverted; 0 = Brownout, 1 = Power OK)
        uint8_t sbevana : 1;            //(Sticky) Analog Domain Voltage Brownout Error (Inverted; 0 = Brownout, 1 = Power OK)
        uint8_t svmodem : 1;            //(Sticky) Modem Domain Voltage Regulator Ready
        uint8_t svana : 1;              //(Sticky) Analog Domain Voltage Regulator Ready
        uint8_t svref : 1;              //(Sticky) Reference Voltage Regulator Ready
        uint8_t sref : 1;               //(Sticky) Reference Ready
        uint8_t ssum : 1;               //(Sticky) Summary Ready Status (one when all unmasked POWIRQMASK power sources are ready)
    };
    uint8_t raw;
} PwrStatus;

#endif

