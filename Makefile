override CFLAGS := -fvisibility=hidden -fpic -ansi -Wall -Wno-comment -Werror -Wno-error=unused-variable -g $(CFLAGS)

all: libronnie.so include main test

include: caos.h value.h ronnie.h common.h
	@echo " CP $^ => include/ronnie/"
	@mkdir -p include/ronnie
	@cp $^ include/ronnie

test: test.o libronnie.so
	@echo " LD $^ => $@"
	@${CXX} $^ -L. -lronnie -lgtest -lgtest_main -o $@ -Wl,-rpath,.

main: main.o libronnie.so
	@echo " LD $^ => $@"
	@${CC} $^ -L. -lronnie -o $@ -Wl,-rpath,.

main.o: include main.c
	@echo " CC main.c => $@"
	@${CC} -c -Iinclude main.c ${CFLAGS} -std=c99

libronnie.so: caos.o value.o ronnie.o lexer.o
	@echo " LD $^ => $@"
	@${CC} -shared -lstdc++ $^ -o $@

%.o: %.c
	@echo " CC $^ => $@"
	@${CC} -c $^ ${CFLAGS} -std=c99

%.o: %.cpp
	@echo " CC $^ => $@"
	@${CXX} -c $^ ${CFLAGS}

clean:
	-rm main libronnie.so test
	-rm -frv *.o
	-rm *~
	-rm -frv include
