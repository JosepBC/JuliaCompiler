%option noyywrap
%{
  #include "./sintactico.tab.h"
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
%}
letter                  [a-zA-Z]
digit                   [0-9]
letterDigit             [a-zA-Z0-9]
f                       -?{digit}+([.,]{digit}+)?(e-?{digit}+)?
string                  \".*\"
i                       -?{digit}+
enter                   \n
equal                   =
b                       (true|false)
id                      {letter}+(_?{letterDigit}+)*
space                   " "
bad_id                  {letterDigit}+(_?{letterDigit}+)*
comment                 #.*\n?
multi_line_comment      #=(.*|\n)*=#
%%

{equal} {return EQUALS;}
{b} {printf("Debug: Bool\n"); yylval.b = yytext[0] == 't'; return BOOL;}
{string} {printf("Debug: String %s\n", yytext); yylval.str = strdup(yytext); return STRING;}
{id} {printf("Debug ID: %s\n", yytext); yylval.str = strdup(yytext); return ID;}
{i} {printf("Debug: Int %s\n", yytext);yylval.i = atoi(yytext); return INT;}
{f} {printf("Debug: Float %s\n", yytext); yylval.f = atof(yytext); return FLOAT;}
{enter} {return ENTER;}
{space}|{comment}|{multi_line_comment} {}
.|{bad_id} {printf("Invalid token %s\n", yytext); exit(1);}
%%