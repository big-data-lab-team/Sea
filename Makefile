gcc_opts=-Wall -fPIC -c
debug_opts=-g -ggdb
all: passthrough.so
passthrough.o: passthrough.cpp passthrough.h
	gcc ${gcc_opts} ${debug_opts} passthrough.cpp
functions.o: functions.cpp functions.h
	gcc ${gcc_opts} ${debug_opts} functions.cpp
logger.o: logger.cpp logger.h
	gcc ${gcc_opts} ${debug_opts} logger.cpp
passthrough.so: passthrough.o functions.o logger.o
	gcc -shared passthrough.o functions.o logger.o ${debug_opts} -o passthrough.so -ldl -lpthread -lstdc++
clean:
	\rm -f logger.o functions.o passthrough.o passthrough.so *.gcda *.gcov
