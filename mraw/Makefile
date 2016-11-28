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
CINT  = Raw

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../mgeom -I../mfileio -I../mcalib -I../mcore
# mgui (MCamEvent), mgeom(MGeomCam)

SRCFILES = MRawRunHeader.cc \
	   MRawEvtHeader.cc \
	   MRawEvtData.cc \
	   MRawEvtPixelIter.cc \
	   MRawCrateArray.cc \
	   MRawCrateData.cc \
	   MRawBoardsFACT.cc \
           MRawRead.cc \
           MRawFileRead.cc \
           MRawFitsRead.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
