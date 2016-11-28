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
CINT  = SimCamera

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgeom -I../msim -I../msignal -I../mcorsika \
	   -I../mmc -I../mgui -I../mcalib -I../mraw -I../mfileio -I../melectronics \
	   -I../mtrigger -I../mpedestal -I../msimreflector

SRCFILES = MSimPSF.cc \
	   MSimGeomCam.cc \
	   MSimRandomPhotons.cc \
	   MSimAPD.cc \
	   MSimBundlePhotons.cc \
	   MSimCalibrationSignal.cc \
	   MSimExcessNoise.cc \
	   MSimCamera.cc \
	   MSimTrigger.cc \
	   MSimReadout.cc \
           MSimSignalCam.cc \
           MTruePhotonsPerPixelCont.cc \
           MPulse.cc \
					 MCollectSimulationTruth.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
