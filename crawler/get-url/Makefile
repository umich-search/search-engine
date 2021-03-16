CXX = g++
CXXFLAGS = -std=c++11 -g
LDFLAGS = -lpthread -L/usr/local/Cellar/openssl@1.1/1.1.1i/lib -lssl -lcrypto

all: LinuxGetSSL LinuxGetUrl

LinuxGetSSL: LinuxGetSSL.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -lz -o LinuxGetSSL

LinuxGetUrl: LinuxGetUrl.cpp
	$(CXX) $(CXXFLAGS) $^ -o LinuxGetUrl

.PHONY: clean

clean:
	rm -f LinuxGetSSL LinuxGetUrl
