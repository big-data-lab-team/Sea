gcc_opts=-Wall -fPIC -c -std=c++0x
debug_opts=-g -ggdb

all: sea.so
test: sea.so test_custom.o

config.o: src/config.cpp
	gcc ${gcc_opts} ${debug_opts} src/config.cpp -o build/config.o
passthrough.o: src/passthrough.cpp src/passthrough.h
	gcc ${gcc_opts} ${debug_opts} src/passthrough.cpp -o build/passthrough.o
sea.o: src/sea.cpp src/sea.h
	gcc ${gcc_opts} ${debug_opts} src/sea.cpp -o build/sea.o
functions.o: src/functions.cpp src/functions.h
	gcc ${gcc_opts} ${debug_opts} src/functions.cpp -o build/functions.o
logger.o: src/logger.cpp src/logger.h
	gcc ${gcc_opts} ${debug_opts} src/logger.cpp -o build/logger.o
sea.so: passthrough.o functions.o logger.o config.o sea.o
	gcc -shared build/passthrough.o build/functions.o build/logger.o build/config.o build/sea.o ${debug_opts} -o build/sea.so -ldl -lpthread -lstdc++ -liniparser
test_custom.o: tests/test_custom.cpp passthrough.o sea.o config.o logger.o
	gcc ${debug_opts} -std=c++0x -Wall -fPIC tests/test_custom.cpp build/passthrough.o build/sea.o build/config.o build/logger.o -o tests/test_custom -lstdc++ -liniparser -ldl -lgtest -lpthread 
clean:
	\rm -f build/logger.o build/functions.o build/passthrough.o build/config.o build/sea.o build/sea.so build/*.gcda build/*.gcov build/*.gcno tests/test_custom
