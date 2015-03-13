#include <stdio.h>
#include <stdlib.h>
#define MAX 10000
#define BUFF_SIZE 4096
#define ASCII_COUNT 256

typedef struct Node
{
	int value; // number of appearance of the character
	int character; 
	struct Node * left;
	struct Node * right;
} Node;

typedef struct Tree
{
	Node nodes [MAX];
} Tree;

typedef int myType[2][ASCII_COUNT];

// table of huffman codes joined to characters 
// code length <= 30
int table [ASCII_COUNT][30] ; 

// coded file
FILE * out;

char buff [BUFF_SIZE];
int state = 0;
int outBit = 0;

int partition (myType niz, int start, int end)
{
	int b = niz[0][end];
	int index = start;
	int k, temp0, temp1;
	for (k = start; k < end; k++)
	{
		if (niz[0][k] <= b)
		{
			temp0 = niz[0][k];
			niz[0][k] = niz[0][index]; 
			niz[0][index] = temp0;
			
			temp1 = niz[1][k];
			niz[1][k] = niz[1][index]; 
			niz[1][index] = temp1;
			
			index ++;
		}
					
	}

	temp0 = niz[0][end];
	niz[0][end] = niz[0][index]; 
	niz[0][index] = temp0;
	
	temp1 = niz[1][k];
	niz[1][k] = niz[1][index]; 
	niz[1][index] = temp1;

	return index;
}

// just quicksort
void quicksort (myType * niz, int start, int end)
{
	int pIndex;
	if (start >= end)
		return;
	pIndex = partition (niz[0], start, end);
	quicksort (niz, start, pIndex - 1);
	quicksort (niz, pIndex + 1, end);
}

Node *newNode (Node *first, Node *second)
{
	Node *new;
	new = (Node *) malloc (sizeof (Node));
	new -> value = first -> value + second -> value;
	new -> left = first;
	new -> right = second;
	new -> character = -1;
	return new;
}

void placeNode (Tree * Tree, Node * Node, int begg, int end)
{
	int i,j;
	i = begg;
	j = end;
	int flag = 0;
	
	while (j > i)
	{
		if ((Tree -> nodes[i]).value > Node -> value)
			break;
		i ++;
	}

	for (j = end; j >= i ; j --)
	{
		Tree -> nodes[j + 1] = Tree -> nodes [j];
	}
	Tree -> nodes [i] = *Node;

	return;

}

void fillTable(struct Node* n, int k, int* kod)
{
	int j = 0;
    if (n->character != -1)
	{
		while (j < k)
		{
			if (*(kod + j) != -1)
				table[(int)n->character][j] = *(kod + j);
	
			j ++;
		}
		table[(int)n->character][j] = -1;
	}
        
    else
	{
		*(kod + k) = 0;
        fillTable(n->left, k + 1, kod);
		*(kod + k) = 1;
        fillTable(n->right, k + 1, kod);
    }

    return;
}


int main(int argc, char* argv [])
{
	int sz; // input file size
	int indBegg = 0, indLast = ASCII_COUNT - 1;
	int i,b,n,m;

	// ( chars[0][0], chars[1][0] ) = ( number of appearance of ASCII 0, ASCII 0 )
        // ( chars[0][1], chars[1][1] ) = ( number of appearance of ASCII 1, ASCII 1 )
        // ...
	int chars [2][ASCII_COUNT] = {0};
	myType *input;
        input = &chars;

	Node *curr;
	Tree *tree;
	int code [30];
	Node root;
	FILE *in, *ftable;
	
	for (i = 0; i < 30; i ++)
		code [i] = -1;

	for (m = 0; m < ASCII_COUNT; m ++)
	{
		for (i = 0; i < 30; i ++)
		{
			table [m][i] = -1;
		}
	}
			
	in = fopen (argv[1], "rb");
	while  ( ( i = fgetc( in ) ) != EOF )
	{
		sz ++;
		(*input)[0][i] ++;
	}
		
	for (i = 0; i < ASCII_COUNT; i ++)
	{
		(*input)[1][i] = i;
	}
	
	quicksort (input, 0 , ASCII_COUNT - 1 );	
	tree = (Tree *)malloc (sizeof (Tree));
	
	for (i = 0; i < ASCII_COUNT; i ++)
	{
		curr = (Node *) malloc (sizeof (Node));
		curr -> value = (*input)[0][i];
		curr -> left = NULL;
		curr -> right = NULL;
		curr -> character = (*input)[1][i];
		tree -> nodes [i] = *curr;
	}
	
	
	while (indLast - indBegg >= 1)
	{
		Node *nn;
		nn = (Node *)malloc (sizeof (Node));
		nn = newNode (&tree->nodes[indBegg], &tree->nodes[indBegg + 1]);
		indBegg += 2;
		indLast += 1;
		placeNode (tree, nn, 0, indLast);		
	}
	
	root = tree -> nodes [indBegg];
	fillTable(&root, 0, &code[0]);	
	
	ftable = fopen (argv[2], "w+");
	
	
	for (i = 0; i < ASCII_COUNT; i ++)
	{

		for (b = 0; b < 30; b ++)
		{
			if (table[i][b] == -1)
				break;
			fprintf (ftable, "%d" , table[i][b]);
		}
			
		fprintf (ftable, "\n");	
	}
	
	fclose (ftable);
	
	
	out = fopen (argv[3], "wb");
	fseek ( in , 0 , SEEK_SET );
	fwrite(&sz, sizeof(int), 1, out);

	int currSz = 0;
	while ((currSz = fread(buff, sizeof(char), BUFF_SIZE, in)) > 0) 
        {
             int j;
             for (i = 0; i < currSz; ++i)  
             {
                 int c = buff[i];
                 for (j = 0; table[c][j]!= -1; ++j)
                 {
	            if (table[c][j] == 0)
	               state &= ~(1<<outBit);
	            else
	               state |= 1<<outBit;
	            ++outBit;
	            if (outBit == 8) 
                    {
	               outBit = 0;
	               fwrite(&state, sizeof(char), 1, out);
	               state = 0;
	            }
                 }
             }
        }

    fwrite(&state, sizeof(char), 1, out);

    fclose (in);
    fclose(out);

    return 0;
}
