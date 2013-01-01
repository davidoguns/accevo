rm *.o
rm *.tab.*
rm lex.yy.c
bison -d parse_enum.y && cc -c -o parse_enum.o parse_enum.tab.c -lfl
flex parse_enum.l && cc -c -o lex_enum.o lex.yy.c -lfl
cc -o make_map_of_enum lex_enum.o parse_enum.o -lfl
