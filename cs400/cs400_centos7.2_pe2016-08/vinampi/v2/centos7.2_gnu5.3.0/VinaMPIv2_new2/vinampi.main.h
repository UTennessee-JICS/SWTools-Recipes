#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include "mpi.h"
#include "/software/dev_tools/swtree/cs400_centos7.2_pe2016-08/openmpi/1.10.3/centos7.2_gnu5.3.0_romio/include/mpi.h"

#define MAX_LINE_LEN_L 25
#define MAX_LINE_LEN_R 100
#define MAX_NUM_PARAMS 16
#define MAX_PARAM_LEN 20
#define MAX_DIR_PATH_LEN 50

void open_file(FILE **p2fp, char *name);

void parse_ligand(FILE **p2fp, int ligs, char ligands[][MAX_LINE_LEN_L], int dof[]);

int parse_receptor(FILE **p2fp, int recs, char receptors[][MAX_LINE_LEN_R], char pars[][MAX_PARAM_LEN]);

void strip_newline( char *str, int size );
