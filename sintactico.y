%code requires {
    #include "types.h"
    #include "arithmetic_ops.h"
    #include "bool_ops.h"
    #include "code_generation.h"
}

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symtab.h"

    int yylex();
    extern FILE* yyin;
    FILE* out;
    int yyerror(const char *s);
%}

%define parse.error verbose


%union { 
    Variable var;
    char *str;
    NodeRow *nr;
    NodeCol *nc;
    Type t;
    ArgList *args;
};

%token<var> ID
%token<var> INT
%token<var> FLOAT
%token<var> STRING 
%token<var> BOOL
%token EQUALS
%token ENTER
%token OPEN_M
%token CLOSE_M
%token SEMICOLON
%token BOOL_EQUALS
%token BOOL_AND
%token BOOL_OR
%token BOOL_NOT
%token BOOL_LOWER_EQUAL
%token BOOL_HIGHER_EQUAL
%token BOOL_LOWER_THAN
%token BOOL_HIGHER_THAN
%token BOOL_DIFF
%token ARITHMETIC_POW
%token ARITHMETIC_MOD
%token ARITHMETIC_DIV
%token ARITHMETIC_MULT
%token ARITHMETIC_SUB
%token ARITHMETIC_ADD
%token OPEN_P
%token CLOSE_P
%token COMMA
%token FOO
%token END
%token DOUBLE_COLON
%token<t> INT_T
%token<t> FLOAT_T
%token<t> INT_MATRIX_T
%token<t> FLOAT_MATRIX_T
%token<t> INT_VECTOR_T
%token<t> FLOAT_VECTOR_T
%token RETURN

%type<var> int_expression;
%type<var> float_expression;
%type<var> string_expression;
%type<var> expression;
%type<var> id_expression;
%type<var> number;
%type<nc> number_list;
%type<nr> row_list;
%type<nc> row;
%type<var> m;
%type<var> pow_list;
%type<var> mult_list;
%type<var> add_list;
%type<var> value;
%type<var> arithmetic_parenthesis;
%type<var> vector_elem;
%type<var> matrix_elem;
%type<var> function_signature;
%type<var> action_signature;
%type<var> header;
%type<t> type;

%type<args> param_list;
%type<args> non_empty_param_list;
%type<args> param;

%%
prog : function_list start sentence_list {emet_end_main();};
function_list : non_empty_function_list ENTER | %empty;
non_empty_function_list : non_empty_function_list ENTER function | function;
start : %empty {emet_start_foo("main");};
sentence_list : sentence_list sentence ENTER | sentence_list ENTER | %empty;
sentence : assignation_sentence | expression {emet_print_var($1);};
assignation_sentence : ID EQUALS expression {
    emet_assignation($1, $3, out);
};

type : INT_T {$$ = $1;} | FLOAT_T {$$ = $1;} |
        INT_MATRIX_T {$$ = $1;} | FLOAT_MATRIX_T {$$ = $1;} |
        INT_VECTOR_T {$$ = $1;} | FLOAT_VECTOR_T {$$ = $1;};

param : ID DOUBLE_COLON type {
    $$ = (ArgList*) malloc(sizeof(ArgList));
    $$->arg.name = strdup($1.var_name);
    $$->arg.type = $3;
    $$->n_args = 1;
    $$->next = NULL;
    // printf("Param %s of type %i\n", $1.var_name, (int)$3);
};

non_empty_param_list : param COMMA non_empty_param_list {
    $$ = (ArgList*) malloc(sizeof(ArgList));
    $$->arg = $1->arg;
    $$->next = $3;
    $$->n_args = $1->n_args + 1;
} | param {$$ = $1;};

param_list : non_empty_param_list {$$ = $1;} | %empty {$$ = NULL;};

function_signature : header OPEN_P param_list CLOSE_P DOUBLE_COLON type {
    $$.type = Function;
    $$.var_name = $1.var_name;

    store_args($3, &$$.val.Function.args, &$$.val.Function.n_args);
    if(DEBUG) print_args($$);

    $$.val.Function.return_type = $6;
    if(DEBUG) printf("Return of type %i\n", (int)$6);

    store_val($$);

    push_symtab();

    store_return_type($6);
};

action_signature : header OPEN_P param_list CLOSE_P {
    $$.type = Action;
    $$.var_name = $1.var_name;

    store_args($3, &$$.val.Action.args, &$$.val.Action.n_args);
    if(DEBUG) print_args($$);

    store_val($$);
    push_symtab();
};

header : FOO ID {
    check_function_existance($2.var_name);
    $$ = $2;
    emet_start_foo($2.var_name);
};

function_sentence_list : sentence_list RETURN expression {
    emet_return($3);
};

action_sentence_list : sentence_list RETURN ENTER | sentence_list;


function : function_signature ENTER function_sentence_list ENTER END {
    emet_end_foo();
    pop_symtab();
} | action_signature ENTER action_sentence_list END {
    emet_action_return();
    emet_end_foo();
    pop_symtab();
};

expression : add_list{$$ = $1;};



add_list : add_list ARITHMETIC_ADD mult_list {
    if(DEBUG) printf("add\n");

    if(is_literal($1) && is_literal($3)) do_add($1, $3, &$$);
    else emet_add($1, $3, &$$);
} | add_list ARITHMETIC_SUB mult_list {
    if(DEBUG) printf("sub\n");

    if(is_literal($1) && is_literal($3)) do_sub($1, $3, &$$);
    else emet_sub($1, $3, &$$);
} | mult_list {$$ = $1;}


mult_list : mult_list ARITHMETIC_MULT pow_list {
    if(DEBUG) printf("mult\n");

    if(is_literal($1) && is_literal($3)) do_mult($1, $3, &$$);
    else emet_mult($1, $3, &$$);

} | mult_list ARITHMETIC_DIV pow_list {
    if(DEBUG) printf("div\n");

    if(is_literal($1) && is_literal($3)) do_div($1, $3, &$$);
    else emet_div($1, $3, &$$);
} | mult_list ARITHMETIC_MOD pow_list {
    if(DEBUG) printf("mod\n");

    if(!is_int($1) || !is_int($3)) yyerror("Ilegal type in mod!");

    if(is_literal($1) && is_literal($3)) {
        $$.type = Int64;
        $$.val.Int64 = $1.val.Int64 % $3.val.Int64;
    } else {
        emet_mod($1, $3, &$$);
    }

} | pow_list { 
    $$ = $1;
}

pow_list : pow_list ARITHMETIC_POW value {
    if(DEBUG) printf("Pow list\n");
    if(!is_int_or_float($1) || !is_int_or_float($3)) yyerror("Ilegal type in pow!");
    
    if(is_literal($1) && is_literal($3)) do_pow($1, $3, &$$);
    else emet_pow($1, $3, &$$);
} | value {
    if(DEBUG) printf("expression\n");
    $$ = $1;
}




matrix_elem : ID OPEN_M expression COMMA expression CLOSE_M {
    emet_matrix_elem($1, $3, $5, &$$);
};

vector_elem : ID OPEN_M expression CLOSE_M {
    emet_vector_elem($1, $3, &$$);
    // get_vector_elem($1.var_name, $3.val.Int64, &$$);
};

arithmetic_parenthesis : OPEN_P add_list CLOSE_P {$$ = $2;};


int_expression : INT {
    $$ = $1;
};

float_expression : FLOAT {
    $$ = $1;
};

string_expression : STRING {
    $$ = $1;
};

id_expression : ID {
    $$ = $1;
};

value : int_expression {$$ = $1;} | float_expression {$$ = $1;} |
            string_expression {$$ = $1;} |
            id_expression {/* get_val($1.var_name, &$$);*/} | m {$$ = $1;} | arithmetic_parenthesis {$$ = $1;} |
            vector_elem{$$ = $1;} | matrix_elem {$$ = $1;} |
            ARITHMETIC_SUB value {
                if(is_literal($2)) do_chs($2, &$$);
                else emet_chs($2, &$$);
            };




m : OPEN_M row_list CLOSE_M {
    if(DEBUG) print_node_row($2);
    store_matrix($2, &$$);
    // if(DEBUG) print_var("matrix declaration", $$);
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
    // yyin = fopen(argv[1], "r");
    // out = fopen(argv[2], "w");
    out = stdout;
    yyparse();

    // fclose(out);
    // fclose(yyin);
    return 1;
}