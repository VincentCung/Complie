
bison -d parser.y
flex parser.l 
gcc  lex.yy.c parser.tab.c main.c analysis.c -o parser 
.\parser test.c
