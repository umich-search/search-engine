# To make the executable file, change the exec file name(under "TODO") and run "make all".
# This command first build the object file for the target and then link it to all object files
# we have(including the utility). It re-compiles all necessary external object files when their
# headers are changed.
# 
##############################################
# 	basic information of the library
##############################################
# utility
UIDIR = ../include
USDIR = ../src
UODIR = $(USDIR)/obj

UHEADERS = $(wildcard $(UIDIR)/*.h)
USRC = $(wildcard $(USDIR)/*.cpp)
UOBJ = $(patsubst $(USDIR)/%.cpp, $(UODIR)/%.o, $(USRC))

#####################################################
# 	compile commands
#####################################################
CXX = g++
CXXFLAGS = -std=c++11 -g -I$(UIDIR)
LIBS = -lssl -lcrypto -lz -pthread
LIBCRYPTO_CFLAGS = -I/usr/local/include 
LIBCRYPTO_LIBS = -L/usr/local/lib -lcrypto
# TODO: add the name of the main source file here
EXEC = testBloomFilter

all: $(EXEC)

utilityObj: $(UOBJ)

#############################################################################
# update dependencies if any of the lib header or source file change
#############################################################################

# utility source files depend on utilities' headers
$(UODIR)/%.o: $(USDIR)/%.cpp $(UHEADERS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

#############################################################################
#  compile the main function and link to all the other files
#############################################################################

$(EXEC).o: $(EXEC).cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(EXEC): $(EXEC).o $(UOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean cleanObj utilityObj all

cleanObj:
	rm -f $(UOBJ)
clean:
	rm -rf $(EXEC) $(EXEC).o

