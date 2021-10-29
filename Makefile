CFLAGS = -Wall -g -fsanitize=address

%: %.c
	gcc $(CFLAGS) -o $@ $^

all:
	gcc -o shell shell.c childprocess.c input.c

clean:
	rm -rf shell
	rm -rf child
	rm -rf *.o

