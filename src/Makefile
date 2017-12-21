#Create executable
all : main.o openlib.o
		gcc main.o openlib.o -o MiniShell

#These rules is only executed if main.c is newer than main.o
main.o : main.c
		gcc -c main.c -o main.o
		
openlib.o : openlib/openlib.c
		gcc -c openlib/openlib.c -o openlib.o

#Delete temporary files created during compile
clean :
		rm -rf *.io
		
#Delete all other files except source files for 
#clean reconstruction 
mrproper : clean
		rm -rf MiniShell
	