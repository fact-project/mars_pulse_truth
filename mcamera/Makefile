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
CINT  = Camera

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui
# mgui - MCameraDC <MCamEvent>

SRCFILES = MCameraAUX.cc \
	   MCameraCalibration.cc\
	   MCameraCooling.cc \
	   MCameraHV.cc \
	   MCameraLV.cc \
	   MCameraTH.cc \
	   MCameraTD.cc \
	   MCameraRecTemp.cc \
	   MCameraDC.cc \
	   MCameraLid.cc \
	   MCameraLids.cc \
	   MCameraActiveLoad.cc \
	   MCameraCentralPix.cc \
	   MCameraPowerSupply.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
