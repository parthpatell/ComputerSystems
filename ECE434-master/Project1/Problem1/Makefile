all: myfiles myfiles2

myfiles: myfiles.c
	gcc -Wall -Werror -fsanitize=address myfiles.c -o myfiles
	
myfile2s: myfiles2.c
	gcc -Wall -Werror -fsanitize=address myfiles2.c -o myfiles2

clean:
	rm -rf myfiles
	rm -rf myfiles2
