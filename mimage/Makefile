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
CINT  = Image

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mgeom -I../msignal \
	   -I../mgui -I../mmc -I../mpointing -I../mpedestal \
           -I../mhist -I../manalysis

# mhist: MHHillas (MHCamera)


SRCFILES = MImgCleanStd.cc \
           MImgCleanTime.cc \
           MImgCleanSimple.cc \
           MHillas.cc \
           MHillasSrc.cc \
           MHillasExt.cc \
           MHillasCalc.cc \
           MImagePar.cc \
	   MNewImagePar.cc \
	   MNewImagePar2.cc \
	   MConcentration.cc \
           MHHillas.cc \
           MHHillasSrc.cc \
           MHHillasExt.cc \
           MHImagePar.cc \
	   MHNewImagePar.cc \
	   MHNewImagePar2.cc \
           MHVsSize.cc \
	   MStereoPar.cc \
	   MStereoCalc.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
