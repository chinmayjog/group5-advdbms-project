CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lm

.PHONY: all clean

all : Main.o BufferManagerInterface.o BufferManager.o BufferManagerUtils.o
	${CC} ${CFLAGS} Main.o BufferManager.o BufferManagerInterface.o BufferManagerUtils.o ${LDFLAGS} -o BufferManager

BufferManagerUtils.o : BufferManagerUtils.cpp BufferManager.h
	${CC} ${CFLAGS} -c BufferManagerUtils.cpp

BufferManager.o : BufferManager.cpp BufferManager.h
	${CC} ${CFLAGS} -c BufferManager.cpp

BufferManagerInterface.o : BufferManagerInterface.cpp BufferManager.h
	${CC} ${CFLAGS} -c BufferManagerInterface.cpp

Main.o : Main.cpp BufferManager.h 
	${CC} ${CFLAGS} -c Main.cpp

clean:
	rm -f BufferManager BufferManager.o BufferManagerInterface.o Main.o BufferManagerUtils.o
