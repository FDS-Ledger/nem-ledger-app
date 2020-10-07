/*******************************************************************************
*   NEM Wallet
*   (c) 2017 Ledger
*    (c) 2020 FDS
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/
#include <os_io_seproxyhal.h>
#include <string.h>
#include "readers.h"

char int_to_number_char(uint64_t value) {
    if (value > 9) {
        return '?';
    }

    return (char) ('0' + value);
}

uint16_t sprintf_number(char *dst, uint16_t len, uint64_t value) {
    uint16_t numDigits = 0, i;
    uint64_t base = 1;
    while (base <= value) {
        base *= 10;
        numDigits++;
    }
    if (numDigits > len - 1) {
        THROW(EXCEPTION_OVERFLOW);
    }
    base /= 10;
    for (i=0; i<numDigits; i++) {
        dst[i] = int_to_number_char((value / base) % 10);
        base /= 10;
    }
    dst[i] = '\0';
    return i;
}

void sprintf_hex(char *dst, uint16_t maxLen, uint8_t *src, uint16_t dataLength, uint8_t reverse) {
    if (2 * dataLength > maxLen - 1) {
        THROW(EXCEPTION_OVERFLOW);
    }
    for (uint16_t i = 0; i < dataLength; i++) {
        SPRINTF(dst + 2 * i, "%02X", reverse==1?src[dataLength-1-i]:src[i]);
    }
    dst[2*dataLength] = '\0';
}

//todo nonprintable ch + utf8
void sprintf_ascii(char *dst, uint16_t maxLen, uint8_t *src, uint16_t dataLength) {
    if (dataLength > maxLen - 1) {
        THROW(EXCEPTION_OVERFLOW);
    }
    char *tmpCh = (char *) src;
    for (uint8_t j=0; j < dataLength; j++){
        dst[j] = tmpCh[j];
    }
    dst[dataLength] = '\0';
}

void sprintf_mosaic(char *dst, uint16_t maxLen, uint8_t *mosaic, uint16_t dataLength) {
    uint16_t mosaicNameLen = dataLength - 8;
    uint16_t len = sprintf_number(dst, maxLen, read_uint64(mosaic + mosaicNameLen));
    strcat(dst, " ");
    sprintf_ascii(dst+len+1, maxLen-len-1, mosaic, mosaicNameLen);
}

uint64_t read_uint64(uint8_t *src) {
    uint64_t value ;
    value = src[7] ;
    value = (value << 8 ) + src[6] ;
    value = (value << 8 ) + src[5] ;
    value = (value << 8 ) + src[4] ;
    value = (value << 8 ) + src[3] ;
    value = (value << 8 ) + src[2] ;
    value = (value << 8 ) + src[1] ;
    value = (value << 8 ) + src[0] ;
    return value ;
}

uint8_t read_uint8(uint8_t *src) {
    return (uint8_t) *((uint8_t *)src);
}

uint16_t read_uint16(uint8_t *src) {
    return (uint16_t) *((uint16_t *)src);
}

uint32_t read_uint32(uint8_t *src) {
    return (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
}

int8_t read_int8(uint8_t *src) {
    return (int8_t) *((uint8_t*) src);
}