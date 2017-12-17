//Header file for open command


/*
  General global functions and variables declarations.
 */
#define DIM 5

extern int indexv;
extern int indexa;
extern char pathcompare[500];
extern bool first_lauch_flag;
extern bool addnewpath;
extern bool add_flag ;

extern struct recentPaths{
	int index;
    char data[500];
	int counter;
}recentPaths;

extern struct recentPaths paths[DIM];   //5 recent paths, static array to assure array stays the same throughout execution


//Functions
void printAllPaths(void);   //Print all paths currently saved in open array
int dim(void);
//void addNewPath(char paths[]);        //Add a new path to open array
//void replacePath(char *);        //Areplace least used path with new path to open array
//int doesPathExist(char *);     //Check if pathalready exists in open array, return index if path exists, else return 0
//void incrementCounter(int index); // Increment counter of path at index sent in argument