# Linux Makefile

CXX := g++
CXXFLAGS := -g -c -std=c++11 -pthread -Iinclude
LDFLAGS := -pthread -lssl -lcrypto

TARGETS := SSLClient.out ForkSSLServer.out ThreadSSLServer.out

all: $(TARGETS)

Socket.o: src/Socket.cc include/Socket.h include/VSocket.h
	$(CXX) $(CXXFLAGS) src/Socket.cc -o Socket.o

VSocket.o: src/VSocket.cc include/VSocket.h
	$(CXX) $(CXXFLAGS) src/VSocket.cc -o VSocket.o

SSLSocket.o: src/SSLSocket.cpp include/SSLSocket.h include/Socket.h include/VSocket.h
	$(CXX) $(CXXFLAGS) src/SSLSocket.cpp -o SSLSocket.o

SSLClient.o: src/SSLClient.cc include/SSLSocket.h
	$(CXX) $(CXXFLAGS) src/SSLClient.cc -o SSLClient.o

SSLClient.out: SSLClient.o SSLSocket.o Socket.o VSocket.o
	$(CXX) SSLClient.o SSLSocket.o Socket.o VSocket.o -o SSLClient.out $(LDFLAGS)

ForkSSLServer.o: src/ForkSSLServer.cc include/SSLSocket.h
	$(CXX) $(CXXFLAGS) src/ForkSSLServer.cc -o ForkSSLServer.o

ForkSSLServer.out: ForkSSLServer.o SSLSocket.o Socket.o VSocket.o
	$(CXX) ForkSSLServer.o SSLSocket.o Socket.o VSocket.o -o ForkSSLServer.out $(LDFLAGS)

ThreadSSLServer.o: src/ThreadSSLServer.cc include/SSLSocket.h
	$(CXX) $(CXXFLAGS) src/ThreadSSLServer.cc -o ThreadSSLServer.o

ThreadSSLServer.out: ThreadSSLServer.o SSLSocket.o Socket.o VSocket.o
	$(CXX) ThreadSSLServer.o SSLSocket.o Socket.o VSocket.o -o ThreadSSLServer.out $(LDFLAGS)

clean:
	rm -f *.o *.out