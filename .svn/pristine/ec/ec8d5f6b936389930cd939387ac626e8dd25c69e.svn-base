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
CINT  = Drs

#
#  connect the include files defined in the config.mk file
#
INCLUDES = \
	-I. -I../mbase -I../mgeom -I../mbadpixels -I../mraw -I../msignal \
	-I../mhbase -I../mcore -I../mhist -I../mgui -I../mpedestal

SRCFILES = MCalibrateDrsTimes.cc \
	   MDrsCalibApply.cc \
	   MHDrsCalibration.cc \
	   MDrsCalibration.cc \
	   MDrsCalibrationTime.cc \
	   MHDrsCalibrationCheck.cc \
	   MHDrsCalibrationTime.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
