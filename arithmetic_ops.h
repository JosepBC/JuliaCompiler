#ifndef ARITHMETIC_HEADER
#define ARITHMETIC_HEADER
#include "types.h"

extern void do_pow(Variable v1, Variable v2, Variable *res);
extern void do_div(Variable v1, Variable v2, Variable *res);
extern void do_mult(Variable v1, Variable v2, Variable *res);
extern void do_add(Variable v1, Variable v2, Variable *res);
extern void do_sub(Variable v1, Variable v2, Variable *res);
extern void do_chs(Variable src, Variable *dst);

#endif