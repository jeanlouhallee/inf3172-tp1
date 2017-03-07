exe: fileSystem.o
	gcc -o exe fileSystem.o
	rm *.o
fileSystem.o: fileSystem.c fileSystem.h
	clear
	gcc -c -Wall fileSystem.c 


.PHONY: clean

clean:
		rm -f exe
		rm -f disque
		rm -f blocs
		rm -f repertoires
		rm -f inodes
		clear
