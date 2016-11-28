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
CINT  = Sim

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mmc -I../mgeom -I../mgui -I../mcorsika \
           -I../mpointing -I../msimreflector -I../mhbase -I../mraw

SRCFILES = MPhotonData.cc \
	   MPhotonEvent.cc \
	   MHPhotonEvent.cc \
	   MSimMMCS.cc \
	   MSimAtmosphere.cc \
	   MSimAbsorption.cc \
	   MSimPointingPos.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
