// The following code implements the PRR algorithm according //
// to the explanation in the document "Remainders Revolution //
// Pseudo Random Number Generator".                          //
//                                                           //
// PRR is initialized by calling initialize(int iSeed) once; //
// Generation of 31 or 32 bit long integers is made          //
// recurring to PRR31BitsInt() or to PRR32BitsInt().         //
//                                                           //
// This C-program was written by Pedro R. M. Inacio.         //
// Please address any comments or suggestions to             //
// inacioalwaysright@gmail.com.                              //

#include <stdio.h>

#define iMP 0x7fffffff // A Mersenne Prime.
#define iC 729 // The width of the table (=3^6).
#define iCdiv 243 // Convenience variable (=3^5).

static unsigned int iX0; static unsigned int iW0;
static unsigned int iA3; static unsigned int iA4;
static unsigned int iXn_1; static unsigned int iWn_1;
static unsigned int iYn_1; static unsigned int iZn_1;
static unsigned int iR; static unsigned int iL;
static unsigned int iaSeedFactors[iC];

void initializePRR(unsigned long iSeed)
{
 iL=0;
 iR=0;
 // Generation of seeds and factors.
 // The LCG iUn_1 = 1664525 * iUn + 1013904223 (used to
 // initialize the several parameters) is known to be a fairly
 // good generator from W. Press, S. Teukolsky, W. Vetterling,
 // B.Flannery, Numerical Recipes in C, 2nd Edition, Cambridge
 // University Press, Cambridge, UK, 1992.
 unsigned int iUn_1 = 1664525 * iSeed + 1013904223;
 iA3 = (iUn_1 % iCdiv)*3 + 1;

 iUn_1 = 1664525 * iUn_1 + 1013904223;
 iA4 = (iUn_1<<2) + 1;
 
 iUn_1 = 1664525 * iUn_1 + 1013904223;
 iXn_1 = iUn_1 % iC;
 iX0 = iXn_1;

 iUn_1 = 1664525 * iUn_1 + 1013904223;
 iYn_1 = (iUn_1 % iCdiv)*3 + 1;

 iUn_1 = 1664525 * iUn_1 + 1013904223;
 iWn_1 = iUn_1 & iMP;
 iW0 = iWn_1;

 iUn_1 = 1664525 * iUn_1 + 1013904223;
 iZn_1 = ((iUn_1 & (iMP>>2))<<2)+1;

 // Initialization of the columns of the prime remainders
 // table (seed factors). A carefree initialization procedure
 // is achieved by setting the length of the table to a prime
 // number.
 int i = 0;
 for (i = 0; i < iC; i++) {
     iUn_1 = 1664525 * iUn_1 + 1013904223;
     if( (iUn_1 % iMP) == 0)
       iUn_1 = 1664525 * iUn_1 + 1013904223;
     iaSeedFactors[i] = iUn_1 % iMP;
 }
}

// The following procedure generates 31 bit long integers
// according to the PRR rationale.
int PRR31BitsInt ()
{
 long long lKapa = (long long) iaSeedFactors[iXn_1] * (long long) iWn_1;
 unsigned int iR = (lKapa & iMP) + (lKapa >> 31);
 
 // Column selection
 iXn_1 = (iXn_1 * iYn_1 + 1) % iC;

 // Line selection
 iWn_1 = (iWn_1 * iZn_1 + 1) & iMP;
 
 if (iX0 == iXn_1){
     iYn_1 = (((iYn_1-1)/3 * iA3 + 1) % iC ) * 3 + 1;
     if (iW0 == iWn_1) {
         iZn_1 = ((((iZn_1>>2) * iA4 + 1) & (iMP>>2))<< 2) + 1;
         // If the length of the period is to be extended, the
         // additional if statements should be included here...
     }
 }
return (iR & iMP) + (iR >> 31);
}


// The following function generates 32 bit long integers. It
// calls PRR31BitsInt () at least once per execution. If
// required, it calls PRR31BitsInt () twice, complementing
// the 31 bit long output of one of calls with 1 bit of the
// other. The remaining bits are stored in the lR variable.
int get32BitsInt()
{
 int iR2 = iR;
 if (iL == 0) {
     iR2 = PRR31BitsInt();
     iR = iR2 >> 1;
     iL = 30;
     return (PRR31BitsInt()<<1) ^ iR2;
 }

 iL--;
 iR = iR2 >> 1;

 return (PRR31BitsInt()<<1) ^ iR2;
}

// This main initializes PRR with the current system time and
// prints the first 1000 generated 32 bit long integers.
int mainPRR(int argc, char *argv[])
{
 int i;
 int iSeed = time(0);

 initializePRR(iSeed);
 printf("PRR initialized with %d.\n\n", iSeed);

 for( i = 0; i < 1000; i++)
   printf("%10d \n",get32BitsInt());
 printf("\n !Done!");

 return 0;
}
