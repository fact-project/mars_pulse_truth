##################################################################
#
#   subdirectory makefile
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
CINT  = Movie

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mraw -I../mgeom -I../mhbase -I../mgui \
           -I../mcalib -I../msignal -I../mbadpixels -I../mpedestal   \
           -I../mhist -I../mmc

SRCFILES = MMoviePrepare.cc \
           MMovieData.cc \
           MMovieWrite.cc

############################################################

all:  $(OBJS)

include ../Makefile.rules

clean:	rmcint rmobjs rmcore rmlib

mrproper:	clean rmbak

