###############################
#
#  Author Sally R. Ellingson
#  9/25/12
#  revised 11/03/12
#  This scripts ranks the file ligand.txt by tdof and # of atoms
#  Used when preprocessing has already been done
#
##################################

f1=open('ligands.txt', 'r')

tdof={}

for line in f1:
  ligand_name = line.strip()
  file_name = 'ligands/'+ligand_name+'.pdbqt'
  ftemp=open(file_name, 'r')
  dof_line=ftemp.readline()
  dof=dof_line.split()[1]
  natoms = ''
  temp_line = ftemp.readline()
  while temp_line != '':
    tsplit = temp_line.split()
    if tsplit[0] == 'ATOM':
      natoms = tsplit[1]
    temp_line = ftemp.readline()
  dof_i = int(dof)
  if dof_i in tdof:
    tdof[dof_i].append([int(natoms),ligand_name])
  else:
    tdof[dof_i] = []
    tdof[dof_i].append([int(natoms),ligand_name])
  ftemp.close()

f1.close()  

tdof_list = tdof.keys()
tdof_list.sort()
tdof_list.reverse()

f2=open('ligands_sort2.txt', 'w')
for this_dof in tdof_list:
  #print this_dof
  #print tdof[this_dof]
  tdof[this_dof].sort()
  tdof[this_dof].reverse()
  for lig in tdof[this_dof]:
    #print lig
    f2.write(lig[1]+' '+str(this_dof)+'\n')

