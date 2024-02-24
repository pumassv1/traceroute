CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2

all: traceroute

traceroute: traceroute.o send.o receive.o
	$(CXX) $(CXXFLAGS) -o traceroute traceroute.o send.o receive.o

traceroute.o: traceroute.cpp traceroute.h

send.o: send.cpp traceroute.h

receive.o: receive.cpp traceroute.h

clean:
	rm -f *.o

distclean:
	rm -f traceroute *.o