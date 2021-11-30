#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "code_generation.h"

int tmp_count = 0;
int line_number = 0;

//-------------General foos-------------
void emet(char *str, ...) {
   va_list args;

   va_start(args, str);
   printf("%i: ", line_number++);
   vprintf(str, args);
   printf("\n");
   va_end(args);
}

void generate_tmp(Variable *v) {
    v->var_name = (char*) malloc(12 * sizeof(char));
    v->var_name_len = 12;
    v->is_variable = true;
    sprintf(v->var_name, "$t%i", tmp_count++);
}

char* var_to_string(Variable v, char *str, int size) {
    if(v.is_variable) {
        strncpy(str, v.var_name, size);
        str[size - 1] = 0;
    } else {
        switch(v.type) {
            case Int64:
                snprintf(str, size, "%i", v.val.Int64);
                break;
            case Float64:
                snprintf(str, size, "%f", v.val.Float64);
                break;
            case Int64Vector:
            case Float64Vector:
            case Int64Matrix:
            case Float64Matrix:
                error("Missing index!");
                break;
            default:
                error("Unknown type\n");
        }
    }

    return str;
}

char* var_to_string_idx(Variable v, char *str, int size, int idx) {
    if(is_int_or_float(v)) error("Extra index");

    if(v.is_variable) {
        strncpy(str, v.var_name, size);
        str[size - 1] = 0;
    } else {
        switch(v.type) {
            case Int64Vector:
            case Float64Vector:
            case Int64Matrix:
            case Float64Matrix:
                snprintf(str, size, "%s[%i]", v.var_name, idx);
                break;
            default:
                error("Unknown type\n");
        }
    }

    return str;
}

void check_variable_existance(Variable *v) {
    Variable var;
    if(v->is_variable) {
        if(v->var_name[0] != '$' && !val_exists_in_symtab(v->var_name)) printf_error("Undefined variable '%s'", v->var_name);
        get_val(v->var_name, &var);
        v->type = var.type;
    }
}

//-------------Type change-------------
void to_int(Variable v, Variable *res) {
    if(is_literal(v)) {
        *res = v;
        res->type = Int64;
        res->val.Int64 = get_val_int(v);
        return;
    }

    generate_tmp(res);

    res->type = Int64;
    res->is_variable = v.is_variable;
    res->var_name_len = 12;
    if(!res->is_variable) printf("Sould be done in compile time");

    switch (v.type) {
        case Int64:
            *res = v;
            break;

        case Float64:
            emet("%s := F2I %s", res->var_name, v.var_name);
            break;
            
        default:
            printf_error("Ilegal type in to int when converting variable '%s' to int", v.var_name);
            break;
    }
}

void to_float(Variable v, Variable *res) {
    if(is_literal(v)) {
        *res = v;
        res->type = Float64;
        res->val.Float64 = get_val_float(v);
        return;
    }

    generate_tmp(res);

    res->type = Float64;
    res->var_name_len = 12;
    res->is_variable = v.is_variable;
    if(!res->is_variable) printf("Sould be done in compile time");

    switch (v.type) {
        case Float64:
            *res = v;
            break;

        case Int64:
            emet("%s := I2F %s", res->var_name, v.var_name);
            break;
            
        default:
            printf_error("Ilegal type in to float when converting variable '%s' to float", v.var_name);
            break;
    }
}


//-------------Emet basic-------------
void general_arithmetic_emet(Variable v1, Variable v2, Variable *res, char op[4]) {
    check_variable_existance(&v1);
    check_variable_existance(&v2);

    if(!is_int_or_float(v1) || !is_int_or_float(v2)) printf_error("Ilegal type in %s", op);
    
    if(is_float(v1) || is_float(v2)) res->type = Float64;
    else res->type = Int64;

    if(is_literal(v1) && is_literal(v2)) printf("Emet %s: Sould be done in compile time", op);

    if(is_float(*res)) {
        to_float(v1, &v1);
        to_float(v2, &v2);
    }

    int v1_len = get_var_string_len(v1);
    int v2_len = get_var_string_len(v2);

    char *v1_str = (char*) malloc(v1_len * sizeof(char));
    char *v2_str = (char*) malloc(v2_len * sizeof(char));
    generate_tmp(res);

    emet("%s := %s %s%c %s", res->var_name, var_to_string(v1, v1_str, v1_len), op, is_int(*res) ? 'I' : 'F', var_to_string(v2, v2_str, v2_len));
}

void emet_pow(Variable v1, Variable v2, Variable *res) {
    check_variable_existance(&v1);
    check_variable_existance(&v2);
    if(is_float(v2)) printf_error("Ilegal exponent type in pow %f ^ %f", get_val_float(v1), get_val_float(v2));

    if(!is_int_or_float(v1) || !is_int(v2)) error("Ilegal type in pow");


    if(is_literal(v1) && is_literal(v2)) printf("Emet pow: Sould be done in compile time");

    //i = 0
    Variable idx;
    generate_tmp(&idx);
    emet("%s := 0", idx.var_name);

    generate_tmp(res);
    
    //Res type = base type
    if(is_float(v1)) emet("%s := 1.0", res->var_name);
    else emet("%s := 1", res->var_name);

    res->type = v1.type;

    int goto_line = line_number;


    



    int v1_len = get_var_string_len(v1);

    char *v1_str = (char*) malloc(v1_len * sizeof(char));

    emet("%s := %s MUL%c %s", res->var_name, res->var_name, is_int(*res) ? 'I' : 'F', var_to_string(v1, v1_str, v1_len));

    emet("%s := %s ADDI 1", idx.var_name, idx.var_name);

    int v2_len = get_var_string_len(v2);

    char *v2_str = (char*) malloc(v2_len * sizeof(char));

    emet("IF %s LTI %s GOTO %i", idx.var_name, var_to_string(v2, v2_str, v2_len), goto_line);




}

void emet_mod(Variable v1, Variable v2, Variable *res) {
    general_arithmetic_emet(v1, v2, res, "MOD");
}

void emet_div(Variable v1, Variable v2, Variable *res) {
    general_arithmetic_emet(v1, v2, res, "DIV");
}

void emet_mult(Variable v1, Variable v2, Variable *res) {
    general_arithmetic_emet(v1, v2, res, "MUL");
}

void emet_sub(Variable v1, Variable v2, Variable *res) {
    general_arithmetic_emet(v1, v2, res, "SUB");
}

void emet_add(Variable v1, Variable v2, Variable *res) {
    general_arithmetic_emet(v1, v2, res, "ADD");
}


void emet_chs(Variable v, Variable *res) {
    check_variable_existance(&v);

    if(!is_int_or_float(v)) error("Ilegal type in chs");

    generate_tmp(res);

    int v_len = get_var_string_len(v);

    char *str = (char*) malloc(v_len * sizeof(char));

    emet("%s := CHS%c %s", res->var_name, is_int(*res) ? 'I' : 'F',var_to_string(v, str, v_len));
}

//-------------Emet assignation v1 = v2-------------
void emet_assignation(Variable v1, Variable v2, FILE *f) {
    int v2_len = get_var_string_len(v2);
    char *v2_str = (char*) malloc(v2_len * sizeof(char));
    emet("%s := %s", v1.var_name, var_to_string(v2, v2_str, v2_len));
    
    v1.type = v2.type;
    v1.is_variable = v2.is_variable;
    v1.val = v2.val;
    v1.var_name_len = v2.var_name_len;

    store_val(v1);
}

//-------------Emet vector and matrix-------------
void emet_vector_elem(Variable v, Variable i, Variable *res) {

}

void emet_matrix_elem(Variable m, Variable i, Variable j, Variable *res) {

}
