##################################################################
#
#   makefile
# 
#   for the MARS software
#
##################################################################
include ../Makefile.conf.$(OSTYPE)
include ../Makefile.conf.general

#------------------------------------------------------------------------------

#
# Handling name of the Root Dictionary Files
#
CINT  = HistMc

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mhist -I../mmc -I../manalysis \
    	   -I../mimage -I../mgeom -I../mgui

SRCFILES = MHMcEnergy.cc \
           MHMcEfficiency.cc \
           MHMcEfficiencyImpact.cc \
           MHMcEfficiencyEnergy.cc \
           MHMcEnergyImpact.cc \
	   MHMcRate.cc \
	   MHMcCollectionArea.cc \
           MHMcIntRate.cc \
           MHMcDifRate.cc \
           MHMcEnergyMigration.cc \
	   MHMcCT1CollectionArea.cc \
	   MHMcTriggerLvl2.cc \
	   MHMcUnfoldCoeff.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
