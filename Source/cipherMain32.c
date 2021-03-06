#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "salsa20.c"

#include "../Header/cipher32.h"

// Command line arguments 
//-enc -K mykey -iv myiv -in plaintex.ext -out ciphertext.rkhc
//-dec -K mykey -iv myiv -in ciphertext.rhill -out recovered.ext

// Used to convert a string (in) into an array of uint8_t (out)
// http://stackoverflow.com/questions/16519056/convert-ascii-char-to-hexadecimal-char-in-c
// Biribu
void hex_decode(char * in, size_t len, uint8_t * out);

//Used to convert a uint8_t array (in) into a single uint32_t
uint32_t Uint8ArrtoUint32(uint8_t* in);

// Convert key from array of 64 8 bits (keystream) ints to array of 16 32 bits ints (seed)
void convertKey(uint8_t keystream[64], uint32_t seed[16]);

// Expand and create the seed using the key
//void createSeed(char * K, uint32_t seed[16], char * nonce);
void createSeed(uint32_t seed[16], size_t size, char * arg);

// Fill or truncate an array to the desired size.
// Used with the key, the iv and the nonce
void fillOrTruncate(char* in, char* out, int desiredSize);

// Encrypting using RKHC in CBC
void cbcEncrypt32(uint32_t * seed, char * in, char * out, char * iv);

// Decrypting using RKHC in CBC
void cbcDecrypt32(uint32_t * seed, char * in, char * out, char * iv);

int main(int argc, char * argv[])
{
	if (argc == 10)
	{
		// Check if the parameters are valid
		if (!(strcmp(argv[2], "-K") || strcmp(argv[4], "-iv") || strcmp(argv[6], "-in") || strcmp(argv[8], "-out")))
		{
			// Generate the seed
			uint32_t seed[16];
			size_t size = strlen(argv[3]); 
			char * arg = argv[3];
			createSeed(seed, size, arg);

			// Initialize the seed
			initializeCipher(seed);

			// Encrypting in CBC
			if (strcmp(argv[1], "-enc") == 0)
			{
				cbcEncrypt32(seed, argv[7], argv[9], argv[5]);
			}
			// Decrypting in CBC
			else if (strcmp(argv[1], "-dec") == 0)
			{
				cbcDecrypt32(seed, argv[7], argv[9], argv[5]);
			}
			else
			{
				//fprintf(stderr, "ERROR: Wrong entry parameters!\n");
				printf("ERROR: Wrong entry parameters!\n");
				return EXIT_FAILURE;
			}
		}
		else
		{
			//fprintf(stderr, "ERROR: Wrong entry parameters!\n");
			//printf("ERROR: Wrong entry parameters!\n");
			getchar();
			return EXIT_FAILURE;
		}
	}
	else
	{
		//fprintf(stderr, "ERROR: Wrong entry parameters!\n");
		printf("ERROR: Wrong entry parametersMAF!\n");
		getchar();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// Encrypting in CBC
void cbcEncrypt32(uint32_t * seed, char * in, char * out, char * iv)
{
	// The IV
	char ivc[iBits / 4];

	fillOrTruncate(iv, ivc, iBits / 4);

	uint8_t ivInHex[iBits / 8];

	// The intended size is iBits (the block size)
	if ((iBits / 4) == 8)
	{
		hex_decode(ivc, iBits / 4, ivInHex);
	}
	else {
		//fprintf(stderr, "ERROR: Problem with the IV!\n");
		printf("ERROR: Problem with the IV!\n");
		getchar();
		//return EXIT_FAILURE;
		exit(EXIT_FAILURE);
	}

	uint32_t ivFinal = Uint8ArrtoUint32(ivInHex);

	// Opening of the file that contains the text to be encrypted
	FILE * fPlaintextFile;

	// Open the file for reading
	if ((fPlaintextFile = fopen(in, "rb")) == NULL)
	{
		//fprintf(stderr, "ERROR: Problem with the IV!\n");
		printf("ERROR: Problem with the IV!\n");
		exit(EXIT_FAILURE);
	}

	// Opening of the file to save the ciphertext
	FILE *fCiphertextFile;

	if ((fCiphertextFile = fopen(out, "wb")) == NULL)
	{
		//fprintf(stderr, "ERROR: Problem with the IV!\n");
		printf("ERROR: Problem with the IV!\n");
		exit(EXIT_FAILURE);
	} 

	// Where each block of plaintext is kept
	uint32_t plainTextBlock;
	// Where each block of ciphertext is kept
	uint32_t cipherTextBlock;

	// Used in shifts
	uint32_t uShiftAux;

	// The size of the block read
	uint8_t blockSize;

	uint8_t i;

	while (!feof(fPlaintextFile)) {
		plainTextBlock = 0;
		blockSize = fread(&plainTextBlock, 1, (iBits / 8), fPlaintextFile);

		// Last block needs padding
		// TODO: NO SIDE CHANNEL PROBLEM, RIGHT?
		// THIS ONLY REVEALS THE SIZE OF THE MESSAGE
		if (blockSize < (iBits / 8)) {
			uShiftAux = ((uint32_t)((iBits / 8) - blockSize));

			// Moves the read bytes to the left for padding
			plainTextBlock <<= uShiftAux * 8;

			for (i = 0; i < (iBits / 8) - blockSize; i++) {
				plainTextBlock ^= uShiftAux;

				uShiftAux <<= 8;
			}
		}

		// Xor with the iv
		plainTextBlock ^= ivFinal;

		// Ecrypt the block
		encrypt(&plainTextBlock, &cipherTextBlock);

		// Update the iv and the plaintext
		ivFinal = cipherTextBlock;

		// Writing to the file
		fwrite(&cipherTextBlock, 1, (iBits / 8), fCiphertextFile);
	}

	fclose(fPlaintextFile);

	fclose(fCiphertextFile);
}

// Decrypting in CBC
// Considers that there is no empty file, since even when the original plaintext is 0 bytes
// the ciphertext will still be the size of a single block
void cbcDecrypt32(uint32_t *seed, char* in, char* out, char * iv)
{
	// The IV
	char ivc[iBits / 4];

	fillOrTruncate(iv, ivc, iBits / 4);

	uint8_t ivInHex[iBits / 8];

	// The intended size is iBits (the block size)
	if (iBits / 4 == 8)
	{
		hex_decode(ivc, iBits / 4, ivInHex);
	}
	else {
		printf("ERROR: Problem with the IV!\n");
		//fprintf(stderr, "ERROR: Problem with the IV!\n");
		getchar();
		//return EXIT_FAILURE;
		exit(EXIT_FAILURE);
	}

	uint64_t ivFinal = Uint8ArrtoUint32(ivInHex);

	// Opening of the file that contains the text to be encrypted
	FILE * fCiphertextFile;

	if ((fCiphertextFile = fopen(in, "rb")) == NULL)
	{
		printf("ERROR!\n");
		//fprintf(stderr, "ERROR: %s!\n", strerror(errno));
		getchar();
		exit(EXIT_FAILURE);
	}

	FILE *fPlaintextFile;

	if ((fPlaintextFile = fopen(out, "wb")) == NULL)
	{

		printf("ERROR!\n");
		//fprintf(stderr, "ERROR: %s!\n", strerror(errno));
		getchar();
		exit(EXIT_FAILURE);
	}

	// Where each block of plaintext is kept
	uint32_t plainTextBlock;
	// Where each block of ciphertext is kept
	uint32_t cipherTextBlock;

	uint8_t j;

	// Used with shifts
	uint32_t uShiftAux;

	// Used as buffer to check if the file ended
	uint32_t nextPlaintextBlock;

	// Read a block which is the minimum
	fread(&cipherTextBlock, 1, (iBits / 8), fCiphertextFile);

	while (fread(&nextPlaintextBlock, 1, (iBits / 8), fCiphertextFile) != 0) {
		
		// Decipher a block
		decrypt(&cipherTextBlock, &plainTextBlock);

		// Xor with the iv
		plainTextBlock ^= ivFinal;

		// Update the iv
		ivFinal = cipherTextBlock;

		fwrite(&plainTextBlock, 1, (iBits / 8), fPlaintextFile);

		cipherTextBlock = nextPlaintextBlock;
	}

	// Decipher a block
	decrypt(&cipherTextBlock, &plainTextBlock);

	// Xor with the iv
	plainTextBlock ^= ivFinal;

	// Check the last byte for the number of padded bytes
	uint32_t auxWord = plainTextBlock << (iBits - 8);
	auxWord = auxWord >> (iBits - 8);

	// The number of padded bytes in the block
	uint8_t paddedBytes = (unsigned int)auxWord;

	if (paddedBytes > (iBits / 8))
	{
		//fprintf(stderr, "ERROR: Padding was wrong!\n");
		printf("ERROR: Padding was wrong!\n");
		getchar();
		exit(EXIT_FAILURE);
	}

	// Eliminate the padding
	plainTextBlock >>= 8 * paddedBytes;

	fwrite(&plainTextBlock, 1, (iBits / 8) - paddedBytes, fPlaintextFile);

	fclose(fCiphertextFile);

	fclose(fPlaintextFile);
}


// Fill or truncate an array to the desired size.
// Used with the key, the iv and the nonce
void fillOrTruncate(char* in, char* out, int desiredSize)
{
	uint8_t i, initialSize, truncateSize;

	// Dummy variable to prevent side channel attacks
	initialSize = strlen(in);

	// Truncate the in if needed
	if (initialSize > desiredSize)
		truncateSize = desiredSize;
	else
		truncateSize = initialSize;

	// Places 0's on the left on the rest of the necessary
	// and copies the in
	for (i = 0; i < desiredSize; i++)
		out[i] = in[i - (desiredSize - truncateSize)] * (i >= desiredSize - truncateSize);
}

// Used to convert a string into an array of uint8_t
// http://stackoverflow.com/questions/16519056/convert-ascii-char-to-hexadecimal-char-in-c
// Biribu
void hex_decode(char* in, size_t len, uint8_t* out)
{
	unsigned int i, t, hn, ln;

	for (t = 0, i = 0; i < len; i += 2, t++) {

		hn = in[i] > '9' ? (in[i] | 32) - 'a' + 10 : in[i] - '0';
		ln = in[i + 1] > '9' ? (in[i + 1] | 32) - 'a' + 10 : in[i + 1] - '0';

		out[t] = (hn << 4) | ln;
	}
}

//Used to convert a uint8_t array into a single uint32_t
// TODO: SHOULD WE COPY FROM LEFT TO RIGHT OR VICE-VERSA
uint32_t Uint8ArrtoUint32(uint8_t* in)
{
	uint8_t i;
	uint32_t out = 0;

	for (i = 0; i < (iBits / 8); i++)
		out |= (uint32_t)in[i] << (8 * i);

	return out;
}

// Convert key from array of 64 8 bit ints to array of 16 32 bits ints
// TODO: SHOULD WE COPY FROM LEFT TO RIGHT OR VICE-VERSA
// THE ORDER WAS INVERTED
void convertKey(uint8_t keystream[64], uint32_t seed[16])
{
	uint8_t i;

	for (i = 0; i < 16; i++)
		seed[i] = Uint8ArrtoUint32(&keystream[i * 4]);
}

void createSeed(uint32_t seed[16], size_t size, char * arg)
{
	if (size > 16)
		for (int i=0; i<size; i++)
			if (i<16)
				seed[i]= arg[i];
			else
				seed[i%16] += arg[i];
	else
		for (int i=0; i<16; i++)
			if (i<size)
				seed[i]= arg[i];
			else
				seed[i] = arg[i%size];
}

/*
// Expand and create the seed using the key and a (the) nounce
// Prepared for keys of 16 and 32 bytes - changing the value of iKeySize
void createSeed(char * K, uint32_t seed[16], char * nonce)
{
	// The nonce
	char noncec[16];

	fillOrTruncate(nonce, noncec, 16);

	uint8_t nonceInHex[16];

	// The intended size is 16

	hex_decode(noncec, 16, nonceInHex);
	
	//The key
	char key[iKeySize * 2];

	fillOrTruncate(K, key, iKeySize * 2);

	uint8_t keyInHex[iKeySize], keystream[64];

	// The intended size is 32 chars * 4 = 128 bits
	if (iKeySize == 16)
	{
		hex_decode(key, iKeySize * 2, keyInHex);

		//uint8_t i; //?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????

		s20_expand16(keyInHex, nonceInHex, keystream);

		convertKey(keystream, seed);
	}
	else if (iKeySize == 32)
	{
		hex_decode(key, iKeySize * 2, keyInHex);

		//uint8_t i; //?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????

		s20_expand32(keyInHex, nonceInHex, keystream);

		convertKey(keystream, seed);
	}
} 
*/