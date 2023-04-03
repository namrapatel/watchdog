# Download
download: 
	make cdown && make rdown

cdown:  ./src/download.cpp
	g++ -I/opt/homebrew/include/ -o ./down ./src/download.cpp -lcurl

rdown: ./down
	./down

# Upload
upload:
	make cup && make rup

cup:  ./src/upload.cpp
	g++ -I/opt/homebrew/include/ -o ./up ./src/upload.cpp -lcurl

rup: ./up
	./up

# Ping
make ping:
	make cping && make rping

cping:  ./src/ping.cpp
	g++ -I/opt/homebrew/include/ -o ./ping ./src/ping.cpp -lcurl

rping: ./ping
	./ping

# Bandwidth
bandwidth:
	make cband && make rband

cband:  ./src/bandwidth_usage.cpp
	g++ -I/opt/homebrew/include/ -std=c++11 -o ./band ./src/bandwidth_usage.cpp -lpcap -lcurl

rband: ./band
	sudo ./band
	
# Jitter
jitter: 
	make cjitter && make rjitter
cjitter:  ./src/jitter.cpp
	g++ -o ./jitter ./src/jitter.cpp
rjitter: ./jitter
	./jitter google.ca
	
# Packet Loss
packetloss:
	make cploss && make rploss
cploss: ./src/packetloss.cpp
	g++ -o ./packetloss ./src/packetloss.cpp
rploss: ./packetloss
	./packetloss google.ca
