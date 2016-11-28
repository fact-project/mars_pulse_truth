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

INCLUDES = -I. -I../mbase -I../mdata -I../mfileio -I../mhbase

CINT     = FBase

SRCFILES = MF.cc \
	   MFDataPhrase.cc \
           MFilterList.cc \
           MFEventSelector.cc \
           MFEventSelector2.cc \
	   MFDataChain.cc \
	   MFDataMember.cc \
           MFRealTimePeriod.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
