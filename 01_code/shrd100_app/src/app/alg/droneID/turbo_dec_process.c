#include <stdint.h> // *int*_t
#include <stdio.h>  // fprintf, fopen, feof, fseek, ftell
#include <string.h>	// memcpy

// The turbofec includes must be imported as extern C otherwise the linker will fail
#ifdef __cplusplus
extern "C" {
#endif

#include "turbofec/rate_match.h"
#include "turbofec/turbo.h"
#include "turbo_dec_process.h"
#include "testData/testData_get_process.h"
#include "droneID_utils.h"


#ifdef __cplusplus
}
#endif


int8_t turbo_decoder_input[INPUT_FILE_BIT_COUNT] = { 0 };
int8_t d1[TURBO_DECODER_BIT_COUNT] = { 0 };
int8_t d2[TURBO_DECODER_BIT_COUNT] = { 0 };
int8_t d3[TURBO_DECODER_BIT_COUNT] = { 0 };
int8_t bit_value_lut[2] = {-63, 63};


int TurboDecProcess(int8_t *decodeInBits, uint8_t *decodeOutBytes)
{
    /* Create the required structures to run the Turbo decoder */
	struct tdecoder *turbo_decoder = alloc_tdec();
    struct lte_rate_matcher *rate_matcher = lte_rate_matcher_alloc();
    struct lte_rate_matcher_io rate_matcher_io =
    {
        .D = TURBO_DECODER_BIT_COUNT,
        .E = INPUT_FILE_BIT_COUNT,
        .d = {&d1[0], &d2[0], &d3[0]},
        .e = &decodeInBits[0]
    };

#if 0
    for (int idx = 0; idx < INPUT_FILE_BIT_COUNT; idx++)
    {
        const int8_t value = (int8_t)decodeInBits[idx];

        /* Don't let values other than 0 or 1 past as these will walk past the edge of the LUT */
        if (value != 1 && value != 0)
        {
            fprintf(stderr, "Invalid bit value '%02x' at offset %d.  Must be 0x1 or 0x0\n", value, idx);
        }

        /* Map the 1/0 value to the correct magic number from the lookup table */
        turbo_decoder_input[idx] = bit_value_lut[value];
    }
#endif

    /* Setup the rate matching logic */
    lte_rate_match_rv(rate_matcher, &rate_matcher_io, 0);

    /* Run the turbo decoder (will do rate matching as well) */
	const int decode_status = lte_turbo_decode(turbo_decoder, EXPECTED_PAYLOAD_BITS, GetDroneIDTurboDecIterNum(),
        &decodeOutBytes[0], &d1[0], &d2[0], &d3[0]);

    if (decode_status != 0)
    {
        fprintf(stderr, "[ERROR] Failed to remove Turbo coder.  Exit code %d\n", decode_status);
    }
#if 0
    /* Print out the frame in hex */
    for (uint32_t idx = 0; idx < EXPECTED_PAYLOAD_BYTES; idx++)
    {
        fprintf(stdout, "%02x", decodeOutBytes[idx]);
    }
    fprintf(stdout, "\n");
#endif
    free_tdec(turbo_decoder);
    lte_rate_matcher_free(rate_matcher);

    return 0;
}
