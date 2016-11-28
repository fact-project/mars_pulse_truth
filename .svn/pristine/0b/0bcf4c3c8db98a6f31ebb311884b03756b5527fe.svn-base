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

INCLUDES = -I. -I../mbase -I../mraw -I../mmc -I../mreflector -I../mgui \
	   -I../mdata -I../mbadpixels -I../msim -I../mcore
#mraw:       MRawRunHeader (MReadMaraFile)
#mmc:        MMcRunHeader  (MReadMarsFile)
#mreflector: MRfl*         (MReadRflFile)
#mgui:       MCamEvent     (MReadRflFile)
#mdata:      MData*        (MWriteAsciiFile)

CINT     = FileIO

SRCFILES = MRead.cc \
           MChain.cc \
	   MReadTree.cc \
           MReadFiles.cc \
           MReadReports.cc \
           MReadMarsFile.cc \
           MReadScanFile.cc \
           MWriteFile.cc \
           MWriteAsciiFile.cc \
           MWriteRootFile.cc \
           MWriteFitsFile.cc \
           MTopFitsGroup.cc \
           MFitsArray.cc \
           MMatrix.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
