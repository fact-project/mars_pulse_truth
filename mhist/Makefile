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
CINT  = Hist

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mraw -I../manalysis      \
	   -I../mgui -I../mgeom -I../mdata -I../mfilter -I../mimage \
           -I../mpointing -I../mmc  -I../mastro -I../msignal        \
           -I../mbadpixels -I../mcalib -I../mreport -I../mtrigger

SRCFILES = MHEvent.cc \
	   MHCamEvent.cc \
           MHCamEventRot.cc \
           MHStarMap.cc \
           MHHadronness.cc \
	   MHTriggerLvl0.cc \
           MHCamera.cc \
           MHRate.cc \
           MHWeather.cc

#           MHFadcPix.cc \
#           MHFadcCam.cc \
#           MHCompProb.cc \

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
