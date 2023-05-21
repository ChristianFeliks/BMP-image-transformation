CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wconversion
LDLIBS=-lm
SOURCES= main.c bmp.c transformations.c
OUTPUT=bmp

## $@ => same as target
## $^ => same as dependencies
## cppcheck --enable=performance,unusedFunction --error-exitcode=1 $^

all: bmp codeanalysis clean


codeanalysis: $(SOURCES)
	cppcheck --enable=style,warning,performance --error-exitcode=1 $^

$(OUTPUT): main.o bmp.o transformations.o
	$(CC) $(CFLAGS)  main.o bmp.o transformations.o $(LDLIBS) -o $(OUTPUT)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c  $(LDLIBS) -o $@

bmp.o: bmp.c
	$(CC) $(CFLAGS) -c bmp.c  $(LDLIBS) -o $@

transformations.o: transformations.c
	$(CC) $(CFLAGS)  -c transformations.c  $(LDLIBS) -o $@

clean:
	rm -f *.o

valgrind: $(OUTPUT)
	valgrind ./$(OUTPUT)

valgrind_leakcheck: $(OUTPUT)
	valgrind --leak-check=full ./$(OUTPUT)

valgrind_extreme: $(OUTPUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes ./$(OUTPUT)

