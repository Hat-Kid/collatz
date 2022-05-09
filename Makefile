bin = collatz

all: clean collatz run

collatz: main.c
	gcc -o $(bin) main.c

clean:
	rm -f $(bin)

run:
	./collatz
