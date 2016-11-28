##################################################################
#
# makefile
#
# @file        makefile
# @title       Converter of DCCurrent files to new format
# @author      M Merck
# @email       merck@astro.uni-wuerzburg.de
# @date        Wed Dec 17 11:51:11 MET 2003
#
#_______________________________________________________________
#
# Created: Wed Dec 17 11:51:11 MET 2003
# Author:  Martin Merck
# Purpose: Makefile for the compilation of dcconvertor
# Notes:   
#    
##################################################################

# @code

# compilers & tools

CC            = gcc
CXX           = g++
F77           = g77

INCLUDES = -I.

OPTIM    =
DEBUG    = -g

# compilation and linking flags

CXXFLAGS  = ${INCLUDES} ${OPTIM} ${DEBUG}
CFLAGS    = ${CXXFLAGS}
FFLAGS    = ${CXXFLAGS}
LIBS      =

#------------------------------------------------------------------------------

#.SILENT:

.SUFFIXES: .c .cxx .cpp .C .c++ .h .hxx .H .h++ .o .so .f

SRCS = \
	main.cpp \
	DCCurrentEvent.cpp

HEADERS = \
	DCCurrentEvent.h \
	Globals.h

OBJS = \
	DCCurrentEvent.o \
	main.o

PROGRAM=dcconvertor

############################################################

all: ${PROGRAM}

depend:
	@makedepend $(SRCS) -fMakefile 2> /dev/null

${PROGRAM}: $(OBJS)
	@echo "Linking..." $@
	$(CXX) $(CXXFLAGS) $(OBJS) $(LIBS) -o $@
	@echo "done."

.cxx.o:
	@echo "Compiling " $<
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	@echo "Compiling " $<
	$(CC) $(CFLAGS) -c $< -o $@

.f.o:
	@echo "Compiling " $<
	$(F77) $(FFLAGS) -c $< -o $@

lclean:
	@echo "Cleanning..."
	@rm -f *.o core

clean:
	@echo "Cleanning..."
	@rm -f $(OBJS) core 

mrproper: clean
	@echo "Mr.Proper in action . . ."
	@rm -f $(PROGRAM)

ctags:
	@echo "Creating CTAGS file . . ."
	@ctags -txw $(SRCS) $(HEADERS) > CTAGS

etags:
	@echo "Creating TAGS file . . ."
	@etags -C $(SRCS) $(HEADERS)

listsrc:
	@ls -m $(SRCS) $(HEADERS) | sed 's/,//g'

redo: clean all

# @endcode
# DO NOT DELETE

main.o: Globals.h /usr/include/stdlib.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
main.o: /usr/lib/gcc-lib/i486-suse-linux/3.3/include/stddef.h
main.o: DCCurrentEvent.h
DCCurrentEvent.o: DCCurrentEvent.h Globals.h
