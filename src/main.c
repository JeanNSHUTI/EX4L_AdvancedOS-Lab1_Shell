/***************************************************************************//**

  @file         main.c

  @author       SG, CC, MC, CK, JRN
  
  @inspiration  Stephen Brennan LSH (Libstephen SHell)

  @year			2017/2018

  @brief        EX4l-Shell

*******************************************************************************/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
//#include "openlib.h"

/*
  Function Declarations for builtin shell commands:
 */
int open(char **args);
int openshow(char **args);
int openmax(char **args);
int openlast(char **args);
int openman(char **args);
char **lsh_split_line(char *line);
char *lsh_read_line(void);
int lsh_execute(char **args);
int lsh_launch(char **args);


/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  //"openshow",   //openshow and open max can only be called via open -s or open -m
  //"openmax",
  //"openlast",
  "help",
  "open"
};

int (*builtin_func[]) (char **) = {
  //&openshow,
  //&openmax,
  //&openlast,
  &openman,
  &open
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  General functions and global variables declarations.
 */
#define DIM 5
#define LIMIT 4

int indexv = 1;
int indexa = 0;
char pathcompare[500] = "";
bool first_lauch_flag = true;
bool addnewpath = false;
bool add_flag = false;
bool help_flag = true;

struct recentPaths{
	int index;
  char data[500];
	int counter;
};
struct recentPaths paths[DIM];   //5 recent paths, static array to assure array stays the same throughout execution

int dim() {
  return DIM;
};

int getmaxindex(void);
int getminindex(void);


/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
	char cwd[1000];
  // Load config files, if any.

  // Run command loop.
  char *line;
  char **args;
  int status;

  do {
	//Command prompt
	if(getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%-s ~",cwd);
		//printf("%");
	}
    printf(" ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }
  
  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
//refaire ca XX
char *lsh_read_line(void)
{
  char *buffer = malloc(200); // alouer de l espace mémoire au buffer
  int position = 0; // buffer position
  int c;
  while (1) {
    // Read a character
    c = getchar(); // get the first character of the line

    if (c == EOF) {
      exit(EXIT_SUCCESS); // exit if End Of File is reached
    } 
    else if (c == '\n') {
      buffer[position] = '\0'; // return the buffer once we are at the end of the line (toute la ligne se trouve lettre par lettre dans le buffer
      return buffer;
    } 
    else {
      buffer[position] = c; //ajouter une lettrre au buffer
    }
    position++;
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a'"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));  // malloc sert à demander de l espace mémoire au systeme d exploitation
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int open(char **args)
{
	int i = 0;
	char* list = "-s";
  char* max = "-m";
  char* least = "-p";
  char* manual = "-man";
	int a = 0;
  int min_index;
  int previouspath_index;
	//struct recentPaths *paths = (recentPaths*) malloc(4 * sizeof(struct recentPaths));
  
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else if (strcmp(args[1], list ) == 0) {            
    openshow(args);   //call openshow instead
  } else if(strcmp(args[1], max) == 0){
    openmax(args);    //call openmax instead
  } else if(strcmp(args[1], manual) == 0){
    help_flag = false;
    openman(args);    //call manual instead
  } else if(strcmp(args[1], least) == 0){
    openlast(args);    //call openlast instead
  } else if (chdir(args[1]) != 0) {
      perror("lsh");   //Error with chdir
    } else{	   
		if(first_lauch_flag == true){ //chdir succeeded, save first path used with open, execute once!
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
        //if(strcmp(args[1], pathcompare) == 0){   //test if current path is equal to previous path
          printf("\nincrementing counter\n");
          printf("\npaths[a]counter: [%d]\n", paths[i].counter);
          paths[i].counter += 1;
          add_flag = true;
          //break; 
          //continue;
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
        if(indexa <= LIMIT){    
          indexa = indexa + 1;          //Add new path at end of array. 
          printf("\nAdding new path\n");
          paths[indexa].index = ++indexv;
          paths[indexa].counter = 1;
          strcpy(paths[indexa].data, args[1]); 
        } else {
          min_index = getminindex();
          printf("\nminimum counter in array index: [%d]\n", min_index); 
          min_index = min_index - 1; //Get realindex of array,then relplace path and reinitialise counter
          paths[min_index].counter = 1;
          strcpy(paths[min_index].data, args[1]);
        }          
      }
		}
    strcpy(pathcompare, args[1]);
		printf("\npathcompare: [%s]\n", pathcompare);
	  }
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
int openshow(char **args)
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

int openmax(char **args)
{
  int max_index;
  max_index = getmaxindex();
  max_index = max_index - 1; //Get real index in array
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(paths[max_index].data) != 0) {
      perror("lsh");
    }
  }
  return 1;
  
}

int openlast(char **args)
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
    }
  }
  return 1;
}

int openman(char **args)
{
  if(help_flag){
    printf("\n*******************************************\n");
    printf("\nGCCKN SHell based on Stephen Brennan's LSH\n");
    printf("\n___________________________________________\n");
    printf("\nProtoype shell used to simulate the command open and its extensions.\n");
    printf("The following commands are built in:\n");
    printf("\n\t1. help\n");
    printf("\n\t2. open (url)\n");
    printf("\nThe following are Open's extensions:\n");
    printf("\n\t1. open -s\n");
    printf("\n\t2. open -m\n");
    printf("\n\t3. open -p\n");
    printf("\nType 'open -man' for details on extensions.\n\n");
  } else{
    printf("\nOpen allows you to navigate any repository in your workspace all while\n");
    printf("keeping track of your five most favourite and recent directories. Use open's \n");
    printf("extensions to speed up navigation:\n");
    printf("\n\t1. -s  *Lists all paths currently saved in the open array\n");
    printf("\n\t2. -m  *Changes directory to the most used directory\n");
    printf("\n\t3. -p  *Changes directory to the least used and/or recent directory\n\n");
    help_flag = true;
  }
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