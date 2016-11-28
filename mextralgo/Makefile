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
CINT  = Extralgo

#
#  connect the include files defined in the config.mk file
#
INCLUDES =  -I.

SRCFILES = MExtralgoSpline.cc \
	   MExtralgoDigitalFilter.cc 

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
