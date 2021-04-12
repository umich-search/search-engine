# To make the executable file, change the exec file name(under "TODO") and run "make all".
# This command first build the object file for the target and then link it to all object files
# we have(including the utility). It re-compiles all necessary external object files when their
# headers are changed.
# 
##############################################
# 	basic information of the library
##############################################
# utility
UIDIR = ../../utility/include
USDIR = ../../utility/src
UODIR = $(USDIR)/obj

UHEADERS = $(wildcard $(UIDIR)/*.h)
USRC = $(wildcard $(USDIR)/*.cpp)
UOBJ = $(patsubst $(USDIR)/%.cpp, $(UODIR)/%.o, $(USRC))

# crawler
IDIR = ../include
SDIR = ../src
ODIR = $(SDIR)/obj

HEADERS = $(wildcard $(IDIR)/*.h)
_SRC_ = $(wildcard $(SDIR)/*.cpp)
_SRC = $(filter-out $(SDIR)/FrontierOpt.cpp,$(_SRC_))
SRC = $(filter-out $(SDIR)/HtmlParserDemo.cpp,$(_SRC))
OBJ = $(patsubst $(SDIR)/%.cpp,$(ODIR)/%.o,$(SRC))

# index
IIDIR = ../../index/include
ISDIR = ../../index/src
IODIR = ../../index/obj

IHEADERS = $(wildcard $(IIDIR)/*.h)
ISRC = $(wildcard $(ISDIR)/*.cpp)
IOBJ = $(patsubst $(ISDIR)/%.cpp, $(IODIR)/%.o, $(ISRC))

#####################################################
# 	compile commands
#####################################################
CXX = g++
CXXFLAGS = -std=c++11 -g -I$(IDIR) -I$(UIDIR) -I$(IIDIR)
LIBS = -lssl -lcrypto -lz -pthread
LIBCRYPTO_CFLAGS = -I/usr/local/include 
LIBCRYPTO_LIBS = -L/usr/local/lib -lcrypto
# TODO: add the name of the main source file here
EXEC = managerTestM1

all: $(EXEC)

utilityObj: $(UOBJ)

crawlerObj: $(OBJ)

indexObj: $(IOBJ)

#############################################################################
# update dependencies if any of the lib header or source file change
#############################################################################

# utility source files depend on utilities' headers
$(UODIR)/%.o: $(USDIR)/%.cpp $(UHEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

# crawler source files depend on both its and utilities' headers
$(ODIR)/%.o: $(SDIR)/%.cpp $(UHEADERS) $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(IODIR)/%.o: $(ISDIR)/%.cpp $(UHEADERS) $(IHEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

#############################################################################
#  compile the main function and link to all the other files
#############################################################################

$(EXEC).o: $(EXEC).cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(EXEC): $(EXEC).o $(OBJ) $(UOBJ) $(IOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean cleanObj utilityObj crawlerObj all

cleanObj:
	rm -f $(OBJ) $(UOBJ)
clean:
	rm -rf $(EXEC) *.o
cleanStorage:
	rm -rf frontier/* index_files/* bloomfilter