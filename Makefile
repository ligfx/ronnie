main: main.o caos.o token.o value.o dairy.o
	clang -g $^ -lstdc++ -o $@

%.o: %.c
	clang -g -c $^

%.o: %.cpp
	clang -g -c $^

clean:
	rm main *.o
