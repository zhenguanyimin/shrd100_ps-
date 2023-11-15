
#ifndef PROTOCOL_CRC_H
#define PROTOCOL_CRC_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    uint16_t crc_16bits_compute(uint8_t *ptr, uint32_t len);
    uint16_t crc_16bits_update(uint16_t init, uint8_t *ptr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_CRC_H */
