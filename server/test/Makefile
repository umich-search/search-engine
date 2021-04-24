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
IDIR = ../../crawler/include
SDIR = ../../crawler/src
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

# constraint solver
CIDIR = ../../constraint_solver/include
CSDIR = ../../constraint_solver/src
CODIR = ../../constraint_solver/obj

CHEADERS = $(wildcard $(CIDIR)/*.h)
CSRC = $(wildcard $(CSDIR)/*.cpp)
COBJ = $(patsubst $(CSDIR)/%.cpp, $(CODIR)/%.o, $(CSRC))

# query compiler
QIDIR = ../../query_compiler/include
QSDIR = ../../query_compiler/src
QODIR = ../../query_compiler/obj

QHEADERS = $(wildcard $(QIDIR)/*.h)
QSRC = $(wildcard $(QSDIR)/*.cpp)
QOBJ = $(patsubst $(QSDIR)/%.cpp, $(QODIR)/%.o, $(QSRC))

# ranker
RIDIR = ../../ranker/include
RSDIR = ../../ranker/src
RODIR = ../../ranker/obj

RHEADERS = $(wildcard $(RIDIR)/*.h)
RSRC = $(wildcard $(RSDIR)/*.cpp)
ROBJ = $(patsubst $(RSDIR)/%.cpp, $(RODIR)/%.o, $(RSRC))

# server
SIDIR = ../../server/include
SSDIR = ../../server/src
SODIR = ../../server/obj

SHEADERS = $(wildcard $(SIDIR)/*.h)
SSRC = $(wildcard $(SSDIR)/*.cpp)
SOBJ = $(patsubst $(SSDIR)/%.cpp, $(SODIR)/%.o, $(SSRC))

#####################################################
# 	compile commands
#####################################################
CXX = g++
CXXFLAGS = -std=c++11 -g  -I$(UIDIR) -I$(IIDIR) -I$(CIDIR) -I$(QIDIR) -I$(RIDIR) -I$(SIDIR) -I$(IDIR)
LIBS = -lssl -lcrypto -lz -pthread
LIBCRYPTO_CFLAGS = -I/usr/local/include 
LIBCRYPTO_LIBS = -L/usr/local/lib -lcrypto
# TODO: add the name of the main source file here
EXEC = tinyCrawler

all: $(EXEC)

utilityObj: $(UOBJ)

crawlerObj: $(OBJ)

indexObj: $(IOBJ)

csObj: $(COBJ)

qcObj: $(QOBJ)

rankerObj: $(ROBJ)

serverObj: $(SOBJ)

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

# constraint solver dependencies
$(CODIR)/%.o: $(CSDIR)/%.cpp $(CHEADERS) $(UHEADERS) $(IHEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

# query compiler dependencies
$(QODIR)/%.o: $(QSDIR)/%.cpp $(QHEADERS) $(CHEADERS) $(UHEADERS) $(IHEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

# ranker dependencies
$(RODIR)/%.o: $(RSDIR)/%.cpp $(RHEADERS) $(QHEADERS) $(CHEADERS) $(UHEADERS) $(IHEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

# server dependencies
$(SODIR)/%.o: $(SSDIR)/%.cpp $(SHEADERS) $(RHEADERS) $(QHEADERS) $(CHEADERS) $(UHEADERS) $(IHEADERS) $(HEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

#############################################################################
#  compile the main function and link to all the other files
#############################################################################

$(EXEC).o: $(EXEC).cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(EXEC): $(EXEC).o $(UOBJ) $(IOBJ) $(COBJ) $(QOBJ) $(ROBJ) $(SOBJ) $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)

.PHONY: clean cleanObj utilityObj indexObj qcObj csObj rankerObj all

cleanObj:
	rm -f $(UOBJ) $(IOBJ) $(COBJ) $(QOBJ) $(ROBJ) $(SOBJ) $(OBJ)
clean:
	rm -rf $(EXEC) *.o
# cleanStorage:
# 	rm -rf frontier/* index_files/* bloomfilter