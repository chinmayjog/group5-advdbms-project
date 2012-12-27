NAME = parser

.PHONY: all clean

all: $(NAME).tab.c lex.yy.c
	g++ -o $(NAME) $(NAME).tab.c lex.yy.c main.cpp 

$(NAME).tab.c $(NAME).tab.h: $(NAME).y
	bison -d $(NAME).y

lex.yy.c: $(NAME).tab.h
	flex --header-file=flexparser.h $(NAME).l

clean:
	rm -f $(NAME).tab.c $(NAME).tab.h lex.yy.c $(NAME) flexparser.h
