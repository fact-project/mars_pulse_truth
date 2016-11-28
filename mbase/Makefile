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

INCLUDES = -I. -I../mfileio -I../mfbase -I../mastro -I../mcore
# mfileio:  MRead  (MEvtLoop)
# mfbase:   MF     (MContinue)
# mastro:   MAstro (MTime)

CINT = Base

SRCFILES = MAGIC.cc \
	   MLogo.cc \
           MArgs.cc \
           MString.cc \
           MMath.cc \
           MSpline3.cc \
           MReflection.cc \
	   MQuaternion.cc \
           MEnv.cc \
	   MLog.cc \
           MLogManip.cc \
           MLogPlugin.cc \
           MLogHtml.cc \
	   MParSpline.cc \
	   MParList.cc \
           MParContainer.cc \
	   MParEmulated.cc \
	   MParEnv.cc \
           MParameters.cc \
	   MInputStreamID.cc \
           MTask.cc \
           MTaskEnv.cc \
           MTaskInteractive.cc \
	   MTaskList.cc \
           MFilter.cc \
           MEvtLoop.cc \
           MProgressBar.cc \
           MSearch.cc \
           MIter.cc \
           MDirIter.cc \
           MReadSocket.cc \
           MGGroupFrame.cc \
           MGMenu.cc \
           MGMap.cc \
           MGList.cc \
           MGTask.cc \
           MStatusArray.cc \
           MStatusDisplay.cc \
	   MLut.cc \
	   MArray.cc \
	   MArrayB.cc \
	   MArrayS.cc \
	   MArrayD.cc \
	   MArrayF.cc \
	   MArrayI.cc \
           MTime.cc \
           MClone.cc \
           MContinue.cc \
           MPrint.cc \
	   MThread.cc \
           MObjLookup.cc

############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
