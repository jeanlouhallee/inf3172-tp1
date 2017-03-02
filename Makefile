exe: fileSystem.o
	gcc -o exe fileSystem.o
	rm *.o
fileSystem.o: fileSystem.c fileSystem.h
	gcc -c fileSystem.c 


.PHONY: clean

clean:
		rm exe
		rm disque
		rm blocs
		rm repertoires
