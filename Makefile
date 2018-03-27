CFLAGS=-Wall -ggdb -Ilib/cpp -Ilib
LDFLAGS=-Llib lib/cpp/libmosquittopp.so.1 lib/libmosquitto.so.1 -lpthread

.PHONY: all clean

all: fds

fds: main.o mymosq.o util.o
	${CXX} $^ -o $@ ${LDFLAGS} `pkg-config --libs opencv`

main.o: main.cpp
#	${CXX} -std=c++11 $^ `pkg-config --libs --cflags opencv` ${CFLAGS} -o $@ 
	${CXX} -c -std=c++11 `pkg-config --cflags opencv` $^ -o $@ ${CFLAGS}
mymosq.o: mymosq.cpp
	${CXX} -c -std=c++11 $^ -o $@ ${CFLAGS}
util.o: util.cpp
	${CXX} -c -std=c++11 $^ -o $@ ${CFLAGS}
clean : 
	-rm -f *.o fds
