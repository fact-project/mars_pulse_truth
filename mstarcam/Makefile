##################################################################
#
#   subdirectory makefile
# 
#   for the MARS software
#
##################################################################
include ../Makefile.conf.$(OSTYPE)
include ../Makefile.conf.general

#------------------------------------------------------------------------------

INCLUDES = -I. -I../mbase -I../mhbase -I../mhist -I../mgeom -I../mastro

CINT = Starcam

SRCFILES = MStarCam.cc \
	   MStarPos.cc \
           MStarCamTrans.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
