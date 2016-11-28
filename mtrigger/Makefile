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
CINT  = Trigger

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mgui -I../mraw -I../mcalib

SRCFILES = MTriggerIPR.cc \
	   MTriggerCell.cc\
	   MTriggerBit.cc\
	   MTriggerPrescFact.cc\
	   MTriggerLiveTime.cc \
           MTriggerPattern.cc \
           MTriggerPatternDecode.cc \
           MFTriggerPattern.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
