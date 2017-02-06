Executable: fonction.o ia.o main.o
	gcc $^ -o $@
ia.o: ia.c ia.h
	gcc -g -Wall -c $< -o $@
main.o: main.c main.h
	gcc -g -Wall -c $< -o $@
fonction.o: fonction.c ia.h
	gcc -g -Wall -c $< -o $@
clean:
	rm -f *.o Executable
