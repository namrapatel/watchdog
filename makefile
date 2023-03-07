compile down:  download.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./down download.cpp

run down: ./down
	./down

compile down:  upload.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./up upload.cpp

run down: ./up
	./up