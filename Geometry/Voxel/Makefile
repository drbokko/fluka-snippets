# Makefile 

FLUTIL = ${FLUPRO}/flutil
fff    = ${FLUTIL}/fff
lfluka = ${FLUTIL}/lfluka

.SILENT:

all: compile

compile:
	${fff} writeVoxelFile.f 
	${lfluka} writeVoxelFile.o -o writeVoxelFile

clean :
	rm -rf writeVoxelFile
