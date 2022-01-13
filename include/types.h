#ifndef TYPES_HEADER
#define TYPES_HEADER
#define DEBUG 0
#include <stdbool.h>
#include <stdio.h>

typedef enum Type_t {Unknown, Int64, Float64, String, Bool, Int64Vector, Float64Vector, Int64Matrix, Float64Matrix, Function, Action} Type;

typedef struct Arg_t {
    char *name;
    Type type;
} Arg;

typedef struct Arg_list_t {
    Arg arg;
    struct Arg_list_t *next;
    int n_args;
} ArgList;

typedef struct Int_list_t {
    int val;
    struct Int_list_t *next;
} IntList;

typedef struct Variable_t {
    Type type;
    char *var_name;
    int var_name_len;
    bool is_variable;
    IntList *trues;
    IntList *falses;
    IntList *nexts;
    int for_condition_line;
    union {
        char *String;
        int Int64;
        float Float64;
        unsigned char Bool;
        struct Int64Vector {
            int *v;
            int n_elem;
        } Int64Vector;
        struct Float64Vector {
            float *v;
            int n_elem;
        } Float64Vector;
        struct Int64Matrix {
            int *m;
            int n_rows;
            int n_cols;
        } Int64Matrix;
        struct Float64Matrix {
            float *m;
            int n_rows;
            int n_cols;
        } Float64Matrix;
        struct Function {
            Arg *args;
            int n_args;
            Type return_type;
        } Function;
        struct Action {
            Arg *args;
            int n_args;
        } Action;
    } val;
} Variable;

typedef struct Node_col_t {
    Variable val;
    struct Node_col_t *next;
    int n_elem_col;
    Type col_type;
} NodeCol;

typedef struct Node_row_t {
    NodeCol *val;
    struct Node_row_t *next;
    int n_elem_row;
    Type row_type;
} NodeRow;

typedef struct Call_arg_list_t {
    Variable arg;
    struct Call_arg_list_t *next;
    int n_args;
} CallArgList;

extern bool is_int_or_float(Variable v1);
extern bool is_int(Variable v);
extern bool is_float(Variable v);
extern bool is_int_vector(Variable v);
extern bool is_float_vector(Variable v);
extern bool is_int_matrix(Variable v);
extern bool is_float_matrix(Variable v);
extern bool is_vector(Variable v);
extern bool is_matrix(Variable v);
extern bool is_string(Variable v);
extern bool is_bool(Variable v);
extern bool is_literal(Variable v);
extern bool is_variable(Variable v);
extern bool is_action(Variable v);
extern bool is_function(Variable v);
extern bool is_function_or_action(Variable v);

extern void error(char *str);
extern void printf_error(char *str, ...);
extern const char* fancy_print_type(Type v);

extern void store_val(Variable var);
extern void crop_first_last_elem(char **str);
extern void print_node_row(NodeRow *row);
extern void store_matrix(NodeRow *row, Variable *var);
extern void print_var(char* str, Variable var, FILE *out);

extern void print_matrix(Variable v);
extern void print_vector(Variable v);

extern int get_var_string_len(Variable v);
extern void get_val(char *key, Variable *v);
extern bool val_exists_in_symtab(char *key);
extern void check_function_existance(char *name);
extern void push_symtab();
extern void pop_symtab();
extern void store_return_type(Type t);
extern void get_return_type(Type *t);
extern void store_args_symtab(Variable v);

extern int get_vector_len(Variable v);

extern int get_matrix_rows(Variable v);
extern int get_matrix_cols(Variable v);

extern void get_vector_elem(char *vector_name, int idx, Variable *dst);
extern void get_matrix_elem(char *matrix_name, int row, int col, Variable *dst);
extern void get_id_vector_elem(char *vector_name, char *vector_idx_name, Variable *res);
extern void get_id_matrix_elem(char *matrix_name, char *row_idx_name, char *col_idx_name, Variable *dst);

extern float get_val_float(Variable var);
extern int get_val_int(Variable var);

extern void store_args(ArgList *linked_args, Arg **args, int *n_args);
extern void print_args(Variable v);
#endif