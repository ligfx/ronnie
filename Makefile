override CFLAGS := -fvisibility=hidden -fpic -ansi -Wall -Wno-comment -Werror -Wno-error=unused-variable -g $(CFLAGS)

all: libronnie.so include main

include: caos.h value.h ronnie.h common.h
	@mkdir -p include/ronnie
	@echo " CP $^ => include/ronnie/"
	@cp $^ include/ronnie

main: main.o libronnie.so
	@${CC} $^ -lstdc++ -L. -lronnie -o $@ -Wl,-rpath,.
	@echo " LD $^ => $@"

main.o: include main.c
	@${CC} -c -Iinclude main.c ${CFLAGS} -std=c99
	@echo " CC main.c => $@"

libronnie.so: caos.o value.o ronnie.o lexer.o
	@${CC} -shared -lstdc++ $^ -o $@
	@echo " LD $^ => $@"

%.o: %.c
	@${CC} -c $^ ${CFLAGS} -std=c99
	@echo " CC $^ => $@"

%.o: %.cpp
	@${CC} -c $^ ${CFLAGS}
	@echo " CC $^ => $@"

clean:
	-rm main libronnie.so
	-rm -frv *.o
	-rm *~
	-rm -frv include
