weak:
	gcc -std=c11 -Wall -Wextra -c -o weak.o weak.c
	gcc -std=c11 -Wall -Wextra -c -o weak_aux.o weak_aux.c
	gcc -std=c11 -Wall -Wextra -o weak weak.o weak_aux.o
	objdump -t weak &> weak-sym-tab

clean:
	rm -fr a.out weak.o weak_aux.o weak weak-sym-tab

.PHONY: weak clean
