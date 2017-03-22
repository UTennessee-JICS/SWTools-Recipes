#include "linked.list.h"
#include "vinampi.main.h"
#include "vinampi.work.h"


int WORKER(int me, char receptors[][MAX_LINE_LEN_R], char ligands[][MAX_LINE_LEN_L], int dof[], char params[][MAX_PARAM_LEN], int nparams, int nr, int nl, int nw, char * r_path, char * l_path)
{
  double dt0, dt1, dt;
  if (me ==0)
  {
    //get job distribution time on one worker
    dt0 = MPI_Wtime();
  }

  //printf("Hello from worker %d\n", me);
  //array to hold tasks
  int tasks[nr*nl][2];
  int ntask;

  ntask = get_work_set(me, nl, nr, nw, tasks, dof);

  if (me == 0)
  {
    //end timer
    double dt1 = MPI_Wtime();
    double dt = dt1 - dt0;
    printf("\nTime for job distribution on worker %d: %lf\n", me, dt);
  }

  printf("The number of tasks for worker %d is %d\n", me, ntask);
  /*int g;
  for (g=0; g<ntask; ++g)
  {
    printf("%d\t%d\t%d\n", tasks[g][0], tasks[g][1], dof[tasks[g][1]]);
  }*/

  int cid;
  char call[MAX_CALL_LEN];
  call[0] = '\0';
  

  //make directories for results
  //system("mkdir -p log");
  system("mkdir -p out");

  for (cid = 0; cid < ntask; ++cid)
  {
    //printf("%d\n", cid);
    get_call(cid, receptors, ligands, params, nparams, nr, nl, r_path, l_path, tasks, call);
    //puts(call);
    system(call);
  } 
}

int get_work_set(int me, int nl, int nr, int nw, int tasks[][2], int dof[])
{
  int total;
  total=nl*nr;
  //printf("Total number of jobs = %d\n", total);

  //create linked list of worker numbers
  int i;
  for (i=0; i<nw; ++i)
  {
    add_to_list(i,true);
  }
  /*print_list();
  for (i=0; i<10; ++i)
  {
    printf("%d\n", iter->val);
    update_current();
  }*/


  //create and initialize an array to keep count of dofs
  int dof_count[nw];
  for (i=0; i<nw; ++i)
  {
    dof_count[i] = 0;
  }
  //create and initialize an array to keep count of number of tasks
  int task_count[nw];
  for (i=0; i<nw; ++i)
  {
    task_count[i] = 0;
  }

  //find average degrees of freedom
  int adof;
  float tdof = 0.0;
  for (i = 0; i < nl; ++i)
  {
    tdof = tdof + dof[i];
  }

  tdof = tdof * nr;
  adof = ceil(tdof / nw);

  if (me == 0)
  {
    printf("tdof %.0f\n", tdof);
    printf("adof %d\n", adof);
  }
  //keep count of remaining tasks and workers
  int rtasks = total;
  int rworkers = nw;

  int current_dof, worker_iter, current_tdof, temp_dof, lowest_tdof, first; 

  //assign all tasks
  int lig_i;
  for (lig_i=0; lig_i < nl; ++lig_i)
  {
    //get current dof
    current_dof = dof[lig_i];
    int rec_i;
    for (rec_i=0; rec_i < nr; ++rec_i)
    {
      //get worker iterator
      worker_iter = iter->val;
      //get current total dof
      current_tdof = dof_count[worker_iter];
      //printf("%d", current_tdof);
      //if current dof + tdof < average dof then add to this workflow
      temp_dof = current_dof + current_tdof;
      //printf("%d\t%d\n", temp_dof, adof);
      //if (rtasks > rworkers)
      //{
        //find a worker with room / if whole list does not pass give to first
        int ilist = rworkers;
	first = 1;
        while (temp_dof > adof && ilist != 0)
        {
          //check for lowest tdof
          if (first == 1)
          {
            lowest_tdof = current_tdof;
            first = 0;
          }
          update_current();
          //get worker iterator
          worker_iter = iter->val;
          //get current total dof
          current_tdof = dof_count[worker_iter];
          //printf("%d", current_tdof);
          temp_dof = current_dof + current_tdof;
          ilist = ilist - 1;
	  //check for lowest tdof
	  if (current_tdof < lowest_tdof)
	  {
            lowest_tdof = current_tdof;
	  }
        }
	//if all have > adof skip to lowest dof
	if(ilist == 0)
	{
          while(current_tdof != lowest_tdof)
	  {
            update_current();
            worker_iter = iter->val;
	    //get current total dof
            current_tdof = dof_count[worker_iter];
	  }
	}
        //update current total
        //printf("Current dof = %d Current total = %d")
        dof_count[worker_iter] = dof_count[worker_iter] + current_dof;
        //give task to mpi worker
        if (me == worker_iter)
        {
          tasks[task_count[worker_iter]][0] = rec_i;
          tasks[task_count[worker_iter]][1] = lig_i;
        }
        //add to task counter
        task_count[worker_iter] = task_count[worker_iter] + 1;

        /*//WTF!/
        if (me == 512 && worker_iter == 512)
        {
          int x;
          printf("dof: ");
          for(x=0; x<nl; ++x)
          {
            printf("%d ", dof[x]);
          }
          printf("\nRec: %d and Lig: %d with dof %d assigned to worker %d. New tdof is %d\n", rec_i, lig_i, current_dof, me, dof_count[me]);
        }*/
        //get next worker
        update_current();
        //update remaining tasks
        rtasks = rtasks - 1;
        //delete worker for worker queue if filled
        if (dof_count[worker_iter] >= adof)
        {
          //printf("%d\t%d\t%d\n", temp_dof, adof, rtasks);
          delete_from_list(worker_iter);
          //updating remaining workers
          rworkers = rworkers - 1;
        }
        
      //}



    }
  }

  printf("The tdof for worker %d is %d\n", me, dof_count[me]);

  return task_count[me];

  /*if(me == 0)
  {
    printf("Min tasks per worker: %d\nNumber of workers with min tasks: %d\n", min_tasks, nmin);
  }*/
 
}

char * get_call(int current_id, char receptors[][MAX_LINE_LEN_R], char ligands[][MAX_LINE_LEN_L], char params[][MAX_PARAM_LEN], int nparams, int nr, int nl, char * r_path, char * l_path, int tasks[][2], char* call_string)
{
  int receptor_id;
  int ligand_id;
  //int total;
  //total = nr * nl;
  receptor_id = tasks[current_id][0];
  ligand_id = tasks[current_id][1];
  //printf("The receptor id is %d and the ligand id is %d\n", receptor_id, ligand_id);

  char *receptor = &receptors[receptor_id][0];
  char *ligand = &ligands[ligand_id][0];

  //puts(receptor);
  //puts(ligand);
  //puts(receptor_path);
  //puts(l_path);

  //parse line in receptor file (parameter values)
  char r_list[nparams][MAX_PVALUE_LEN];

  int r_len;
  r_len=strlen(receptor);
  //printf("r_len = %d\n", r_len);

  int i;
  //parameter number, final value will be number of parameter values in a receptor input line
  int pnum = 0;
  //index into parameter name
  int pindex = 0;
  for(i=0; i<r_len; ++i)
  {
    if (receptor[i] != ' ')
    {
      r_list[pnum][pindex] = receptor[i];
      pindex = pindex + 1;
    }
    else
    {
      r_list[pnum][pindex] = '\0';
      pnum = pnum + 1;
      pindex = 0;
    }
    //terminate last string
    r_list[pnum][pindex] = '\0';
  }

  pnum = pnum + 1;

  /*for(i=0; i<pnum; ++i)
less  {
    puts(r_list[i]);
  }*/ 

  char tcall[MAX_CALL_LEN];
  tcall[0]='\0';

  //place holder for receptor name
  int rname;
char *vina_string = "/software/dev_tools/swtree/cs400_centos7.2_pe2016-08/vinampi/v2/centos7.2_gnu5.3.0/autodock_vina_1_1_2_linux_x86/bin/vina "; 

//  char *vina_string = "/lustre/scratch/proj/bcmb422/kraken_project/autodock_vina_1_1_2_rev/autodock_vina_1_1_2/build/linux/release/vina ";
  //puts(params[0]);
  strcat(tcall, vina_string);
  //puts(tcall);
  for (i=0; i<nparams; ++i)
  {
    strcat(tcall, "--");
    strcat(tcall, params[i]);
    strcat(tcall, " ");
    if(i<pnum)
    {
      if (strcmp(params[i], "receptor") == 0)
      {
        strcat(tcall, r_path);
        strcat(tcall, "/");
        puts(r_list[i]);
        strcat(tcall, r_list[i]);
        strcat(tcall, ".pdbqt");
        rname = i;
      }
      else
      {
        strcat(tcall, r_list[i]);
      }
      strcat(tcall, " ");
    }
  }
  strcat(tcall, "--ligand ");
  strcat(tcall, l_path);
  strcat(tcall, "/");
  strcat(tcall, ligand);
  strcat(tcall, ".pdbqt --out out/");
  strcat(tcall, r_list[rname]);
  strcat(tcall, "_");
  strcat(tcall, ligand);
  strcat(tcall, ".pdbqt");
  /*strcat(tcall, ".pdbqt --log log/");
  strcat(tcall, r_list[rname]);
  strcat(tcall, "_");
  strcat(tcall, ligand);
  strcat(tcall, ".log");*/
  //puts(tcall);

  

  strcpy(call_string, tcall);

  return call_string;

}
