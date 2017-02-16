exe: fileSystem.o
	gcc -o exe fileSystem.o
	rm *.o
fileSystem.o: fileSystem.c
	gcc -c fileSystem.c


.PHONY: clean

clean:
		rm exe
