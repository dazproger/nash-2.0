CC=g++
CFLAGS=-std=c++20
CDEBUG=-fsanitize=address,undefined -g -Wall

SOURCES=$(shell echo *.cpp)
HEADERS=$(shell echo *.h)

all: $(SOURCES) $(HEADERS) 
	$(CC) $(CFLAGS) $(SOURCES)
debug: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(CDEBUG) $(SOURCES)
run: all
	./a.out
debug-run: debug
	./a.out
clean:
	rm -rf a.out
