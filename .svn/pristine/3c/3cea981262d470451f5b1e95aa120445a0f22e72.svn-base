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
CINT  = Optim

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mhbase -I../mfbase -I../mdata -I../mfileio \
           -I../manalysis -I../mgeom -I../mfilter -I../mjobs
# mdata:     MDataMember
# mfbase:    MF
# mfilter:   MFMagicCuts
# mhbase:    MHMatrix
# manalysis: MMatrixLoop
# mjobs:     MJob
# mgeom:     MGeomCam

SRCFILES = MJOptimizeBase.cc \
           MJOptimize.cc \
           MJOptimizeCuts.cc \
           MJOptimizeDisp.cc \
           MJOptimizeEnergy.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
