%{
#include <stdio.h>
#define YYSTYPE char *

%}
TYPEDEF			typedef
ENUM			enum
VALUE			[^ \n\t\r,;]*
IDENT			[a-zA-Z][a-zA-Z0-9_]*
SEMI			;
EQ			=
CURLY			[{}]
COMMA			,
WS			[ \n\t\r]*
%%
{TYPEDEF}		{ printf("{TD:%s}", yytext); }
{ENUM}			{ printf("{ENUM:%s}", yytext); }
{IDENT}			{ printf("{IDENT:%s}", yytext); }
{SEMI}			{ printf("{SEMI:%s}", yytext); }
{EQ}			{ printf("{EQ:%s}", yytext); }
{CURLY}			{ printf("{CURLY:%s}", yytext); }
{COMMA}			{ printf("{COMMA:%s}", yytext); }
{WS}			{ }
{VALUE}			{ printf("{VALUE:%s}", yytext); }
%%

