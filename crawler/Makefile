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
	rm -f CrawlerManager