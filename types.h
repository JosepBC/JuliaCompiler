#ifndef TYPES_HEADER
#define TYPES_HEADER
typedef struct Variable_t {
    char *var_name;
    enum type_t{Int64, Float64, String, Bool} type;
    union {
        char *String;
        int Int64;
        float Float64;
        unsigned char Bool;
    } val;
} Variable;

extern void store_val(Variable var, int debug);
extern void show_val(char *key, int debug);

#endif