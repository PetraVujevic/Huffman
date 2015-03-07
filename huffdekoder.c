#include <stdio.h>
#include <string.h>
#define BUFF_SIZE 4096
#define ASCII_COUNT 256

FILE* in;
int codesTable[ASCII_COUNT][30];
int codesLength[ASCII_COUNT];
char code[30];

char buff[BUFF_SIZE];
int current = 0;
int bit = 0;
int sz = 0;

int readInput() 
{
    if (current == sz) 
    { 
        current = 0;
        sz = fread(buff, sizeof(char), BUFF_SIZE, in);
        if (sz == 0) 
	        return -1;
        bit = 0;
    }
    int ret = (buff[current] >> bit) & 1;
    if (++bit == 8) 
    {
        bit = 0;
        ++current;
    }
    return ret;
}

void tableToInt (FILE* table) 
{
    int i, len, j;
    for (i = 0; i < ASCII_COUNT; ++i) 
    {
        fscanf(table, "%s", code);
        len = strlen(code);
        codesLength[i] = len;
        for (j = 0; j < len; ++j)
            codesTable[i][j] = code[j] - '0';
    }
}

int main(int argc, char* argv[]) 
{ 
    int i, j;
    FILE* ftable, * out; 

    ftable = fopen(argv[1], "r");
    in = fopen(argv[2], "rb");
    out = fopen(argv[3], "wb");

    tableToInt(ftable);
    fclose(ftable);

    int size = 0;
    fread(&size, sizeof(int), 1, in);
    int input[ASCII_COUNT];
    int inputSz = 0;
  
    while (size) 
    {
        input[inputSz++] = readInput();
        for (i = 0; i < ASCII_COUNT; i++) 
	    {
            if (codesLength[i] == inputSz) 
	        {
	            for (j = 0; j < inputSz; j++)
	                if (input[j] != codesTable[i][j])
	                    break;
	            if (j == inputSz) 
		        {
                    fprintf(out, "%c", i);	               
                    inputSz = 0;
                    size --;	               
                    break;
	            }
            } 
        }
    }
  
    fclose(in);
    fclose(out);
    return 0;
}
