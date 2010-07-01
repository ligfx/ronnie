override CFLAGS := -fvisibility=hidden -fpic -ansi -Wall -Wno-comment -Werror -Wno-error=unused-variable -g $(CFLAGS)

all: libronnie.so include main test

include: caos.h value.h ronnie.h common.h
	@mkdir -p include/ronnie
	@echo " CP $^ => include/ronnie/"
	@cp $^ include/ronnie

test: test.o libronnie.so
	@gcc $^ -L. -lronnie -lgtest_main -o $@ -Wl,-rpath,.
	@echo " LD $^ => $@"

main: main.o libronnie.so
	@${CC} $^ -L. -lronnie -o $@ -Wl,-rpath,.
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
	-rm main libronnie.so test
	-rm -frv *.o
	-rm *~
	-rm -frv include
