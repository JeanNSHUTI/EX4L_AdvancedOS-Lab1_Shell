//Header file for open command


//Variables





//Functions
void printAllPaths(struct **);   //Print all paths currently saved in open array
void addNewPath(char *);        //Add a new path to open array
int doesPathExist(char *);     //Check if pathalready exists in open array, return index if path exists, else return 0
void incrementCounter(int index); // Increment counter of path at index sent in argument