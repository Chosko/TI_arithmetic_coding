compile:
	gcc -o ac.out ac.c

run:
	gcc -o ac.out ac.c
	./ac.out

vrun:
	gcc -o ac.out ac.c
	./ac.out -v

clean:
	rm ac.out