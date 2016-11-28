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
CINT  = Report

#
#  connect the include files defined in the config.mk file
#
INCLUDES = -I. -I../mbase -I../mastro -I../mcamera -I../mgui -I../mtrigger \
	-I../mcore
# mgui - MCameraDC <MCamEvent>

SRCFILES = MReport.cc \
           MReportCC.cc \
           MReportRec.cc \
           MReportRun.cc \
           MReportDAQ.cc \
           MReportHelp.cc \
           MReportDrive.cc \
           MReportCamera.cc \
           MReportTrigger.cc \
           MReportCurrents.cc \
           MReportFitsRead.cc \
           MReportFileRead.cc \
           MReportFileReadCC.cc \
           MReportStarguider.cc \
           MReportPyrometer.cc \
           MReportRates.cc \
           MReportTemperatures.cc \
           MReportHumidity.cc


############################################################

all: $(OBJS)

include ../Makefile.rules

mrproper:	clean rmbak
