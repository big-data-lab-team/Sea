all: passthrough.so
passthrough.o: passthrough.cpp passthrough.h
	gcc -Wall -fPIC -g -c -ggdb passthrough.cpp
functions.o: functions.cpp functions.h
	gcc -Wall -fPIC -g -c -ggdb functions.cpp
logger.o: logger.cpp logger.h
	gcc -Wall -fPIC -g -c -ggdb logger.cpp
passthrough.so: passthrough.o functions.o logger.o
	gcc -shared passthrough.o functions.o logger.o -g -o passthrough.so  -ldl -lpthread -ggdb -lstdc++
clean:
	\rm -f logger.o functions.o passthrough.o passthrough.so
