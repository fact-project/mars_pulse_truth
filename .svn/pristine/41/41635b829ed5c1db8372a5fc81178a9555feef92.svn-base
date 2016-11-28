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
CINT  = Signal

#
#  connect the include files defined in the config.mk file
#
INCLUDES =  -I. -I../mbase -I../mgui -I../mraw -I../manalysis \
            -I../mgeom -I../mtools -I../mpedestal -I../mbadpixels \
            -I../mcalib -I../mextralgo

# mgui (MCamEvent):         MExtractSignalCam
# mgeom(MGeomCam):          MArrivalTime
# mtools(MCubicSpline):     MArrivalTime
# mraw (MRawEvtData):       MExtractSignal
# manalysis (MPedestalCam): MExtractSignal

SRCFILES = MExtractedSignalCam.cc \
           MExtractedSignalPix.cc \
	   MExtractedSignalPINDiode.cc \
	   MExtractedSignalBlindPixel.cc \
           MExtractor.cc \
	   MExtractPINDiode.cc \
	   MExtractBlindPixel.cc \
           MExtractTime.cc \
           MExtractTimeAndCharge.cc \
           MExtractTimeAndChargeSpline.cc \
           MExtractTimeAndChargeDigitalFilter.cc \
           MExtractFACT.cc \
           MSignalCam.cc \
           MSignalPix.cc \
           MSignalCalc.cc \
	   MFilterData.cc \
	   MTreatSaturation.cc \
           MArrivalTimeCam.cc \
	   MArrivalTimePix.cc

#           MExtractSignal.cc \
#           MExtractSignal2.cc \
#           MExtractSignal3.cc \
#           MExtractSignalABcorr.cc \

#           MExtractFixedWindow.cc \
#           MExtractSlidingWindow.cc \
#           MExtractFixedWindowPeakSearch.cc \
#           MExtractFixedWindowSpline.cc \
#           MExtractAmplitudeSpline.cc \
#           MExtractTimeSpline.cc \
#           MExtractTimeFastSpline.cc \
#           MExtractTimeHighestIntegral.cc \
#           MExtractTimeAndChargeSlidingWindow.cc \
#           MExtractTimeAndChargeDigitalFilterPeakSearch.cc \

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
