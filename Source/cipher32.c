// For debugging only (pdrintf).
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
//#include "../funcoes_matrizes.c"

//#include "../Header/cipher32.h"
// The block size in bits to use in all cryptosystem
#define iBits 32

//To use for creating seed according Salsa20
#define iKeySize 16

uint32_t ulMatrix[iBits]; //The matrix to be generated 
uint32_t ulMask; //The mask to be xored with some plaintext

uint32_t currentSeed[16]; //The seed to be used in salsa20 in any moment


// Auxilary functions
// ------------------------------------------------------------------------------------

// Assumes little endian
// http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
// Enno Gr�per
void printBits(size_t const size, void const * const ptr)
{
	unsigned char *b = (unsigned char*)ptr;
	unsigned char byte;
	int i, j;

	for (i = size - 1; i >= 0; i--)
	{
		for (j = 7; j >= 0; j--)
		{
			byte = b[i] & (1 << j);
			byte >>= j;
			printf("%u ", byte);
		}
	}
	puts("");
}

// Print a matrix of bits which, in this case, is an array of 32 bits ints
// printing each int with printBits
void printMatrix(uint32_t matrix[iBits]) {
	uint8_t i = 0;

	for (i = 0; i < iBits; i++)
		printBits(sizeof(matrix[i]), &matrix[i]);

	printf("\n");
}

// Copies the values between two arrays of size size bits, if inferior to 32
void copyArray(uint32_t* out, uint32_t* in, size_t size) {
	uint8_t i;

	//if (size > iBits || size < 0) {
	if (size > iBits) {
		//perror(stderr, "ERROR: Something went wrong!\n");
		printf("ERROR: Something went wrong!\n");
		getchar();
		exit(EXIT_FAILURE);
	}

	// TODO: NO PROBLEM FOR BEING SIZE RIGHT?
	for (i = 0; i < size; i++)
		out[i] = in[i];
}

// Receives a 16*32 bits seed that is copied to a global variable and used in salsa20
void initializeCipher(uint32_t seed[16]){
	copyArray(currentSeed, seed, 16);
}

// Uses salsa20 to generate randombits and updates the seed with the new result
// Only needs to be called once directly, after that we can use getXRandomBits
void getNextRandomBits(uint32_t out[16], uint32_t seed[16]) { //seed = currentSeed
	salsa20_word_specification(out, seed);

	// The new output is the next seed
	copyArray(seed, out, 16); //now seed becomes out
}

// Gets "numberBits" random bits from the "randomBits", according to those already used ("usedBits")
// and places them in the most significant bits of the "out", starting at the "startPosition"
// The start position is considered from the left to the right (most significant bits)
// "Out" maintains all that it has, the generated bits are xored with "out"
// Automatically generates more random bits, if necessary calling "getNextRandomBits"
int getXRandomBits(uint32_t* out, uint8_t startPosition, uint8_t numberBits, uint32_t randomBits[16], uint16_t* usedBits) {
	// Cannot get more bits than what the output variable can handle
	if (startPosition + numberBits > iBits)
		return EXIT_FAILURE;

	// The randomBits array position to be used
	uint8_t arrayPosition = *usedBits / 32;
	// The used bits in that position
	uint8_t posUsedBits = *usedBits % 32;

	uint8_t i, numbUnnecessary, usefulBitsThisPosition;

	uint32_t thisPosition;

	// Run through the aray until we get all the random bits we need
	// (iBits) / 32 + 1 is the max number of the array positions we might need
	for (i = 0; i < (iBits) / 32 + 1; i++) {
		// If the number of randoms bits left are 0
		// Get next random bits and continue
		// TODO: NO SIDE CHANNEL CORRECT?
		if (*usedBits >= 16 * 32) {
			getNextRandomBits(randomBits, currentSeed);

			arrayPosition = 0;
			*usedBits = 0;
		}

		// Calculate the number of bits that we don't need
		numbUnnecessary = (32 - posUsedBits - numberBits) * (numberBits < 32 - posUsedBits);

		// Remove the used bits on the left
		thisPosition = randomBits[arrayPosition] << posUsedBits;

		// Remove the unnecessary bits on the right
		// Place the needed bits on the right
		thisPosition >>= (numbUnnecessary + posUsedBits);

		// The number of bits we'll use from this position
		usefulBitsThisPosition = 32 - posUsedBits - numbUnnecessary;

		// Place the bits in the position they are required
		thisPosition <<= (iBits - startPosition - usefulBitsThisPosition);

		// Update these values
		numberBits -= usefulBitsThisPosition;
		startPosition += usefulBitsThisPosition;
		arrayPosition++;
		posUsedBits = 0;
		*usedBits += usefulBitsThisPosition;

		// Place the bits in the output position
		*out |= thisPosition;

		if (numberBits == 0)
			break;
		else if (numberBits < 0) {
			fprintf(stderr, "ERROR: Something went wrong!\n");
			getchar();
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}

// Gets "numberBits" random bits from the "randomMatrix" in the "matrixPosition", according to those already used ("usedBits")
// and places them in the most significant bits of the "out", starting at the "startPosition"
// The start position is considered from the left to the right (most significant bits)
// "Out" maintains all that it has, the generated bits are xored with "out"
// Automatically changes the positions of the matrix where the random bits are stored from a previous generation
// but it does not check if there are more positions CAREFULL
int getXRandomBitsMatrix(uint32_t* out, uint8_t startPosition, uint8_t numberBits, uint32_t randomMatrix[8][16], uint8_t matrixPosition, uint16_t usedBits)
{
	// Cannot get more bits than what the output can handle
	if (startPosition + numberBits > iBits)
		return EXIT_FAILURE;

	// The randomBits array position to be used
	uint8_t arrayPosition = usedBits / 32;
	// The used bits in that position
	uint8_t posUsedBits = usedBits % 32;

	uint8_t i, numbUnnecessary, usefulBitsThisPosition;

	uint32_t thisPosition;

	// Run through the aray until we get all the random bits we need
	// (iBits) / 32 + 1 is the max number of the array positions we might need
	for (i = 0; i < (iBits) / 32 + 1; i++) {
		// If the number of randoms bits left are 0
		// Get next random bits and continue
		// TODO: NO SIDE CHANNEL CORRECT?
		if (usedBits >= 16 * 32) {
			matrixPosition++;

			arrayPosition = 0;
			usedBits = 0;
		}

		// Calculate the number of bits that we don't need
		numbUnnecessary = (32 - posUsedBits - numberBits) * (numberBits < 32 - posUsedBits);

		// Remove the used bits on the left
		thisPosition = randomMatrix[matrixPosition][arrayPosition] << posUsedBits;

		// Remove the unnecessary bits on the right
		// Place the needed bits on the right
		thisPosition >>= (numbUnnecessary + posUsedBits);

		// The number of used bits from this position
		usefulBitsThisPosition = 32 - posUsedBits - numbUnnecessary;

		// Place the bits in the position they are required
		thisPosition <<= (iBits - startPosition - usefulBitsThisPosition);

		// Update these values
		numberBits -= usefulBitsThisPosition;
		startPosition += usefulBitsThisPosition;
		arrayPosition++;
		posUsedBits = 0;
		usedBits += usefulBitsThisPosition;

		// Place the bits in the output position
		*out |= thisPosition;

		if (numberBits == 0)
			break;
		else if (numberBits < 0) {
			fprintf(stderr, "ERROR: Something went wrong!\n");
			getchar();
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}

// Returns 1 if the matrix is nonsingular, therefore invertible
// TODO: THIS FUNCTION IS NOT NEEDED IN THE END
short testSingularity(uint32_t* matrix)
{
	uint32_t ulAuxMatrix[iBits];
	uint32_t bitShiftAux = 0x80000000;

	// Copying the matrix to an auxiliary matrix
	copyArray(ulAuxMatrix, matrix, iBits);

	int i, j;

	// Iterate over the lines of the matrix from top to bottom
	for (i = 0; i < iBits - 1; i++) {
		j = i + 1;

		// If the position in the diagonal is zero
		if ((ulAuxMatrix[i] & bitShiftAux) == 0) {
			// Find the line where the 1 on that column is placed
			while (((ulAuxMatrix[j] & bitShiftAux) == 0) && (j < iBits))
				j++;

			if (j >= iBits) {
				fprintf(stderr, "ERROR: Something went wrong!\n");
				getchar();
				exit(EXIT_FAILURE);
			}

			ulAuxMatrix[i] ^= ulAuxMatrix[j];
		}

		for (; j < iBits; j++)
			if ((ulAuxMatrix[j] & bitShiftAux) > 0) {
				ulAuxMatrix[j] ^= ulAuxMatrix[i];
			}

		bitShiftAux >>= 1;
	}

	short sInvertible = 1;
	bitShiftAux = 0x80000000;

	for (i = 0; i < iBits; i++) {
		if ((ulAuxMatrix[i] & bitShiftAux) == 0)
			sInvertible = 0;
		bitShiftAux >>= 1;
	}

	return sInvertible;
}

// A simplification of the process used in "invertMatrix"
// that can be used to invert permutation matrixes with size iBits
void invertPermutationMatrix(uint32_t* permutMatrix)
{
	uint32_t ulAuxMatrix[iBits];
	uint32_t bitShiftAux = 0x80000000;
	uint32_t aux;

	int i, j, selectedRow, row;

	// Copy of the original matrix to an auxilary one
	// The main matrix becomes a diagonal matrix
	for (i = 0; i < iBits; i++) {
		ulAuxMatrix[i] = permutMatrix[i];
		permutMatrix[i] = bitShiftAux;
		bitShiftAux >>= 1;
	}

	bitShiftAux = 0x80000000;

	// Runs through all the matrix lines
	// from top to bottom
	for (i = 0; i < iBits - 1; i++) {
		j = i + 1;

		selectedRow = i;

		for (; j < iBits; j++) {
			row = (ulAuxMatrix[j] & bitShiftAux);

			if (row == 0)
				selectedRow += row;
			else
				selectedRow += (j - i);
		}

		// Exchange the lines
		// Reproduce the same operation on the identity matrix
		// to obtain the inverse
		aux = ulAuxMatrix[i];
		ulAuxMatrix[i] = ulAuxMatrix[selectedRow];
		ulAuxMatrix[selectedRow] = aux;

		aux = permutMatrix[i];
		permutMatrix[i] = permutMatrix[selectedRow];
		permutMatrix[selectedRow] = aux;

		bitShiftAux >>= 1;
	}
} 

// Invert the upper matrix with size iBits
void invertUpperMatrix(uint32_t * upperMatrix)
{
	uint32_t ulAuxMatrix[iBits];
	uint32_t bitShiftAux = 0x80000000;

	int i, j, aux;

	// Copy of the original matrix to an auxilary one
	// The main matrix becomes a diagonal matrix
	for (i = 0; i < iBits; i++) {
		ulAuxMatrix[i] = upperMatrix[i];
		upperMatrix[i] = bitShiftAux;
		bitShiftAux >>= 1;
	}

	bitShiftAux = 0x80000000;

	// Runs through all the matrix lines
	// from top to bottom
	for (i = 0; i < iBits - 1; i++) {
		for (j = i + 1; j < iBits; j++)
			// Remove the 1 on the ith column in all lines other than i
			// Reproduce the same operation on the identity matrix
			// to obtain the inverse
			if ((ulAuxMatrix[j] & bitShiftAux) > 0) {
				ulAuxMatrix[j] ^= ulAuxMatrix[i];
				upperMatrix[j] ^= upperMatrix[i];
			}
			// Dummy operations to prevent side channel attacks
			else {
				aux = 0;

				ulAuxMatrix[j] ^= aux;
				upperMatrix[i] ^= aux;
			}

		bitShiftAux >>= 1;
	}

	bitShiftAux = 0x00000001;

	// Runs through all the matrix lines
	// from bottom to top
	for (i = iBits - 1; i > 0; i--) {
		for (j = i - 1; j >= 0; j--)
			// Remove the 1 on the ith column in all lines other than i
			// Reproduce the same operation on the identity matrix
			// to obtain the inverse
			if ((ulAuxMatrix[j] & bitShiftAux) > 0) {
				ulAuxMatrix[j] ^= ulAuxMatrix[i];
				upperMatrix[j] ^= upperMatrix[i];
			}
			// Dummy operations to prevent side channel attacks
			else {
				aux = 0;

				ulAuxMatrix[j] ^= aux;
				upperMatrix[i] ^= aux;
			}

		bitShiftAux <<= 1;
	}
}


// Our method for matrix generation
// ------------------------------------------------------------------------------------

// Generate a random matrix using our method
void generateRandomMatrix() 
{
	uint8_t i, j;
	uint32_t bitShiftAux;

	uint32_t randomBits[16];
	uint16_t usedBits = 0;

	// In this loop we need exactly iBits + (iBits-1) + (iBits-2) + .... + 1
	// If iBits = 64 -> 2016
	// If iBits = 32 -> 496
	getNextRandomBits(randomBits, currentSeed);

	// Get a matrix with a random upper triangle
	// TODO: Iddeally we could generate a number with this format by scratch
	for (i = 0; i < iBits - 1; i++) {
		ulMatrix[i] = 0;

		// Generate a 32 bits number where the bits < i are 0
		// Those equal to i are 1
		// Those bigger than i are random
		getXRandomBits(&ulMatrix[i], (i + 1), (iBits - (i + 1)), randomBits, &usedBits);

		// Place 1 at the i-th bit
		bitShiftAux = 1 << (iBits - (i + 1));

		// Creates a row in the matrix with the desired properties
		// 1 at the diagonal position and all random bits on the right
		ulMatrix[i] = ulMatrix[i] ^ bitShiftAux;
	}

	// The last line will always starts as 1
	ulMatrix[iBits - 1] = 1;

	uint32_t r;

	// Get a matrix with a random lower triangle
	for (i = iBits - 2; i <= iBits - 2; i--) {
		// Generate a iBits - (i+1) random bits
		r = 0;
		getXRandomBits(&r, (i + 1), (iBits - (i + 1)), randomBits, &usedBits);

		//printf("%d\n", r);

		bitShiftAux = 1 << (i + 1);

		for (j = 0; j < iBits - 1 - i; j++) {
			// If bit == 1
			if (r & bitShiftAux)
				ulMatrix[iBits - 1 - j] = ulMatrix[i] ^ ulMatrix[iBits - 1 - j];
			// Dummy operations to prevent side channel attacks
			else
				ulMatrix[iBits - 1 - j] = ulMatrix[iBits - 1 - j] ^ 0;

			bitShiftAux <<= 1;

			//printf("%d\n", r);
		}
	}

	// Generate a random number between 0 and iBits-1
	// Here we'll need log2(iBits) bits = 6 for iBits = 64
	r = 0;
	
	//             out,     startPosition,            numberBits,      randomBits[16], usedBits) 
	getXRandomBits(&r, iBits - (short)log2(iBits), (short)log2(iBits), randomBits, &usedBits);

	uint32_t ulAux = ulMatrix[0];
	ulMatrix[0] = ulMatrix[r];
	ulMatrix[r] = ulAux;

	// Take care of the mask
	getXRandomBits(&ulMask, 0, iBits, randomBits, &usedBits);
}

// Calculate the inverse matrix for the matrix generated using our method
// by starting with the indentity and using the same random bits used in the generation in a reverse order
void generateInverseMatrix() {
	// Generate and store all the random numbers we'll need
	// Now we'll need 496 * 2 + 6 = 998 -> 1.95 = 2
	uint32_t randomMatrix[2][16];

	uint8_t i, j;
	short sNotSingular = 0;

	for (i = 0; i < 2; i++)
		getNextRandomBits(randomMatrix[i], currentSeed);

	uint32_t bitShiftAux = 0x80000000;

	// Generate the identity matrix
	for (i = 0; i < iBits; i++) {
		ulMatrix[i] = bitShiftAux;

		bitShiftAux >>= 1;
	}

	uint16_t usedBits = 480;
	uint8_t matrixPosition = 1;

	// 4th Step
	// Here we'll need log2(iBits) bits = 5 for iBits = 32
	//496 + 5 < 512
	uint32_t r = 0;
	getXRandomBitsMatrix(&r, iBits - (short)log2(iBits), (short)log2(iBits), randomMatrix, matrixPosition, usedBits);

	uint32_t ulAux = ulMatrix[0];
	ulMatrix[0] = ulMatrix[r];
	ulMatrix[r] = ulAux;

	// Get a matrix with a random lower triangle
	for (i = 0; i < iBits - 1; i++) {
		if (usedBits - (iBits - (i + 1)) < 0) {
			matrixPosition--;

			usedBits = 512 + (usedBits - (iBits - (i + 1)));
		}
		else
			usedBits -= (iBits - (i + 1));

		r = 0;
		getXRandomBitsMatrix(&r, i + 1, (iBits - (i + 1)), randomMatrix, matrixPosition, usedBits);

		bitShiftAux = 1 << (iBits - 1);

		for (j = iBits - 2 - i; j <= iBits - 2 - i; j--) {
			// If bit == 1
			if (r & bitShiftAux)
				ulMatrix[iBits - 1 - j] = ulMatrix[i] ^ ulMatrix[iBits - 1 - j];
			// Dummy operations to prevent side channel attacks
			else
				ulMatrix[iBits - 1 - j] = ulMatrix[iBits - 1 - j] ^ 0;

			bitShiftAux >>= 1;
		}
	}

	// Get a matrix with a random upper triangle
	// TODO: Iddeally we could generate a number with this format by scratch
	for (i = iBits - 2; i <= iBits - 2; i--) {
		if (usedBits - (iBits - (i + 1)) < 0) {
			matrixPosition--;

			usedBits = 512 + (usedBits - (iBits - (i + 1)));
		}
		else
			usedBits -= (iBits - (i + 1));

		// Generate a 32 bits number where the bits < i are 0
		// Those equal to i are 1
		// Those bigger than i are random
		r = 0;
		getXRandomBitsMatrix(&r, (i + 1), (iBits - (i + 1)), randomMatrix, matrixPosition, usedBits);

		bitShiftAux = 1;

		for (j = 0; j < iBits - 1 - i; j++) {
			// If bit == 1
			if (r & bitShiftAux)
				ulMatrix[i] = ulMatrix[i] ^ ulMatrix[iBits - 1 - j];
			// Dummy operations to prevent side channel attacks
			else
				ulMatrix[iBits - 1 - j] = ulMatrix[iBits - 1 - j] ^ 0;

			bitShiftAux <<= 1;
		}
	}

	usedBits = 485;

	// Take care of the mask
	getXRandomBits(&ulMask, 0, iBits, randomMatrix[1], &usedBits);

	sNotSingular = testSingularity(ulMatrix);

	if (!sNotSingular)
		printf("Singular!");
}

// Dana Randall's Nonsingular Matrix Generation
// ------------------------------------------------------------------------------------

// Return the first position in an int that has not yet been captured by any r in allR.
// Tested with all inputs from one matrix.
uint8_t getFirstFreePosition(uint8_t aLine, uint8_t* allR) {
	uint8_t i, x;

	x = 0;

	for (i = 0; i < aLine; i++)
		x += (i >= allR[i]);

	return x;
}

// R is the position of the first 1 in v.
// Tested with all v's from a single matrix.
uint8_t findR(uint32_t v) {
	uint8_t i, x, y;
	uint32_t bitShiftAux = 0x80000000;

	x = 0;
	y = 1;

	for (i = 0; i < iBits; i++){
		// x sums one until (v & bitShiftAux) != 0 for the first time
		x += (((v & bitShiftAux) == 0) && y);

		// Once the (v & bitShiftAux) becomes != 0
		// the y stays false until the end
		y = (((v & bitShiftAux) == 0) && y);

		bitShiftAux >>= 1;
	}

	return x;
}

// Creates an int composed ony by 0's and one 1 at the r'th position.
// Tested with all inputs from one matrix.
uint32_t createEr(uint8_t* r, uint8_t aLine, uint8_t* allR) {
	uint32_t er;
	uint32_t bitShiftAux = 0x80000000;

	uint8_t i, x = 0, available = -1, y = 1, ri = 0;

	for (i = 0; i < iBits; i++) {
		available += (i < allR[x]);
		x += (i >= allR[x]);

		// The new r
		// This sums one until the available == *r
		ri += ((available != *r) && y);

		// Once the available becomes == to *r
		// the y stays false until the end
		y = ((available != *r) && y);

		// After the y becomes false, the bitShiftAux stays the same until the end
		bitShiftAux >>= 1 & y;
	}

	// Place 1 in the r position
	// according to aLine and allR
	er = bitShiftAux;

	// Update r to be the global value not to the local
	*r = ri;

	return er;
}

// Function responsible for adapting v to the previously selected r's in allR.
// Tested with all the inputs from a single matrix.
uint32_t changeV(uint32_t v, uint8_t aLine, uint8_t* allR) {
	uint32_t result = 0, tempV;
	uint32_t bitShiftAux = 0x80000000;

	uint8_t i, o = 0, x = 0;

	for (i = 0; i < iBits; i++) {
		// TODO: CHECK IF THE IF/ELSE IS THE SAME AND REPLACE IT
		tempV = (v & (bitShiftAux ^ ((i >= allR[x]) << (iBits - o - 1))));

		//if (i < allR[x])
		//	tempV = (v & bitShiftAux);
		//else
		//	tempV = 0;

		// TODO: GOOD IF << TAKES AS LONG AS >>
		if (o > i)
			tempV <<= (o - i);
		else
			tempV >>= (i - o);

		result = result ^ tempV;

		// Shift right 1 if (i < allR[x]) is true
		bitShiftAux >>= (i < allR[x]);

		// Sums 1 to x if (i < allR[x]) is true
		o += (i < allR[x]);

		// Sums 1 to x if (i >= allR[x]) is true
		x += (i >= allR[x]);
	}

	return result;
}

// Function responsible for adding a new r to allR.
// Tested for all the inputs in a single matrix generation.
void addR(uint8_t r, uint8_t aLine, uint8_t* allR) {
	uint8_t i, aux;

	// TODO: THERE IS NO PROBLEM IN THIS RUNNING ONLY UNTIL aLine, CORRECT?
	for (i = 0; i < aLine; i++) {
		if (r < allR[i]) {
			aux = allR[i];
			allR[i] = r;

			r = aux;
		}
		// TODO: NOT SURE THIS PREVENTS SIDE CHANNEL ATTACKS HERE
		else {
			aux = r;
			r = allR[i];

			r = aux;
		}
	}

	allR[aLine] = r;
}

// Generate both matrices A and T that when multiplied generate a random matrix
void generateMatrixNDana(uint32_t* A, uint32_t* T, uint8_t n, uint8_t aLine, uint8_t* allR, uint32_t* randomBits, uint16_t usedBits)
{
	uint8_t r;

	uint32_t er;

	uint8_t realPositionL;
	uint8_t realPositionC;

	// TODO: IS THERE A PROBLEM WITH THIS?
	if (n == 1) {
		// Places 1 in the position 0, 0 of the A
		// We need to consider the aLine and allR

		realPositionL = aLine;
		realPositionC = getFirstFreePosition(aLine, allR);

		A[realPositionL] = A[realPositionL] ^ (1 << (iBits - realPositionC - 1));

		T[realPositionC] = T[realPositionC] ^ (1 << (iBits - realPositionC - 1));

		// Take care of the mask
		getXRandomBits(&ulMask, 0, iBits, randomBits, &usedBits);
	}
	else {
		uint32_t v = 0;

		// TODO: IS THERE A PROBLEM WITH THIS?
		// Make sure v is diferent that 0
		while (v == 0) {
			// v will have the important bits on the left
			getXRandomBits(&v, 0, n, randomBits, &usedBits);
		}

		// Find the first position from the left to the right, where v is different than 0
		r = findR(v);

		// Adapt v according to aLine and allR
		v = changeV(v, aLine, allR);

		// Create the number where only the rth bit is 1, all the rest is 0
		// Update r to be the global value not to the local
		er = createEr(&r, aLine, allR);

		// Assign er to the first row of A
		A[aLine] = A[aLine] ^ er;

		// Assign v to the rth row of T
		T[r] = T[r] ^ v;

		// Add the new r to allR
		addR(r, aLine, allR);

		generateMatrixNDana(A, T, n - 1, aLine + 1, allR, randomBits, usedBits);
	}
}

// Calculates the multiplication between two matrices of bits
// where each matrix is a vector of 32 bit integer.
// Tested against two examples using an online calculator
void matrixMultiplication(uint32_t A[iBits], uint32_t T[iBits]) {
	uint8_t i, j, k;
	uint32_t bitShiftAuxi;
	uint32_t bitShiftAuxj;

	uint32_t tempU, tempC, tempA, tempT;

	// This represents the lines of A
	for (k = 0; k < iBits; k++) {
		tempU = 0;

		bitShiftAuxi = 0x80000000;

		// This represents the columns of A and the lines of T
		for (i = 0; i < iBits; i++) {
			tempC = 0;

			bitShiftAuxj = 0x80000000;

			// This represents the columns of T
			for (j = 0; j < iBits; j++) {
				// One element from each of the matrices
				tempA = A[k] & bitShiftAuxj;
				tempT = T[j] & bitShiftAuxi;

				// Align the element from the A matrix to correctly execute the xor
				if (j > i)
					tempA <<= (j - i);
				else
					tempA >>= (i - j);

				bitShiftAuxj >>= 1;

				// Update each of the bits in a line
				tempC = tempC ^ (tempA & tempT);
			}

			bitShiftAuxi >>= 1;

			// Update each of the lines
			tempU = tempU ^ tempC;
		}

		// Set the final value for a line
		ulMatrix[k] = tempU;
	}
}

// Generates a random nonsingular matrix using Dana Randal's algorithm
void generateMatrixDana() { 
	// n*n being the size of the generated matrix
	uint8_t n = iBits;
	
	uint32_t A[iBits];
	uint32_t T[iBits];

	uint8_t i;

	// Fill the matrixes with 0's
	for (i = 0; i < iBits; i++) {
		A[i] = 0;
		T[i] = 0;
	}

	uint8_t aLine = 0;
	uint8_t allR[iBits];

	uint32_t randomBits[16];

	getNextRandomBits(randomBits, currentSeed);

	generateMatrixNDana(A, T, n, aLine, allR, randomBits, 0);

	// The result is
	matrixMultiplication(A, T);

	short sNotSingular = 0;

	sNotSingular = testSingularity(ulMatrix);

	/*********************SHIFT ROWS TO RANDOMIZE THE MATRIX NOT TO BE ALMOST UPPER TRIANGULAR****************/
	// Generate a random number between 0 and iBits-1
	// Here we'll need log2(iBits) bits = 6 for iBits = 64
	uint32_t r1 = 0, r2 =0;
	uint32_t ulAux;
	uint16_t usedBits = 0;
	//getNextRandomBits(randomBits, currentSeed);
	for (int i=0; i<32; i++)
	{ 
	//             out,     startPosition,            numberBits,      randomBits[16], usedBits) 
		getXRandomBits(&r1, iBits - (short)log2(iBits), (short)log2(iBits), randomBits, &usedBits);
		getXRandomBits(&r2, iBits - (short)log2(iBits), (short)log2(iBits), randomBits, &usedBits);

		getXRandomBits(&r1, 29, 3, randomBits, &usedBits);

		printf("%d  ", r1); printf("%d\n", r2);
		
		ulAux = ulMatrix[r1];
		ulMatrix[r1] = ulMatrix[r2];
		ulMatrix[r2] = ulAux;
	}
	/*********************SHIFT ROWS TO RANDOMIZE THE MATRIX NOT TO BE ALMOST UPPER TRIANGULAR****************/

	if (!sNotSingular)
		printf("Singular!\n");

	//print_x32bits_matrix (A); //printMatrix (A); 
	//printMatrix (T);

}

// Generates the inverse matrix for the matrix generated using Dana Randal's algorithm
void generateInverseMatrixDana() {
	// n*n being the size of the generated matrix
	uint8_t n = iBits;

	uint32_t A[iBits];
	uint32_t T[iBits];

	uint8_t i;

	// Fill the matrixes with 0's
	for (i = 0; i < iBits; i++) {
		A[i] = 0;
		T[i] = 0;
	}

	uint8_t aLine = 0;
	uint8_t allR[iBits];

	uint32_t randomBits[16];

	getNextRandomBits(randomBits, currentSeed);

	generateMatrixNDana(A, T, n, aLine, allR, randomBits, 0);

	invertPermutationMatrix(A);

	invertUpperMatrix(T);

	// The result is
	matrixMultiplication(T, A);

	short sNotSingular = 0;

	sNotSingular = testSingularity(ulMatrix);

	if (!sNotSingular)
		printf("Singular!\n");
}

// Main functions
// ------------------------------------------------------------------------------------

// Cipher a 32 bit integer with Hill Cipher
void encrypt(uint32_t * in, uint32_t * out) {
	uint32_t ulOut = 0;
	uint32_t bitShiftAux = 0x80000000;

	// Needed to clean the garbage
	ulMask = 0;

	// These methods include the generation of the mask inside
	// Our method
	generateRandomMatrix();

	// Dana Randall's
	//generateMatrixDana();

	uint32_t ulIn = (*in) ^ ulMask; //Mask the plaintext (in) into ulIn

	uint8_t i = 0;

	for (i = 0; i < iBits; i++) {
		ulOut ^= ulMatrix[i] * ((ulIn & bitShiftAux) > 0); //Multiply plaintext (ulIn) to the key matrix (ulMatrix)

		bitShiftAux >>= 1;
	}

	(*out) = ulOut;
}

// Decipher a 32 bit integer with Hill Cipher
void decrypt(uint32_t * in, uint32_t * out) {
	uint32_t ulOut = 0;
	uint32_t ulIn = (*in);
	uint32_t bitShiftAux = 0x80000000;

	// Needed to clean the garbage
	ulMask = 0;

	// Our method
	generateInverseMatrix();

	// Dana Randall's
	//generateInverseMatrixDana();

	uint8_t i = 0;

	for (i = 0; i < iBits; i++) {
		ulOut ^= ulMatrix[i] * ((ulIn & bitShiftAux) > 0);

		bitShiftAux >>= 1;
	}

	(*out) = ulOut ^ ulMask;
}