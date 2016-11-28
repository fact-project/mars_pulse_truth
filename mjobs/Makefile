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
CINT  = Jobs

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../mgeom -I../mdata -I../mhbase \
	   -I../mhcalib -I../mfileio -I../mfilter -I../manalysis -I../mhist \
           -I../mcalib -I../mbadpixels -I../msignal -I../mraw -I../mpedestal \
           -I../mtools -I../mimage -I../mpointing -I../mastro -I../mfbase \
           -I../mhvstime -I../mtrigger -I../mmuon -I../mmc -I../mmovie \
           -I../mextralgo -I../mhflux -I../msql -I../mreport -I../mcorsika \
	   -I../msim -I../msimreflector -I../msimcamera -I../melectronics \
	   -I../mcore

SRCFILES = MSequence.cc \
           MSequenceSQL.cc \
           MDataSet.cc \
           MJob.cc \
	   MJSimulation.cc \
           MJSimReflector.cc \
	   MJPedestal.cc \
           MJCalib.cc \
           MJCalibration.cc \
           MJCalibrateSignal.cc \
	   MJMerpp.cc \
	   MJStar.cc \
           MJCut.cc \
           MJSpectrum.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
