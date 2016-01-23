test:
	make build;
	make run;

build:
	gcc -std=c99 -c sdlTest.c;
	gcc sdlTest.o -o sdlTest -lSDL2 -lSDL2_gfx;

run:
	./sdlTest;

clean:
	-rm sdlTest;
	-rm *.o;
	-rm *~
