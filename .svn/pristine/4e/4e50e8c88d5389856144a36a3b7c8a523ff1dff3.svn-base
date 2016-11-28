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
CINT  = MonteCarlo

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mmc -I../mhbase -I../mhist -I../mhistmc \
	   -I../mgeom -I../manalysis -I../mtools -I../mfileio          \
           -I../mfilter -I../mdata -I../mfbase 

SRCFILES = MMcWeightEnergySpecCalc.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
