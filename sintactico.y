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
%token<var> INTEGER_VECTOR
%token<var> FLOAT_VECTOR
%token EQUALS
%token ENTER
%token<var> INT_MATRIX
%token<var> FLOAT_MATRIX

%type<var> int_expression;
%type<var> float_expression;
%type<var> string_expression;
%type<var> bool_expression;
%type<var> expression;
%type<str> id_expression;
%type<var> int_vector_expression;
%type<var> float_vector_expression;
%type<var> int_matrix_expression;
%type<var> float_matrix_expression;

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
        case Int64Vector:
            v.val.Int64Vector = $3.val.Int64Vector;
            break;
        case Float64Vector:
            v.val.Float64Vector = $3.val.Float64Vector;
            break;
        case Int64Matrix:
        case Float64Matrix:
            printf("Matrix\n");
            break;
        default:
            yyerror("Unknown type\n");
    }
    store_val(v, DEBUG);
};

expression : int_expression {$$ = $1;} | float_expression {$$ = $1;} | 
            string_expression {$$ = $1;} | bool_expression {$$ = $1;} | 
            id_expression {show_val($1, DEBUG);} | int_vector_expression {$$ = $1;} | 
            float_vector_expression {$$ = $1;} | int_matrix_expression {$$ = $1;} |
            float_matrix_expression {$$ = $1;};

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

int_vector_expression : INTEGER_VECTOR {
    $$ = $1;
    //printf("Int vector\n");
};

float_vector_expression : FLOAT_VECTOR {
    $$ = $1;
    //printf("Float vector\n");
};

int_matrix_expression : INT_MATRIX{
    $$ = $1;
    printf("Int matrix\n");
}

float_matrix_expression : FLOAT_MATRIX{
    $$ = $1;
    printf("Float matrix\n");
}

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