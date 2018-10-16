#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main()
{
	int n = 1000*1000*128; //Number of bytes (1000 * 1000 = 1 MB)
	FILE * file_pointer;
	file_pointer = fopen ("/Users/mac/Documents/RKHC/C-Hill-Cipher1/Source/file1.txt", "w");
	if (file_pointer == NULL)
		printf("%s\n", "Something is wrong on opening the file!");
	else
	{
		srand(time(NULL));
		/*********************************************Put One char by one**********************/
		for (int i=0; i<n; i++) //1000mb = real	1m3.726s //rc4 = real	0m18.249s //aes-128-cbc = 0m14.906s // rkhc = real	62m27.246s
		{
			char c = rand() % 255;
			//char c = 'a';
			fputc (c, file_pointer);
		}
		/*********************************************Put One char by one**********************/
		
		/*********************************************Put all string **********************/
		/*char * bytes = malloc (n * sizeof(char));

		for (int i=0; i<n; i++) //less than 10 seconds. The operation of save something into the HD take a very long time...
		{
			//bytes[i] = rand() % 255;
			bytes[i] = 'a';
		}
		fputs (bytes, file_pointer);*/

		//printf("%s\n", bytes);
		//printf("%lu\n", strlen(bytes));
		/*********************************************Put all string **********************/
	}

	n = fclose (file_pointer);
	if (n != 0)
		printf("%s\n", "Something is wrong on closing the file!");

	return 0;
}