CFLAGS = -Wall -g -fsanitize=address

%: %.c
	gcc $(CFLAGS) -o $@ $^

all:
	gcc $(CFLAGS) -o shell shell.c childprocess.c input.c internal.c

debug:
	gcc -g -Werror -Wvla -o shell shell.c childprocess.c input.c internal.c

clean:
	rm -rf shell
	rm -rf child
	rm -rf *.o

