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
CINT  = Pedestal

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../mraw -I../mmc -I../mgeom \
           -I../msignal -I../manalysis -I../mbadpixels -I../mhcalib \
           -I../mhbase -I../mcalib -I../mtrigger -I../mcore
# MCamEvent
# MMcPedestalCopy   (MRawRunHeader)
# MMcPedestalCopy   (MMcRunHeader)
# MMcPedestalNSBAdd (MGeomCam)
# MPedCalcPedRun    (MExtractedSignal)
# MPedPhotCalc      (MCerPhotEvt)
# MPedPhotCalc      (MBadPixelsCam)

SRCFILES = MMcPedestalCopy.cc \
           MMcPedestalNSBAdd.cc \
	   MExtractPedestal.cc \
           MPedCalcPedRun.cc \
	   MPedCalcFromLoGain.cc \
           MPedPhotCalc.cc \
           MPedPhotCam.cc \
           MPedPhotPix.cc \
           MPedestalCam.cc \
	   MPedestalPix.cc \
           MHPedestalCor.cc \
           MPedestalSubtract.cc \
           MPedestalSubtractedEvt.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

clean:	rmcint rmobjs rmcore rmlib

mrproper:	clean rmbak
