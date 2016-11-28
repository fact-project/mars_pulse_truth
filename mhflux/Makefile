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
CINT  = Flux

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mraw -I../manalysis      \
	   -I../mgui -I../mgeom -I../mdata -I../mfilter -I../mimage \
           -I../mmain -I../mmc -I../mreflector -I../mpointing       \
           -I../mastro -I../mpedestal -I../msignal -I../mbadpixels

SRCFILES = MAlphaFitter.cc \
	   MHAlpha.cc \
           MHThetaSq.cc \
           MHThetaSqN.cc \
           MHPhi.cc \
           MHDisp.cc \
           MHFalseSource.cc \
	   MHEnergyEst.cc \
	   MHEffectiveOnTime.cc \
           MHCollectionArea.cc \
           MHThreshold.cc \
           MMcSpectrumWeight.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
