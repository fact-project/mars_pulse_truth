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
CINT  = RanForest

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mdata -I../manalysis -I../mmc -I../mfileio
#               MParContainer MH     MDataArray MHadronness    MMcEvt

SRCFILES = MRanTree.cc \
           MRanForest.cc \
           MRanForestGrow.cc \
           MRanForestCalc.cc \
	   MHRanForest.cc \
	   MHRanForestGini.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
