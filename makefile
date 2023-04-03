CC=g++
CFLAGS=-I/opt/homebrew/include/
LIBS=-lcurl -lpcap

# Download
download: cdown rdown

cdown: ./src/download.cpp
	$(CC) $(CFLAGS) -o ./down $< $(LIBS)

rdown: ./down
	./down

# Upload
upload: cup rup

cup: ./src/upload.cpp
	$(CC) $(CFLAGS) -o ./up $< $(LIBS)

rup: ./up
	./up

# Ping
ping: cping rping

cping: ./src/ping.cpp
	$(CC) $(CFLAGS) -o ./ping $< $(LIBS)

rping: ./ping
	./ping

# Bandwidth
bandwidth: cband rband

cband: ./src/bandwidth_usage.cpp
	$(CC) $(CFLAGS) -std=c++11 -o ./band $< $(LIBS)

rband: ./band
	sudo ./band

# Jitter
jitter: cjitter rjitter

cjitter: ./src/jitter.cpp
	$(CC) -o ./jitter $<

rjitter: ./jitter
	./jitter google.ca

# Packet Loss
packetloss: cploss rploss

cploss: ./src/packetloss.cpp
	$(CC) -o ./packetloss $<

rploss: ./packetloss
	./packetloss google.ca
