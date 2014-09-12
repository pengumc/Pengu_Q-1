#author: Michiel van der Coelen
#date 2014-03

#take .cpp from src, compile to .o in root, then link to dll
#-fPIC was removed, since it's default (for my compiler
# and spams warnings under -Wall

NAME = quadruped
TESTNAME = Q1test
CFLAGS = -std=c++03 -Isrc -Wall -Wconversion -I. -ggdb
LIBNAME = $(NAME).dll
EXT = .exe
TARGETDIR = bin
CLIBS = -L$(TARGETDIR) -lhidapi -lrose_gait_generator -static-libgcc -static-libstdc++
CLIBS_TEST = $(CLIBS) -l$(NAME)
ifeq ($(shell uname), Linux)
	CFLAGS = -std=c++03 -Isrc -Wall -Wconversion -I. -ggdb -fPIC
	LIBNAME = lib$(NAME).so
	EXT = 
	CLIBS = -L$(TARGETDIR) -lhidapi-libusb -lusb-1.0 -lrose_gait_generator
	CLIBS_TEST = $(CLIBS) -l$(NAME)
endif
CC = gcc
CPPC = g++
OBJECTS = utility.o log.o hmatrix.o pivot.o leg.o wrapper.o invkinematic.o usbcom.o
OBJECTS_DLL = $(OBJECTS) $(NAME).o
OBJECTS_TEST = $(TESTNAME).o
OBJECTS_ALL = $(OBJECTS_DLL) $(OBJECTS_TEST)


.PHONY:all library doc stylecheck everything

#build library
all:library

everything:library stylecheck doc

%.o:src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:src/%.cpp
	$(CPPC) $(CFLAGS) -c $< -o $@

library:lib/$(LIBNAME)

lib/$(LIBNAME):$(OBJECTS_DLL)
	$(CPPC) -shared -Wall -o $(TARGETDIR)/$(LIBNAME) $(OBJECTS_DLL) $(CLIBS)

doc:
	doxygen

stylecheck:
	python cpplint.py --filter=-legal,-build/include_what_you_use,-runtime/arrays src/*.cpp $(filter-out include/hidapi.h, $(wildcard include/*.h))

clean:
	rm $(OBJECTS_ALL)

test:library $(TARGETDIR)/$(TESTNAME)$(EXT)

$(TARGETDIR)/$(TESTNAME)$(EXT):$(OBJECTS_TEST)
	$(CPPC) -Wall -o $@ $(OBJECTS_TEST) $(CLIBS_TEST)
