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
CINT  = Hft

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../mgeom -I../msignal

#manalysis: MChisqEval (MParameters)

SRCFILES = MHexagonalFT.cc \
           MHexagonalFTCalc.cc \
   	   MHexagonFreqSpace.cc \
           MGeomCamMagicXT.cc \
           MGeomCamMagicEnhance.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
