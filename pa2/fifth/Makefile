all: fifth

fifth:	fifth.c
	gcc -Wall -Werror -g -fsanitize=address,undefined fifth.c -o fifth

clean:
	rm -rf fifth
