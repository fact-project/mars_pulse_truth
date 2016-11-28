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
CINT  = Geom

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../MBase

SRCFILES = MGeom.cc \
           MGeomPix.cc \
           MGeomRectangle.cc \
           MGeomCam.cc \
           MGeomCamCT1.cc \
           MGeomCamDwarf.cc \
           MGeomCamFact.cc \
           MGeomCamFACT.cc \
           MGeomCamMagic.cc \
           MGeomCamSquare.cc \
           MGeomCorsikaCT.cc \
	   MGeomMirror.cc \
	   MGeomPMT.cc


#           MGeomCamCT1Daniel  \
#           MGeomCamMagicHG    \
#           MGeomCamMagic919   \
#           MGeomCamMagic1183  \
#           MGeomCamECO1000    \
#           MGeomCamECO1000HG  \

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
