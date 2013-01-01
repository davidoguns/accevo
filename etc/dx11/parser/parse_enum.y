%{
#include <stdio.h>

#define YYSTYPE char *

YYSTYPE yylval;

const char * map_name = "s_map";

void print_entry(char const *m_name, char const *name)
{
	printf("%s.Insert(SID(\"%s\"), %s);\n", m_name, name, name);
}

void yyerror(char const *str)
{
	printf("Error parsing: %s\n", str);
}

int yywrap(void) { return 1; }

%}
%token TYPEDEF ENUM VALUE IDENT SEMI EQ CURLY COMMA WS
%%
statement: typ_def SEMI {} | enum_decl SEMI {};
typ_def: TYPEDEF enum_decl IDENT { };
enum_decl: ENUM IDENT CURLY enum_entries_i CURLY { };
enum_entries_i: 
	entry enum_entries_f { };
enum_entries_f:
	COMMA entry enum_entries_f { }
	| ;
entry:
	IDENT EQ VALUE { print_entry(map_name, $1); } |
	IDENT EQ IDENT { print_entry(map_name, $1); };
%%

int main(int argc, char **argv)
{
	map_name = argv[1];
	yyparse();
}

