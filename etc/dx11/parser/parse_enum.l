%{
#include <stdio.h>
#define YYSTYPE char *
#include "parse_enum.tab.h"

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
{TYPEDEF}		{ return TYPEDEF; }
{ENUM}			{ return ENUM; }
{IDENT}			{ yylval=strdup(yytext); return IDENT; }
{SEMI}			{ return SEMI; }
{EQ}			{ return EQ; }
{CURLY}			{ return CURLY; }
{COMMA}			{ return COMMA; }
{WS}			{ }
{VALUE}			{ return VALUE; }
%%

