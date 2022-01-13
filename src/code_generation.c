#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "code_generation.h"

int tmp_count = 1;
int line_number = 1;
FILE *out_file = NULL;

InstructionList *generated_instrucitons = NULL;

//-------------General foos-------------
void set_out_file(FILE *f) {
    out_file = f;
}

void emet(char *str, ...) {
    if(out_file == NULL) printf_error("Out file is null can't emet code");

    va_list args;

    va_start(args, str);
    fprintf(out_file, "%i: ", line_number++);
    vfprintf(out_file, str, args);
    fprintf(out_file, "\n");
    va_end(args);
}

void generate_tmp(Variable *v) {
    v->var_name = (char*) malloc(12 * sizeof(char));
    v->var_name_len = 13;
    v->is_variable = true;
    sprintf(v->var_name, "$t%i", tmp_count++);
}

char* var_to_string(Variable v, char *str, int size) {
    if(v.is_variable) {
        strncpy(str, v.var_name, size);
    } else {
        switch(v.type) {
            case Int64:
                snprintf(str, size, "%i", v.val.Int64);
                break;
            case Float64:
                snprintf(str, size, "%f", v.val.Float64);
                break;
            case Bool:
                if(v.val.Bool) snprintf(str, size, "true");
                else snprintf(str, size, "false");
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

    str[size - 1] = 0;
    return str;
}

char* var_to_string_idx(Variable v, char *str, int size, int idx) {
    if(is_int_or_float(v)) error("Extra index");

    switch(v.type) {
        case Int64Vector:
        case Float64Vector:
        case Int64Matrix:
        case Float64Matrix:
            snprintf(str, size, "%s[%i]", v.var_name, idx);
            break;
        default:
            printf_error("Unknown type %i", v.type);
    }

    str[size - 1] = 0;
    return str;
}

void check_variable_existance(Variable *v) {
    Variable var;
    if(v->is_variable) {
        if(v->var_name[0] == '$') return;
        if(!val_exists_in_symtab(v->var_name)) printf_error("Undefined variable '%s' type: '%s'", v->var_name, fancy_print_type(v->type));
        get_val(v->var_name, &var);

        if(is_function(var)){
            Variable tmp;
            generate_tmp(&tmp);
            emet_function_assignation(tmp, var);
            v->var_name = tmp.var_name;
        } else v->type = var.type;

        v->val = var.val;
    }
}

char* get_string_elem(Variable v, int idx, char *str) {
    switch (v.type) {
        case Int64Vector:
            if(idx >= v.val.Int64Vector.n_elem) printf_error("Index [%i] out of bounds", idx);
            snprintf(str, 12, "%i", v.val.Int64Vector.v[idx]);
            break;
        case Float64Vector:
            if(idx >= v.val.Float64Vector.n_elem) printf_error("Index [%i] out of bounds", idx);
            snprintf(str, 12, "%f", v.val.Float64Vector.v[idx]);
            break;
        case Int64Matrix:
            snprintf(str, 12, "%i", v.val.Int64Matrix.m[idx]);
            break;
        case Float64Matrix:
            snprintf(str, 12, "%f", v.val.Float64Matrix.m[idx]);
            break;
    }

    str[11] = 0;
    return str;
}

void reset_numbers() {
    tmp_count = 1;
    line_number = 1;
}


//-------------Instruction list handle-------------
void print_instruction_list() {
    if(out_file == NULL) printf_error("Can't emet because out file is not seted!");

    InstructionList *it = generated_instrucitons;
    while(it != NULL) {
        fprintf(out_file, "%s", it->curr.instr_str);
        if(it->curr.is_goto) fprintf(out_file, " %i", it->curr.goto_line);
        fprintf(out_file, "\n");
        it = it->next;
    }
}

void insert_instruction(Instruction to_insert) {
    InstructionList *newNode = (InstructionList*)malloc(sizeof(InstructionList));
    newNode->curr = to_insert;
    newNode->next = NULL;

    if(generated_instrucitons == NULL) {
        generated_instrucitons = newNode;
    } else {
        InstructionList *last_node = generated_instrucitons;

        while(last_node->next != NULL) last_node = last_node->next;

        last_node->next = newNode;
    }

}

/**
 * @brief Get the n instruction object
 *
 * @param instr_idx Number of instruction to return, [1..nElem]
 * @return InstructionList* Instruction
 */
InstructionList *get_n_instruction(int instr_idx) {
    InstructionList *it = generated_instrucitons;
    int i = 1;

    while(i < instr_idx && it != NULL) {
        it = it->next;
        i++;
    }

    return it;
}

/**
 * @brief Completes the unknown goto lines in 'int_list' with line 'val' in global var of instructions
 *
 * @param int_list
 * @param val
 */
void completa(IntList *int_list, int val) {
    while(int_list != NULL) {
        InstructionList *instruction = get_n_instruction(int_list->val);
        if(!instruction->curr.is_goto) printf("WARN INSTRUCTION '%s' ITS NOT AN INCOMPLETE GOTO\n", instruction->curr.instr_str);
        if(instruction->curr.completed_goto) {
            printf("WARN INSTRUCTION '%s' ITS A COMPLETED GOTO TO LINE '%i'!\n", instruction->curr.instr_str, instruction->curr.goto_line);
        } else {
            instruction->curr.goto_line = val;
            instruction->curr.completed_goto = true;
            if(DEBUG) printf("Completed instruction '%s' with goto to line '%i'\n", instruction->curr.instr_str, instruction->curr.goto_line);
        }

        int_list = int_list->next;
    }
}

/**
 * @brief Creates a new list with the elements of l1 and l2 and returns a pointer to its head
 *
 * @param l1
 * @param l2
 * @return IntList*
 */
IntList *fusiona(IntList *l1, IntList *l2) {
    if(l1 == NULL) return l2;

    IntList *last_node = l1;

    while(last_node->next != NULL) {
        last_node = last_node->next;
    }

    last_node->next = l2;

    return l1;
}

void print_int_list(IntList *l) {
    IntList *it = l;
    printf("------INTLIST------\n");
    while(it != NULL) {
        printf("%i, ", it->val);
        it = it->next;
    }
    printf("\n------END-------\n");
}

IntList *create_int_list(int initial_val) {
    IntList *l = (IntList*) malloc(sizeof(IntList));
    l->val = initial_val;
    l->next = NULL;
    return l;
}

void emet_in_list(bool is_goto, char *fmt, ...) {
    va_list arglist;
    char buff[MAXINSTRSIZE];

    va_start(arglist,fmt);
    snprintf(buff, sizeof(buff), "%i: ", line_number++);
    vsnprintf(buff + strlen(buff), sizeof(buff), fmt, arglist);
    va_end(arglist);
    // printf("%s\n", buff);

    Instruction *newInstr = (Instruction*) malloc(sizeof(Instruction));
    newInstr->instr_str = strdup(buff);
    newInstr->is_goto = is_goto;
    newInstr->completed_goto = false;

    insert_instruction(*newInstr);
}

int get_line_number() {
    return line_number;
}

//-------------Type change-------------
void to_float(Variable v, Variable *res) {
    if(is_literal(v)) {
        *res = v;
        res->type = Float64;
        res->val.Float64 = get_val_float(v);
        return;
    }

    switch (v.type) {
        case Float64:
            *res = v;
            break;

        case Int64:
            generate_tmp(res);

            res->type = Float64;
            emet_in_list(false, "%s := I2F %s", res->var_name, v.var_name);
            break;
            
        default:
            printf_error("Ilegal type in to float when converting to float. Variable '%s' has type '%s' and should be 'Int64' or 'Float64'", v.var_name, fancy_print_type(v.type));
            break;
    }
}


//-------------Emet basic-------------
void general_arithmetic_emet(Variable v1, Variable v2, Variable *res, char op[4]) {
    check_variable_existance(&v1);
    check_variable_existance(&v2);

    if(!is_int_or_float(v1)) printf_error("Ilegal type '%s' in '%s' op 1", fancy_print_type(v1.type), op);
    if(!is_int_or_float(v2)) printf_error("Ilegal type '%s' in '%s' op 2", fancy_print_type(v2.type), op);
    
    if(is_float(v1) || is_float(v2)) res->type = Float64;
    else res->type = Int64;

    if(is_literal(v1) && is_literal(v2)) printf("Emet %s: Sould be done in compile time\n", op);

    if(is_float(*res)) {
        to_float(v1, &v1);
        to_float(v2, &v2);
    }

    int v1_len = get_var_string_len(v1);
    int v2_len = get_var_string_len(v2);

    char *v1_str = (char*) malloc(v1_len * sizeof(char));
    char *v2_str = (char*) malloc(v2_len * sizeof(char));
    generate_tmp(res);

    emet_in_list(false, "%s := %s %s%c %s", res->var_name, var_to_string(v1, v1_str, v1_len), op, is_int(*res) ? 'I' : 'F', var_to_string(v2, v2_str, v2_len));
}

void emet_pow(Variable v1, Variable v2, Variable *res) {
    check_variable_existance(&v1);
    check_variable_existance(&v2);
    if(is_float(v2)) printf_error("Ilegal exponent type 'Float64' in pow %f ^ %f", get_val_float(v1), get_val_float(v2));

    if(!is_int_or_float(v1)) printf_error("Ilegal type '%s' in pow", fancy_print_type(v1.type));
    if(!is_int(v2)) printf_error("Ilegal type '%s' in pow", fancy_print_type(v2.type));


    if(is_literal(v1) && is_literal(v2)) printf("Emet pow: Sould be done in compile time\n");

    //i = 0
    Variable idx;
    generate_tmp(&idx);
    emet_in_list(false, "%s := 0", idx.var_name);

    generate_tmp(res);
    
    //Res type = base type
    if(is_float(v1)) emet_in_list(false, "%s := 1.0", res->var_name);
    else emet_in_list(false, "%s := 1", res->var_name);

    res->type = v1.type;

    int goto_line = line_number;

    int v1_len = get_var_string_len(v1);

    char *v1_str = (char*) malloc(v1_len * sizeof(char));

    emet_in_list(false, "%s := %s MUL%c %s", res->var_name, res->var_name, is_int(*res) ? 'I' : 'F', var_to_string(v1, v1_str, v1_len));

    emet_in_list(false, "%s := %s ADDI 1", idx.var_name, idx.var_name);

    int v2_len = get_var_string_len(v2);

    char *v2_str = (char*) malloc(v2_len * sizeof(char));

    emet_in_list(false, "IF %s LTI %s GOTO %i", idx.var_name, var_to_string(v2, v2_str, v2_len), goto_line);
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

    if(!is_int_or_float(v)) printf_error("Ilegal type '%s' in chs", fancy_print_type(v.type));

    generate_tmp(res);

    int v_len = get_var_string_len(v);

    char *str = (char*) malloc(v_len * sizeof(char));

    emet_in_list(false, "%s := CHS%c %s", res->var_name, is_int(*res) ? 'I' : 'F', var_to_string(v, str, v_len));
}


//-------------Emet assignation v1 = v2-------------

//v[1] = 10
void emet_vector_elem_assignation(Variable v1, Variable i, Variable v2) {
    check_variable_existance(&v1);

    if(!is_vector(v1)) printf_error("Ilegal operator on type '%s'", fancy_print_type(v1.type));

    if(is_variable(i)) check_variable_existance(&i);

    if(!is_int(i)) printf_error("Ilegal type '%s' in index '%s' of vector '%s'", fancy_print_type(i.type), i.var_name, v1.var_name);

    int v2_len = get_var_string_len(v2);
    char *v2_str = (char*) malloc(v2_len * sizeof(char));

    if(is_variable(i)) {
        Variable idx;
        generate_tmp(&idx);
        emet_in_list(false, "%s := %s MULI 4", idx.var_name, i.var_name);
        emet_in_list(false, "%s[%s] := %s", v1.var_name, idx.var_name, var_to_string(v2, v2_str, v2_len));

    } else {
        emet_in_list(false, "%s[%i] := %s", v1.var_name, get_val_int(i) * 4, var_to_string(v2, v2_str, v2_len));
    }
}

//m[1,1] = 10
void emet_matrix_elem_assignation(Variable v1, Variable i, Variable j, Variable v2) {
    check_variable_existance(&v1);

    if(!is_matrix(v1)) printf_error("Ilegal operator on type '%s'", fancy_print_type(v1.type));

    if(is_variable(i)) check_variable_existance(&i);
    if(is_variable(j)) check_variable_existance(&j);

    if(!is_int(i)) printf_error("Ilegal type '%s' in index '%s' of vector '%s'", fancy_print_type(i.type), i.var_name, v1.var_name);

    int v2_len = get_var_string_len(v2);
    char *v2_str = (char*) malloc(v2_len * sizeof(char));


    if(is_variable(i) && !is_variable(j)) {
        Variable i_cols;
        generate_tmp(&i_cols);
        emet_in_list(false, "%s := %s MULI %i", i_cols.var_name, i.var_name, get_matrix_cols(v1));

        Variable mla;
        generate_tmp(&mla);
        emet_in_list(false, "%s := %s ADDI %i", mla.var_name, i_cols.var_name, get_val_int(j));

        Variable idx;
        generate_tmp(&idx);
        emet_in_list(false, "%s := %s MULI 4", idx.var_name, mla.var_name);

        emet_in_list(false, "%s[%s] := %s", v1.var_name, idx.var_name, var_to_string(v2, v2_str, v2_len));
    } else if(!is_variable(i) && is_variable(j)) {
        Variable mla;
        generate_tmp(&mla);
        emet_in_list(false, "%s := %i ADDI %s", mla.var_name, get_val_int(i) * get_matrix_cols(v1), j.var_name);

        Variable idx;
        generate_tmp(&idx);
        emet_in_list(false, "%s := %s MULI 4", idx.var_name, mla.var_name);

        emet_in_list(false, "%s[%s] := %s", v1.var_name, idx.var_name, var_to_string(v2, v2_str, v2_len));
    } else if(is_variable(i) && is_variable(j)) {
        Variable i_cols;
        generate_tmp(&i_cols);
        emet_in_list(false, "%s := %s MULI %i", i_cols.var_name, i.var_name, get_matrix_cols(v1));

        Variable mla;
        generate_tmp(&mla);
        emet_in_list(false, "%s := %s ADDI %s", mla.var_name, i_cols.var_name, j.var_name);

        Variable idx;
        generate_tmp(&idx);
        emet_in_list(false, "%s := %s MULI 4", idx.var_name, mla.var_name);

        emet_in_list(false, "%s[%s] := %s", v1.var_name, idx.var_name, var_to_string(v2, v2_str, v2_len));
    } else {
        emet_in_list(false, "%s[%i] := %s", v1.var_name, (get_val_int(i) * get_matrix_cols(v1) + get_val_int(j)) * 4, var_to_string(v2, v2_str, v2_len));
    }
}

void emet_function_assignation(Variable v1, Variable v2) {
    v1.type = v2.val.Function.return_type;
    v1.is_variable = true;
    emet_in_list(false, "%s := CALL %s,%i", v1.var_name, v2.var_name, v2.val.Function.n_args);
}

void emet_assignation_vector(Variable v1, Variable v2) {
    v1.type = v2.type;
    v1.val = v2.val;
    v1.is_variable = true;

    int v2_len = get_vector_len(v2);

    int var_len = get_var_string_len(v1);
    char *var_name = (char*) malloc(var_len * sizeof(char));

    char *str = (char*) malloc(12 * sizeof(char));

    for(int i = 0; i < v2_len; i++) {
        emet_in_list(false, "%s := %s", var_to_string_idx(v1, var_name, 12, i * 4), get_string_elem(v2, i, str));
    }

    store_val(v1);

    free(var_name);
    free(str);
}

void emet_assignation_matrix(Variable v1, Variable v2) {
    v1.type = v2.type;
    v1.val = v2.val;
    v1.is_variable = true;

    int mtx_rows = get_matrix_rows(v2);
    int mtx_cols = get_matrix_cols(v2);

    int var_len = get_var_string_len(v1);
    char *var_name = (char*) malloc(var_len * sizeof(char));

    char *str = (char*) malloc(12 * sizeof(char));

    for(int i = 0; i < mtx_cols; i++) {
        for(int j = 0; j < mtx_cols; j++) {
            emet_in_list(false, "%s := %s", var_to_string_idx(v1, var_name, 12, (i * mtx_cols + j) * 4), get_string_elem(v2, (i * mtx_cols + j), str));
        }
    }

    store_val(v1);

    free(var_name);
    free(str);
}

void emet_simple(Variable v1, Variable v2) {
    int v2_len = get_var_string_len(v2);
    char *v2_str = (char*) malloc(v2_len * sizeof(char));
    emet_in_list(false, "%s := %s", v1.var_name, var_to_string(v2, v2_str, v2_len));
    
    v1.type = v2.type;
    v1.is_variable = true;
    v1.val = v2.val;
    v1.var_name_len = v2.var_name_len;

    store_val(v1);
}

void emet_assignation(Variable v1, Variable v2) {
    if(v2.is_variable && v2.var_name[0] != '$') {
        if(!val_exists_in_symtab(v2.var_name)) printf_error("Undefined variable '%s'", v2.var_name);
        get_val(v2.var_name, &v2);
    }

    if(is_matrix(v2)) emet_assignation_matrix(v1, v2);
    else if(is_vector(v2)) emet_assignation_vector(v1, v2);
    else if(is_function(v2)) emet_function_assignation(v1, v2);
    else if(is_action(v2)) printf_error("Can't assign result of action");
    else emet_simple(v1, v2);
}


//-------------Emet vector and matrix elem-------------
void emet_vector_elem(Variable v, Variable i, Variable *res) {
    check_variable_existance(&v);
    if(!is_vector(v)) printf_error("Ilegal operator on type '%s'", fancy_print_type(v.type));

    if(is_variable(i)) check_variable_existance(&i);

    if(!is_int(i)) printf_error("Ilegal type '%s' in index '%s' of vector '%s'", fancy_print_type(i.type), i.var_name, v.var_name);

    res->type = is_int_vector(v) ? Int64 : Float64;

    if(is_variable(i)) {
        Variable idx;
        generate_tmp(&idx);
        emet_in_list(false, "%s := %s MULI 4", idx.var_name, i.var_name);
        generate_tmp(res);
        emet_in_list(false, "%s := %s[%s]", res->var_name, v.var_name, idx.var_name);

    } else {
        generate_tmp(res);
        emet_in_list(false, "%s := %s[%i]", res->var_name, v.var_name, get_val_int(i) * 4);
    }
}

void emet_matrix_elem(Variable m, Variable i, Variable j, Variable *res) {
    check_variable_existance(&m);
    if(is_variable(i)) check_variable_existance(&i);
    if(is_variable(j)) check_variable_existance(&j);

    if(!is_int(i)) printf_error("Ilegal type '%s' in index '%s' of matrix '%s'", fancy_print_type(i.type), i.var_name, m.var_name);
    if(!is_int(j)) printf_error("Ilegal type '%s' in index '%s' of matrix '%s'", fancy_print_type(j.type), j.var_name, m.var_name);

    res->type = is_int_matrix(m) ? Int64 : Float64;

    if(is_variable(i) && !is_variable(j)) {
        Variable i_cols;
        generate_tmp(&i_cols);
        emet_in_list(false, "%s := %s MULI %i", i_cols.var_name, i.var_name, get_matrix_cols(m));

        Variable mla;
        generate_tmp(&mla);
        emet_in_list(false, "%s := %s ADDI %i", mla.var_name, i_cols.var_name, get_val_int(j));

        Variable idx;
        generate_tmp(&idx);
        emet_in_list(false, "%s := %s MULI 4", idx.var_name, mla.var_name);

        generate_tmp(res);
        emet_in_list(false, "%s := %s[%s]", res->var_name, m.var_name, idx.var_name);
    } else if(!is_variable(i) && is_variable(j)) {
        Variable mla;
        generate_tmp(&mla);
        emet_in_list(false, "%s := %i ADDI %s", mla.var_name, get_val_int(i) * get_matrix_cols(m), j.var_name);

        Variable idx;
        generate_tmp(&idx);
        emet_in_list(false, "%s := %s MULI 4", idx.var_name, mla.var_name);

        generate_tmp(res);
        emet_in_list(false, "%s := %s[%s]", res->var_name, m.var_name, idx.var_name);

    } else if(is_variable(i) && is_variable(j)) {
        Variable i_cols;
        generate_tmp(&i_cols);
        emet_in_list(false, "%s := %s MULI %i", i_cols.var_name, i.var_name, get_matrix_cols(m));

        Variable mla;
        generate_tmp(&mla);
        emet_in_list(false, "%s := %s ADDI %s", mla.var_name, i_cols.var_name, j.var_name);

        Variable idx;
        generate_tmp(&idx);
        emet_in_list(false, "%s := %s MULI 4", idx.var_name, mla.var_name);

        generate_tmp(res);
        emet_in_list(false, "%s := %s[%s]", res->var_name, m.var_name, idx.var_name);
    } else {
        generate_tmp(res);
        emet_in_list(false, "%s := %s[%i]", res->var_name, m.var_name, (get_val_int(i) * get_matrix_cols(m) + get_val_int(j)) * 4);
    }

}


//-------------Emet prints------------
void emet_print_simple(Variable v) {
    if(!is_int_or_float(v)) printf_error("Ilegal type '%s' in emet print simple", fancy_print_type(v.type));

    if(!is_variable(v)) v.var_name = (char*) malloc(get_var_string_len(v) * sizeof(char));

    emet_in_list(false, "PARAM %s", var_to_string(v, v.var_name, get_var_string_len(v)));

    emet_in_list(false, "CALL PUT%c,1", is_int(v) ? 'I' : 'F');
}

void emet_print_bool(Variable v) {
    if(!is_bool(v)) printf_error("Ilegal type '%s' in emet print simple", fancy_print_type(v.type));

    if(!is_variable(v)) v.var_name = (char*) malloc(get_var_string_len(v) * sizeof(char));

    emet_in_list(false, "PARAM %s", var_to_string(v, v.var_name, get_var_string_len(v)));

    emet_in_list(false, "CALL PUTB,1");
}

void emet_print_vector(Variable v) {
    if(!is_vector(v)) printf_error("Ilegal type '%s' in emet vector", fancy_print_type(v.type));

    if(!is_variable(v)) {
        generate_tmp(&v);
        emet_assignation_vector(v, v);
    }

    emet_in_list(false, "PARAM %s", v.var_name);
    emet_in_list(false, "PARAM %i", get_vector_len(v));

    emet_in_list(false, "CALL PUTV%c,2", is_int_vector(v) ? 'I' : 'F');
}

void emet_print_matrix(Variable v) {
    if(!is_matrix(v)) printf_error("Ilegal type '%s' in emet matrix", fancy_print_type(v.type));

    if(!is_variable(v)) {
        generate_tmp(&v);
        emet_assignation_matrix(v, v);
    }

    emet_in_list(false, "PARAM %s", v.var_name);
    emet_in_list(false, "PARAM %i", get_matrix_rows(v));
    emet_in_list(false, "PARAM %i", get_matrix_cols(v));

    emet_in_list(false, "CALL PUTM%c,3", is_int_matrix(v) ? 'I' : 'F');
}

void emet_print_function(Variable v) {
    Variable tmp;
    generate_tmp(&tmp);
    tmp.type = v.val.Function.return_type;
    emet_in_list(false, "%s := CALL %s,%i", tmp.var_name, v.var_name, v.val.Function.n_args);
    emet_print_var(tmp);
}

void emet_print_var(Variable v) {
    check_variable_existance(&v);

    switch (v.type) {
        case Int64:
        case Float64:
            emet_print_simple(v);
            break;
        case Bool:
            emet_print_bool(v);
            break;
        case Int64Matrix:
        case Float64Matrix:
            emet_print_matrix(v);
            break;
        case Int64Vector:
        case Float64Vector:
            emet_print_vector(v);
            break;
        case Function:
            emet_print_function(v);
            break;
    }

}


//-------------Emet functions-------------
void emet_end_main() {

    InstructionList *it = generated_instrucitons;
    while(it != NULL) {
        if(it->curr.is_goto && !it->curr.completed_goto) {
            it->curr.goto_line = line_number;
            if(DEBUG) printf("[HALT] Completed instruction '%s' with goto to line '%i'\n", it->curr.instr_str, it->curr.goto_line);
            it->curr.completed_goto = true;
        }

        it = it->next;
    }

    emet_in_list(false, "HALT");
    emet_in_list(false, "END");
}

void emet_start_foo(char *foo_name) {
    // reset_numbers();
    emet_in_list(false, "START %s", foo_name);
}

void emet_end_foo() {
    emet_in_list(false, "END\n");
}

void emet_action_return() {
    emet_in_list(false, "RETURN");
}

void emet_return(Variable v) {
    if(!is_variable(v)) v.var_name = (char*) malloc(get_var_string_len(v) * sizeof(char));
    check_variable_existance(&v);
    Type ret_type;
    get_return_type(&ret_type);

    if(ret_type != v.type) printf_error("Ilegal type in return, type is '%s' and should be '%s'", fancy_print_type(v.type), fancy_print_type(ret_type));

    if(!is_variable(v) && (is_matrix(v) || is_vector(v))) {
        generate_tmp(&v);
        if(is_matrix(v)) emet_assignation_matrix(v, v);
        if(is_vector(v)) emet_assignation_vector(v, v);
    }

    emet_in_list(false, "RETURN %s", var_to_string(v, v.var_name, get_var_string_len(v)));
}

void function_call_emet(char *foo_name, CallArgList *args, Variable *foo) {
    if(!val_exists_in_symtab(foo_name)) printf_error("Function '%s' is not declared", foo_name);

    get_val(foo_name, foo);

    if(!is_function_or_action(*foo)) printf_error("'%s' is not a funtion or action, is a '%s'", foo_name, fancy_print_type(foo->type));

    int n_args = is_function(*foo) ? foo->val.Function.n_args : foo->val.Action.n_args;
    int n_call_args = args == NULL ? 0 : args->n_args;

    if(n_args != n_call_args) printf_error("Function '%s' has '%i' args but call has '%i'", foo_name, n_args, n_call_args);

    int i = 0;
    Arg *foo_args = is_function(*foo) ? foo->val.Function.args : foo->val.Action.args;

    while(args != NULL) {
        Type definition_type = foo_args[i++].type;
        Type call_type = args->arg.type;
        if(call_type != definition_type) printf_error("Error, arg '%i' must be '%s' but is '%s'", i, fancy_print_type(definition_type), fancy_print_type(call_type));

        Variable v = args->arg;
        check_variable_existance(&v);

        if(!is_variable(v)) {
            generate_tmp(&v);
            v.is_variable = false;
            emet_assignation(v, v);
        }

        emet_in_list(false, "PARAM %s", v.var_name);


        if(DEBUG) printf("Call arg '%i' of type '%s'\n", i, fancy_print_type(call_type));
        args = args->next;
    }


    if(is_action(*foo)){
        emet_in_list(false, "CALL %s,%i", foo_name, n_args);
    }

}

void emet_relational(Variable v1, Variable v2, Variable *res, const char op[4]) {
    res->trues = create_int_list(line_number);

    int v1_len = get_var_string_len(v1);
    int v2_len = get_var_string_len(v2);

    char *v1_str = (char*) malloc(v1_len * sizeof(char));
    char *v2_str = (char*) malloc(v2_len * sizeof(char));

    emet_in_list(true, "IF %s %s %s GOTO", var_to_string(v1, v1_str, v1_len), op, var_to_string(v2, v2_str, v2_len));

    res->falses = create_int_list(line_number);
    emet_in_list(true, "GOTO");
}

//-------------NON literal relational ops-------------
void emet_general_bool_relational(Variable v1, Variable v2, Variable *res, const char op[4]) {
    check_variable_existance(&v1);
    check_variable_existance(&v2);

    if(!is_int_or_float(v1)) printf_error("Ilegal type '%s' in '%s' op 1", fancy_print_type(v1.type), op);
    if(!is_int_or_float(v2)) printf_error("Ilegal type '%s' in '%s' op 2", fancy_print_type(v2.type), op);

    if(is_float(v1) || is_float(v2)) res->type = Float64;
    else res->type = Int64;

    if(is_float(*res)) {
        to_float(v1, &v1);
        to_float(v2, &v2);
    }

    int v1_len = get_var_string_len(v1);
    int v2_len = get_var_string_len(v2);

    char *v1_str = (char*) malloc(v1_len * sizeof(char));
    char *v2_str = (char*) malloc(v2_len * sizeof(char));

    res->trues = create_int_list(line_number);
    emet_in_list(true, "IF %s %s%c %s GOTO", var_to_string(v1, v1_str, v1_len), op, is_int(*res) ? 'I' : 'F', var_to_string(v2, v2_str, v2_len));

    res->falses = create_int_list(line_number);
    emet_in_list(true, "GOTO");

    res->type = Bool;
}

void emet_bool_higher_than(Variable v1, Variable v2, Variable *res) {
    if(DEBUG) printf("Emet bool higher than '%s'>'%s'\n", v1.var_name, v2.var_name);
    emet_general_bool_relational(v1, v2, res, "GT");
}

void emet_bool_lower_than(Variable v1, Variable v2, Variable *res) {
    if(DEBUG) printf("Emet bool lower than '%s'<'%s'\n", v1.var_name, v2.var_name);
    emet_general_bool_relational(v1, v2, res, "LT");
}

void emet_bool_higher_equal(Variable v1, Variable v2, Variable *res) {
    if(DEBUG) printf("Emet bool higher equal '%s'>='%s'\n", v1.var_name, v2.var_name);
    emet_general_bool_relational(v1, v2, res, "GE");

}

void emet_bool_lower_equal(Variable v1, Variable v2, Variable *res) {
    if(DEBUG) printf("Emet bool lower equal '%s'<='%s'\n", v1.var_name, v2.var_name);
    emet_general_bool_relational(v1, v2, res, "LE");
}

//-------------NON literal eq ops-------------
void emet_general_equals(Variable v1, Variable v2, Variable *res, const char op[4]) {
    check_variable_existance(&v1);
    check_variable_existance(&v2);

    if(!is_int_or_float(v1)) printf_error("Ilegal type '%s' in '%s' op 1", fancy_print_type(v1.type), op);
    if(!is_int_or_float(v2)) printf_error("Ilegal type '%s' in '%s' op 2", fancy_print_type(v2.type), op);

    if(is_float(v1) || is_float(v2)) res->type = Float64;
    else res->type = Int64;

    if(is_float(*res)) {
        to_float(v1, &v1);
        to_float(v2, &v2);
    }

    int v1_len = get_var_string_len(v1);
    int v2_len = get_var_string_len(v2);

    char *v1_str = (char*) malloc(v1_len * sizeof(char));
    char *v2_str = (char*) malloc(v2_len * sizeof(char));

    res->trues = create_int_list(line_number);
    emet_in_list(true, "IF %s %s %s GOTO", var_to_string(v1, v1_str, v1_len), op, var_to_string(v2, v2_str, v2_len));

    res->falses = create_int_list(line_number);
    emet_in_list(true, "GOTO");

    res->type = Bool;
}


void emet_bool_equals(Variable v1, Variable v2, Variable *res) {
    emet_general_equals(v1, v2, res, "EQ");
}

void emet_bool_diff(Variable v1, Variable v2, Variable *res) {
    emet_general_equals(v1, v2, res, "NE");
}


void emet_for_header(Variable for_id, Variable start, Variable end, Variable *res) {
    if(!is_variable(for_id)) printf_error("For id must be a variable!\n");
    store_val(for_id);
    check_variable_existance(&for_id);
    check_variable_existance(&start);
    check_variable_existance(&end);

    if(!is_int(start)) printf_error("Ilegal type '%s' in for start", fancy_print_type(start.type));
    if(!is_int(end)) printf_error("Ilegal type '%s' in for end", fancy_print_type(end.type));

    int start_len = get_var_string_len(start);
    int end_len = get_var_string_len(end);

    char *start_str = (char*) malloc(start_len * sizeof(char));
    char *end_str = (char*) malloc(end_len * sizeof(char));
    emet_in_list(false, "%s := %s", for_id.var_name, var_to_string(start, start_str, start_len));
    res->for_condition_line = line_number;
    emet_in_list(false, "IF %s LEI %s GOTO %i", for_id.var_name, var_to_string(end, end_str, end_len), line_number + 2);

    res->nexts = create_int_list(line_number);
    emet_in_list(true, "GOTO");
}

void emet_simple_for_header(Variable for_id, Variable start, Variable end, Variable *res) {
    emet_for_header(for_id, start, end, res);

    res->for_increment = (char*) malloc(2 * sizeof(char));
    res->for_increment = "1";
}

void emet_increment_for_header(Variable for_id, Variable start, Variable increment, Variable end, Variable *res) {
    emet_for_header(for_id, start, end, res);

    int increment_len = get_var_string_len(increment);
    res->for_increment = (char*) malloc(increment_len * sizeof(char));
    res->for_increment = var_to_string(increment, res->for_increment, increment_len);
}
