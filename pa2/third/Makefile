all: third

second: third.c
	gcc -Wall -Werror -g -fsanitize=address,undefined third.c -o third

clean:
	rm -rf third
