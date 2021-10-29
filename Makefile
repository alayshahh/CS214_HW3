CFLAGS = -Wall -g -fsanitize=address

%: %.c
	gcc $(CFLAGS) -o $@ $^

all:
	gcc -Wall -Werror -c -o childprocess.o childprocess.c
	gcc -Wall -Werror -c -o input.o input.c
	gcc -Wall -Werror -c -o shell.o shell.c
	gcc -Wall -Werror -o shell shell.o input.o childprocess.o

clean:
	rm -rf shell
	rm -rf child

