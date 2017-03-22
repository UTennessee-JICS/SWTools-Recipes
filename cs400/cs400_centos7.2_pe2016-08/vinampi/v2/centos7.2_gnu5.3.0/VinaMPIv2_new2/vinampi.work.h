#include<stdbool.h>

#define MAX_CALL_LEN 500
#define MAX_PVALUE_LEN 15

int WORKER(int me, char receptors[][MAX_LINE_LEN_R], char ligands[][MAX_LINE_LEN_L], int dof[], char params[][MAX_PARAM_LEN], int nparams, int nr, int nl, int nw, char * r_path, char * l_path);

int get_work_set(int me, int nr, int nl, int nw, int tasks[][2], int dof[]);

char * get_call(int current_id, char receptors[][MAX_LINE_LEN_R], char ligands[][MAX_LINE_LEN_L], char params[][MAX_PARAM_LEN], int nparams, int nr, int nl, char * r_path, char * l_path, int tasks[][2], char* call_string);


