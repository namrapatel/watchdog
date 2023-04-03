#include <iostream>
#include <cstdlib>

using namespace std;

void download();
void upload();
void ping();
void bandwidth();
void jitter();
void packetloss();
void clean();

int main() {
    int choice;
    
    do {
        cout << "Please choose an option:" << endl;
        cout << "1. Download" << endl;
        cout << "2. Upload" << endl;
        cout << "3. Ping" << endl;
        cout << "4. Bandwidth" << endl;
        cout << "5. Jitter" << endl;
        cout << "6. Packet Loss" << endl;
        cout << "0. Exit" << endl;
        cout << "Your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                download();
                break;
            case 2:
                upload();
                break;
            case 3:
                ping();
                break;
            case 4:
                bandwidth();
                break;
            case 5:
                jitter();
                break;
            case 6:
                packetloss();
                break;
            case 0:
                clean();
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
        
        cout << endl;
    } while (choice != 0);
    
    return 0;
}

void download() {
    system("g++ -std=c++11 -I/opt/homebrew/include/ -o ./down ./src/download.cpp -lcurl -lpcap");
    system("./down");
}

void upload() {
    system("g++ -I/opt/homebrew/include/ -o ./up ./src/upload.cpp -lcurl -lpcap");
    system("./up");
}

void ping() {
    system("g++ -I/opt/homebrew/include/ -o ./ping ./src/ping.cpp -lcurl -lpcap");
    system("./ping");
}

void bandwidth() {
    system("g++ -Wno-deprecated-declarations -I/opt/homebrew/include/ -std=c++11 -o ./band ./src/bandwidth_usage.cpp -lcurl -lpcap");
    system("sudo ./band");
}

void jitter() {
    system("g++ -o ./jitter ./src/jitter.cpp");
    system("./jitter google.ca");
}

void packetloss() {
    system("g++ -o ./packetloss ./src/packetloss.cpp");
    system("./packetloss google.ca");
}

void clean() {
    system("make clean");
}