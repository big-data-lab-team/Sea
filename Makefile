all: passthrough.so
passthrough.o: passthrough.cpp passthrough.h
	gcc -Wall -fPIC -g -c -ggdb passthrough.cpp
passthrough.so: passthrough.o
	gcc -shared -g -o passthrough.so passthrough.o -ldl -lpthread -ggdb -lstdc++
clean:
	\rm -f passthrough.o passthrough.so
