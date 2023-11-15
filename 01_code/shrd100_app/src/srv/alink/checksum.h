#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_

// Visual Studio versions before 2010 don't have stdint.h, so we just error out.
#if (defined _MSC_VER) && (_MSC_VER < 1600)
#error "The C-MAVLink implementation requires Visual Studio 2010 or greater"
#endif

#include <stdint.h>

    /**
     *
     *  CALCULATE THE CHECKSUM
     *
     */

#define X25_INIT_CRC 0xffff
#define X25_VALIDATE_CRC 0xf0b8

    void alink_crc_init(uint16_t *crcAccum);
    void crc_accumulate(uint8_t data, uint16_t *crcAccum);
    uint16_t crc_calculate(const uint8_t *pBuffer, uint16_t length);
    void crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length);

#endif /* _CHECKSUM_H_ */

#ifdef __cplusplus
}
#endif
