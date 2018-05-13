FILES = lua-emacs.c emacs-interaction.c

all:
	gcc -L$(shell pwd)/target/release/ -lemacs_module -fPIC -shared -lluajit-5.1 -g -Wall $(FILES) -o luajit.so
	
clean:
	-rm luajit.so