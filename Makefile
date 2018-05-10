all:
	gcc -fPIC -shared -lluajit-5.1 -g -Wall module.c -o module.so
	
clean:
	-rm module.so