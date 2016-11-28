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

INCLUDES = -I. -I../mbase -I../mfbase -I../mraw -I../mdata \
           -I../manalysis -I../mpointing -I../mfileio -I../mgeom \
           -I../mimage -I../mgui -I../mmc -I../mhbase -I../msignal \
           -I../mpedestal -I../mbadpixels -I../mcalib

CINT     = Filter

SRCFILES = MFTriggerLvl1.cc \
	   MFTriggerLvl2.cc \
           MFMagicCuts.cc \
	   MFGeomag.cc \
	   MFEvtNumber.cc \
           MFDeltaT.cc \
	   MFParticleId.cc \
           MFSoftwareTrigger.cc \
           MFCosmics.cc \
	   MFEnergySlope.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
