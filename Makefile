all:
	gcc -fPIC -shared -lluajit-5.1 -g -Wall module.c -o luajit.so
	
clean:
	-rm luajit.so