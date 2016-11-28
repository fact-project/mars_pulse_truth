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
CINT  = Corsika

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mfileio -I../mmc -I../msim -I../mgui -I../mcore

SRCFILES = MCorsikaRunHeader.cc \
	   MCorsikaFormat.cc \
	   MCorsikaEvtHeader.cc \
           MCorsikaRead.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
