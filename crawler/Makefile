CXX = g++
CXXFLAGS = -std=c++11 -g

all: CrawlerManager

CrawlerManager: CrawlerManager.cpp
	$(CXX) $(CXXFLAGS) $^ -o CrawlerManager

.PHONY: clean

clean:
	rm -f CrawlerManager