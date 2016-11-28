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
CINT = Pointing

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mraw -I../mreport -I../mmc \
           -I../mastro -I../mgeom -I../mfileio -I../mhbase \
	   -I../mcore

SRCFILES = MPointing.cc \
	   MPointingDev.cc \
	   MPointingDevCalc.cc \
	   MPointingPos.cc \
	   MPointingPosCalc.cc \
	   MPointingPosInterpolate.cc \
           MHPointing.cc \
	   MHSrcPosCam.cc \
           MSrcPosCam.cc \
           MSrcPosCalc.cc \
           MSrcPosRndm.cc \
           MSrcPosCorrect.cc \
           MSrcPosFromModel.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
