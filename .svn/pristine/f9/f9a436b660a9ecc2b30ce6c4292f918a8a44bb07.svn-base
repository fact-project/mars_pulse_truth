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

CINT     = Temp

#------------------------------------------------------------------------------

INCLUDES = -I. \
	   -I../mbase \
	   -I../mjobs \
	   -I../mpedestal \
	   -I../mbadpixels \
	   -I../mfileio \
           -I../mraw \
           -I../manalysis \
	   -I../mgui \
	   -I../mgeom \
	   -I../msignal \
	   -I../mcalib \
	   -I../mfilter \
	   -I../mhbase \
	   -I../mimage \
	   -I../mpointing \
	   -I../mcamera \
	   -I../mhist \
	   -I../mmc \
	   -I../mreport \
	   -I../mastro \
           -I../mstarcam

SRCFILES = \
        MFindStars.cc \
        MTelAxisFromStars.cc \
        MHTelAxisFromStars.cc \
        MSkyCamTrans.cc \
	MStarLocalCam.cc \
        MSourceDirections.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

clean:	rmcint rmobjs rmcore rmlib
mrproper:	clean rmbak
