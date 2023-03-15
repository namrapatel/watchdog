cdown:  ./src/download.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./down ./src/download.cpp

rdown: ./down
	./down

cup:  ./src/upload.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./up ./src/upload.cpp

rup: ./up
	./up

cping:  ./src/ping.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./ping ./src/ping.cpp

rping: ./ping
	./ping

cband:  ./src/bandwidth_usage.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./band ./src/bandwidth_usage.cpp

rband: ./band
	./band