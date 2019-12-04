CC=g++
CPPFLAGS=-g -Wall -Werror -Wextra -Wpedantic -Wfatal-errors

tests: tests.o
	(cd tests && $(CC) -o tests tests.o)

tests.o:
	(cd tests && $(CC) $(CPPFLAGS) -o tests.o -c main.cpp)

clean:
	rm -Rf *.o
	(cd tests && rm -Rf *.o tests)
