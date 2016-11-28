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

INCLUDES = -I.  -I../mbase -I../MBase -I../mgeom -I../mgui -I../mcore

CINT     = Mc

SRCFILES = MMcTrigHeader.cxx \
	   MMcFadcHeader.cxx \
	   MMcRunHeader.cxx \
	   MMcEvt.cxx \
           MMcTrig.cxx \
	   MMcConfigRunHeader.cc \
	   MMcCorsikaRunHeader.cc \
	   MMcEvtBasic.cc

############################################################

all: $(OBJS)

include ../Makefile.rules 

mrproper:	clean rmbak
