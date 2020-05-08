all: passthrough.so
passthrough.o: passthrough.cpp passthrough.h
	gcc -Wall -fPIC -c -ggdb passthrough.cpp
passthrough.so: passthrough.o
	gcc -shared -o passthrough.so passthrough.o -ldl -lpthread -ggdb
clean:
	rm passthrough.o passthrough.so
