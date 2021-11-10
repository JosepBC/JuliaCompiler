#include "bool_ops.h"

//-------------bool ops-------------
void do_bool_and(Variable a, Variable b, Variable *dst) {
    if(!is_bool(a) || !is_bool(b)) error("Ilegal type in and");
    dst->type = Bool;
    dst->val.Bool = a.val.Bool && b.val.Bool;
}

void do_bool_or(Variable a, Variable b, Variable *dst) {
    if(!is_bool(a) || !is_bool(b)) error("Ilegal type in and");
    dst->type = Bool;
    dst->val.Bool = a.val.Bool || b.val.Bool;
}

void do_bool_not(Variable a, Variable *dst) {
    if(!is_bool(a)) error("Ilegal type in and");
    dst->type = Bool;
    dst->val.Bool = !a.val.Bool;
}


//-------------relational ops-------------
void do_bool_equals(Variable a, Variable b, Variable *dst) {
    dst->type = Bool;
    if(is_bool(a) && is_bool(b)) {
        dst->val.Bool = a.val.Bool == b.val.Bool;
    } else if(is_int(a) && is_int(b)) {
        dst->val.Bool = a.val.Int64 == b.val.Int64;
    } else if(is_int(a) && is_float(b)) {
        dst->val.Bool = (float)a.val.Int64 == b.val.Float64;
    } else if(is_float(a) && is_int(b)) {
        dst->val.Bool = a.val.Float64 == (float)b.val.Int64;
    } else if(is_float(a) && is_float(b)) {
        dst->val.Bool = a.val.Float64 == b.val.Float64;
    } else {
        error("Ilegal type in bool equals");
    }
}

void do_bool_diff(Variable a, Variable b, Variable *dst) {
    dst->type = Bool;
    if(is_bool(a) && is_bool(b)) {
        dst->val.Bool = a.val.Bool != b.val.Bool;
    } else if(is_int(a) && is_int(b)) {
        dst->val.Bool = a.val.Int64 != b.val.Int64;
    } else if(is_int(a) && is_float(b)) {
        dst->val.Bool = (float)a.val.Int64 != b.val.Float64;
    } else if(is_float(a) && is_int(b)) {
        dst->val.Bool = a.val.Float64 != (float)b.val.Int64;
    } else if(is_float(a) && is_float(b)) {
        dst->val.Bool = a.val.Float64 != b.val.Float64;
    } else {
        error("Ilegal type in bool diff");
    }
}

void do_bool_higher_than(Variable a, Variable b, Variable *dst) {
    dst->type = Bool;
    if(is_bool(a) && is_bool(b)) {
        dst->val.Bool = a.val.Bool > b.val.Bool;
    } else if(is_int(a) && is_int(b)) {
        dst->val.Bool = a.val.Int64 > b.val.Int64;
    } else if(is_int(a) && is_float(b)) {
        dst->val.Bool = (float)a.val.Int64 > b.val.Float64;
    } else if(is_float(a) && is_int(b)) {
        dst->val.Bool = a.val.Float64 > (float)b.val.Int64;
    } else if(is_float(a) && is_float(b)) {
        dst->val.Bool = a.val.Float64 > b.val.Float64;
    } else {
        error("Ilegal type in bool higher than");
    }
}

void do_bool_lower_than(Variable a, Variable b, Variable *dst) {
    dst->type = Bool;
    if(is_bool(a) && is_bool(b)) {
        dst->val.Bool = a.val.Bool < b.val.Bool;
    } else if(is_int(a) && is_int(b)) {
        dst->val.Bool = a.val.Int64 < b.val.Int64;
    } else if(is_int(a) && is_float(b)) {
        dst->val.Bool = (float)a.val.Int64 < b.val.Float64;
    } else if(is_float(a) && is_int(b)) {
        dst->val.Bool = a.val.Float64 < (float)b.val.Int64;
    } else if(is_float(a) && is_float(b)) {
        dst->val.Bool = a.val.Float64 < b.val.Float64;
    } else {
        error("Ilegal type in bool lower_than");
    }
}

void do_bool_higher_equal(Variable a, Variable b, Variable *dst) {
    dst->type = Bool;
    if(is_bool(a) && is_bool(b)) {
        dst->val.Bool = a.val.Bool >= b.val.Bool;
    } else if(is_int(a) && is_int(b)) {
        dst->val.Bool = a.val.Int64 >= b.val.Int64;
    } else if(is_int(a) && is_float(b)) {
        dst->val.Bool = (float)a.val.Int64 >= b.val.Float64;
    } else if(is_float(a) && is_int(b)) {
        dst->val.Bool = a.val.Float64 >= (float)b.val.Int64;
    } else if(is_float(a) && is_float(b)) {
        dst->val.Bool = a.val.Float64 >= b.val.Float64;
    } else {
        error("Ilegal type in bool higher equal");
    }
}

void do_bool_lower_equal(Variable a, Variable b, Variable *dst) {
    dst->type = Bool;
    if(is_bool(a) && is_bool(b)) {
        dst->val.Bool = a.val.Bool <= b.val.Bool;
    } else if(is_int(a) && is_int(b)) {
        dst->val.Bool = a.val.Int64 <= b.val.Int64;
    } else if(is_int(a) && is_float(b)) {
        dst->val.Bool = (float)a.val.Int64 <= b.val.Float64;
    } else if(is_float(a) && is_int(b)) {
        dst->val.Bool = a.val.Float64 <= (float)b.val.Int64;
    } else if(is_float(a) && is_float(b)) {
        dst->val.Bool = a.val.Float64 <= b.val.Float64;
    } else {
        error("Ilegal type in bool lower equal");
    }
}
