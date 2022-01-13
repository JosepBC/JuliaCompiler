%option noyywrap
%{
    #include "sintactico.tab.h"
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include "types.h"
    #include "code_generation.h"
%}
letter                  [a-zA-Z]
digit                   [0-9]
letterDigit             [a-zA-Z0-9]
f                       {digit}+([.]{digit}+)?(e-?{digit}+)?
string                  \".*\"
i                       {digit}+
enter                   \n+
equal                   =
b                       (true|false)
id                      {letter}+(_?{letterDigit}+)*
space                   " "
bad_id                  {letterDigit}+(_?{letterDigit}+)*
comment                 #.*
multi_line_comment      "#="[^=]*"="+([^=#]*"="*"="+)*"#"
open_m                  \[
close_m                 \]
semicolon               ;
and                     &&
or                      \|\|
not                     !
lower_than              <
higher_than             >
lower_equal             <=
higher_equal            >=
eq                      ==
diff                    !=
pow                     \^
mod                     %
div                     \/
mult                    \*
sub                     -
add                     \+
open_p                  \(
close_p                 \)
comma                   ,
foo                     function
end                     end
ret                     return
double_colon            ::
int_t                   Int32
float_t                 Float32
float_matrix_t          Matrix\{Float32\}
int_matrix_t            Matrix\{Int32\}
float_vector_t          Vector\{Float32\}
int_vector_t            Vector\{Int32\}
if                      if
else                    else
while                   while
do                      do
for                     for
in                      in
colon                   :
elseif                  elseif
%%
{elseif} {
    return ELSEIF;
}

{colon} {
    return COLON;
}

{in} {
    return IN;
}

{for} {
    return FOR;
}

{while} {
    return WHILE;
}

{do} {
    return DO;
}

{if} {
    return IF;
}

{else} {
    return ELSE;
}

{comma} {
    return COMMA;
}

{open_p} {
    return OPEN_P;
}

{close_p} {
    return CLOSE_P;
}

{pow} {
    if(DEBUG) printf("[FLEX debug] pow\n");
    return ARITHMETIC_POW;
}

{mod} {
    if(DEBUG) printf("[FLEX debug] mod\n");
    return ARITHMETIC_MOD;
}

{div} {
    if(DEBUG) printf("[FLEX debug] div\n");
    return ARITHMETIC_DIV;
}

{mult} {
    if(DEBUG) printf("[FLEX debug] mult\n");
    return ARITHMETIC_MULT;
}

{sub} {
    if(DEBUG) printf("[FLEX debug] sub\n");
    return ARITHMETIC_SUB;
}

{add} {
    if(DEBUG) printf("[FLEX debug] add\n");
    return ARITHMETIC_ADD;
}

{eq} {
    if(DEBUG) printf("[FLEX debug] bool equals\n");
    return BOOL_EQUALS;
}

{and} {
    if(DEBUG) printf("[FLEX debug] bool and\n");
    return BOOL_AND;
}

{or} {
    if(DEBUG) printf("[FLEX debug] bool or\n");
    return BOOL_OR;
}

{not} {
    if(DEBUG) printf("[FLEX debug] bool not\n");
    return BOOL_NOT;
}

{lower_equal} {
    if(DEBUG) printf("[FLEX debug] bool lower_equal\n");
    return BOOL_LOWER_EQUAL;
}

{higher_equal} {
    if(DEBUG) printf("[FLEX debug] bool higher_equal\n");
    return BOOL_HIGHER_EQUAL;
}

{lower_than} {
    if(DEBUG) printf("[FLEX debug] bool lower_than\n");
    return BOOL_LOWER_THAN;
}

{higher_than} {
    if(DEBUG) printf("[FLEX debug] bool higher_than\n");
    return BOOL_HIGHER_THAN;
}

{diff} {
    if(DEBUG) printf("[FLEX debug] bool diff\n");
    return BOOL_DIFF;
}

{equal} {return EQUALS;}

{b} {
    if(DEBUG) printf("[FLEX Debug] Bool\n");
    yylval.var.val.Bool = yytext[0] == 't';
    yylval.var.is_variable = false;
    yylval.var.type = Bool;
    return BOOL;
}

{ret} {
    return RETURN;
}

{int_matrix_t} {
    yylval.t = Int64Matrix;
    return INT_MATRIX_T;
}

{float_matrix_t} {
    yylval.t = Float64Matrix;
    return FLOAT_MATRIX_T;
}

{int_vector_t} {
    yylval.t = Int64Vector;
    return INT_VECTOR_T;
}

{float_vector_t} {
    yylval.t = Float64Vector;
    return FLOAT_VECTOR_T;
}

{int_t} {
    yylval.t = Int64;
    return INT_T;
}

{float_t} {
    yylval.t = Float64;
    return FLOAT_T;
}

{foo} {
    if(DEBUG) printf("Function\n");
    return FOO;
}

{end} {
    if(DEBUG) printf("foo end\n");
    return END;
}

{double_colon} {
    return DOUBLE_COLON;
}

{string} {
    if(DEBUG) printf("[FLEX Debug] String %s\n", yytext); 
    crop_first_last_elem(&yytext);
    yylval.var.val.String = strdup(yytext);
    yylval.var.is_variable = false;
    yylval.var.type = String; 
    return STRING;
}

{id} {
    if(DEBUG) printf("[FLEX Debug] ID: %s\n", yytext); 
    yylval.var.is_variable = true;
    yylval.var.var_name = strdup(yytext);
    yylval.var.var_name_len = strlen(yytext);
    return ID;
}

{i} {
    if(DEBUG) printf("[FLEX Debug] Int64 %s\n", yytext);
    yylval.var.var_name = "";
    yylval.var.val.Int64 = atoi(yytext);
    yylval.var.is_variable = false;
    yylval.var.type = Int64;
    return INT;
}

{f} {
    if(DEBUG) printf("[FLEX Debug] Float64 %s\n", yytext);
    yylval.var.var_name = "";
    yylval.var.val.Float64 = atof(yytext);
    yylval.var.is_variable = false;
    yylval.var.type = Float64;
    return FLOAT;
}

{open_m} {
    if(DEBUG) printf("[FLEX Debug] Open m\n");
    return OPEN_M;
}
{close_m} {
    if(DEBUG) printf("[FLEX Debug] Close m\n");
    return CLOSE_M;
}
{semicolon} {
    if(DEBUG) printf("[FLEX Debug] Semicolon\n");
    return SEMICOLON;
}

{space}|{multi_line_comment} {if(DEBUG) printf("[FLEX Debug] Multiline comment, space\n");}

{comment} {
    if(DEBUG) printf("Comment\n");
}

{enter} {return ENTER;}


.|{bad_id} {printf("Invalid token %s\n", yytext); exit(1);}

%%