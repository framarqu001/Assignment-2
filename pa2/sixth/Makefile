all: sixth

sixth: sixth.c
	gcc -Wall -Werror -g -fsanitize=address,undefined sixth.c -o sixth

clean:
	rm -rf sixth