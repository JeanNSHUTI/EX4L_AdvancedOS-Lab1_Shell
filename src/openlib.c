#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "openlib.h"

int indexa = 0;
int indexv = 1;
char pathcompare[500] = "";
bool first_lauch_flag = true;
bool addnewpath = false;
bool add_flag = false;
struct recentPaths paths[]; 

void printAllPaths(void){
	int a;
	//for(a = 0; a < 4; a++)
    //{
		printf("\ni : [%d] ; index: [%d] ; First path in array: [%s] ; counter: [%d]\n", indexa, paths[a].index, paths[a].data, paths[a].counter);			
	//}

}

int dim() {
  return DIM;
};

/*void addNewPath(char *)
{

}

int doesPathExist(char *)
{

}

void incrementCounter(int index)
{

}*/