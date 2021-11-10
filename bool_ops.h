#ifndef BOOL_OPS_HEADER
#define BOOL_OPS_HEADER
#include "types.h"

extern void do_bool_equals(Variable a, Variable b, Variable *dst);
extern void do_bool_and(Variable a, Variable b, Variable *dst);
extern void do_bool_or(Variable a, Variable b, Variable *dst);
extern void do_bool_not(Variable a, Variable *dst);
extern void do_bool_diff(Variable a, Variable b, Variable *dst);
extern void do_bool_higher_than(Variable a, Variable b, Variable *dst);
extern void do_bool_lower_than(Variable a, Variable b, Variable *dst);
extern void do_bool_higher_equal(Variable a, Variable b, Variable *dst);
extern void do_bool_lower_equal(Variable a, Variable b, Variable *dst);

#endif