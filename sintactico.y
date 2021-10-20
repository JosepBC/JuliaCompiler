%define parse.error verbose
%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>

  int yylex();
  extern FILE* yyin;
  int yyerror(const char *s);
  FILE* csv;

  int filas = 0;
  int cols = 0;

  char delimiter = ',';
  int max_files = -1;

  int first_col = 1;

  char *csv_file = "out.csv";

  void set_atr(char *s);
  void print_val(char *s);
%}

%union {
    char *str;
    int i;
    float f;
    unsigned char b;
};

%token<str> ID
%token<i> INT
%token<f> FLOAT
%token<str> STRING 
%token<b> BOOL
%token EQUALS
%token ENTER

%%
prog : sentence_list ;
sentence_list : sentence_list sentence ENTER | sentence ENTER;
sentence : assignation_sentence | expression;
assignation_sentence : ID EQUALS expression;
expression : int_expression | float_expression | string_expression | bool_expression;
int_expression : INT {
    printf("Int %i\n", $1);
};
float_expression : FLOAT {
    printf("float %f\n", $1);
};
string_expression : STRING {
    printf("string %s\n", $1);
};
bool_expression : BOOL {
    printf("bool %i\n", $1);
};
%%
int yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n",  s);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    //yyin = fopen(argv[1], "r");
    yyparse();
    return 1;
}