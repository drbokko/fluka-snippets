#
# Makefile 
# - V.Boccone March '11
#

CXXFLAGS += -I$(ROOTSYS)/include
LDFLAGS += -L$(shell root-config --libdir) -lCore -lCint -lRIO -lNet -lHist -lGraf  -lGpad -lTree -lRint -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic

FLUTIL = ${FLUPRO}/flutil
gpp    = g++
fff    = ${FLUTIL}/fff
lfluka = ${FLUTIL}/lfluka

.SILENT:

all: compile

compile:
	${gpp} ${CXXFLAGS} ${LDFLAGS} writeTarget.cc -o writeTarget
#
	${fff} writeVoxel_fine.f 
	${lfluka} writeVoxel_fine.o -o writeVoxelF
#
	${fff} writeVoxel_intermediate.f 
	${lfluka} writeVoxel_intermediate.o -o writeVoxelI
#
	${fff} writeVoxel_coarse.f 
	${lfluka} writeVoxel_coarse.o -o writeVoxelC

clean :
	rm -rf writeTarget writeVoxelF writeVoxelI writeVoxelC *.o *.mat *.ascii *.vxl target.inp *.map

sweepall : clean
	rm -rf bunch_*
