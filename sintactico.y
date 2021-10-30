%code requires {
    #include "types.h"
}

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symtab.h"

    int yylex();
    extern FILE* yyin;
    int yyerror(const char *s);
%}

%define parse.error verbose


%union { 
    Variable var;
    char *str;
    NodeRow *nr;
    NodeCol *nc;
};

%token<var> ID
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
%token OPEN_M
%token CLOSE_M
%token SEMICOLON

%type<var> int_expression;
%type<var> float_expression;
%type<var> string_expression;
%type<var> bool_expression;
%type<var> expression;
%type<str> id_expression;
%type<var> number;
%type<nc> number_list;
%type<nr> row_list;
%type<nc> row;
%type<var> m;

%%
prog : sentence_list ;
sentence_list : sentence_list sentence ENTER | sentence ENTER;
sentence : assignation_sentence | expression;
assignation_sentence : ID EQUALS expression {
    Variable v;
    v.var_name = $1.var_name;
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
            v.val.Int64Matrix = $3.val.Int64Matrix;
            break;
        case Float64Matrix:
            v.val.Float64Matrix = $3.val.Float64Matrix;
            break;
        default:
            yyerror("Unknown type\n");
    }
    store_val(v);
};

expression : int_expression {$$ = $1;} | float_expression {$$ = $1;} | 
            string_expression {$$ = $1;} | bool_expression {$$ = $1;} | 
            id_expression {show_val($1);} | m {$$ = $1;};

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
    $$ = $1.var_name;
};

m : OPEN_M row_list CLOSE_M {
    if(DEBUG) print_node_row($2);
    store_matrix($2, &$$);
    if(DEBUG) print_var("matrix declaration", $$);
};

row_list : row SEMICOLON row_list {
    $$ = (NodeRow*) malloc(sizeof(NodeRow));
    $$->val = $1;
    $$->next = $3;
    $$->n_elem_row = $3->n_elem_row + 1;
    $$->row_type = $1->col_type == Float64 || $3->row_type == Float64 ? Float64 : Int64;
} | row {
    $$ = (NodeRow*) malloc(sizeof(NodeRow));
    $$->val = $1;
    $$->next = NULL;
    $$->row_type = $1->col_type;
    $$->n_elem_row = 1;
};

row : number_list {
    $$ = $1;

};

number_list : number number_list {
    //Creamos toda la lista
    $$ = (NodeCol*) malloc(sizeof(NodeCol));
    $$->val = $1;
    $$->next = $2;
    $$->n_elem_col = $2->n_elem_col + 1;
    $$->col_type = $1.type == Float64 || $2->col_type == Float64 ? Float64 : Int64;
} | number {
    //Creamos un nodo con toda la info de un numero
    $$ = (NodeCol*) malloc(sizeof(NodeCol));
    $$->val = $1;
    $$->next = NULL;
    $$->n_elem_col = 1;
    $$->col_type = $1.type;
};

number : INT {
    $$ = $1;
} | FLOAT {
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