FILES = lua-emacs.c emacs-interaction.c module.c

all:
	gcc -fPIC -shared -lluajit-5.1 -g -Wall $(FILES) -o luajit.so
	
clean:
	-rm luajit.so