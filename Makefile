#variable definitions
CC = gcc
CFLAGS =  -Wall -Wwrite-strings -Wmissing-prototypes  -O3
DEBUG = -DDEBUG

OBJECTS=swf_parse.o parser.o swf_destroy.o swf_error.o print_utils.o
K_OBJECTS=kitty_parser.o swf_parse.o swf_destroy.o swf_error.o print_utils.o
SOURCES=swf_parse.c parser.c swf_destroy.c swf_error.c print_utils.c
HEADERS=swf_parse.h parser.h swf_destroy.h swf_error.h print_utils.h  swf_types.h
MISC=Makefile readme todo gpl.txt dos.pl oldswfparse.cpp configure.in sizes.h.in 
DIR=$(shell  pwd | perl -ne 's!^.*/!!;')
DATE=$(shell perl -MPOSIX -e 'print POSIX::strftime("%Y-%m-%d", localtime);')

#build rules

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: swf_parse kitty_parse

kitty_parse: $(K_OBJECTS)
	$(CC) $(CFLAGS) -I . -o kitty_parse $(K_OBJECTS)

swf_parse: $(OBJECTS)
	$(CC) $(CFLAGS) -I . -o swf_parse $(OBJECTS)

swf_destroy.o: swf_destroy.c swf_destroy.h swf_types.h

swf_error.o: swf_error.c swf_error.h

print_utils.o: print_utils.c print_utils.h

swf_parse.o: swf_parse.c swf_parse.h swf_error.h swf_types.h swf_destroy.h

kitty_parser.o: kitty_parser.c swf_parse.h swf_types.h swf_destroy.h print_utils.h

parser.o: parser.c swf_parse.h swf_types.h swf_destroy.h print_utils.h

zip: $(SOURCES) $(HEADERS) $(MISC)
	tar -cf - $(SOURCES) $(HEADERS) $(MISC) | gzip > libswfparse-$(DATE).tar.gz

clean:
	rm *.o *.tar.gz swf_parse swfparse *~

