CC=clang
CFLAGS +=-fvisibility=hidden -fpic -ansi -Wall -Wno-comment -Werror -Wno-error=unused-variable

all: libronnie.so include main

include: caos.h value.h ronnie.h common.h
	@mkdir -p include/ronnie
	@echo " CP $^ => include/ronnie/"
	@cp $^ include/ronnie

main: main.o libronnie.so
	@${CC} -g $^ -lstdc++ -L. -lronnie -o $@ -Wl,-rpath,.
	@echo " LD $^ => $@"

main.o: include main.c
	@${CC} -g -c -Iinclude main.c ${CFLAGS}
	@echo " CC main.c => $@"

libronnie.so: caos.o value.o ronnie.o
	@${CC} -shared -lstdc++ $^ -o $@
	@echo " LD $^ => $@"

%.o: %.c
	@${CC} -g -c $^ ${CFLAGS}
	@echo " CC $^ => $@"

%.o: %.cpp
	@${CC} -g -c $^ ${CFLAGS}
	@echo " CC $^ => $@"

clean:
	-rm main libronnie.so
	-rm -frv *.o
	-rm -frv include
