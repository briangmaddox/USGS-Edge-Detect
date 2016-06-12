# $Id: Makefile,v 1.1.1.1 2002-02-20 16:41:31 cbilderback Exp $
# Makefile for the UGM program
# Last modified by $Author: cbilderback $ on $Date: 2002-02-20 16:41:31 $

prefix       = /home/cbilder
host_os      = linux
srcdir       = .
top_srcdir   = .
enable_debug = no

# Set up the include paths
INCPATHS = -I$(prefix)/include -I$(prefix)/include/tiff -I$(prefix)/include/geotiff 
LIBDIRS  = -L$(prefix)/lib

# Libraries we need to link in
LIBS =  -lImageLib  -lgeotiff -ltiff

# Linker flags
LDFLAGS   = $(LIBDIRS)
LOADLIBES = $(LIBS)

# Set our compiler options
ifeq ($(enable_debug),yes)
DEBUG = -g -Wall -DTNT_NO_BOUNDS_CHECK 
else
DEBUG = -O3 -Wall -DTNT_NO_BOUNDS_CHECK
#-march=pentiumpro -mcpu=pentiumpro -fomit-frame-pointer -mieee-fp -fschedule-insns2 -finline-functions -frerun-loop-opt -fstrength-reduce -ffast-math -funroll-loops -fexpensive-optimizations -fthread-jumps
endif

# Compiler and other defs
CC   = gcc
CXX  = g++
CXXFLAGS = $(DEBUG) $(INCPATHS)

# Suffix rules
.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

# Dependencies for the master program
OBJS = main.o Thinner.o

all: thinit

thinit : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o thinit $(LIBDIRS) $(LIBS)
clean:
	rm -f $(OBJS) $(SOBJ) *~ thinit











