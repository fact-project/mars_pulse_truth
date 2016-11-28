##################################################################
#
#   subdirectory makefile
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
CINT  = AnalysisCT1

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mdata -I../mfbase -I../mgeom     \
           -I../mfileio -I../mfilter -I../manalysis -I../mpedestal -I../mmc \
           -I../mhist -I../mpointing -I../mimage -I../mgui -I../mbadpixels  \
           -I../mraw

SRCFILES = MCT1FindSupercuts.cc \
           MCT1PointingCorrCalc.cc \
           MCT1Supercuts.cc \
           MCT1SupercutsCalc.cc \
           MCT1ReadPreProc.cc \
           MCT1ReadAscii.cc

# Doesn't compile: Please replace MBlindPixels by MBadPixel*
#           MCT1PadONOFF.cc \
#           MCT1PadSchweizer.cc \

############################################################

all:  $(OBJS)

include ../Makefile.rules

clean:	rmcint rmobjs rmcore rmlib

mrproper:	clean rmbak
