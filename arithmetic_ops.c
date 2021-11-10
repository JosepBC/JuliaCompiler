#include "arithmetic_ops.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//-------------pow-------------
void do_pow(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = pow(v1.val.Int64, v2.val.Int64);
        return;
    }

    res->type = Float64;

    if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = pow(v1.val.Int64, v2.val.Float64);
    }

    if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = pow(v1.val.Float64, v2.val.Int64);
    }

    if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = pow(v1.val.Float64, v2.val.Float64);
    }

}


//-------------div-------------
void do_div(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = v1.val.Int64 / v2.val.Int64;
        return;
    } 
    
    res->type = Float64;
    
    if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Int64 / v2.val.Float64;
    }

    if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = v1.val.Float64 / v2.val.Int64;
    }

    if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Float64 / v2.val.Float64;
    }

}


//-------------mult-------------
void mult_int_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Int64Vector.v[i] = v1.val.Int64Vector.v[i] * v2.val.Int64Vector.v[i];
    }
}

void mult_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Float64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Float64Vector.v[i] * v2.val.Float64Vector.v[i];
    }
}

void mult_int_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Int64Vector.v[i] * v2.val.Float64Vector.v[i];
    }
}

void do_vector_mult(Variable v1, Variable v2, Variable *res) {
    if(is_int_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Int64Vector;
        res->val.Int64Vector.v = (int *)malloc(sizeof(int) * v1.val.Int64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        mult_int_vector(v1, v2, res);
    }
    
    if(is_int_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Int64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        mult_int_float_vector(v1, v2, res);
    }

    if(is_float_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        mult_int_float_vector(v2, v1, res);
    }

    if(is_float_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        mult_float_vector(v1, v2, res);
    }
}

void mult_int_scalar_vector(Variable v1, Variable v2, Variable *res) {
    if(is_int_vector(v2)) {
        res->type = Int64Vector;
        res->val.Int64Vector.n_elem = v2.val.Int64Vector.n_elem;
        res->val.Int64Vector.v = (int *)malloc(sizeof(int) * v2.val.Int64Vector.n_elem);
        for(int i = 0; i < res->val.Int64Vector.n_elem; i++) {
            res->val.Int64Vector.v[i] = v1.val.Int64 * v2.val.Int64Vector.v[i];
        }
    } else {
        res->type = Float64Vector;
        res->val.Float64Vector.n_elem = v2.val.Float64Vector.n_elem;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v2.val.Float64Vector.n_elem);
        for(int i = 0; i < res->val.Float64Vector.n_elem; i++) {
            res->val.Float64Vector.v[i] = v1.val.Int64 * v2.val.Float64Vector.v[i];
        }
    }
}

void mult_float_scalar_vector(Variable v1, Variable v2, Variable *res) {
    res->type = Float64Vector;
    if(is_int_vector(v2)) {
        res->val.Float64Vector.n_elem = v2.val.Int64Vector.n_elem;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v2.val.Int64Vector.n_elem);
        for(int i = 0; i < res->val.Float64Vector.n_elem; i++) {
            res->val.Float64Vector.v[i] = v1.val.Float64 * v2.val.Int64Vector.v[i];
        }
    } else {
        res->val.Float64Vector.n_elem = v2.val.Float64Vector.n_elem;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v2.val.Float64Vector.n_elem);
        for(int i = 0; i < res->val.Float64Vector.n_elem; i++) {
            res->val.Float64Vector.v[i] = v1.val.Float64 * v2.val.Float64Vector.v[i];
        }
    }
}

void do_mult_vector_scalar(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_vector(v2)) mult_int_scalar_vector(v1, v2, res);
    if(is_float(v1) && is_vector(v2)) mult_float_scalar_vector(v1, v2, res);
    
    if(is_vector(v1) && is_int(v2)) mult_int_scalar_vector(v2, v1, res);
    if(is_vector(v1) && is_float(v2)) mult_float_scalar_vector(v2, v1, res);
}

char* concat_string(Variable v1, Variable v2) {
    int size_v1 = strlen(v1.val.String);
    int size_v2 = strlen(v2.val.String);
    char *concated = malloc(size_v1 + size_v2 + 1);
    strcpy(concated, v1.val.String);
    strcat(concated, v2.val.String);
    return concated;
}

void do_mult(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = v1.val.Int64 * v2.val.Int64;
    } 
    
    if(is_string(v1) && is_string(v2)) {
        res->type = String;
        res->val.String = concat_string(v1, v2);
    }
    
    if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Int64 * v2.val.Float64;
        res->type = Float64;
    }

    if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = v1.val.Float64 * v2.val.Int64;
        res->type = Float64;
    }

    if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Float64 * v2.val.Float64;
        res->type = Float64;
    }

    if(is_vector(v1) && is_int_or_float(v2) || is_int_or_float(v1) && is_vector(v2)) do_mult_vector_scalar(v1, v2, res);

    if(is_vector(v1) && is_vector(v2)) do_vector_mult(v1, v2, res);

}


//-------------add-------------
void add_int_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Int64Vector.v[i] = v1.val.Int64Vector.v[i] + v2.val.Int64Vector.v[i];
    }
}

void add_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Float64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Float64Vector.v[i] + v2.val.Float64Vector.v[i];
    }
}

void add_int_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Int64Vector.v[i] + v2.val.Float64Vector.v[i];
    }
}

void do_vector_add(Variable v1, Variable v2, Variable *res) {
    if(is_int_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Int64Vector;
        res->val.Int64Vector.v = (int *)malloc(sizeof(int) * v1.val.Int64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        add_int_vector(v1, v2, res);
    } else if(is_int_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Int64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        add_int_float_vector(v1, v2, res);
    } else if(is_float_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        add_int_float_vector(v2, v1, res);
    } else if(is_float_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        add_float_vector(v1, v2, res);
    } else {
        error("Ilegal type in vector add");
    }
}


void do_add(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = v1.val.Int64 + v2.val.Int64;
    } else if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Int64 + v2.val.Float64;
        res->type = Float64;
    } else if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = v1.val.Float64 + v2.val.Int64;
        res->type = Float64;
    } else if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Float64 + v2.val.Float64;
        res->type = Float64;
    } else if(is_vector(v1) && is_vector(v2)) {
        do_vector_add(v1, v2, res);
    } else {
        error("Ilegal type in add");
    }
}


//-------------sub-------------
void sub_int_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Int64Vector.v[i] = v1.val.Int64Vector.v[i] - v2.val.Int64Vector.v[i];
    }
}

void sub_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Float64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Float64Vector.v[i] - v2.val.Float64Vector.v[i];
    }
}

void sub_int_float_vector(Variable v1, Variable v2, Variable *res) {
    for(int i = 0; i < v1.val.Int64Vector.n_elem; i++) {
        res->val.Float64Vector.v[i] = v1.val.Int64Vector.v[i] - v2.val.Float64Vector.v[i];
    }
}

void do_vector_sub(Variable v1, Variable v2, Variable *res) {
    if(is_int_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Int64Vector;
        res->val.Int64Vector.v = (int *)malloc(sizeof(int) * v1.val.Int64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        sub_int_vector(v1, v2, res);
    } else if(is_int_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Int64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Int64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Int64Vector.n_elem;
        
        sub_int_float_vector(v1, v2, res);
    } else if(is_float_vector(v1) && is_int_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Int64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Int64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        sub_int_float_vector(v2, v1, res);
    } else if(is_float_vector(v1) && is_float_vector(v2)) {
        if(v1.val.Float64Vector.n_elem != v2.val.Float64Vector.n_elem) error("Can't add two vectors with different size");
        
        res->type = Float64Vector;
        res->val.Float64Vector.v = (float *)malloc(sizeof(float) * v1.val.Float64Vector.n_elem);
        res->val.Float64Vector.n_elem = v1.val.Float64Vector.n_elem;
        
        sub_float_vector(v1, v2, res);
    } else {
        error("Ilegal type in sub vector");
    }
}

void do_sub(Variable v1, Variable v2, Variable *res) {
    if(is_int(v1) && is_int(v2)) {
        res->type = Int64;
        res->val.Int64 = v1.val.Int64 - v2.val.Int64;
    } else if(is_int(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Int64 - v2.val.Float64;
        res->type = Float64;
    } else if(is_float(v1) && is_int(v2)) {
        res->val.Float64 = v1.val.Float64 - v2.val.Int64;
        res->type = Float64;
    } else if(is_float(v1) && is_float(v2)) {
        res->val.Float64 = v1.val.Float64 - v2.val.Float64;
        res->type = Float64;
    } else if(is_vector(v1) && is_vector(v2)) {
        do_vector_sub(v1, v2, res);
    } else {
        error("Ilegal type in sub");
    }

}



//-------------chs-------------
void do_chs(Variable src, Variable *dst) {
    dst->type = src.type;
    dst->var_name = src.var_name;
    switch (src.type) {
        case Int64:
            dst->val.Int64 = -1 * src.val.Int64;
            break;
        case Float64:
            dst->val.Float64 = -1 * src.val.Float64;
            break;
        default:
            error("Ilegal type in chs!");
            break;
    }
}
