//Header file for op and cr command
/*
  General declarations.
 */
#define OPTIONS 3
#define DIM 5
#define LIMIT 4
#define MODE (S_IWUSR | S_IXUSR | S_IRUSR | S_IRGRP | S_IROTH)  //read and write permissions

struct recentPaths{
  int index;
  char data[500];
  int counter;
};


//Functions
int dim(void);
int op(char **args);
int opshow(char **args);
int opmax(char **args);
int oplast(char **args);
int man(char **args);
int cr(char **args);
int getmaxindex(void);
int getminindex(void);
//void addNewPath(char paths[]);        //Add a new path to open array
//int replacePath(char *);        //replace least used path with new path to open array
//int doesPathExist(char *);     //Check if pathalready exists in open array, return index if path exists, else return 0
//void incrementCounter(int index); // Increment counter of path at index sent in argument