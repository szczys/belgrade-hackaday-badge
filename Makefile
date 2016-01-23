test:
	make build;
	make run;

build:
	gcc -std=c99 -c bh-badge.c bh-badge-animate.c;
	gcc bh-badge.o bh-badge-animate.o -o bh-badge -lSDL2 -lSDL2_gfx;

run:
	./bh-badge;

clean:
	-rm bh-badge;
	-rm *.o;
	-rm *~
