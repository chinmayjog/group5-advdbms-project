.PHONY: all clean

all: lex.yy.o parser.tab.o main.o
	g++ obj/parser.tab.o obj/lex.yy.o obj/main.o ../cachemgr/obj/BufferManager*.o -lfl -o parser

main.o: parser.tab.o lex.yy.o
	g++ -c src/parser.tab.c -o obj/parser.tab.o
	g++ -c src/lex.yy.c -o obj/lex.yy.o
	g++ -c src/main.cpp -o obj/main.o

parser.tab.o: src/parser.y
	bison -d src/parser.y
	mv parser.tab.c src/
	mv parser.tab.h src/
	

lex.yy.o : src/parser.l 
	flex -i --header-file=src/flexparser.h src/parser.l
	mv lex.yy.c src/


clean:
	rm -f src/parser.tab.c src/parser.tab.h src/lex.yy.c parser src/flexparser.h obj/parser.tab.o obj/lex.yy.o obj/main.o
