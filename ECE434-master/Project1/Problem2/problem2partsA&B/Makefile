all: prob2parta prob2partb

prob2parta: prob2parta.c
	gcc -Wall -Werror -fsanitize=address prob2parta.c -o prob2parta
	
prob2partb: prob2partb.c
	gcc -Wall -Werror -fsanitize=address prob2partb.c -o prob2partb

clean:
	rm -rf prob2parta
	rm -rf prob2partb
