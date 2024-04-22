all:
	gcc -m32 -g -c ml_main.c -o ml_mainreloc.o
	gcc -m32 -shared -o libmlreloc.so ml_mainreloc.o
	gcc -m32 -g -c driver.c -o driver.o
	gcc -m32 -o driver driver.o -L. -lmlreloc -Wl,-rpath=.

clean:
	rm -fr ml_mainreloc.o libmlreloc.so driver.o driver

.PHONY: all clean
