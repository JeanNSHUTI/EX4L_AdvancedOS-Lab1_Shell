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
char **lsh_split_line(char *line);
char *lsh_read_line(void);
int lsh_execute(char **args);
int lsh_launch(char **args);


/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "openshow",
  "open"
};

int (*builtin_func[]) (char **) = {
  &openshow,
  &open
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  General global functions and variables declarations.
 */
#define DIM 5

int indexv = 1;
int indexa = 0;
char pathcompare[500] = "";
bool first_lauch_flag = true;
bool addnewpath = false;
bool add_flag = false;

struct recentPaths{
	int index;
  char data[500];
	int counter;
};

struct recentPaths paths[DIM];   //5 recent paths, static array to assure array stays the same throughout execution

int dim() {
  return DIM;
};


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
#define LSH_TOK_DELIM " \t\r\n\a"
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
	char* test = "/home";
	int a = 0;
	//struct recentPaths *paths = (recentPaths*) malloc(4 * sizeof(struct recentPaths));
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");   //Error with chdir
    } else{		
      
		if(first_lauch_flag == true){ //chdir succeeded, save first path used with open, execute once!
			printf("\nAdding first path\n");		  
      paths[indexa].index= indexv;
      paths[indexa].counter = 1;
      strcpy(paths[indexa].data, args[1]);
			first_lauch_flag = false;
		}
		if (strcmp(pathcompare, "") != 0){			   //Don't execute if first launch
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
        if(i == 4){
          printf("\ni = 4\n");
          if(add_flag != true){
            addnewpath = true;            
            printf("\nactivated add\n");
          }
        }
      }
      printf("%s", addnewpath ? "true":"false");
      if(addnewpath == true){
        //if array is not full
        if(i != 4){  
          indexa = indexa + 1;          //Add new path at end of array.          
          printf("\nAdding new path\n");
          paths[indexa].index = ++indexv;
          paths[indexa].counter = 1;
          strcpy(paths[indexa].data, args[1]); 
        } //else replace path smallest counter           
      }
		}
    strcpy(pathcompare, args[1]);
		printf("\npathcompare: [%s]\n", pathcompare);
	  }
	}
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
  if (args[1] != NULL) {
    fprintf(stderr, "lsh: expected no argument\n");
  } else {
      for(a = 0; a < dim(); a++)
      {
		    printf("\ni : [%d] ; index: [%d] ; First path in array: [%s] ; counter: [%d]\n", indexa, paths[a].index, paths[a].data, paths[a].counter);			
		  }
   }
  return 1;
}