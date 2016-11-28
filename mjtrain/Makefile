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
CINT  = Train

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mfbase -I../mdata -I../mfileio \
           -I../manalysis -I../mfilter -I../mjobs -I../mtools \
           -I../mranforest -I../mhflux -I../mmc -I../mhist -I../mpointing \
           -I../mimage -I../mjoptim

SRCFILES = MJTrainRanForest.cc \
	   MJTrainEnergy.cc \
           MJTrainDisp.cc \
           MJTrainImpact.cc \
           MJTrainSeparation.cc \
           MJTrainCuts.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
