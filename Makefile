gcc_opts=-Wall -fPIC -c -std=c++0x
debug_opts=-g -ggdb
all: passthrough.so test
config.o: config.cpp
	gcc ${gcc_opts} ${debug_opts} config.cpp
passthrough.o: passthrough.cpp passthrough.h
	gcc ${gcc_opts} ${debug_opts} passthrough.cpp
sea.o: sea.cpp sea.h
	gcc ${gcc_opts} ${debug_opts} sea.cpp
functions.o: functions.cpp functions.h
	gcc ${gcc_opts} ${debug_opts} functions.cpp
logger.o: logger.cpp logger.h
	gcc ${gcc_opts} ${debug_opts} logger.cpp
passthrough.so: passthrough.o functions.o logger.o config.o sea.o
	gcc -shared passthrough.o functions.o logger.o config.o sea.o ${debug_opts} -o passthrough.so -ldl -lpthread -lstdc++ -liniparser
test_custom.o: tests/test_custom.cpp passthrough.o sea.o config.o logger.o
	gcc ${debug_opts} -std=c++11 -Wall -fPIC tests/test_custom.cpp passthrough.o sea.o config.o logger.o -o tests/test_custom -lstdc++ -liniparser -ldl -lgtest -lpthread 
test: test_custom.o
clean:
	\rm -f logger.o functions.o passthrough.o config.o sea.o passthrough.so *.gcda *.gcov *.gcno tests/test_custom
