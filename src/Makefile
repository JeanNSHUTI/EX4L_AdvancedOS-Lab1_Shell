#Create executable
all : main.o
		gcc main.o -o MiniShell
	
main.o : main.c
		gcc -c main.c -o main.o

#Delete temporary files created during compile
clean :
		rm -rf *.io
		
#Delete all other files except source files for 
#clean reconstruction 
mrproper : clean
		rm -rf MiniShell
	