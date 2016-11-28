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
CINT  = HCalib

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase  -I../mgui -I../mgeom -I../mhbase -I../mcalib \
           -I../manalysis -I../mraw -I../mtools -I../mmc -I../mhist  \
           -I../mimage -I../msignal -I../mbadpixels -I../mpedestal 

# mhbase:    MBinning MH 
# mgui:      MCamEvent (McalibrationCam)
# mgeom:     MGeomCam (McalibrationCam - necessary?)
# mcalib:    MCalibration*Cam
# manalysis: MExtractedSignal, MCerPhotEvt (move to mcalib?)
# mraw:      MRawRunHeader, MRawEvtHeader, MRawEvtPixelIter (3xMCalibrationCalc)
# mmc:       MMcFadcHeader, MMcEvt
# mimage     MHillas

SRCFILES = MHCalibrationCam.cc \
           MHCalibrationPix.cc \
           MHCalibrationChargeCam.cc \
           MHCalibrationChargePix.cc \
           MHCalibrationChargeBlindCam.cc \
           MHCalibrationChargeBlindPix.cc \
           MHCalibrationChargePINDiode.cc \
           MHCalibrationRelTimeCam.cc \
           MHCalibrationPulseTimeCam.cc \
	   MHGausEvents.cc 

#           MHPedestalCam.cc \
#           MHPedestalPix.cc \

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
