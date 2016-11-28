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
CINT  = Muon

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mmc -I../mraw -I../mgeom -I../mfilter \
	   -I../mdata -I../mhbase -I../mhist -I../mgui -I../mimage   \
           -I../mhistmc -I../mfileio -I../mmain -I../mhcalib -I../mcalib \
           -I../msignal -I../mpointing -I../mtools -I../mfbase       \
           -I../mbadpixels -I../mastro -I../mpedestal -I../manalysis

SRCFILES = MMuonSearchPar.cc \
           MMuonSearchParCalc.cc \
           MMuonCalibPar.cc \
           MMuonCalibParCalc.cc \
           MHMuonPar.cc \
           MHSingleMuon.cc \
           MMuonSetup.cc

############################################################

all:  $(OBJS)

include ../Makefile.rules

clean:	rmcint rmobjs rmcore rmlib

mrproper:	clean rmbak







