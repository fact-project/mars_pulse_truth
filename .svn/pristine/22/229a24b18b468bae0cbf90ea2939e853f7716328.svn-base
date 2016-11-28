##################################################################
#
#   makefile
# 
#   for the MARS IFAE library
#
##################################################################
# @maintitle

# @code

#
#  please change all system depend values in the 
#  config.mk.${OSTYPE} file 
#
#
include ../../../Makefile.conf.$(OSTYPE)
include ../../../Makefile.conf.general

#
CINT     = IFAE
#

SOLIB    = ../../../libmars.so

#------------------------------------------------------------------------------

INCLUDES = -I. \
	   -I../../../mbase \
	   -I../../../mjobs \
	   -I../../../mpedestal \
	   -I../../../mbadpixels \
	   -I../../../mfileio \
           -I../../../mraw \
           -I../../../manalysis \
	   -I../../../mgui \
	   -I../../../mgeom \
	   -I../../../msignal \
	   -I../../../mcalib \
	   -I../../../mfilter \
	   -I../../../mhbase \
	   -I../../../mimage \
	   -I../../../mpointing \
	   -I../../../mcamera \
	   -I../../../mastro \
	   -I../../../mhist \
	   -I../../../mfbase \
	   -I../../../mmc \
	   -I../../../mdata \
	   -I../../


SRCFILES = \
	MDisplay.cc \
	MHillasDisplay.cc \
        MPSFFit.cc \
        MPSFFitCalc.cc \
	MSrcPlace.cc \
        MSrcPosFromFile.cc \
	MSrcRotate.cc \
	MSrcTranslate.cc \
        MIslands.cc \
	MImgIsland.cc \
        MIslandsCalc.cc \
        MIslandsClean.cc \
        MFHVNotNominal.cc \
        MCalibrateDC.cc \
        MHPSFFromStars.cc \
	MControlPlots.cc \
        MSrcPosFromStars.cc \
        MLiveTime.cc \
        MLiveTimeCalc.cc \
	MTopology.cc \
	MTopologyCalc.cc \
	MImageParDisp.cc \
	MDispParameters.cc \
	MDispCalc.cc \
	MHDisp.cc \
	MFDisp.cc \
	MFindDisp.cc \
	MEffAreaAndCoeffCalc.cc \
	MGainFluctuationPix.cc \
	MGainFluctuationCam.cc \
	MAddGainFluctuation.cc


############################################################

all: $(OBJS)

include ../../../Makefile.rules

clean:	rmcint rmobjs rmcore rmlib
mrproper: clean rmbak

# @endcode
