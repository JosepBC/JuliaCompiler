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
%%

{equal} {return EQUALS;}
{b} {
    printf("Debug: Bool\n");
    yylval.var.val.Bool = yytext[0] == 't';
    yylval.var.type = Bool;
    return BOOL;
}

{string} {
    printf("Debug: String %s\n", yytext); 
    char *s = strdup(yytext);    
    char *str = s + 1;
    str[strlen(str) - 1] = '\0';
    yylval.var.val.String = str;  
    yylval.var.type = String; 
    return STRING;
}

{id} {
    printf("Debug ID: %s\n", yytext); 
    yylval.var.var_name = strdup(yytext); 
    return ID;
}

{i} {
    printf("Debug: Int %s\n", yytext);
    yylval.var.val.Int64 = atoi(yytext);
    yylval.var.type = Int64;
    return INT;
}

{f} {
    printf("Debug: Float %s\n", yytext);
    yylval.var.val.Float64 = atof(yytext);
    yylval.var.type = Float64;
    return FLOAT;
}

{enter} {return ENTER;}

{space}|{comment}|{multi_line_comment} {printf("Debug: Comment\n");}

.|{bad_id} {printf("Invalid token %s\n", yytext); exit(1);}

%%