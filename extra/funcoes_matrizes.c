#include <stdio.h>
#include <stdlib.h> //calloc is also here
#include <math.h>


//OBSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
// void *
//Declarar o valor de retorno das funções como void *, bem como os parâmetros.
//Ao utilizar basta fazer (cast) para o tipo específco. A função free, por exemplo recebe void *
//A fução malloc, por exemplo, devolve void*

/*VECTORS*/

void * create_vector (unsigned int n, unsigned int nbytes) //Return the pointer to the first element on the memory sequency if there is space or NULL otherwise
{
	void * vector = malloc (n * nbytes);
	return vector;
}//In place of int here we can put void to generalize...

void * create_inicialized_vector (unsigned int n, unsigned int nbytes) //Instead of malloc, in this case we will use calloc in which all bits in the memory inicialized with 0's
{
        void * vector = calloc (n, nbytes); //NULL if there is no enough space
        return vector;//All bits in the memory inicialized with 0's               
}

void * realloc_vector (void * vector, unsigned int n2, unsigned nbytes2) //Change the dimension of the vector maintainning the remaining values
{
	void * vector2 = (int *) realloc (vector, n2 * nbytes2);
	return vector2;
}

void free_vector (void * vector)
{
	free (vector);
}

void put_vector (void * vector, unsigned int pos, void * element)
{
	//vector[pos] = *element; //void is not isignable?
}

void print_vector (void * vector, unsigned int length)
{
        for (int i = 0; i<length; i++)
        {
              //printf ("%p ", vector[i]); //incomplete
        }
}

/*VECTORS*/


/*MATRICES*/

int ** create_matrix (unsigned int m, unsigned int n) //Return the pointer to the first element on the memory sequency if there is space or NULL otherwise
{
        int ** matrix = (int **) malloc (m * sizeof (int *)); //NULL if there is no enough space
	for (int i = 0; i<m; i++)
		matrix[i] = (int *) malloc (n * sizeof (int)); //NULL if there is no enough space
	return matrix;
}//In place of int here we can put void to generalize...

int ** create_inicialized_matrix (unsigned int m, unsigned int n, int e) //Instead of malloc, in this case we will use calloc in which all bits in the memory inicialized with 0's
{
    int ** matrix = (int **) malloc (m * sizeof (int *)); //NULL if there is no enough space
    if (e == 0)
    	for (int i = 0; i<m; i++)
            matrix[i] = (int *) calloc (n, sizeof (int)); //NULL if there is no enough space
    else
    {
        for (int i = 0; i<m; i++)
            matrix[i] = (int *) malloc (n * sizeof (int)); //NULL if there is no enough space
        for (int i = 0; i<m; i++)
            for (int j = 0; j<n; j++)
                matrix[i][j] = e;
    }
    return matrix;//All bits in the memory inicialized with 0's
}

int ** realloc_matrix (int ** matrix, unsigned int m1, unsigned int m2, unsigned int n2) //Change the dimension of the vector maintainning the remaining values
{//???????????????????????????????????????????????????????????????????????????????????????????
    int ** matrix2 = (int **) realloc (matrix, m2 * sizeof (int));
	for (int i = 0; i<m1; i++) //The earlier locations with values
        matrix2[i] = (int *) realloc (matrix[i], n2 * sizeof (int)); //NULL if there is no enough space
    for (int i = m1; i<m2; i++) //The earlier locations with values
        matrix2[i] = (int *) calloc (n2, sizeof (int));
    return matrix2;
}

void free_matrix (int **  matrix, unsigned int m)
{
	for(int i=0; i<m; i++)
		free(matrix[i]); //free each line
	free (matrix); //free the vector of lines...
}

void put_matrix (int ** matrix, unsigned int m1, unsigned int n1, unsigned int element)
{
        matrix[m1][n1] = element;
}

void print_matrix (int ** matrix, unsigned int m, unsigned int n)
{
        for (int i = 0; i<m; i++)
	{
		for(int j=0; j<n; j++)
			printf ("%i ", matrix[i][j]);
		printf("\n");
	}
    printf("\n");
}

int ** create_identity (unsigned int m) //For square matrix
{
        int ** identity = create_inicialized_matrix (m, m, 0);
        for (int i=0; i<m; i++)
                put_matrix(identity, i, i, 1); //Identity
        return identity;
}


/*MATRICES*/

//OPERATIONS ON NORMAL MATRICES

short compare_matrix (int ** m1, int ** m2, unsigned int l, unsigned int c) //As m is square, m is a n*n matrix
{
    for (int i=0; i<l; i++)
        for (int j=0; j<c; j++)
            if (m1[i][j] != m2[i][j])
                return 0;
    return 1;
}

int ** matrix_sum (int ** m1, int ** m2, int l, int c)
{
    int ** sum = create_matrix (l, c);
    for(int i = 0; i < l; i++) //for each row (l)
        for (int j = 0; j < c; j++) //for each column
            sum [i][j] = m1 [i][j] + m2 [i][j]; //Sum the elements in the same position ij
    return sum;
}

int ** matrix_subtract (int ** m1, int ** m2, int l, int c)
{
    int ** subtract = create_matrix (l, c);
    for(int i = 0; i < l; i++) //for each row (l)
        for (int j = 0; j < c; j++) //for each column
            subtract [i][j] = m1 [i][j] - m2 [i][j]; //Sum the elements in the same position ij
    return subtract;
}

int ** matrix_multiply (int ** m1, int ** m2, int l1, int c1, int c2) //c1 = l2
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    int ** mult = create_inicialized_matrix (l1, c2, 0);
    for(int i = 0; i < l1; i++) //for each row (l1)
        for (int j = 0; j < c2; j++) //for each column in m2 
            for (int k = 0; k < c1; k++) //c1 = l2 (for each column in m1 = each line in m2)
                mult [i][j] += m1 [i][k] * m2 [j][k]; //Sum the elements in the same position ij
    return mult;
}

int ** matrix_multiply_scalar (int ** m, int s, int l, int c)
{
    int ** mult = create_matrix (l, c);
    for(int i = 0; i < l; i++) //for each row (l)
        for (int j = 0; j < c; j++) //for each column
            mult [i][j] = m [i][j] * s; //Sum the elements in the same position ij
    return mult;
}

int matrix_determinant (int ** m, int n) //As m is square, m is a n*n matrix
{
    int determinant = 0;
    //...
    return determinant;
}

int ** matrix_inverse (int ** m, int n) //As m is square, m is a n*n matrix
{
    int ** inverse = create_matrix (n, n);
    //...
    return inverse;
}

short is_nonsingular_matrix (int ** m, int n) //As m is square, m is a n*n matrix
{
    if (matrix_determinant (m, n) != 0)
        return 1;
    else
        return 0;
}

short is_self_invertible_matrix (int ** m, int x)
{
    int ** identity = create_identity (x); 
    int ** multiply = matrix_multiply (m, m, x, x, x); //in this case l1=c1=l2=c2=x
    if (compare_matrix(multiply, identity, x, x)) 
        return 1; //m * m = I -> m = m^-1
    else
        return 0;
}

// END OF THE /OPERATIONS ON NORMAL MATRICES


/*SIMPLE MATRICES*/

int * create_simple_matrix (unsigned int m, unsigned int n) //Return the pointer to the first element on the memory sequency if there is space or NULL otherwise
{
        int * matrix = (int *) malloc (m * n * sizeof (int)); //NULL if there is no enough space
        return matrix;
}//In place of int here we can put void to generalize...


int * create_inicialized_simple_matrix (unsigned int m, unsigned int n, int e) //Instead of malloc, in this case we will use calloc in which all bits in the memory inicialized with 0's
{
    int * matrix;
    if (e==0)
        matrix = (int *) calloc (m * n, sizeof (int *)); //NULL if there is no enough space
    else
    {
        matrix = (int *) malloc (m * n * sizeof (int)); 
        int limit = m * n;
        for (int i = 0; i<limit; i++)
            matrix[i] = e;
    }
        return matrix;//All bits in the memory inicialized with 0's
}


int * realloc_simple_matrix (int * matrix, unsigned int m2, unsigned int n2) //Change the dimension of the vector maintainning the remaining values
{
        int * matrix2 = (int *) realloc (matrix, m2 * n2 * sizeof (int));
        return matrix2;
}



void put_simple_matrix (int * matrix, unsigned int n,  unsigned int i, unsigned int j, int element)
{
        matrix[n*i+j] = element; //here n is the number of column
}


int get_simple_matrix (int * matrix, unsigned int n,  unsigned int i, unsigned int j)
{
        return matrix[n*i+j]; //here n is the number of column
}


void print_simple_matrix (int * matrix, unsigned int m, unsigned int n)
{
        for (int i = 0; i<m; i++)
    {
        for (int j=0; j<n; j++)
            printf("%i  ", matrix[n*i+j]); //here n is the number of column
        printf("\n");
    }
}


/*SIMPLE MATRICES*/

//OPERATIONS ON SIMPLE MATRICES
short compare_simple_matrix (int * m1, int * m2, unsigned int l, unsigned int c) //As m is square, m is a n*n matrix
{
    int limit = l * c;
    for (int i=0; i<limit; i++)
        if (m1[i] != m2[i])
            return 0;
    return 1;
}

int * simple_matrix_sum (int * m1, int * m2, int l, int c)
{
    int * sum = create_simple_matrix (l, c);
    for(int i = 0; i < l; i++) //for each row (l)
        for (int j = 0; j < c; j++) //for each column
            sum[c*i+j] = m1 [c*i+j] + m2 [c*i+j];//Sum the elements in the same position ij
    return sum;
}

int * simple_matrix_subtract (int * m1, int * m2, int l, int c)
{
    int * subtract = create_simple_matrix (l, c);
    for(int i = 0; i < l; i++) //for each row (l)
        for (int j = 0; j < c; j++) //for each column
            subtract[c*i+j] = m1 [c*i+j] - m2 [c*i+j];//Sum the elements in the same position ij
    return subtract;
}

int * simple_matrix_multiply (int * m1, int * m2, int l1, int c1, int c2) //c1 = l2
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    int * mult = create_inicialized_simple_matrix (l1, c2, 0);
    for(int i = 0; i < l1; i++) //for each row (l1)
        for (int j = 0; j < c2; j++) //for each column in m2 
            for (int k = 0; k < c1; k++) //c1 = l2 (for each column in m1 = each line in m2)
                mult [c2*i+j] += m1 [c1*i+k] * m2 [c2*k+j]; //Sum the elements in the same position ij
    return mult;
}

int * simple_matrix_multiply_scalar (int * m, int s, int l, int c)
{
    int * mult = create_simple_matrix (l, c);
    for(int i = 0; i < l; i++) //for each row (l)
        for (int j = 0; j < c; j++) //for each column
            mult [c*i+j] = m [c*i+j] * s; //Sum the elements in the same position ij
    return mult;
}


// END OF THE /OPERATIONS ON SIMPLE MATRICES


/*SQUARE MATRICES*/

int * create_square_matrix (unsigned int m) //Return the pointer to the first element on the memory sequency if there is space or NULL otherwise
{
        int * matrix = (int *) malloc (m * m * sizeof (int)); //NULL if there is no enough space
        return matrix;
}//In place of int here we can put void to generalize...


int * create_inicialized_square_matrix (unsigned int m, int e) //Instead of malloc, in this case we will use calloc in which all bits in the memory inicialized with 0's
{
    int * matrix;
    if (e == 0)
        matrix = (int *) calloc (m * m, sizeof (int)); //NULL if there is no enough space
    else
    {
        matrix = (int *) malloc (m * m * sizeof (int)); 
        int limit = m * m;
        for (int i = 0; i<limit; i++)
            matrix[i] = e;
    }
        return matrix;//All bits in the memory inicialized with 0's
}


int * realloc_square_matrix (int * matrix, unsigned int m2) //Change the dimension of the vector maintainning the remaining values
{
        int * matrix2 = (int *) realloc (matrix, m2 * m2 * sizeof (int));
        return matrix2;
}


void put_square_matrix (int * matrix, unsigned int m,  unsigned int i, unsigned int j, int element)
{
        matrix[m*i+j] = element; //here m is the number of column
}


int get_square_matrix (int * matrix, unsigned int m,  unsigned int i, unsigned int j)
{
        return matrix[m*i+j]; //here m is the number of column
}


void print_square_matrix (int * matrix, unsigned int m)
{
        for (int i = 0; i<m; i++)
	{
		for (int j=0; j<m; j++)
			printf("%i  ", matrix[m*i+j]); //For non-square matrix the formula shoud be another, because each column has not the same number of elements that the rows have.
		printf("\n");
	}
}

void print_square_matrix2 (int * matrix, unsigned int m)
{
	int limit = m * m; //Only one multiplication instead of making in each comparison
        printf("%i  ", matrix[0]);
        for (int i = 1; i<limit; i++) 
        {
            if (i%m == 0) //Chancge row when we find a multiple of m
			     printf("\n");
            printf("%i  ", matrix[i]); //For non-square matrix the formula shoud be another, because each column has not the same number of elements that the rows $
           
        }
}


int * create_square_identity (unsigned int m) //For square matrix
{
        int * identity = create_inicialized_square_matrix (m, 0);
        int limit = m * m; //Only one multiplication instead of making in each comparison
        for (int i = 0; i<limit; i++)
                put_square_matrix(identity, m, i, i, 1); //Identity
        return identity;
}

/*SQUARE MATRICES*/

//OPERATIONS ON SQUARE MATRICES
short compare_square_matrix (int * m1, int * m2, int n) //As m is square, m is a n*n matrix
{
    int limit = n * n;
    for (int i=0; i<limit; i++)
        if (m1[i] != m2[i])
            return 0;
    return 1;
}

int * square_matrix_sum (int * m1, int * m2, int n)
{
    int * sum = create_square_matrix (n);
    for(int i = 0; i < n; i++) //for each row (l)
        for (int j = 0; j < n; j++) //for each column
            sum[n*i+j] = m1 [n*i+j] + m2 [n*i+j];//Sum the elements in the same position ij
    return sum;
}

int * square_matrix_subtract (int * m1, int * m2, int n)
{
    int * subtract = create_square_matrix (n);
    for(int i = 0; i < n; i++) //for each row (l)
        for (int j = 0; j < n; j++) //for each column
            subtract[n*i+j] = m1 [n*i+j] - m2 [n*i+j];//Sum the elements in the same position ij
    return subtract;
}

int * square_matrix_multiply (int * m1, int * m2, int n)
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    int * mult = create_inicialized_square_matrix (n, 0);
    for(int i = 0; i < n; i++) //for each row (l1)
        for (int j = 0; j < n; j++) //for each column in m2 
            for (int k = 0; k < n; k++) //c1 = l2 (for each column in m1 = each line in m2)
                mult [n*i+j] += m1 [n*i+k] * m2 [n*k+j]; //Sum the elements in the same position ij
    return mult;
}

int * square_matrix_multiply_scalar (int * m, int s, int n)
{
    int * mult = create_square_matrix (n);
    for(int i = 0; i < n; i++) //for each row (l)
        for (int j = 0; j < n; j++) //for each column
            mult [n*i+j] = m [n*i+j] * s; //Sum the elements in the same position ij
    return mult;
}

int square_matrix_determinant (int * m, int n) //As m is square, m is a n*n matrix
{
    int determinant = 0;
    //...
    return determinant;
}

int * square_matrix_inverse (int * m, int n) //As m is square, m is a n*n matrix
{
    int * inverse = create_square_matrix (n);
    //...
    return inverse;
}

short is_nonsingular_simple_matrix (int * m, int n) //As m is square, m is a n*n matrix
{
    if (square_matrix_determinant (m, n) != 0)
        return 1;
    else
        return 0;
}

short is_self_invertible_square_matrix (int * m, int n)
{
    int * identity = create_square_identity (n);
    int * multiply = square_matrix_multiply (m, m, n); //Self multiply
    if (compare_square_matrix(multiply, identity, n)) 
        return 1; //m * m = I -> m = m^-1
    else
        return 0;
}

// END OF THE /OPERATIONS ON SQUARE MATRICES

/*****************************************************************************32 BITS MATRICES*********************************************************************************/
uint32_t * create_x32bits_matrix () //Return the pointer to the first element on the memory sequency if there is space or NULL otherwise
{
    uint32_t * matrix = (uint32_t *) malloc (32 * sizeof (uint32_t)); //Each of 32 integers contains one row of the matrix with 32 bits
    return matrix;

}

uint16_t * create_x16bits_matrix () //Return the pointer to the first element on the memory sequency if there is space or NULL otherwise
{
    uint16_t * matrix = (uint16_t *) malloc (16 * sizeof (uint16_t)); //Each of 32 integers contains one row of the matrix with 32 bits
    return matrix;

}

uint8_t * create_x8bits_matrix () //Return the pointer to the first element on the memory sequency if there is space or NULL otherwise
{
    uint8_t * matrix = (uint8_t *) malloc (8 * sizeof (uint8_t)); //Each of 32 integers contains one row of the matrix with 32 bits
    return matrix;

}

uint32_t * create_inicialized_x32bits_matrix (uint8_t b) //e is 1 or 0
{//In place of int here we can put void to generalize...
    uint32_t * matrix;
    if (b == 0)
        matrix = (uint32_t *) calloc (32, sizeof (uint32_t)); //all bits inicialized with 0
    else
    {
        matrix = (uint32_t *) malloc (32 * sizeof (uint32_t)); 
        for (uint8_t i = 0; i<32; i++)
            matrix[i] = 0xffffffff; //-1 is the bigest negative formed by 32 1's. The msb 1 is the sign -
            //matrix[i] = 0b01111111111111111111111111111111; //-1 is the bigest negative formed by 32 1's. The msb 1 is the sign -
    }
    return matrix;//All bits in the memory inicialized with 0's
}

uint16_t * create_inicialized_x16bits_matrix (uint8_t b) //e is 1 or 0
{//In place of int here we can put void to generalize...
    uint16_t * matrix;
    if (b == 0)
        matrix = (uint16_t *) calloc (16, sizeof (uint16_t)); //all bits inicialized with 0
    else
    {
        matrix = (uint16_t *) malloc (16 * sizeof (uint16_t)); 
        for (uint8_t i = 0; i<16; i++)
            matrix[i] = 0xffff; //-1 is the bigest negative formed by 32 1's. The msb 1 is the sign -
            //matrix[i] = 0b01111111111111111111111111111111; //-1 is the bigest negative formed by 32 1's. The msb 1 is the sign -
    }
    return matrix;//All bits in the memory inicialized with 0's
}

uint8_t * create_inicialized_x8bits_matrix (uint8_t b) //e is 1 or 0
{//In place of int here we can put void to generalize...
    uint8_t * matrix;
    if (b == 0)
        matrix = (uint8_t *) calloc (8, sizeof (uint8_t)); //all bits inicialized with 0
    else
    {
        matrix = (uint8_t *) malloc (8 * sizeof (uint8_t)); 
        for (uint8_t i = 0; i<8; i++)
            matrix[i] = 0xff; //-1 is the bigest negative formed by 32 1's. The msb 1 is the sign -
            //matrix[i] = 0b01111111111111111111111111111111; //-1 is the bigest negative formed by 32 1's. The msb 1 is the sign -
    }
    return matrix;//All bits in the memory inicialized with 0's
}

uint8_t get_x32bits_matrix (uint32_t * matrix, uint8_t i, uint8_t j)
{
    uint32_t r = matrix[i] >> (31-j); //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
    if(r & 1) // 1 = 0000000000.....1. 
        return 1;
    else
        return 0;
}

uint8_t get_x16bits_matrix (uint16_t * matrix, uint8_t i, uint8_t j)
{
    uint32_t r = matrix[i] >> (15-j); //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
    if(r & 1) // 1 = 0000000000.....1. 
        return 1;
    else
        return 0;
}

uint8_t get_x8bits_matrix (uint8_t * matrix, uint8_t i, uint8_t j)
{
    uint8_t r = matrix[i] >> (8-j); //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
    if(r & 1) // 1 = 0000000000.....1. 
        return 1;
    else
        return 0;
}

uint8_t get_x32bits (uint32_t m, uint8_t j)
{
    uint32_t r = m >> (31-j); //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
    if(r & 1) // 1 = 0000000000.....1. 
        return 1;
    else
        return 0;
}

uint8_t get_x16bits (uint16_t m, uint8_t j)
{
    uint16_t r = m >> (15-j); //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
    if(r & 1) // 1 = 0000000000.....1. 
        return 1;
    else
        return 0;
}

uint8_t get_x8bits (uint8_t m, uint8_t j)
{
    uint8_t r = m >> (7-j); //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
    if(r & 1) // 1 = 0000000000.....1. 
        return 1;
    else
        return 0;
}

void put_x32bits_matrix (uint32_t * matrix, uint8_t i, uint8_t j, uint8_t bit)
{
    if (bit == 0)
    {
        if (get_x32bits_matrix (matrix, i, j) == 1) //If he bit is already 0 then do nothing
           matrix [i] = (matrix [i]) ^ (uint32_t) (pow(2, 31-j)); //Change the j-th bit 1 to 0 making inclusive or with 1  
    } //pow (2,31) = msb = 1
    else //i = 1  
       matrix [i] = (matrix [i]) | (uint32_t)(pow(2, 31-j)); //Change the j-th bit to 1 making inclusive or with 1   
}

void put_x16bits_matrix (uint16_t * matrix, uint8_t i, uint8_t j, uint8_t bit)
{
    if (bit == 0)
    {
        if (get_x16bits_matrix (matrix, i, j) == 1) //If he bit is already 0 then do nothing
           matrix [i] = (matrix [i]) ^ (uint16_t) (pow(2, 15-j)); //Change the j-th bit 1 to 0 making inclusive or with 1  
    } //pow (2,31) = msb = 1
    else //i = 1  
       matrix [i] = (matrix [i]) | (uint16_t)(pow(2, 15-j)); //Change the j-th bit to 1 making inclusive or with 1   
}

void put_x8bits_matrix (uint8_t * matrix, uint8_t i, uint8_t j, uint8_t bit)
{
    if (bit == 0)
    {
        if (get_x8bits_matrix (matrix, i, j) == 1) //If he bit is already 0 then do nothing
           matrix [i] = (matrix [i]) ^ (uint8_t) (pow(2, 7-j)); //Change the j-th bit 1 to 0 making inclusive or with 1  
    } //pow (2,31) = msb = 1
    else //i = 1  
       matrix [i] = (matrix [i]) | (uint8_t)(pow(2, 7-j)); //Change the j-th bit to 1 making inclusive or with 1   
}

void put_x32bits (uint32_t * m, uint8_t j, uint8_t bit)
{
    if (bit == 0)
    {
        if (get_x32bits (*m, j) == 1) //If he bit is already 0 then do nothing
           *m = *m ^ (uint32_t) (pow(2, 31-j)); //Change the j-th bit 1 to 0 making inclusive or with 1  
    } //pow (2,31) = msb = 1
    else //i = 1  
       *m = *m | (uint32_t)(pow(2, 31-j)); //Change the j-th bit to 1 making inclusive or with 1   
}

void put_x16bits (uint16_t * m, uint8_t j, uint8_t bit)
{
    if (bit == 0)
    {
        if (get_x16bits (*m, j) == 1) //If he bit is already 0 then do nothing
           *m = *m ^ (uint16_t) (pow(2, 15-j)); //Change the j-th bit 1 to 0 making inclusive or with 1  
    } //pow (2,31) = msb = 1
    else //i = 1  
       *m = *m | (uint16_t)(pow(2, 15-j)); //Change the j-th bit to 1 making inclusive or with 1   
}


void put_x8bits (uint8_t * m, uint8_t j, uint8_t bit)
{
    if (bit == 0)
    {
        if (get_x8bits (*m, j) == 1) //If he bit is already 0 then do nothing
           *m = *m ^ (uint8_t) (pow(2, 7-j)); //Change the j-th bit 1 to 0 making inclusive or with 1  
    } //pow (2,31) = msb = 1
    else //i = 1  
       *m = *m | (uint8_t)(pow(2, 7-j)); //Change the j-th bit to 1 making inclusive or with 1   
}


void print_x32bits_matrix (uint32_t * matrix)
{
    for (uint8_t j = 0; j<32; j++)
    {  
        uint32_t r; // shift
        for(short i = 31; i > -1; i--) {
            r = matrix[j] >> i; //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
            if(r & 1) // 1 = 0000000000.....1. 
                printf("1 ");
            else
                printf("0 ");
        }
        printf("\n");  
    }
}


void print_x16bits_matrix (uint16_t * matrix)
{
    for (uint8_t j = 0; j<16; j++)
    {  
        uint16_t r; // shift
        for(short i = 15; i > -1; i--) {
            r = matrix[j] >> i; //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
            if(r & 1) // 1 = 0000000000.....1. 
                printf("1 ");
            else
                printf("0 ");
        }
        printf("\n");  
    }
}

void print_x8bits_matrix (uint8_t * matrix)
{
    for (uint8_t j = 0; j<8; j++)
    {  
        uint8_t r; // shift
        for(short i = 7; i > -1; i--) {
            r = matrix[j] >> i; //Primeiro empurramos o msb para a 1ª posição (31 passos), depois o que vem a segir (30 passos) e assim sucessivamente  
            if(r & 1) // 1 = 0000000000.....1. 
                printf("1 ");
            else
                printf("0 ");
        }
        printf("\n");  
    }
}

void print_x32bits (uint32_t m)
{
        uint32_t r; // shift
        for(short i = 31; i > -1; i--) 
        {
            r = m >> i; 
            if(r & 1) // 1 = 0000000000.....1. 
                printf("1  ");
            else
                printf("0  ");
        }
        printf("\n\n");
}

void print_x16bits (uint16_t m)
{
        uint16_t r; // shift
        for(short i = 15; i > -1; i--) 
        {
            r = m >> i; 
            if(r & 1) // 1 = 0000000000.....1. 
                printf("1  ");
            else
                printf("0  ");
        }
        printf("\n\n");
}

void print_x8bits (uint8_t m)
{
        uint8_t r; // shift
        for(short i = 7; i > -1; i--) 
        {
            r = m >> i; 
            if(r & 1) // 1 = 0000000000.....1. 
                printf("1  ");
            else
                printf("0  ");
        }
        printf("\n\n");
}


uint32_t * create_x32bits_identity () //For square matrix
{
    uint32_t * identity = create_inicialized_x32bits_matrix (0);
    for (uint8_t i = 0, j = 0; i<32; i++, j++)
        put_x32bits_matrix (identity, i, j, 1);
    return identity;
}

uint16_t * create_x16bits_identity () //For square matrix
{
    uint16_t * identity = create_inicialized_x16bits_matrix (0);
    for (uint8_t i = 0, j = 0; i<16; i++, j++)
        put_x16bits_matrix (identity, i, j, 1);
    return identity;
}
/*****************************************************************************32 BITS MATRICES********************************************************************************/
//multiply and sum directly
//OPERATIONS ON BIT MATRICES  
//All operations are performed mod 2 (10). That´s to say 1+1 = 10 but we wil use only 0 because 10 mod 10 = 0 
uint8_t compare_x32bits_matrix (uint32_t * m1, uint32_t * m2) //As m is square, m is a n*n matrix
{
    for (uint8_t i=0; i<32; i++)
        if (m1[i] != m2[i])
            return 0;
    return 1;
}

uint8_t compare_xbits_matrix (uint32_t * m1, uint32_t * m2, uint8_t n) //As m is square, m is a n*n matrix
{
    for (uint8_t i=0; i<n; i++)
        if (m1[i] != m2[i])
            return 0;
    return 1;
}

void assign_x32bits_matrix (uint32_t * m1, uint32_t * m2) //As m is square, m is a n*n matrix
{
    for (uint8_t i=0; i<32; i++)
        m1[i] = m2[i];
}

void assign_xbits_matrix (uint32_t * m1, uint32_t * m2, uint8_t n) //As m is square, m is a n*n matrix
{
    for (uint8_t i=0; i<n; i++)
        m1[i] = m2[i];
}

uint32_t * x32bits_matrix_sum (uint32_t * m1, uint32_t * m2)
{
    uint32_t * sum = create_inicialized_x32bits_matrix (0);
    for(uint8_t i = 0; i < 32; i++) //for each row (l)
        sum[i] = m1[i] ^ m2 [i];//Sum the elements in the same position ij
    return sum;
}

uint16_t * x16bits_matrix_sum (uint16_t * m1, uint16_t * m2)
{
    uint16_t * sum = create_inicialized_x16bits_matrix (0);
    for(uint8_t i = 0; i < 16; i++) //for each row (l)
        sum[i] = m1[i] ^ m2 [i];//Sum the elements in the same position ij
    return sum;
}

uint8_t * x8bits_matrix_sum (uint8_t * m1, uint8_t * m2)
{
    uint8_t * sum = create_inicialized_x8bits_matrix (0);
    for(uint8_t i = 0; i < 8; i++) //for each row (l)
        sum[i] = m1[i] ^ m2 [i];//Sum the elements in the same position ij
    return sum;
}

uint32_t * x32bits_matrix_subtract (uint32_t * m1, uint32_t * m2) //The same as the sum
{
    uint32_t * sum = create_inicialized_x32bits_matrix (0);
    for(uint8_t i = 0; i < 32; i++) //for each row (l)
        sum[i] = m1[i] ^ m2 [i];//Sum the elements in the same position ij
    return sum;
}

uint16_t * x16bits_matrix_subtract (uint16_t * m1, uint16_t * m2) //The same as the sum
{
    uint16_t * sum = create_inicialized_x16bits_matrix (0);
    for(uint8_t i = 0; i < 16; i++) //for each row (l)
        sum[i] = m1[i] ^ m2 [i];//Sum the elements in the same position ij
    return sum;
}

uint8_t * x8bits_matrix_subtract (uint8_t * m1, uint8_t * m2) //The same as the sum
{
    uint8_t * sum = create_inicialized_x8bits_matrix (0);
    for(uint8_t i = 0; i < 8; i++) //for each row (l)
        sum[i] = m1[i] ^ m2 [i];//Sum the elements in the same position ij
    return sum;
}

uint32_t * x32bits_matrix_multiply (uint32_t * m1, uint32_t * m2)
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    uint32_t * mult = create_inicialized_x32bits_matrix (0);
    for(uint8_t i = 0; i < 32; i++) //for each row (l1)
        for (uint8_t j = 0; j < 32; j++) //for each column in m2 
            for (uint8_t k = 0; k < 32; k++) //c1 = l2 (for each column in m1 = each line in m2)
                put_x32bits_matrix ( mult, i, j, (uint8_t) (1 & ( (mult[i]>>(31-j)) ^ ( (m1[i]>>(31-k)) & (m2[k]>>(31-j)) ) ) ) ); //Sum the elements in the same position ij
    return mult; //First we make and (&) - multily - the correspondig bits and then we sum (^) with the element on resulting matrix 
}

uint16_t * x16bits_matrix_multiply (uint16_t * m1, uint16_t * m2)
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    uint16_t * mult = create_inicialized_x16bits_matrix (0);
    for(uint8_t i = 0; i < 16; i++) //for each row (l1)
        for (uint8_t j = 0; j < 16; j++) //for each column in m2 
            for (uint8_t k = 0; k < 16; k++) //c1 = l2 (for each column in m1 = each line in m2)
                put_x16bits_matrix ( mult, i, j, (uint8_t) (1 & ( (mult[i]>>(15-j)) ^ ( (m1[i]>>(15-k)) & (m2[k]>>(15-j)) ) ) ) ); //Sum the elements in the same position ij
    return mult; //First we make and (&) - multily - the correspondig bits and then we sum (^) with the element on resulting matrix 
}

uint8_t * x8bits_matrix_multiply (uint8_t * m1, uint8_t * m2)
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    uint8_t * mult = create_inicialized_x8bits_matrix (0);
    for(uint8_t i = 0; i < 8; i++) //for each row (l1)
        for (uint8_t j = 0; j < 8; j++) //for each column in m2 
            for (uint8_t k = 0; k < 8; k++) //c1 = l2 (for each column in m1 = each line in m2)
                put_x8bits_matrix ( mult, i, j, (uint8_t) (1 & ( (mult[i]>>(7-j)) ^ ( (m1[i]>>(7-k)) & (m2[k]>>(7-j)) ) ) ) ); //Sum the elements in the same position ij
    return mult; //First we make and (&) - multily - the correspondig bits and then we sum (^) with the element on resulting matrix 
}

uint32_t x32bits_matrix_multiply_vector (uint32_t m1, uint32_t * m2)
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    uint32_t mult = 0;
        for (uint8_t j = 0; j < 32; j++) //for each column in m2 
            for (uint8_t k = 0; k < 32; k++) //c1 = l2 (for each column in m1 = each line in m2)
               put_x32bits ( &mult, j, (uint8_t) (1 & ( (mult>>(31-j)) ^ ( (m1>>(31-k)) & (m2[k]>>(31-j)) ) ) ) ); //Sum the elements in the same position ij
    return mult; //First we make and (&) - multily - the correspondig bits and then we sum (^) with the element on resulting matrix 
}

uint16_t x16bits_matrix_multiply_vector (uint16_t m1, uint16_t * m2)
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    uint16_t mult = 0;
        for (uint8_t j = 0; j < 16; j++) //for each column in m2 
            for (uint8_t k = 0; k < 16; k++) //c1 = l2 (for each column in m1 = each line in m2)
               put_x16bits ( &mult, j, (uint8_t) (1 & ( (mult>>(15-j)) ^ ( (m1>>(15-k)) & (m2[k]>>(15-j)) ) ) ) ); //Sum the elements in the same position ij
    return mult; //First we make and (&) - multily - the correspondig bits and then we sum (^) with the element on resulting matrix 
}

uint32_t x8bits_matrix_multiply_vector (uint8_t m1, uint8_t * m2)
{//c1 = l2; the resulting matrix will have l1xc2 dimension
    uint8_t mult = 0;
        for (uint8_t j = 0; j < 8; j++) //for each column in m2 
            for (uint8_t k = 0; k < 8; k++) //c1 = l2 (for each column in m1 = each line in m2)
               put_x8bits ( &mult, j, (uint8_t) (1 & ( (mult>>(7-j)) ^ ( (m1>>(7-k)) & (m2[k]>>(7-j)) ) ) ) ); //Sum the elements in the same position ij
    return mult; //First we make and (&) - multily - the correspondig bits and then we sum (^) with the element on resulting matrix 
}


//*********************************************************************************
uint32_t x32bits_matrix_determinant (uint32_t * m)
{
    uint8_t determinant = 0;
    //...
    return determinant;
}

//**********************************************************************************
uint32_t * x32bits_matrix_inverse (uint32_t * m1)
{
    uint32_t * inverse = create_inicialized_x32bits_matrix (0);
    //...
    return inverse;
}

uint8_t is_nonsingular_x32bits_matrix (uint32_t * m) //As m is square, m is a n*n matrix
{
    if (x32bits_matrix_determinant (m) != 0)
        return 1;
    else
        return 0;
}

uint8_t is_self_invertible_x32bits_matrix (uint32_t * m)
{
    uint32_t * identity = create_x32bits_identity ();
    uint32_t * multiply = x32bits_matrix_multiply (m, m); //Self multiply
    if (compare_x32bits_matrix(multiply, identity)) 
        return 1; //m * m = I -> m = m^-1
    else
        return 0;
}

// END OF THE /OPERATIONS ON BIT MATRICES
/*****************************************************************************64 BITS MATRICES*********************************************************************************/

/*****************************************************************************64 BITS MATRICES*********************************************************************************/
