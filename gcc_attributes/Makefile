all: weak weakref

weak:
	gcc -std=c11 -Wall -Wextra -c -o weak.o weak.c
	gcc -std=c11 -Wall -Wextra -c -o weak_aux.o weak_aux.c
	gcc -std=c11 -Wall -Wextra -o weak weak.o weak_aux.o
	objdump -t weak &> weak-sym-tab

weakref:
	gcc -std=c11 -Wall -Wextra -c -o weakref.o weakref.c
	gcc -std=c11 -Wall -Wextra -c -o weakref_aux.o weakref_aux.c
	gcc -std=c11 -Wall -Wextra -o weakref weakref.o weakref_aux.o
	objdump -t weakref &> weakref-sym-tab

clean:
	rm -fr a.out weak.o weak_aux.o weak weak-sym-tab weakref.o weakref_aux.o weakref weakref-sym-tab

.PHONY: all weak weakref clean
