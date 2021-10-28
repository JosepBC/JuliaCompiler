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
integer_vector          \[({i};{space}*)*{i}\]
unique_integer_vector   \[{i}\]
float_vector            \[({f};{space}*)*{f}\]
unique_float_vector     \[{f}\]
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

{unique_integer_vector} {
    printf("[FLEX Debug] Unique integer vector will be treated as simple Int64: %s\n", yytext);
    crop_first_last_elem(&yytext);
    yylval.var.val.Int64 = atoi(yytext);
    yylval.var.type = Int64;
    return INT;
}

{unique_float_vector} {
    printf("[FLEX Debug] Float vector of one elem, will be treated as simple Float64: %s\n", yytext);
    crop_first_last_elem(&yytext);
    yylval.var.val.Float64 = atof(yytext);
    yylval.var.type = Float64;
    return FLOAT;
}

{integer_vector} {
    printf("[FLEX Debug] Integer vector %s\n", yytext);
    yylval.var.type = Int64Vector;
    crop_first_last_elem(&yytext);
    fill_vector(yytext, &yylval.var);
    return INTEGER_VECTOR;
}

{float_vector} {
    printf("[FLEX Debug] Float vector: %s\n", yytext);
    yylval.var.type = Float64Vector;
    crop_first_last_elem(&yytext);
    fill_vector(yytext, &yylval.var);
    return FLOAT_VECTOR;
}

{space}|{comment}|{multi_line_comment} {printf("[FLEX Debug] Comment, multiline comment, space\n");}

{enter} {return ENTER;}


.|{bad_id} {printf("Invalid token %s\n", yytext); exit(1);}

%%