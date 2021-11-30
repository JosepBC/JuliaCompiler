#ifndef CODE_GENERATION_HEADER
#define CODE_GENERATION_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"

extern void emet_pow(Variable v1, Variable v2, Variable *res);
extern void emet_mod(Variable v1, Variable v2, Variable *res);
extern void emet_div(Variable v1, Variable v2, Variable *res);
extern void emet_mult(Variable v1, Variable v2, Variable *res);
extern void emet_sub(Variable v1, Variable v2, Variable *res);
extern void emet_add(Variable v1, Variable v2, Variable *res);
extern void emet_chs(Variable v, Variable *res);

extern void emet_assignation(Variable v1, Variable v2, FILE *f);

extern void emet_vector_elem(Variable v, Variable i, Variable *res);
extern void emet_matrix_elem(Variable m, Variable i, Variable j, Variable *res);

#endif