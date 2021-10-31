#ifndef TYPES_HEADER
#define TYPES_HEADER
#define DEBUG 0
typedef enum Type_t {Int64, Float64, String, Bool, Int64Vector, Float64Vector, Int64Matrix, Float64Matrix} Type;
typedef struct Variable_t {
    Type type;
    char *var_name;
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

extern void store_val(Variable var);
extern void show_val(char *key);
extern void fill_vector(char *in_str, Variable *var);
extern void crop_first_last_elem(char **str);
extern void print_vector(Variable v);
extern void print_matrix(Variable v);
extern void print_node_row(NodeRow *row);
extern void print_node_col(NodeCol *col);
extern void store_matrix(NodeRow *row, Variable *var);
extern void print_var(char* str, Variable var);

#endif