#include "vinampi.main.h"

int nWRs;             //= number of workers   = nPROC-1
int myID;             //= rank of a worker (=1,2,...,nPROC)


int main(int argc, char *argv[])
{

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nWRs);     //size of communicator
  MPI_Comm_rank(MPI_COMM_WORLD, &myID);      //rank of each member of communicator

  //start timer
  double tt0 = MPI_Wtime();

  if (argc != 7)
  {
    printf("Must specify receptor and ligand input files and number of receptors and ligands and location of receptor and ligand pdbqt files\nEXAMPLE: mpirun -np N VinaMPI receptors.txt ligands.txt N N pdbqt/receptors pdbqt/ligands\n");
    MPI_Finalize();
    exit(1);
  }

  int nrecs;
  int nligs;

  nrecs=atoi(argv[3]);
  nligs=atoi(argv[4]);

  char *receptor_path;
  char *ligand_path;
  receptor_path=argv[5];
  ligand_path=argv[6];
  //puts(receptor_path);
  //puts(ligand_path);

  //printf("%d\t%d", nrecs, nligs);

  FILE *receptor_file=NULL;
  FILE *ligand_file=NULL;

  open_file(&receptor_file, argv[1]);

  char receptors[nrecs][MAX_LINE_LEN_R];
  char params[MAX_NUM_PARAMS][MAX_PARAM_LEN];
  int num_params;
  num_params = parse_receptor(&receptor_file, nrecs, &receptors[0], params);

  /*int i;
  for (i=0; i<num_params; ++i)
  {
    puts(params[i]);
  }*/

  /*int i;
  for (i=0; i<nrecs; ++i)
  {
    puts(receptors[i]);
  }*/

  fclose(receptor_file);

  open_file(&ligand_file, argv[2]);

  char ligands[nligs][MAX_LINE_LEN_L];
  int dof[nligs];
  parse_ligand(&ligand_file, nligs, &ligands[0], dof);

  /*if (myID == 512)
  {
    printf("dof[x] before passed to worker on 512\n");
  }
  if(myID == 512)
  {
    int x;
    for (x=0; x<nligs; ++x)
    printf("%d ", dof[x]);
  }*/

  /*int i;
  for (i=0; i<nligs; ++i)
  {
    puts(ligands[i]);
    printf("%d\n", dof[i]);
  } */ 

  fclose(ligand_file);

  WORKER(myID, receptors, ligands, dof, params, num_params, nrecs, nligs, nWRs, receptor_path, ligand_path);    //call worker
  //printf("Bye from worker %d\n", myID);

  //end timer
  double tt1 = MPI_Wtime();
  double tt = tt1 - tt0;
  printf("\nTime for worker %d: %lf\n", myID, tt);



  MPI_Finalize();

}

//opens necessary file and exits program if error
void open_file(FILE **p2fp, char *name)
{
  *p2fp = fopen(name, "r");
  if(*p2fp == 0)
  {
    printf("%s did not open correctly: file may not exist\n", name);
    MPI_Finalize();
    exit(1);
  }
}


//fetch list of ligands
void parse_ligand(FILE **p2fp, int ligs, char ligands[][MAX_LINE_LEN_L], int dof[])
{
  int i;
  //size_t j;
  char lig_file_line[MAX_LINE_LEN_L];
  //printf("Number of ligands: %d\n", ligs);
  for (i=0; i<ligs; ++i)
  {
    //get line in ligand file
    fgets(lig_file_line, MAX_LINE_LEN_L, *p2fp);
    strip_newline(lig_file_line, strlen(lig_file_line));

    /*if(myID == 512)
    {
      printf("ligs: %d\ni: %d", ligs, i);
      puts(lig_file_line);
    }*/
    ligs = ligs; 

    //length of line
    int len;
    len=strlen(lig_file_line);
    //isLig = 1 for ligand name and 0 for dof
    int isLig=1;
    //index into string
    int sindex = 0;

    //temp variable for ligands name
    char *temp_lig = &ligands[i][0];
    //temp variable for string of dof
    char temp_dof_s[2];

    int j;
    for (j=0; j<len; ++j)
    {
      //if character is not a space
      if (lig_file_line[j] != ' ')
      {
        if (isLig == 1)
        {
          
          //get ligand name
          temp_lig[sindex]=lig_file_line[j];
          sindex = sindex + 1;
        }
        if (isLig == 0)
        {
          //get dof string
          temp_dof_s[sindex]=lig_file_line[j];
          sindex = sindex + 1;
        }
      }
      //if character is a space
      if (lig_file_line[j] == ' ')
      {
        isLig = 0;
        temp_lig[sindex] = '\0';
        sindex = 0;
        //puts(temp_lig);
      }
    }
    //terminate dof string
    temp_dof_s[sindex] = '\0';

    //printf("%s", temp_lig);
    /*if(myID == 512)
    {
      printf("%s\n", temp_dof_s);
    }*/
    //put variables in arrays
    dof[i] = atoi(temp_dof_s);
    //printf("%s\t%d\n", ligands[i], dof[i]);

  }
}

//fetch list of receptors and parameter list
int parse_receptor(FILE **p2fp, int recs, char receptors[][MAX_LINE_LEN_R], char pars[][MAX_PARAM_LEN])
{
  //first line contains parameter names and default values if any on the end parameters
  char par_titles_string[MAX_LINE_LEN_R];
  fgets(par_titles_string, MAX_LINE_LEN_R, *p2fp);
  strip_newline(par_titles_string, strlen(par_titles_string));
  //printf("%s", par_titles_string);

  int len;
  len=strlen(par_titles_string);
  //printf("%d/n", len);
  int j;
  //parameter number
  int pnum = 0;
  //index into parameter name
  int pindex = 0;
  for (j=0; j<len; ++j)
  {
    //if character is not a space
    if (par_titles_string[j] != ' ')
    {
      //replace = sign denoting default value given with a space
      if (par_titles_string[j] == '=')
      {
        pars[pnum][pindex] = ' ';
      }
      else
      {
        pars[pnum][pindex] = par_titles_string[j];
      }

      pindex = pindex + 1;
    }
    else
    {
      pars[pnum][pindex] = '\0';
      pnum = pnum + 1;
      pindex = 0;
    }
    //terminate last string
    pars[pnum][pindex] = '\0';
  }


  //remaining lines contain the parameter values
  int i;
  for (i=0; i<recs; ++i)
  {
    char *receptor = &receptors[i][0];
    fgets(receptor, MAX_LINE_LEN_R, *p2fp);
    strip_newline(receptor, strlen(receptor));
  }
  /*for(i=0; i<recs; ++i)
  {
    puts(receptors[i]);
  }*/

  //return the number of parameter names
  return pnum + 1;

}

//remove newline charatcer and replace with null character to terminate string
//exit program if no newline found since this means the maximum string length was exceeded
//MAX_LINE_LEN_R and MAX_LINE_LEN_L can be changed to larger values if needed
void strip_newline( char *str, int size )
{
  int i;

  // remove null terminator
  for (  i = 0; i < size; ++i )
  {
    if ( str[i] == '\n' )
    {
      str[i] = '\0';

      // exit the function by returning
      return;   
    }
  }

  // no newline which means line length was exceeded
  printf("Exceeded MAX_LINE_LEN_L (%d) or MAX_LINE_LEN_R (%d)\n", MAX_LINE_LEN_L, MAX_LINE_LEN_R);
  MPI_Finalize();
  exit(1);
}
