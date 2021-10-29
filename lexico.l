%option noyywrap
%{
    #include "./sintactico.tab.h"
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include "types.h"
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
multi_line_comment      #=({letterDigit}*|\n)*=#\n?
open_m                  \[
close_m                 \]
semicolon               ;
%%

{equal} {return EQUALS;}
{b} {
    printf("[FLEX Debug] Bool\n");
    yylval.var.val.Bool = yytext[0] == 't';
    yylval.var.type = Bool;
    return BOOL;
}

{string} {
    printf("[FLEX Debug] String %s\n", yytext); 
    crop_first_last_elem(&yytext);
    yylval.var.val.String = yytext;  
    yylval.var.type = String; 
    return STRING;
}

{id} {
    printf("[FLEX Debug] ID: %s\n", yytext); 
    yylval.var.var_name = strdup(yytext); 
    return ID;
}

{i} {
    printf("[FLEX Debug] Int64 %s\n", yytext);
    yylval.var.val.Int64 = atoi(yytext);
    yylval.var.type = Int64;
    return INT;
}

{f} {
    printf("[FLEX Debug] Float64 %s\n", yytext);
    yylval.var.val.Float64 = atof(yytext);
    yylval.var.type = Float64;
    return FLOAT;
}

{open_m} {
    printf("[FLEX Debug] Open m\n");
    return OPEN_M;
}
{close_m} {
    printf("[FLEX Debug] Close m\n");
    return CLOSE_M;
}
{semicolon} {
    printf("[FLEX Debug] Semicolon\n");
    return SEMICOLON;
}
{space}|{comment}|{multi_line_comment} {printf("[FLEX Debug] Comment, multiline comment, space\n");}

{enter} {return ENTER;}


.|{bad_id} {printf("Invalid token %s\n", yytext); exit(1);}

%%