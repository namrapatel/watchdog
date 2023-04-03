CXX = g++
CXXFLAGS = -I/opt/homebrew/include/
LDLIBS = -lcurl -lpcap

all: 
	make test && make run

test: src/main.cpp
	g++ -I/opt/homebrew/include/ src/main.cpp -lcurl -lpcap -o main

run: main
	./main

.PHONY: clean
clean:
	rm -f main down up ping band jitter packetloss
