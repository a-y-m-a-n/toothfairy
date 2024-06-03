# On command line:
# make all = Make software.
# make clean = Clean out built project files.
#
# To rebuild project do "make clean" then "make all".
#

TARGET = cpldprog

XSVFDIR=./playxsvf_src

# List C source files here for all files to be included in the library
# (C dependencies are automatically generated.)
CXXSRC = 
CSRC= xvsf_file.c cpld_main.c conduit.c interface.c ppiwin.c bitbang.c \
	$(XSVFDIR)/lenval.c \
	$(XSVFDIR)/micro.c \
	$(XSVFDIR)/ports.c 

LEX_SRC = 
YACC_SRC = 

DEPS =  $(CXXSRC:.cpp=.d)  $(CSRC:.c=.d)

PREFIX=i686-w64-mingw32-
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
CFLAGS=-Wall -g  -DDEBUG_MODE #-mno-cygwin 
LDFLAGS = $(CFLAGS) 

# ---------------------------------------------------------------------------


# Define all object files.
OBJ = $(CXXSRC:.cpp=.o) $(CSRC:.c=.o) $(YACC_SRC:.y=.tab.o) $(LEX_SRC:.l=.yy.o)


$(TARGET) : $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^

# Compile: create object files from C source files.
%.o : %.c %.d
		$(CC) -c $(CFLAGS)  $< -o $@

# Compile: create object files from Cpp source files.
%.o : %.cpp %.d
		$(CXX) -c $(CFLAGS)  $< -o $@ 


%.yy.c: %.l %.y
	flex -o$@ -i $<

%.tab.c: %.y
	bison -t -b $(TARGET) -d $<



# Target: clean project.
clean:
		-rm -f  $(TARGET) $(TARGET).exe $(OBJ) $(DEPS)
		-rm -f  $(TARGET).yy.* $(TARGET).tab.* *~


# Automatically generate C source code dependencies.
# (Code originally taken from the GNU make user manual and modified (See README.txt Credits).)
# Note that this will work with bash and sed (see the SHELL variable defined above).
# This may not work with other shells or other seds.

%.d : %.cpp
		set -e; $(CXX) -MM $(CFLAGS)  $< | \
		sed -e 's@^\(.*\)\.o:@./\1.d ./\1.o:@' > $@

%.d : %.c
		set -e; $(CC) -MM $(CFLAGS)  $< | \
		sed -e 's@^\(.*\)\.o:@./\1.d ./\1.o:@' > $@


ifeq (, $(findstring $(MAKECMDGOALS), clean ))
-include $(DEPS)
endif

# Listing of phony targets.
.PHONY : all clean



