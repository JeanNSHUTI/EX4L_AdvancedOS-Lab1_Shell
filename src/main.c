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

/*
  Function Declarations for builtin shell commands:
 */
int open(char **args);
char **lsh_split_line(char *line);
char *lsh_read_line(void);
int lsh_execute(char **args);
int lsh_launch(char **args);


/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "open"
};

int (*builtin_func[]) (char **) = {
  &open
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  General global functions and variables declarations.
 */
#define DIM 4
static char* pathcompare = NULL; //char pointer that saves previous path-> for debugging
bool first_lauch_flag = true;
typedef struct recentPaths{
	int index;
	char* data;
	int counter;
}recentPaths;

struct recentPaths paths[DIM];   //5 recent paths, static array to assure array stays the same throughout execution
struct recentPaths recentPath;  //First structure object

/*int dim() {
  return sizeof(paths) * sizeof(recentPaths); 
};*/


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
	int indexv = 0;
	//int a = 0;
	int testresult;
	//struct recentPaths *paths = (recentPaths*) malloc(4 * sizeof(struct recentPaths));
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    } else{	
		recentPath.index = ++indexv;
		recentPath.counter = 1;		
		recentPath.data= args[1];
		if(first_lauch_flag == true){
			paths[i] = recentPath;
			first_lauch_flag = false;
		}
		if (pathcompare != NULL){			
			printf("\npaths not null\n");
			if(strcmp(args[1], pathcompare) == 0){
				printf("\nincrementing counter\n");
				printf("\npaths[a]counter: [%d]\n", paths[i].counter);
				paths[i].counter += 1;
			} else{
				recentPath.index = ++indexv;
				recentPath.counter =1;
				recentPath.data= args[1];
				paths[++i] = recentPath;				
			}					
		}
		printf("\npaths[a] null\n");
		pathcompare = recentPath.data;
		printf("\npathcompare: [%s]\n", pathcompare);
		//for(a = 0; a <= sizeof(*paths); a++){

		printf("\nindex: [%d] ; First path in array: [%s] ; counter: [%d]\n", paths[i].index, paths[i].data, paths[i].counter);			
		//else{			
			
		//}
		//}
		//printf("\nNewpath variable: [%d] ", a);
	}
	}
  return 1;
}