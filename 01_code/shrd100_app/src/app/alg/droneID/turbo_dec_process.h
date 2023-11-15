#ifndef _TURBO_DEC_PROCESS_H_
#define _TURBO_DEC_PROCESS_H_


/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
#define INPUT_FILE_BIT_COUNT		(7200)
#define TURBO_ITERATIONS			(1)
#define TURBO_DECODER_BIT_COUNT		(1412)	// Number of bits that the Turbo decoder will take in
#define EXPECTED_PAYLOAD_BYTES		(176)	// Number of bytes that the Turbo decoder will output
#define EXPECTED_PAYLOAD_BITS		(EXPECTED_PAYLOAD_BYTES * 8)

/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/


/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
int TurboDecProcess(int8_t *decodeInBits, uint8_t *decodeOutBytes);


#endif /* _TURBO_DEC_PROCESS_H_ */
