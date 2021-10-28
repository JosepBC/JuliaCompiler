#ifndef TYPES_HEADER
#define TYPES_HEADER
typedef struct Variable_t {
    char *var_name;
    enum type_t{Int64, Float64, String, Bool, Int64Vector, Float64Vector, Int64Matrix, Float64Matrix} type;
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
            int **m;
            int n_filas;
            int n_cols;
        } Int64Matrix;
        struct Float64Matrix {
            float **m;
            int n_filas;
            int n_cols;
        } Float64Matrix;
    } val;
} Variable;

extern void store_val(Variable var, int debug);
extern void show_val(char *key, int debug);
extern void fill_vector(char *in_str, Variable *var);
extern void crop_first_last_elem(char **str);
extern void print_vector(Variable v);

#endif