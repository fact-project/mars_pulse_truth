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
CINT  = HVsTime

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mgeom -I../mhist \
	   -I../mgui -I../mraw -I../mdata -I../mcalib
# mhist (MHCamera)
# mgui  (MCamEvent)
# mraw  (MRawEvtHeader)

SRCFILES = MHVsTime.cc \
           MHPixVsTime.cc \
           MHSectorVsTime.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
