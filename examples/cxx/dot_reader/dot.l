%{

extern "C" {
  int yylex(void);
}

#include <stdio.h>
#include <stdlib.h>
#include <dot_parser.hpp> /* generated file */

%}

%%
[ \t\n]    /* Ignore */
\{         return OBRACE;
\}         return CBRACE;
;          return SEMICOLON;
\[         return OSBRACE;
\]         return CSBRACE;
=          return  EQUAL;
\"         return QUOTE;
->         return EDGE;
digraph    return DIGRAPH;
weight     return WEIGHT;
name       return NAME;
[a-z,A-Z]+ return WORD;
[0-9]+     yylval = atoi (yytext); return NUMBER;
%%
