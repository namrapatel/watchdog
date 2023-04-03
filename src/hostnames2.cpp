#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

int main()
{
    vector<string> hostnames;
    vector<string> ips;

    // Get default gateway IP
    cout << "Getting default gateway IP..." << endl;
    string gateway = "";
    FILE* pipe = popen("ip r | grep default | cut -d ' ' -f 3", "r");
    if (!pipe) {
        cout << "Error: Could not execute command." << endl;
        return 1;
    }
    char buffer[128];
    fgets(buffer, 128, pipe);
    pclose(pipe);
    if (strlen(buffer) > 0) {
        buffer[strlen(buffer) - 1] = '\0';
        gateway = buffer;
        cout << "Default gateway IP: " << gateway << endl;
    } else {
        cout << "Error: Could not determine default gateway." << endl;
        return 1;
    }

    // Scan network for active devices
    cout << "Scanning network for active devices..." << endl;
    regex ip_regex("\\b(?:\\d{1,3}\\.){3}\\d{1,3}\\b");
    smatch match;
    for (int i = 1; i <= 254; i++) {
        string ip = gateway.substr(0, gateway.rfind(".") + 1) + to_string(i);
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            continue;
        }
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(80);
        inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);
        // Set timeout to 500ms
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        int result = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (result == 0) {
            // Device is active
            struct hostent* host_info = gethostbyaddr(&(serv_addr.sin_addr), sizeof(struct in_addr), AF_INET);
            if (host_info != NULL) {
                hostnames.push_back(host_info->h_name);
            } else {
                hostnames.push_back(ip);
            }
            ips.push_back(ip);
        }
        close(sockfd);
    }

    // Display results
    cout << "Active Devices:" << endl;
    if (ips.empty()) {
        cout << "No active devices found." << endl;
    } else {
        for (int i = 0; i < ips.size(); i++) {
            cout << hostnames[i] << " (" << ips[i] << ")" << endl;
        }
    }

    return 0;
}