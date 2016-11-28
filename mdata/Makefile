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

INCLUDES = -I. -I../mbase -I../mhbase

CINT     = Data

SRCFILES = MData.cc \
	   MDataPhrase.cc \
	   MDataArray.cc \
           MDataElement.cc \
	   MDataMember.cc \
	   MDataValue.cc \
	   MDataList.cc \
	   MDataChain.cc \
           MDataFormula.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak

