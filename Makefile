#variable definitions
CC = gcc
LIBTOOL = ./libtool
CFLAGS =  -Wall -Wwrite-strings -Wmissing-prototypes  -O3
DEBUG = -DDEBUG -g


LIB_OBJECTS=swf_parse.lo swf_destroy.lo swf_error.lo
OBJECTS=parser.o print_utils.o
TEXT_OBJECTS=text_extract.o lib_swfextract.o
D_OBJECTS=swf_parse_d.o parser_d.o swf_destroy_d.o swf_error_d.o print_utils_d.o
SOURCES=swf_parse.c parser.c swf_destroy.c swf_error.c print_utils.c lib_swfextract.c text_extract.c
HEADERS=swf_parse.h parser.h swf_destroy.h swf_error.h print_utils.h  swf_types.h lib_swfextract.h
MISC=Makefile readme todo gpl.txt dos.pl oldswfparse.cpp configure.ac sizes.h.in
DIR=$(shell  pwd | perl -ne 's!^.*/!!;')
DATE=$(shell perl -MPOSIX -e 'print POSIX::strftime("%Y-%m-%d", localtime);')

#build rules

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.lo: %.c
	$(LIBTOOL) --mode=compile $(CC) $(CFLAGS) -c -o $@ $<


all: swf_parse text_extract

debug: swf_parse_debug

libswfparse.la: $(LIB_OBJECTS)
	$(LIBTOOL) --mode=link $(CC) $(CFLAGS) -o libswfparse.la $(LIB_OBJECTS) -rpath /usr/local/lib

swf_parse: libswfparse.la $(OBJECTS)
	$(LIBTOOL) --mode=link $(CC) $(CFLAGS) -o swf_parse $(OBJECTS) libswfparse.la -rpath /usr/local/lib

text_extract: libswfparse.la $(TEXT_OBJECTS)
	$(LIBTOOL) --mode=link $(CC) $(CFLAGS) -o text_extract $(TEXT_OBJECTS) libswfparse.la -rpath /usr/local/lib

swf_destroy.o: swf_destroy.c swf_destroy.h swf_types.h

swf_error.o: swf_error.c swf_error.h

print_utils.o: print_utils.c print_utils.h

swf_parse.o: swf_parse.c swf_parse.h swf_error.h swf_types.h swf_destroy.h

parser.o: parser.c swf_parse.h swf_types.h swf_destroy.h print_utils.h

lib_swfextract.o: lib_swfextract.c swf_parse.h swf_types.h swf_destroy.h

text_extract.o: text_extract.c lib_swfextract.h



swf_parse_debug: $(D_OBJECTS)
	$(CC) $(CFLAGS) -I . $(DEBUG) -o swf_parse $(D_OBJECTS)

swf_destroy_d.o: swf_destroy.c swf_destroy.h swf_types.h
	$(CC) -c $(DEBUG) -o swf_destroy_d.o swf_destroy.c

swf_error_d.o: swf_error.c swf_error.h
	$(CC) -c $(DEBUG) -o swf_error_d.o swf_error.c

print_utils_d.o: print_utils.c print_utils.h
	$(CC) -c $(DEBUG) -o print_utils_d.o print_utils.c

swf_parse_d.o: swf_parse.c swf_parse.h swf_error.h swf_types.h swf_destroy.h
	$(CC) -c $(DEBUG) -o swf_parse_d.o swf_parse.c

parser_d.o: parser.c swf_parse.h swf_types.h swf_destroy.h print_utils.h
	$(CC) -c $(DEBUG) -o parser_d.o parser.c


zip: $(SOURCES) $(HEADERS) $(MISC)
	tar -cf - $(SOURCES) $(HEADERS) $(MISC) | gzip > libswfparse-$(DATE).tar.gz

clean:
	rm -f *.o *.la *.lo *.tar.gz swf_parse text_extract swfparse

reallyclean: clean
	rm -f *~
