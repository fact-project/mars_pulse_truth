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
CINT  = Electronics

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mextralgo

SRCFILES = MAvalanchePhotoDiode.cc \
           MAnalogSignal.cc \
           MAnalogChannels.cc \
	   MDigitalSignal.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
