%code requires {
    #include "types.h"
}

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symtab.h"
    #define DEBUG 1

    int yylex();
    extern FILE* yyin;
    int yyerror(const char *s);
%}

%define parse.error verbose


%union { 
    Variable var;
    char* str;
};

%token<str> ID
%token<var> INT
%token<var> FLOAT
%token<var> STRING 
%token<var> BOOL
%token EQUALS
%token ENTER

%type<var> int_expression;
%type<var> float_expression;
%type<var> string_expression;
%type<var> bool_expression;
%type<var> expression;
%type<str> id_expression;
%%
prog : sentence_list ;
sentence_list : sentence_list sentence ENTER | sentence ENTER;
sentence : assignation_sentence | expression;
assignation_sentence : ID EQUALS expression {
    Variable v;
    v.var_name = $1;
    v.type = $3.type;
    switch(v.type) {
        case Int64:
            v.val.Int64 = $3.val.Int64;
            break;
        case Float64:
            v.val.Float64 = $3.val.Float64;
            break;
        case String:
            v.val.String = $3.val.String;
            break;
        case Bool:
            v.val.Bool = $3.val.Bool;
            break;
        default:
            yyerror("Unknown type\n");
    }
    store_val(v, DEBUG);
};

expression : int_expression {$$ = $1;} | float_expression {$$ = $1;} | string_expression {$$ = $1;} | bool_expression {$$ = $1;} | id_expression {show_val($1, DEBUG);};

int_expression : INT {
    $$ = $1;
};

float_expression : FLOAT {
    $$ = $1;
};

string_expression : STRING {
    $$ = $1;
};

bool_expression : BOOL {
    $$ = $1;
};

id_expression : ID {
    $$ = $1;
};
%%

int yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n",  s);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    //yyin = fopen(argv[1], "r");
    yyparse();
    return 1;
}