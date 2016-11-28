##################################################################
#
#   makefile
# 
#   for the MARS software
#
##################################################################
include ../Makefile.conf.general
include ../Makefile.conf.$(OSTYPE)

#------------------------------------------------------------------------------

#
# Handling name of the Root Dictionary Files
#
CINT  = SimReflector

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mcorsika -I../msim -I../mpointing \
	   -I../mmc -I../mgui -I../mgeom

SRCFILES = MSimReflector.cc \
           MSimRays.cc \
	   MReflector.cc \
	   MMirror.cc \
	   MMirrorSquare.cc \
	   MMirrorHex.cc \
	   MMirrorDisk.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
