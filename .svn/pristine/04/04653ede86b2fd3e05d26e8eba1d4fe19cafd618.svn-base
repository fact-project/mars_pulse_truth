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
CINT  = HBase

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mdata

SRCFILES = MFillH.cc \
           MBinning.cc \
           MH.cc \
           MH3.cc \
	   MHn.cc \
           MHArray.cc \
           MHMatrix.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
