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