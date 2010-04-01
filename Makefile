main: main.c caos.c map.c intstack.c token.c
	clang -g $^ -o $@

clean:
	rm main
