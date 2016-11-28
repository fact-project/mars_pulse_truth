##################################################################
#
#   makefile
# 
#   for the MARS software
#
##################################################################
# @maintitle

# @code

#
#  please change all system depend values in the 
#  config.mk.${OSTYPE} file 
#
#

include Makefile.conf.general
include Makefile.conf.$(OSTYPE)

#
#PROGRAMS = readraw merpp mars test mona status
PROGRAMS := readdaq readraw readcorsika ceres merpp callisto star ganymed sponde showlog showplot mars mars-config mirrorSimulation
SOLIB    := libmars.so

CINT      = M

#
#  connect the include files defined in the config.mk file
#
#    WARNING: the result (whether the linkage works or not) depends on the
#             order of the libraries. It seems, that the most base library
#             must be the last one
#

#
#  ----->>>   mars libraries
#
SUBDIRS := mbase \
	  mastro \
	  mmain \
          mcorsika \
	  mdrs \
          mfbase \
          mfilter \
          mdata \
          mhbase \
          mhvstime \
          mhist \
          manalysis \
          mextralgo \
          msignal \
          mbadpixels \
          mhcalib \
          mhflux \
	  mcalib \
          mpedestal \
          mfileio \
          mgeom \
	  melectronics \
	  msim \
	  msimreflector \
	  msimcamera \
          msql \
          mimage \
          mhft \
          mmc \
          mraw \
          mcamera \
	  mtrigger \
          mpointing \
          mreport \
          mgui \
          mranforest \
          mjobs \
          mjoptim \
          mjtrain \
          mmovie \
          mtools \
          mmuon \

LIBNOVA_PATH := `which libnovaconfig`

ifeq ($(shell basename "$(LIBNOVA_PATH)X"), libnovaconfigX)
#SUBDIRS += libnova
LIBNOVA = -lnova
endif

# Insert here "-I" and the path to your libnova installation, 
# e.g. -ImyFolder/libnova/include.
# Leave it empty, if libnova is installed 
# to the default location (i.e. /usr/include/libnova).
LIBNOVA_INCLUDE_PATH :=

#LIBRARIES = $(SUBDIRS:%=lib/lib%.a)
LIBRARIES := $(SUBDIRS:=.a)
CONDOR    := $(SUBDIRS:=.condor)
SGE       := $(SUBDIRS:=.sge)
MRPROPERS := $(SUBDIRS:=.mrproper)
CLEANERS  := $(SUBDIRS:=.clean)
LIBS      := $(SOLIB)

#------------------------------------------------------------------------------
.SUFFIXES: .c .cc .h .o .cxx .hxx .gch

SRCFILES = \
	   mcore/factofits.h \
	   mcore/factfits.h \
	   mcore/DrsCalib.h \
	   mcore/PixelMap.h \
	   mcore/Interpolator2D.h \
	   mcore/nova.h \
	   mcore/Prediction.h

############################################################
RNDMNAME:=$(shell mktemp)

all: $(SOLIB) $(PROGRAMS)
	@echo " Done. "
	@echo " "

static: LIBS=$(SUBDIRS:=/*.o) $(OBJS)
#static: rmlib $(LIBRARIES) $(PROGRAMS)
static: $(LIBRARIES) $(PROGRAMS)
	@echo " Done. "
	@echo " "

include Makefile.rules

#
# Use $(CXX) -v ... for a more verbose output
#
# ROOTGLIBS must be there - why? How can I link the libraries?
$(SOLIB): $(LIBRARIES) $(OBJS) $(HEADERS)
	@echo " Linking shared object $@ ..."
	$(LINKER) $(CXXFLAGS) $(SOFLAG) $(OBJS) $(SUBDIRS:=/*.o) $(ROOTGLIBS) $(LIBNOVA) -o ${RNDMNAME}
	mv ${RNDMNAME} $@
	chmod go+r $@

# This is a special workaround to create the shared object (bundle, plugin)
# for root and the dynlib (to be linked with the executable) on Mac OSX
ifneq ($(OSTYPE),darwin)
$(PROGRAMS): $(PROGRAMS:=.o) $(SOLIB)
	@echo " Linking $@ ..." 
	$(LINKER) $(CXXFLAGS) $@.o $(MARS_LIB) $(SOLIB) $(ROOTGLIBS) -o $@

# Use this to link the programs statically - for gprof
#$(PROGRAMS): $(OBJS) $(HEADERS) $(PROGRAMS:=.o)
#	@echo " Linking $@ ..." 
#	$(CXX) $(CXXFLAGS) $(ROOTGLIBS) $(OBJS) $(SUBDIRS:=/*.o) $@.o $(MARS_LIB) -o $@
else
$(DYLIB): $(LIBRARIES) $(OBJS) $(HEADERS)
	@echo " Linking dylib $(DYLIB) ..."
	$(LINKER) $(CXXFLAGS) $(DYFLAG) $(OBJS) $(SUBDIRS:=/*.o) $(ROOTGLIBS) -o $@

$(PROGRAMS): $(PROGRAMS:=.o) $(DYLIB)
	@echo " Linking mac executable $@ ..." 
	$(LINKER) $(CXXFLAGS) $(ROOTGLIBS) $(DYLIB) $@.o $(MARS_LIB) -o $@
endif

rmcondor:
	rm -f .makecondor.log

condor:	rmcondor $(CONDOR)
	echo Waiting for jobs to finish...
	condor_wait .makecondor.log
	rm .makecondor.log
	make -j2 $(PROGRAMS)
#	find -name "*/.makecondor.*"
#	find -maxdepth 2 -name ".makecondor.*" -exec echo {}\\\; cat {}\\\; echo {}\\\; \;

sge:	$(SGE)
	make -j16 $(PROGRAMS)

dox: $(SOLIB)
	@echo
	@echo " Creating html documentation and logfile dohtml.log..."
	rm -f dohtml.log
	root -b -q dohtml.C 2>&1 >> dohtml.log | tee -a dohtml.log
	@echo " done."
	@echo

#clean:	rmcint rmobjs rmdep rmcore rmlib

mrproper:	$(MRPROPERS) rmbin rmbak rmbakmac rmhtml clean
	@echo " Done."
	@echo " "

tar:	mrproper
	@echo "Making tar-file"
	root -b -q -l -n tar.C
#	@tar cvf ../mars.tar --exclude=Root .rootrc *
#	@gzip -9 ../mars.tar

#Makefile.depend:
#	(! find ./ Makefile.depend -maxdepth 1 -empty 2> /dev/null && \
#	echo " Generating dependancies into Makefile.depend" && \
#	makedepend -- $(INCLUDES) -- $(PROGRAMS:=.cc) $(SRCS) $(SUBDIRS:=/*.cc) -w1024 -f- 2> /dev/null | grep -v Cint | grep -v "/usr/" > Makefile.depend && \
#	echo " ") || find -maxdepth 0 -true > /dev/null
#
#depend:	Makefile.depend	

# @endcode
