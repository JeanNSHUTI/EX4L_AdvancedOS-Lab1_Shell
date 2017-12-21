#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "openlib.h"

int indexv = 1;
int indexa = 0;
char pathcompare[500] = "";
bool first_lauch_flag = true;
bool addnewpath = false;
bool add_flag = false;

struct recentPaths paths[DIM];   //5 recent paths, static array to assure array stays the same throughout execution

/*
*Functions for op and cr
*/

int dim(void) {
  return DIM;
};

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int op(char **args)
{
  int i = 0;
  char *options[] = {"-s", "-m", "-p", "-man"}; 
  int a = 0;
  int min_index;
  int previouspath_index;

  if(args[2] == NULL){
      if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else if (strcmp(args[1], options[0] ) == 0) {            
    opshow(args);   //call openshow instead
  } else if(strcmp(args[1], options[1]) == 0){
    opmax(args);    //call openmax instead
  } else if(strcmp(args[1], options[3]) == 0){
    man(args);    //call manual instead
  } else if(strcmp(args[1], options[2]) == 0){
    oplast(args);    //call openlast instead
  } else if (chdir(args[1]) != 0) {
      perror("lsh");   //Error with chdir
    } else{	   
		if(first_lauch_flag == true){ //chdir succeeded, save first path used with op, execute once!
			printf("\nAdding first path\n");		  
      paths[indexa].index= indexv;
      paths[indexa].counter = 1;
      strcpy(paths[indexa].data, args[1]);
			first_lauch_flag = false;
		}
		if (strcmp(pathcompare, "") != 0){
      //Don't execute if first launch      
      for(i = 0; i < dim(); i++){		  
        printf("\n%d\n", i);
        if(strcmp(args[1], paths[i].data) == 0){
          printf("\nincrementing counter\n");
          printf("\npaths[a]counter: [%d]\n", paths[i].counter);
          paths[i].counter += 1;
          add_flag = true;
          }    
        if(i == LIMIT){	
          printf("\ni = 4\n");
          if(add_flag != true){
            addnewpath = true;            
            printf("\nactivated add\n");
          }
        }
       }
      printf("\n%s\n", addnewpath ? "true":"false");
      if(addnewpath == true){        
        //if array is not full
        if(indexa < LIMIT){    
          indexa = indexa + 1;          //Add new path at end of array. 
          printf("\nAdding new path\n");
          paths[indexa].index = ++indexv;
          paths[indexa].counter = 1;
          strcpy(paths[indexa].data, args[1]); 
        } else {
          min_index = getminindex();
          printf("\nminimum counter in array index: [%d]\n", min_index); 
          min_index = min_index - 1; //Get real index of array,then relplace path and reinitialise counter
          paths[min_index].counter = 1;
          strcpy(paths[min_index].data, args[1]);
        }          
      }
		}
    strcpy(pathcompare, args[1]);
	  }
  } else{printf("\nEror: Expected only 1 argument\n");}
	//}
  add_flag = false;
  addnewpath = false;
  return 1;
}

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int opshow(char **args)
{
  int a = 0;
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected no argument\n");
  } else {
      for(a = 0; a < dim(); a++)
      {
		  printf("\ni : [%d] ; index: [%d] ; Path in array: [%s] ; counter: [%d]\n", indexa, paths[a].index, paths[a].data, paths[a].counter);			
	  }
   }
  return 1;
}

int opmax(char **args)
{
  int max_index;
  max_index = getmaxindex();
  max_index = max_index - 1; //Get real index in array
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(paths[max_index].data) != 0) {
      perror("lsh");
    } else{paths[max_index].counter += 1;}
  }
  return 1;
  
}

int oplast(char **args)
{
  int counter;
  int result_index = 0;
  int minvalue = paths[0].counter;
  
  for(counter = 0; counter < dim(); counter++){
    if(paths[counter].counter != 0){
      if(paths[counter].counter <= minvalue){    //gets most recent 
      minvalue = paths[counter].counter;
      result_index = paths[counter].index;   
      } 
    }  
  }
  result_index = result_index- 1; //Get real index in array
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(paths[result_index].data) != 0) {
      perror("lsh");
    } else{paths[result_index].counter += 1;}
  }
  return 1;
}

/*
Manual -option. Allows to view individual help
of all commands. Additional manuals can be added here.
*/
int man(char **args)
{
  if(strcmp(args[0], "op") == 0){
    printf("\nop allows you to navigate any repository in your workspace all while\n");
    printf("keeping track of your five most favourite and recent directories.\n");
    printf("Use open's extensions to speed up navigation:\n");
    printf("\n\t1. -s  *Lists all paths currently saved in the open array\n");
    printf("\n\t2. -m  *Changes directory to the most used directory\n");
    printf("\n\t3. -p  *Changes directory to the least used and/or recent directory\n\n");

  } else if (strcmp(args[0], "cr") == 0){
    printf("\ncr allows you to create a file in your current directory, any other \n");
    printf("directory (by provding absolute address + name), in the directory \n");
    printf("most used (c.f. 'op -man') or least used directory (c.f. 'op').\n");
    printf("\n\t1. -m  NameofFile   *Creates NameofFile in most used directory\n");
    printf("\n\t2. -p  NameofFile   *Creates NameofFile in directory the least used\n\n");
  }
  return 1;
}

int cr(char **args)
{
  int file_d;
  char cwd[1024];
  char **options = malloc(OPTIONS * sizeof(char*));
  options[0] = "-m";
  options[1] = "-p";
  options[2] = "-man";
  
  if (args[1] == NULL) {
    printf("Error: Argument expected: '/path/ + nameOfFile' ; just 'nameOfFile' or option \n");
  } else if(strcmp(args[1], options[0]) == 0){
    if(op(args)){
      if((file_d = creat(args[2], MODE))){
        ftruncate(file_d, 1000);
        printf("\ncreating.. [%s]\n", args[2]);
        write(file_d, "", 1);
      }else{ perror("Error:");} 
    }
  } else if(strcmp(args[1], options[2]) == 0){
    man(args);  //call man instead
  }else if(strcmp(args[1], options[1]) == 0){
    if(op(args)){
      if((file_d = creat(args[2], MODE))){
      ftruncate(file_d, 1000);
      printf("\ncreating.. [%s]\n", args[2]);
      write(file_d, "", 1);
  } else { perror("Error:");}
    } 
  } else if((file_d = creat(args[1], MODE)) > 0){
    ftruncate(file_d, 1000); //file size
    printf("\ncreating here.. [%s]\n", args[1]);
    write(file_d, "", 1);
  } else {perror("Error:");}

 return 1; 
}

int getminindex(void)
{
  int counter;
  int result_index = 0;
  int minvalue = paths[0].counter;
  
  for(counter = 0; counter < dim(); counter++){
    if(paths[counter].counter < minvalue){   //gets oldest
      minvalue = paths[counter].counter;
      result_index = paths[counter].index;   
    }   
  }
  return result_index;
}

int getmaxindex(void)
{
  int counter;
  int result_index = 0;
  int maxvalue = paths[0].counter;
  
  for(counter = 0; counter < dim(); counter++){
    if(paths[counter].counter >= maxvalue){
      maxvalue = paths[counter].counter;
      result_index = paths[counter].index;   
    }   
  }
  return result_index;  
  
}


/*void addNewPath(char *)
{

}

int doesPathExist(char *)
{

}

void incrementCounter(int index)
{

}*/