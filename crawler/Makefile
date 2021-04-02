# path to the header files
IDIR = ../utility/include

# compile options
CXX = g++
CC = gcc
CXXFLAGS = -std=c++11 -g

# path to the object files and source files
ODIR = ../utility/src/obj

# library want to include
LIBS = -lz -lssl -lcrypto -pthread

# transform header files into absolute path
_DEPS = BloomFilter.h Common.h Concurrency.h DiskQueue.h HashBlob.h HashTable.h PriorityQueue.h SmartPointer.h string.h Thread.h Timer.h Vector.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# transform source files into absolute path
_OBJ = Common.o DiskQueue.o string.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# make all the object files
all: $(OBJ)

# create the object files
# depend on header files in case later changes in header files
$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c $() $(CXXFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o





CXX = g++
CXXFLAGS = -std=c++11 -g

LDFLAGS =  -lssl -lcrypto

all: CrawlerManager parseRobots

CrawlerManager: CrawlerManager.cpp
	$(CXX) $(CXXFLAGS) $^ -o CrawlerManager

parseRobots: parseRobots.cpp get-url/GetUrl.cpp ../utility/string.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -lz -o parseRobots

.PHONY: clean

clean:
	rm -f CrawlerManager parseRobots