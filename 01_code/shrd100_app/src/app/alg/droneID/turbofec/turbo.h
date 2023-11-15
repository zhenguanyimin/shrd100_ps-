#ifndef _LTE_TURBO_
#define _LTE_TURBO_

#include <stdint.h>

struct tdecoder;

/* Min and max code block sizes */
#define TURBO_MIN_K		40
#define TURBO_MAX_K		6144


#define SSE_ALIGN		__attribute__((aligned(16)))
#define API_EXPORT		__attribute__((__visibility__("default")))

#define NUM_TRELLIS_STATES	8
#define MAX_TRELLIS_LEN		(TURBO_MAX_K + 3)

/* Initialization value for forward and backward sums */
#define SUM_INIT		16000

struct tmetric {
	int16_t bm[NUM_TRELLIS_STATES];
	int16_t fwsums[NUM_TRELLIS_STATES];
};

/*
 * Trellis Object
 *
 * sums       - Accumulated path metrics
 * outputs    - Trellis ouput values
 * metrics    - Path metrics
 * vals       - Input value that led to each state
 */
struct vtrellis {
	struct tmetric *tm;
	int16_t *bwsums;
	int16_t *fwnorm;
	int16_t lvals[MAX_TRELLIS_LEN];
};

/*
 * Turbo Decoder
 *
 * n         - Code order
 * k         - Constraint length
 * len       - Horizontal length of trellis
 * trellis   - Trellis object
 * punc      - Puncturing sequence
 * paths     - Trellis paths
 */
struct tdecoder {
	int len;
	struct vtrellis trellis[2];

	SSE_ALIGN int16_t bwsums[8];
	SSE_ALIGN struct tmetric tm[MAX_TRELLIS_LEN + 1];
	int16_t fwnorm[MAX_TRELLIS_LEN];
};


/*
 * Convolutional encoder descriptor
 *
 * Rate and constraint lengths refer to constituent encoders. Only
 * rate 1/2 contraint length 4 codes supported. Uses fixed LTE turbo
 * interleaver.
 *
 * n    - Rate 2 (1/2)
 * k    - Constraint length 4
 * len  - Length of code block to be encoded
 * rgen - Recursive generator polynomial in octal
 * gen  - Generator polynomial in octal
 */
struct lte_turbo_code {
	int n;
	int k;
	int len;
	unsigned rgen;
	unsigned gen;
};

struct tdecoder *alloc_tdec();
void alloc_tdec_new(struct tdecoder *dec);
void free_tdec(struct tdecoder *dec);

int lte_turbo_encode(const struct lte_turbo_code *code,
		   const uint8_t *input, uint8_t *d0, uint8_t *d1, uint8_t *d2);

/* Packed output */
int lte_turbo_decode(struct tdecoder *dec, int len, int iter, uint8_t *output,
		     const int8_t *d0, const int8_t *d1, const int8_t *d2);

/* Unpacked output */
int lte_turbo_decode_unpack(struct tdecoder *dec, int len, int iter,
			    uint8_t *output, const int8_t *d0,
			    const int8_t *d1, const int8_t *d2);

#endif /* _LTE_TURBO_ */
