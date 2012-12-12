CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lm

.PHONY: all clean

all : Main.o BufferManagerInterface.o BufferManager.o BufferManagerUtils.o
	${CC} ${CFLAGS} obj/Main.o obj/BufferManager.o obj/BufferManagerInterface.o obj/BufferManagerUtils.o ${LDFLAGS} -o BufferManager

BufferManagerUtils.o : src/BufferManagerUtils.cpp src/BufferManager.h
	${CC} ${CFLAGS} -c src/BufferManagerUtils.cpp -o obj/BufferManagerUtils.o

BufferManager.o : src/BufferManager.cpp src/BufferManager.h
	${CC} ${CFLAGS} -c src/BufferManager.cpp -o obj/BufferManager.o

BufferManagerInterface.o : src/BufferManagerInterface.cpp src/BufferManager.h
	${CC} ${CFLAGS} -c src/BufferManagerInterface.cpp -o obj/BufferManagerInterface.o

Main.o : src/Main.cpp src/BufferManager.h 
	${CC} ${CFLAGS} -c src/Main.cpp -o obj/Main.o

clean:
	rm -f BufferManager obj/BufferManager.o obj/BufferManagerInterface.o obj/Main.o obj/BufferManagerUtils.o
