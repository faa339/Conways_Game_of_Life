/*
Author: Faris Alotaibi

This program implements Conway's game of life. It 
allows for the selection of custom number of rows 
and columns,a starting world, and generations.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int** createneighborarr(int rows, int cols);
char** createworld(int rows, int cols);
void evolve(char** world,int rows,int cols);
void setstates(char ** world, int** neighborsarr,int rows, int cols);
void printworld(char** world, int rows, int cols);
void worldcleanup(char** world,int rows);
int cellneighbors(char** world, int rowlocation, int collocation);

int main(int argc, char* argv[])
{
	int rows, cols, generations;
	FILE *initialworld;
	char *endcheck = NULL;
	switch (argc)
	{
		/*Adding 2 to any value of rows or cols to create a 
		buffer zone of 'dead' neighbors.
		Also using strtol to check if any arguments received 
		were invalid -- if endcheck is equal to argv[n], it 
		means nothing was read due to a fault input.*/
		case 2:
			rows = strtol(argv[1],&endcheck,10);
			if(endcheck == argv[1])
			{
				printf("Invalid rows\n");
				return 0;
			}
			rows += 2;
			cols = 12;
			generations = 10;
			initialworld = fopen("life.txt", "r");
			break;

		case 3:
			rows = strtol(argv[1],&endcheck,10);
			if(argv[1] == endcheck)
			{
				printf("Invalid rows\n");
				return 0;
			}
			cols = strtol(argv[2],&endcheck,10);
			if(argv[2] == endcheck)
			{
				printf("Invalid cols\n");
				return 0;
			}
			rows+=2;
			cols+=2;
			generations = 10;
			initialworld = fopen("life.txt", "r");
			break;

		case 4:
			rows = atoi(argv[1]);
			if(argv[1] == endcheck)
			{
				printf("Invalid rows\n");
				return 0;
			}
			cols = strtol(argv[2],&endcheck,10);
			if(argv[2] == endcheck)
			{
				printf("Invalid cols\n");
				return 0;
			}
			rows+=2;
			cols+=2;
			generations = 10;
			initialworld = fopen(argv[3], "r");
			break;

		case 5:
			rows = atoi(argv[1]);
			if(argv[1] == endcheck)
			{
				printf("Invalid rows\n");
				return 0;
			}
			cols = strtol(argv[2],&endcheck,10);
			if(argv[2] == endcheck)
			{
				printf("Invalid cols\n");
				return 0;
			}
			generations = strtol(argv[4],&endcheck,10);
			if(argv[4] == endcheck)
			{
				printf("Invalid generations\n");
				return 0;
			}
			rows+=2;
			cols+=2;
			initialworld = fopen(argv[3], "r");
			break;

		default:
			rows = 12;
			cols = 12;
			generations = 10;
			initialworld = fopen("life.txt", "r");
	};

	//Check to see if file failed to open
	if (initialworld == NULL)
	{
		printf("Failed to open file\n");
		return 0;
	}

	//Create the world
	char **world = createworld(rows,cols);
	if(world == NULL)
	{
		printf("Could not create world\n");
		return 0;
	}
	
	/*Nested loop that reads in characters and places them
	into the world depending on their values. */
	char filechar;
	for(int i=1; i<(rows-1);i++)
	{
		for(int j=1;j<(cols-1);j++)
		{
			filechar= fgetc(initialworld);
			if(filechar != '*') 
			{
				if(filechar == ' ')
				{
					world[i][j] = '-';

				}else if(filechar == '\n' || filechar == EOF)
				{
					/* In this case, we fill out the rest
					of the row with  '-' and move on to the 
					next row*/
					memset(&world[i][j], '-', 
						sizeof(world[i][j]) * (cols-j-1));

					break;

				}else
				{
					world[i][j] = '-';
				}
			}else
			{
				world[i][j] = filechar;
			}
		}
		/*At this point, the loop might have stopped reading in the
		middle of a line. To ensure that it picks up at the start of
		the next line, retrieve characters until filechar stores \n.
		Also checks for EOF so that we don't read past the file. */
		if (filechar!='\n')
		{
			while(filechar != '\n' || filechar != EOF)
			{
				filechar = getc(initialworld);
				if(filechar == '\n' || filechar == EOF)
				{
					break;
				}
			}
		}

	}
	fclose(initialworld); //We're done reading from our file by now.

	
	for(int i = 0; i<=generations; i++)
	{
		printf("Generation %d\n", i);
		printworld(world,rows,cols);
		evolve(world,rows,cols);
		printf("======================\n");
	}

	worldcleanup(world,rows);
	return 0;
}

char** createworld(int rows, int cols)
{
	/*This function creates our world as a 2d array.*/

	char ** world = (char **) malloc(rows* sizeof(char *));
	if(world==NULL)
	{
		return NULL;
	}
	for(int i=0;i<rows;i++)
	{
		world[i] = (char *) malloc(cols* sizeof(char));
		if(world[i] ==NULL)
		{
			return NULL;
		}
	}
	return world;
}

void printworld(char** world, int rows, int cols)
{
	/*This function prints out the world as a grid.*/
	for(int i = 1; i < (rows-1); i++)
	{
		for(int j = 1;j < (cols-1); j++)
		{
			printf("%c ",world[i][j]);
		};
		printf("\n");
	}
}


int** createneighborarr(int rows, int cols)
{
	/*This function creates a 2d array of integers for use 
	in our evolve and setstate functions */

	int **neighborarr = (int **) malloc((rows)*sizeof(int *));
	if(neighborarr == NULL) //Check if malloc failes
	{
		return NULL;
	}
	for(int i = 0; i < rows; i++)
	{
		neighborarr[i] = (int *) malloc((cols)* sizeof(int));
		if(neighborarr[i] == NULL)
		{
			return NULL;
		}
	}
	for(int i = 0; i < rows; i++) //Zero out the array
	{
		for(int j = 0; j < cols; j++)
		{
			neighborarr[i][j] = 0;
		}
	}
	return neighborarr;
}

int cellneighbors(char** world, int rowlocation, int collocation)
{
	/*This is a function to calculate the number of neighbors 
	per given cell. The point (rowlocation,collocation) is the
	location of our cell on the board. By looping from -1 to 1
	and avoiding (0,0), we are able to check the neighbors of a 
	cell without including the cell itself. */
	int neighborcount = 0;
	for(int m = -1; m <= 1; m++)
	{
		for(int n = -1; n <= 1; n++)
		{
			if(m != 0 || n !=0 )
			{
				if(world[m+rowlocation][n+collocation] == '*')
					neighborcount++;
			}
		}
	}
	return neighborcount;
}

void setstates(char ** world, int** neighborsarr, int rows, int cols)
{
	/*This function does the logic required to set the state of a cell
	to alive or dead.*/

	for(int i = 1;i<(rows - 1);i++)
	{
		for(int j = 1;j<(cols - 1);j++)
		{
			if(neighborsarr[i][j] < 2)
			{
				world[i][j] = '-';
			}else if((neighborsarr[i][j] == 2 || neighborsarr[i][j] == 3)
				      && world[i][j]=='*')
			{
				world[i][j] = '*';
			}else if(neighborsarr[i][j] == 3 && world[i][j]=='-')
			{
				world[i][j] = '*';
			}else
			{
				world[i][j] = '-';
			}			
		};
	};
}

void evolve(char** world, int rows, int cols)
{
	/*This is a function that handles the cell evolution. 
	It does this by creating a new 2d array to store the 
	number of neighbors per cell, calculated with the neighbors
	function. The state of our world is then edited based on  
	the corresponding values in neigborarr.
	*/
	int ** neighborarr = createneighborarr(rows,cols);
	if (neighborarr == NULL)
	{
		printf("Could not create neighborarr");
		exit(0);
	}
	for(int i = 1; i < (rows-1); i++)
	{
		for(int j = 1;j < (cols - 1); j++)
		{
			neighborarr[i][j] = cellneighbors(world,i,j);			
		};	
	};
	
	setstates(world,neighborarr,rows,cols);

	for(int i = 0; i < rows; i++) //Dealloc neighborarr
	{
		free(neighborarr[i]);
	}
	free(neighborarr);
}

void worldcleanup(char** world, int rows)
{
	//Deallocate our world pointer
	for(int i = 0; i < rows; i++)
	{
		free(world[i]);
	}
	free(world);
}