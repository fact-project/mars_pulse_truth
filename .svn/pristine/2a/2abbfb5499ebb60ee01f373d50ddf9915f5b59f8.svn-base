##################################################################
#
#   subdirectory makefile
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
CINT  = Analysis

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mmc -I../mraw -I../mgeom -I../mfilter \
	   -I../mdata -I../mhbase -I../mhist -I../mgui -I../mimage   \
           -I../mhistmc -I../mfileio -I../mmain -I../mcalib \
           -I../msignal -I../mpointing -I../mtools -I../mfbase       \
           -I../mbadpixels -I../mastro -I../mpedestal
# mcalib: MGeomApply (MCalibrationCam)

SRCFILES = MGeomApply.cc \
           MCameraData.cc \
           MParameterCalc.cc \
           MEnergyEstimate.cc \
           MMatrixLoop.cc \
           MMultiDimDistCalc.cc \
           MEventRate.cc \
           MEventRateCalc.cc \
           MMcCalibrationUpdate.cc \
	   MSoftwareTrigger.cc \
	   MSoftwareTriggerCalc.cc

#           MMcTriggerLvl2.cc \
#           MMcTriggerLvl2Calc.cc \
#           MCompProbCalc.cc \

############################################################

all:  $(OBJS)

include ../Makefile.rules

clean:	rmcint rmobjs rmcore rmlib

mrproper:	clean rmbak

