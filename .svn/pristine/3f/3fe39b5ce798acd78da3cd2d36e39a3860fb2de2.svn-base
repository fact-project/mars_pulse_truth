##################################################################
#
#   makefile
# 
#   for the MARS software
#
##################################################################
include ../Makefile.conf.general
include ../Makefile.conf.$(OSTYPE)

############################################################

#
# Handling name of the Root Dictionary Files
#
CINT  = Sql

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mraw -I../mfileio

SRCFILES = \
	MSQLServer.cc \
        MSQLMagic.cc \
	MTreeSQL.cc \
	MBasketSQL.cc \
	MBufferSQL.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
