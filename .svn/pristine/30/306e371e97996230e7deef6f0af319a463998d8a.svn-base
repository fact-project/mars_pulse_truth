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
CINT  = Calib

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../mgeom -I../mhcalib -I../mhbase \
           -I../manalysis -I../mraw -I../mtools -I../mmc -I../mhist  \
           -I../mimage -I../msignal -I../mbadpixels -I../mpedestal -I../mcore
# mhbase:    MBinning MH 
# mgui:      MCamEvent (McalibrationCam)
# mgeom:     MGeomCam (McalibrationCam - necessary?)
# manalysis: MExtractedSignal, MCerPhotEvt (move to mcalib?)
# mraw:      MRawRunHeader, MRawEvtHeader, MRawEvtPixelIter (3xMCalibrationCalc)
# mmc:       MMcFadcHeader, MMcEvt
# mimage     MHillas

SRCFILES = MCalibColorSet.cc \
	   MCalibColorSteer.cc \
	   MCalibCalcFromPast.cc \
	   MCalibrateData.cc \
	   MCalibrateFact.cc \
	   MCalibConstCam.cc \
	   MCalibConstPix.cc \
	   MCalibrationPattern.cc \
	   MCalibrationPatternDecode.cc \
	   MCalibrateRelTimes.cc \
           MCalibrationCam.cc \
           MCalibrationPix.cc  \
           MCalibrationPulseTimeCam.cc \
           MCalibrationRelTimeCam.cc \
           MCalibrationRelTimePix.cc \
	   MCalibrationRelTimeCalc.cc \
           MCalibrationQECamMagic.cc \
           MCalibrationQECam.cc \
           MCalibrationQEPix.cc  \
	   MCalibrationTestCalc.cc \
	   MCalibrationTestCam.cc \
	   MCalibrationTestPix.cc \
	   MCalibrationChargeCalc.cc \
           MCalibrationChargeCam.cc \
           MCalibrationChargePix.cc  \
           MCalibrationBlindCam.cc  \
           MCalibrationBlindPix.cc  \
           MCalibrationBlindCamOneOldStyle.cc  \
           MCalibrationBlindCamTwoNewStyle.cc  \
           MCalibrationBlindCamThreeNewStyle.cc  \
           MCalibrationChargePINDiode.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
