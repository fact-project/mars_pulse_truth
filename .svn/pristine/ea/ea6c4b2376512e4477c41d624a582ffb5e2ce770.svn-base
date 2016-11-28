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
CINT  = Main

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../manalysis -I../mdatacheck -I../mraw        \
	   -I../mgui -I../mgeom -I../mhbase -I../mhist -I../mmontecarlo   \
           -I../mfileio -I../mimage -I../mhistmc -I../mgbase -I../mfbase  \
           -I../mdata -I../msignal -I../mcalib -I../mbadpixels            \
           -I../mpointing -I../mpedestal -I../mmuon -I../mfilter -I../mjobs \
	   -I../mtrigger -I../mcorsika -I../msimcamera -I../msim

SRCFILES = MBrowser.cc \
           MEventDisplay.cc \
           MCameraDisplay.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
