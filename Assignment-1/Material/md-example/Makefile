# Makefile for the molecular dynamics code

#include ../make.def

CC          = gcc
CLINKER     = $(CC)
OPTFLAGS    = -fopenmp
LIBS        = -lm

CFLAGS	  = $(OPTFLAGS) 

OBJ=o
RM=rm -f


#
# Object files
#
MDOBJS=    main.$(OBJ) \
	dfill.$(OBJ) \
	domove.$(OBJ) \
      dscal.$(OBJ) \
	fcc.$(OBJ) \
	forces.$(OBJ) \
	mkekin.$(OBJ) \
	mxwell.$(OBJ) \
	prnout.$(OBJ) \
      random.$(OBJ) \
	velavg.$(OBJ) 

#
# Compile
#
md:		$(MDOBJS)
	$(CLINKER) $(OPTFLAGS) -o $@ $(MDOBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< $(OPTFLAGS) 

#
# Clean out object files and the executable.
#
clean:
	$(RM) *.$(OBJ) md
