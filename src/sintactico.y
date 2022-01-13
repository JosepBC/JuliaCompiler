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
    CallArgList *call_args;
    int sq;
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
%token IF
%token ELSE
%token WHILE
%token DO

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
%type<var> or_list;
%type<var> and_list;
%type<var> not_list;
%type<var> bool_relational_list;
%type<var> boolean_expression;
%type<var> bool_parenthesis;
%type<var> bool_value;
%type<var> bool_equals_list;

%type<args> param_list;
%type<args> non_empty_param_list;
%type<args> param;

%type<call_args> param_call_list;
%type<call_args> non_empty_param_call_list;
%type<call_args> param_call;

%type<var> function_call;

%type<sq> lambda;

%type<var> sentence_list;
%type<var> action_sentence_list;
%type<var> sentence;
%type<var> assignation_sentence;

%type<var> ifs;
%type<var> simple_conditional_sentence;
%type<var> simple_else_condition_sentence;
%type<var> skip_else;

%type<var> loops;
%type<var> while;


%%
prog : function_list start sentence_list {emet_end_main(); print_instruction_list();};
function_list : non_empty_function_list ENTER | %empty;
non_empty_function_list : non_empty_function_list ENTER function | function;
start : %empty {emet_start_foo("main");};
sentence_list : sentence_list lambda sentence ENTER {
    completa($1.nexts, $2);
    $$.nexts = $3.nexts;
} | sentence_list ENTER | %empty {};
sentence : assignation_sentence | ifs | loops | expression {emet_print_var($1);};
assignation_sentence : ID EQUALS expression {
    emet_assignation($1, $3);
} | ID OPEN_M expression CLOSE_M EQUALS expression {
    emet_vector_elem_assignation($1, $3, $6);
} | ID OPEN_M expression COMMA expression CLOSE_M EQUALS expression {
    emet_matrix_elem_assignation($1, $3, $5, $8);

};

param_call : expression {
    $$ = (CallArgList*) malloc(sizeof(CallArgList));
    if(is_variable($1)) check_variable_existance(&$1);
    $$->arg = $1;
    $$->n_args = 1;
    $$->next = NULL;
    if(DEBUG) printf("Param call '%s' of type '%s'\n", $1.var_name, fancy_print_type($1.type));
};

non_empty_param_call_list : param_call COMMA non_empty_param_call_list {
    $$ = (CallArgList*) malloc(sizeof(CallArgList));
    $$->arg = $1->arg;
    $$->next = $3;
    $$->n_args = $1->n_args + 1;
} | param_call {$$ = $1;};

param_call_list : non_empty_param_call_list {$$ = $1;} | %empty {$$ = NULL;};

function_call : ID OPEN_P param_call_list CLOSE_P {
    function_call_emet($1.var_name, $3, &$$);
    if(DEBUG) printf("Function call to '%s'\n", $1.var_name);
    if(is_function($$)) {
        $$.type = $$.val.Function.return_type;
        $$.is_variable = true;
    }
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
    if(DEBUG) printf("Header param '%s' of type '%s'\n", $1.var_name, fancy_print_type($3));
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

    store_args_symtab($$);
    store_return_type($6);
};

action_signature : header OPEN_P param_list CLOSE_P {
    $$.type = Action;
    $$.var_name = $1.var_name;

    store_args($3, &$$.val.Action.args, &$$.val.Action.n_args);
    if(DEBUG) print_args($$);

    store_val($$);
    push_symtab();

    store_args_symtab($$);
};

header : FOO ID {
    check_function_existance($2.var_name);
    $$ = $2;
    emet_start_foo($2.var_name);
};

function_sentence_list : sentence_list RETURN expression {
    $1.nexts = fusiona($1.nexts, $3.nexts);
    completa($1.nexts, get_line_number());
    emet_return($3);
};

action_sentence_list : sentence_list RETURN ENTER | sentence_list;


function : function_signature ENTER function_sentence_list ENTER END {
    emet_end_foo();
    pop_symtab();
} | action_signature ENTER action_sentence_list END {
    completa($3.nexts, get_line_number());
    emet_action_return();
    emet_end_foo();
    pop_symtab();
};

expression : add_list {
    $$ = $1;
};

loops : while {};

while : WHILE lambda boolean_expression DO lambda sentence_list END {
    completa($3.trues, $5);
    completa($6.nexts, $2);
    emet_in_list(false, "GOTO %i", $2);
    $$.nexts = $3.falses;
};

ifs : simple_conditional_sentence | simple_else_condition_sentence;

skip_else : %empty {
    $$.nexts = create_int_list(get_line_number());
    emet_in_list(true, "GOTO");
};

simple_else_condition_sentence : IF boolean_expression ENTER lambda sentence_list skip_else lambda ELSE sentence_list END {
    if(DEBUG) printf("Simple if else\n");
    completa($2.trues, $4);
    completa($2.falses, $7);

    $$.nexts = fusiona($5.nexts, $9.nexts);
    $$.nexts = fusiona($$.nexts, $6.nexts);
};

simple_conditional_sentence : IF boolean_expression ENTER lambda sentence_list END {
    if(DEBUG) printf("Simple if\n");
    completa($2.trues, $4);
    $$.nexts = fusiona($2.falses, $5.nexts);
};

boolean_expression : or_list {
    $$ = $1;
};

or_list : or_list BOOL_OR lambda and_list {
    if(DEBUG) printf("Bool or\n");

    $$.trues = fusiona($1.trues, $4.trues);
    $$.falses = $4.falses;
    completa($1.falses, $3);

    // if(is_literal($1) && is_literal($3)) do_bool_or($1, $3, &$$);
    // else emet_bool_or($1, $3, &$$);
} | and_list {
    $$ = $1;
};

and_list : and_list BOOL_AND lambda bool_equals_list {
    if(DEBUG) printf("Bool and\n");

    $$.trues = $4.trues;
    $$.falses = fusiona($1.falses, $4.falses);
    completa($1.trues, $3);

    // if(is_literal($1) && is_literal($3)) do_bool_and($1, $3, &$$);
    // else emet_bool_and($1, $3, &$$);
} | bool_equals_list {
    $$ = $1;
};

bool_equals_list : add_list BOOL_EQUALS add_list {
    if(DEBUG) printf("Bool equals\n");

    // if(is_literal($1) && is_literal($3)) do_bool_equals($1, $3, &$$);
    emet_bool_equals($1, $3, &$$);
} | add_list BOOL_DIFF add_list {
    if(DEBUG) printf("Bool diff\n");

    // if(is_literal($1) && is_literal($3)) do_bool_diff($1, $3, &$$);
    emet_bool_diff($1, $3, &$$);
} | bool_relational_list {
    $$ = $1;
};


bool_relational_list : add_list BOOL_HIGHER_THAN add_list {
    if(DEBUG) printf("Higher than\n");

    //if(is_literal($1) && is_literal($3)) do_bool_higher_than($1, $3, &$$);
    emet_bool_higher_than($1, $3, &$$);
} | add_list BOOL_LOWER_THAN add_list {
    if(DEBUG) printf("Lower than\n");

    //if(is_literal($1) && is_literal($3)) do_bool_lower_than($1, $3, &$$);
    emet_bool_lower_than($1, $3, &$$);
} | add_list BOOL_HIGHER_EQUAL add_list {
    if(DEBUG) printf("Higher equal than\n");

    //if(is_literal($1) && is_literal($3)) do_bool_higher_equal($1, $3, &$$);
    emet_bool_higher_equal($1, $3, &$$);
} | add_list BOOL_LOWER_EQUAL add_list {
    if(DEBUG) printf("Lower equal than\n");

    //if(is_literal($1) && is_literal($3)) do_bool_lower_equal($1, $3, &$$);
    emet_bool_lower_equal($1, $3, &$$);
} | not_list {
    $$ = $1;
};

not_list : BOOL_NOT not_list {
    if(DEBUG) printf("Bool not\n");

    $$.trues = $2.falses;
    $$.falses = $2.trues;

    // if(is_literal($2)) do_bool_not($2, &$$);
    // else emet_bool_not($2, &$$);
} | bool_value {
    // printf("Bool val\n");
    $$ = $1;
};

bool_parenthesis: OPEN_P boolean_expression CLOSE_P {$$ = $2;};

bool_value : bool_parenthesis {$$ = $1;} |
            BOOL {
                $$ = $1;

                if($$.val.Bool) {
                    $$.trues = create_int_list(get_line_number());
                    $$.falses = NULL;
                } else {
                    $$.trues = NULL;
                    $$.falses = create_int_list(get_line_number());
                }

                emet_in_list(true, "GOTO");
            };





add_list : add_list ARITHMETIC_ADD mult_list {
    if(DEBUG) printf("add\n");

    if(is_literal($1) && is_literal($3)) do_add($1, $3, &$$);
    else emet_add($1, $3, &$$);
} | add_list ARITHMETIC_SUB mult_list {
    if(DEBUG) printf("sub\n");

    if(is_literal($1) && is_literal($3)) do_sub($1, $3, &$$);
    else emet_sub($1, $3, &$$);
} | mult_list {$$ = $1;};


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
};

pow_list : pow_list ARITHMETIC_POW value {
    if(DEBUG) printf("Pow list\n");
    if(!is_int_or_float($1) || !is_int_or_float($3)) yyerror("Ilegal type in pow!");
    
    if(is_literal($1) && is_literal($3)) do_pow($1, $3, &$$);
    else emet_pow($1, $3, &$$);
} | value {
    if(DEBUG) printf("expression '%s'\n", $1.var_name);
    $$ = $1;
};




matrix_elem : ID OPEN_M expression COMMA expression CLOSE_M {
    emet_matrix_elem($1, $3, $5, &$$);
};

vector_elem : ID OPEN_M expression CLOSE_M {
    emet_vector_elem($1, $3, &$$);
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
            function_call {$$ = $1;} |
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

lambda : %empty {
    $$ = get_line_number();
};

%%

int yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n",  s);
    exit(EXIT_FAILURE);
}

#define ENTREGA
int main(int argc, char **argv) {
    FILE *out;

    #ifdef ENTREGA
    yyin = fopen(argv[1], "r");
    out = fopen(argv[2], "w");
    #endif

    #ifndef ENTREGA
    yyin = fopen(argv[1], "r");
    out = stdout;
    #endif

    set_out_file(out);
    yyparse();
    fclose(out);
    fclose(yyin);
    return 1;
}