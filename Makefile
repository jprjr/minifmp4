.PHONY: all clean

CFLAGS = -Wall -Wextra -g -fsanitize=undefined
LDFLAGS = -fsanitize=undefined

DEMOS = demo-adts demo-flac

HEADERS =  \
	src/allocator.h \
	src/boxes.h \
	src/close.h \
	src/emsg.h \
	src/defines.h \
	src/flags.h \
	src/fmp4_math.h \
	src/fmp4_stdlib.h \
	src/fmp4_string.h \
	src/getters.h \
	src/init.h \
	src/int.h \
	src/loudness.h \
	src/membuf.h \
	src/minifmp4.h \
	src/mux.h \
	src/pack.h \
	src/setters.h \
	src/sizes.h \
	src/structs.h \
	src/track.h

SOURCES = \
	src/allocator.c \
	src/boxes.c \
	src/close.c \
	src/emsg.c \
	src/flags.c \
	src/getters.c \
	src/init.c \
	src/loudness.c \
	src/membuf.c \
	src/mux.c \
	src/pack.c \
	src/setters.c \
	src/sizes.c \
	src/track.c

all: $(DEMOS) minifmp4.h src/minifmp4.o

clean:
	rm -f $(DEMOS) minifmp4.h src/minifmp4.o
	rm -f demos/demo-adts.o demos/adts.o
	rm -f demos/demo-flac.o demos/flac.o
	rm -f demos/id3.o

minifmp4.h: $(HEADERS) $(SOURCES)
	./tools/build.pl

src/minifmp4.o: src/minifmp4.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

demo-adts: demos/demo-adts.o demos/id3.o demos/adts.o
	$(CC) -o $@ $^ $(LDFLAGS)

demo-flac: demos/demo-flac.o demos/flac.o
	$(CC) -o $@ $^ $(LDFLAGS)

demos/demo-adts.o: demos/demo-adts.c minifmp4.h
	$(CC) $(CFLAGS) -c -o $@ $<

demos/demo-flac.o: demos/demo-flac.c minifmp4.h
	$(CC) $(CFLAGS) -c -o $@ $<

demos/id3.o: demos/id3.c demos/id3.h
	$(CC) $(CFLAGS) -c -o $@ $<

demos/adts.o: demos/adts.c demos/adts.h
	$(CC) $(CFLAGS) -c -o $@ $<

demos/flac.o: demos/flac.c demos/flac.h
	$(CC) $(CFLAGS) -c -o $@ $<
