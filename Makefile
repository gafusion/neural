# Either option 1 or 2
# 1
# The GACODE_ROOT environmental variable should point to the root directory where GACODE is installed.
# Environment variable GACODE_PLATFORM should be set to one of the following systems:
#   ALCF_BGP ALCF_CETUS BABBAGE BANACH CAOS CARVER CMODWS DELPHI
#   DROP EDISON_CRAY EDISON_INTEL FT GFORTRAN_CORE2 GFORTRAN_OSX
#   GFORTRAN_OSX_64 GFORTRAN_OSX_BELLI GFORTRAN_OSX_MACPORTS
#   GFORTRAN_OSX_TECHX GFORTRAN_PENRYN GFORTRAN_TECHX HOPPER_CRAY
#   HOPPER_INTEL HPC_ITER IFORT_CORE2 IFORT_PENRYN JAC LINDGREN
#   LOHAN LOKI LOKI_SCRATCH METIUS NEWT OSX_MOUNTAINLION PACER
#   PGFORTRAN_OSX PG_OPT64 PG_OPT64_FFTW PG_OPT64_MUMPS PPPL
#   PPPL_PATHSCALE RANGER SATURN TITAN_CRAY VENUS
# 2
# Define CC, CFLAGS, FC, FFLAGS, ARCH here or at the command line
#
ifdef GACODE_ROOT
	include $(GACODE_ROOT)/shared/install/make.inc.$(GACODE_PLATFORM)
else
	CC=cc
	CFLAGS=
	FC=gfortran
	FFLAGS=
	ARCH=ar cr
endif

LLIB = libbrainfuse.a

EXEC = brainfuse_run.exe

OBJECTS = brainfuse_lib.o brainfuse_exe.o

$(LLIB): brainfuse_lib.o Makefile
	$(ARCH) $(LLIB) $<

$(EXEC) : brainfuse_run.c $(LLIB) 
	$(CC) $(CFLAGS) -o $@ -I./ -L./ $< -lbrainfuse -lfann -lm -L$(FANN_ROOT)/src -I$(FANN_ROOT)/src/include/

all: $(LLIB) $(EXEC)

toq_profiles_test:
	cd eped1nn; $(FC) -o toq_profiles_test toq_profiles.f90 toq_profiles_test.f90

clean:
	rm -f *.o  *.a *~ $(EXEC)

gacode_install:
	@[ -d $(GACODE_ROOT)/shared/neural ] || mkdir $(GACODE_ROOT)/shared/neural
	cp -f Makefile brainfuse_lib.c brainfuse_lib.h brainfuse_lib.inc $(GACODE_ROOT)/shared/neural/

omfit_install:
	cp -f brainfuse.py $(OMFIT_ROOT)/src/classes/
