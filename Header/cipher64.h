#pragma once

// The block size to use in all cryptosystem
#define iBits 64

//To use for creating seed according Salsa20
#define iKeySize 16

// Auxilary functions
// ------------------------------------------------------------------------------------

// Assumes little endian
// http://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
// Enno Gröper
void printBits(size_t const size, void const * const ptr);

// Print a matrix of bits which, in this case, is an array of 64 bits int
// printing each int with "printBits"
void printMatrix(uint64_t matrix[iBits]);

// Copies the values between two arrays of size "size" bits, if inferior to 64
void copyArray64(uint64_t *out, uint64_t *in, size_t size);

// Receives a 16*32 bits seed (as an array) that is copied to a global variable and used in salsa20
void initializeCipher(uint32_t seed[16]);

// Uses salsa20 to generate randombits and updates the seed with the new result
void getNextRandomBits(uint32_t out[16], uint32_t seed[16]);

// Gets "numberBits" random bits from the "randomBits", according to those already used ("usedBits")
// and places them in the most significant bits of the "out", starting at the "startPosition"
// The start position is considered from the left to the right (most significant bits)
// "out" maintains all that it has, the generated bits are xored with "out"
// Automatically generates more random bits, if necessary calling "getNextRandomBits"
int getXRandomBits(uint64_t * out, uint8_t startPosition, uint8_t numberBits, uint32_t randomBits[16], uint16_t * usedBits);

// Gets "numberBits" random bits from the "randomMatrix" in the "matrixPosition", according to those already used ("usedBits")
// and places them in the most significant bits of the "out", starting at the "startPosition"
// The start position is considered from the left to the right (most significant bits)
// "Out" maintains all that it has, the generated bits are xored with "out"
// Automatically changes the positions of the matrix where the random bits are stored from a previous generation
// but it does not check if there are more positions (CAREFULL!)
int getXRandomBitsMatrix(uint64_t * out, uint8_t startPosition, uint8_t numberBits, uint32_t randomMatrix[8][16], uint8_t matrixPosition, uint16_t usedBits);

// Returns 1 if the matrix is nonsingular, therefore invertible
short testSingularity(uint64_t * matrix);

// A simplification of the process used in "invertMatrix"
// that can be used to invert permutation matrixes with size iBits
void invertPermutationMatrix(uint64_t * permutMatrix);

// A simplification of the process used in "invertMatrix"
// that can be used to invert upper triangular matrixes with size iBits
void invertUpperMatrix(uint64_t * upperMatrix);

// Our method for matrix generation
// ------------------------------------------------------------------------------------

// Generate a random matrix using our method
void generateRandomMatrix();

// Calculate the inverse matrix for the matrix generated using our method
// by starting with the indentity and using the same random bits used in the generation in a reverse order
void generateInverseMatrix();

// Dana Randall's Nonsingular Matrix Generation
// ------------------------------------------------------------------------------------

// Return the first position in an int that has not yet been captured by any r in allR.
// Tested with all inputs from one matrix.
uint8_t getFirstFreePosition(uint8_t aLine, uint8_t* allR);

// R is the position of the first 1 in v.
// Tested with all v's from a single matrix.
uint8_t findR(uint64_t v);

// Creates an int composed ony by 0's and one 1 at the r'th position.
// Tested with all inputs from one matrix.
uint64_t createEr(uint8_t* r, uint8_t aLine, uint8_t* allR);

// Function responsible for adapting v to the previously selected r's in allR.
// Tested with all the inputs from a single matrix.
uint64_t changeV(uint64_t v, uint8_t aLine, uint8_t* allR);

/****************************************************************************/
// Function responsible for adding a new r to allR.
// Copies the values between two arrays of size "size" bits, if inferior to 64
void copyArray64(uint64_t * out, uint64_t * in, size_t size);
/****************************************************************************/

// Function responsible for adding a new r to allR.
// Tested for all the inputs in a single matrix generation.
void addR(uint8_t r, uint8_t aLine, uint8_t* allR);

// Generate both matrices A and T that when multiplied generate a random matrix
void generateMatrixNDana(uint64_t* A, uint64_t* T, uint8_t n, uint8_t aLine, uint8_t* allR, uint32_t* randomBits, uint16_t usedBits);

// Generates a random nonsingular matrix using Dana Randal's algorithm
void generateMatrixDana();

// Generates the inverse matrix for the matrix generated using Dana Randal's algorithm
void generateInverseMatrixDana();

// Calculates the multiplication between two matrices of bits
// where each matrix is a vector of 32 bit integer.
// Tested against two examples using an online calculator
void matrixMultiplication(uint64_t A[iBits], uint64_t T[iBits]);

// Main functions
// ------------------------------------------------------------------------------------

// Cipher a 32 bit integer with Hill Cipher
void encrypt(uint64_t* in, uint64_t* out);

// Decipher a 32 bit integer with Hill Cipher
void decrypt(uint64_t* in, uint64_t* out);