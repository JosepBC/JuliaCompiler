#ifndef CODE_GENERATION_HEADER
#define CODE_GENERATION_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"

typedef struct Instruction_t {
    char *instr_str;
    bool incomplete_goto;
    int goto_line;
} Instruction;

typedef struct Instruction_list_t {
    Instruction curr;
    struct Instruction_list_t *next;
} InstructionList;

extern void set_out_file(FILE *f);
extern void check_variable_existance(Variable *v);

extern void emet_pow(Variable v1, Variable v2, Variable *res);
extern void emet_mod(Variable v1, Variable v2, Variable *res);
extern void emet_div(Variable v1, Variable v2, Variable *res);
extern void emet_mult(Variable v1, Variable v2, Variable *res);
extern void emet_sub(Variable v1, Variable v2, Variable *res);
extern void emet_add(Variable v1, Variable v2, Variable *res);
extern void emet_chs(Variable v, Variable *res);

extern void emet_vector_elem_assignation(Variable v1, Variable i, Variable v2);
extern void emet_matrix_elem_assignation(Variable v1, Variable i, Variable j, Variable v2);
extern void emet_assignation(Variable v1, Variable v2);
extern void emet_function_assignation(Variable v1, Variable v2);

extern void emet_vector_elem(Variable v, Variable i, Variable *res);
extern void emet_matrix_elem(Variable m, Variable i, Variable j, Variable *res);

extern void emet_print_var(Variable v);

extern void emet_end_main();
extern void emet_start_foo(char *foo_name);
extern void emet_end_foo();
extern void emet_return(Variable v);
extern void emet_action_return();

extern void function_call_emet(char *foo_name, CallArgList *args, Variable *foo);
#endif