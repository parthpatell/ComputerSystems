all: project2problem4a project2problem4b

prob2parta: project2problem4a.c
	gcc -Wall -Werror -fsanitize=address project2problem4a.c -o project2problem4a
	
prob2partb: project2problem4b.c
	gcc -Wall -Werror -fsanitize=address project2problem4b.c -o project2problem4b

clean:
	rm -rf project2problem4a
	rm -rf project2problem4b
