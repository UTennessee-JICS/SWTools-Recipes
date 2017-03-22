#---------------------- Makefile for parallel -------------------------#
##usage:  make compile ; make run  or  make pbs
#----------------------------------------------------------------------#
## >>>>>>set LL to one of: omp ogcc oicc  
LL = omp
 
#========================== set parallel compiler =====================#
#-------------------- parallel: for openMPI on local: 
COMP   = mpicc
FLAGs  = -lm
CODEbase = VinaMPI
OUTPUT   = o.out
#-------------------- set code components: 
  CODE_c = vinampi.main.c vinampi.work.c linked.list.c
  CODE_o = vinampi.main.o vinampi.work.o linked.list.o
  CODE   = $(CODEbase)
#======================= create executable: make compile ============# 
.c.o:
	$(COMP) $(FLAGs) -c $?
 
compile: $(CODE_o)
	$(COMP) $(CODE_o) -o $(CODE) $(FLAGs)
	@echo " >>> compiled on `hostname -s` with  $(COMP) <<<"
	rm -f *.o
#======================= execute: make run | make pbs ==========# 
run:
	mpirun -np 12 $(CODE) ~/work/VinaMPI/test/receptors.txt ~/work/VinaMPI/test/ligands_sort.txt 5 5 ~/work/VinaMPI/test/receptors ~/work/VinaMPI/test/ligands > $(OUTPUT)

clean:
	rm -f  o.*  DONE  watch
#----------------------------------------------------------------------#
