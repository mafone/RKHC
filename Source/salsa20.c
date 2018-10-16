#include <stdint.h>
#include <stddef.h>

#define R(a,b) (((a) << (b)) | ((a) >> (32 - (b))))

// http://cr.yp.to/salsa20.html
void salsa20_word_specification(uint32_t out[16], uint32_t in[16])
{
	int i;
	uint32_t x[16];

	for (i = 0; i < 16; ++i)
		x[i] = in[i];

	for (i = 20; i > 0; i -= 2) {
		x[4] ^= R(x[0] + x[12], 7);  x[8] ^= R(x[4] + x[0], 9);
		x[12] ^= R(x[8] + x[4], 13);  x[0] ^= R(x[12] + x[8], 18);
		x[9] ^= R(x[5] + x[1], 7);  x[13] ^= R(x[9] + x[5], 9);
		x[1] ^= R(x[13] + x[9], 13);  x[5] ^= R(x[1] + x[13], 18);
		x[14] ^= R(x[10] + x[6], 7);  x[2] ^= R(x[14] + x[10], 9);
		x[6] ^= R(x[2] + x[14], 13);  x[10] ^= R(x[6] + x[2], 18);
		x[3] ^= R(x[15] + x[11], 7);  x[7] ^= R(x[3] + x[15], 9);
		x[11] ^= R(x[7] + x[3], 13);  x[15] ^= R(x[11] + x[7], 18);
		x[1] ^= R(x[0] + x[3], 7);  x[2] ^= R(x[1] + x[0], 9);
		x[3] ^= R(x[2] + x[1], 13);  x[0] ^= R(x[3] + x[2], 18);
		x[6] ^= R(x[5] + x[4], 7);  x[7] ^= R(x[6] + x[5], 9);
		x[4] ^= R(x[7] + x[6], 13);  x[5] ^= R(x[4] + x[7], 18);
		x[11] ^= R(x[10] + x[9], 7);  x[8] ^= R(x[11] + x[10], 9);
		x[9] ^= R(x[8] + x[11], 13);  x[10] ^= R(x[9] + x[8], 18);
		x[12] ^= R(x[15] + x[14], 7);  x[13] ^= R(x[12] + x[15], 9);
		x[14] ^= R(x[13] + x[12], 13);  x[15] ^= R(x[14] + x[13], 18);
	}

	for (i = 0; i < 16; ++i)
		out[i] = x[i] + in[i];
}

// The 16-byte (128-bit) key expansion function
void s20_expand16(uint8_t *k,
	uint8_t n[16],
	uint8_t keystream[64])
{
	int i, j;
	// The constants specified by the Salsa20 specification, 'tau'
	// "expand 16-byte k"
	uint8_t t[4][4] = {
		{ 'e', 'x', 'p', 'a' },
		{ 'n', 'd', ' ', '1' },
		{ '6', '-', 'b', 'y' },
		{ 't', 'e', ' ', 'k' }
	};

	// Copy all of 'tau' into the correct spots in our keystream block
	for (i = 0; i < 64; i += 20)
		for (j = 0; j < 4; ++j)
			keystream[i + j] = t[i / 20][j];

	// Copy the key and the nonce into the keystream block
	for (i = 0; i < 16; ++i) {
		keystream[4 + i] = k[i];
		keystream[44 + i] = k[i];
		keystream[24 + i] = n[i];
	}
}

// The 32-byte (256-bit) key expansion function
void s20_expand32(uint8_t *k,
	uint8_t n[16],
	uint8_t keystream[64])
{
	int i, j;
	// The constants specified by the Salsa20 specification, 'sigma'
	// "expand 32-byte k"
	uint8_t o[4][4] = {
		{ 'e', 'x', 'p', 'a' },
		{ 'n', 'd', ' ', '3' },
		{ '2', '-', 'b', 'y' },
		{ 't', 'e', ' ', 'k' }
	};

	// Copy all of 'sigma' into the correct spots in our keystream block
	for (i = 0; i < 64; i += 20)
		for (j = 0; j < 4; ++j)
			keystream[i + j] = o[i / 20][j];

	// Copy the key and the nonce into the keystream block
	for (i = 0; i < 16; ++i) {
		keystream[4 + i] = k[i];
		keystream[44 + i] = k[i + 16];
		keystream[24 + i] = n[i];
	}
}