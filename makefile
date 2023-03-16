# Download
download: 
	make cdown && make rdown

cdown:  ./src/download.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./down ./src/download.cpp

rdown: ./down
	./down

# Upload
upload:
	make cup && make rup

cup:  ./src/upload.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./up ./src/upload.cpp

rup: ./up
	./up

# Ping
make ping:
	make cping && make rping

cping:  ./src/ping.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./ping ./src/ping.cpp

rping: ./ping
	./ping

# Bandwidth
bandwidth:
	make cband && make rband

cband:  ./src/bandwidth_usage.cpp
	g++ -I/opt/homebrew/include/ -lcurl -lpcap -o ./band ./src/bandwidth_usage.cpp

rband: ./band
	sudo ./band