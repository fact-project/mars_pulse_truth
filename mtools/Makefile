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
CINT  = Tools

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../mgeom -I../mdata -I../mhbase \
	   -I../mfileio -I../mfbase -I../manalysis -I../mhist

#manalysis: MChisqEval (MParameters)

SRCFILES = MChisqEval.cc \
	   MTFillMatrix.cc \
	   MHSimulatedAnnealing.cc \
	   MSimulatedAnnealing.cc \
	   MFFT.cc \
           MCubicCoeff.cc \
           MCubicSpline.cc \
	   MagicReversi.cc \
	   MagicSnake.cc \
           MagicShow.cc \
           MagicDomino.cc \
           MagicCivilization.cc \
           MineSweeper.cc \
	   MRolke.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
