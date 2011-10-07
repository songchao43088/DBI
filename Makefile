CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=OurDB.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=OurDB

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.o:
	$(CC) $(CFLAGS) $< -o $@
clean: 
	rm $(EXECUTABLE) *.o
