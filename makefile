# compile with make: g++ -I/opt/homebrew/include/ -o ./out main.cpp
compile:  main.cpp
	g++ -I/opt/homebrew/include/ -lcurl -o ./out upload.cpp

# run with make: ./out
run: ./out
	./out