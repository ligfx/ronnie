CC=clang
CFLAGS +=-fvisibility=hidden -fpic -ansi -Wall -Wno-comment -Werror -Wno-error=unused-variable

all: main libronnie.so include

include: caos.h value.h dairy.h common.h
	mkdir -p include/ronnie
	cp $^ include/ronnie

main: main.o libronnie.so
	${CC} -g $^ -lstdc++ -L. -lronnie -o $@ -Wl,-rpath,.

libronnie.so: caos.o value.o dairy.o
	${CC} -shared -lstdc++ $^ -o $@

%.o: %.c
	${CC} -g -c $^ ${CFLAGS}

%.o: %.cpp
	${CC} -g -c $^ ${CFLAGS}

clean:
	-rm main libronnie.so *.o
	-rm -frv include
