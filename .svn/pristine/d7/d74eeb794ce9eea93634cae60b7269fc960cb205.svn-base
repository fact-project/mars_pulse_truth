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
CINT  = Astro

#
#  connect the include files defined in the config.mk file
#
INCLUDES =  -I. -I../mbase -I../mgeom -I../mtemp -I../mstarcam -I../mhist -I../mcore
# mgeom (MAstroCamera): MGeomCam, MGeomMirror

SRCFILES = MVector3.cc \
	   MAstro.cc \
	   MAstroSky2Local.cc \
	   MAstroCatalog.cc \
           MAstroCamera.cc \
           MObservatory.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak













