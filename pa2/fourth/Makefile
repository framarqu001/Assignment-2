all: fourth

second: fourth.c
	gcc -Wall -Werror -g -fsanitize=address,undefined fourth.c -o fourth

clean:
	rm -rf fourth
