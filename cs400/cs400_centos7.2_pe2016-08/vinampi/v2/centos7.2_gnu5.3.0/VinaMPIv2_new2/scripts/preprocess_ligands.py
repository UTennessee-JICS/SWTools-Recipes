###############################
#
#  Author Sally R. Ellingson
#  9/25/12
#  This scripts ranks the file ligand.txt by tdof
#  Used when preprocessing has already been done
#
##################################

f1=open('ligands.txt', 'r')

tdof=[]

for line in f1:
  ligand_name = line.strip()
  file_name = 'pdbqt/ligands/'+ligand_name+'.pdbqt'
  ftemp=open(file_name, 'r')
  dof_line=ftemp.readline()
  ftemp.close()
  dof=dof_line.split()[1]
  tdof.append([int(dof),ligand_name])

f1.close()  
tdof.sort()
tdof.reverse()

f2=open('ligands_sort.txt', 'w')
for lig in tdof:
  f2.write(lig[1]+' '+str(lig[0])+'\n')

