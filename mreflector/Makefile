##################################################################
#
#   makefile
# 
#   for the MARS software
#
##################################################################
include ../Makefile.conf.$(OSTYPE)
include ../Makefile.conf.general

#------------------------------------------------------------------------------

#
# Handling name of the Root Dictionary Files
#
CINT  = Reflector

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../mgeom -I../mhbase

SRCFILES = MRflEvtData.cc \
	   MRflEvtHeader.cc \
	   MRflRunHeader.cc \
	   MRflSinglePhoton.cc \
	   MHReflector.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
