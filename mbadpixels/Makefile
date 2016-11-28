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
CINT  = BadPixels

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../manalysis -I../mgeom -I../mgui \
           -I../mmc -I../mpedestal -I../msignal -I../mpointing \
           -I../mhbase -I../mhist -I../mraw
# MBadPixelsCalc  (manalysis): MPedPhotCam, MSigmabar
# MBadPixelsCalc  (mgeom):     MGeomCam
# MBadPixelsCam   (mgui):      MCamEvent
# MMcBadPixelsSet (mmc):       MMcRunHeader
# MBadPixelsTreat (mpedestal): MPedPhot*
# MBadPixelsTreat (msignal):   MArrivalTime

.SUFFIXES: .c .cc .cxx .h .hxx .o 

SRCFILES = MBadPixelsPix.cc \
           MBadPixelsCam.cc \
           MBadPixelsMerge.cc \
           MBadPixelsCalc.cc \
           MBadPixelsTreat.cc \
           MMcBadPixelsSet.cc \
           MHBadPixels.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

clean:	rmcint rmobjs rmcore rmlib

mrproper:	clean rmbak
