/**
 * @file main.cpp
 * @brief This file represents a program that calculates the packet loss for a given host by sending ping requests and analyzing the output.
 *
 * @author Ryan O'Callaghan <rocalla@uwo.ca>
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <regex>

using namespace std;

/**
 * @class PacketLoss
 * @brief This class represents a packet loss object that retrieves packet loss information for a specified host.
 *
 * @author Ryan O'Callaghan <rocalla@uwo.ca>
 */
class PacketLoss {
private:
    string host_; ///< The host to send ping requests to
    int count_; ///< The number of ping requests to send

public:
    /**
     * @brief Constructor that initializes the PacketLoss object.
     * @param host The hostname to send ping requests to
     * @param count The number of ping requests to send (default: 10)
     */
    PacketLoss(const string& host, int count = 10) {
    	host_ = host;
    	count_ = count;
    }

    /**
     * @brief Function that sends ping requests to the host and calculates the packet loss.
     * @return The packet loss as a percentage, or -1 if the ping command failed to execute.
     */
    int getPacketLoss() {
        // Construct the ping command to retrieve packet loss.
        string cmd = "ping -c " + to_string(count_) + " " + host_;

        // Print some information about the test
        cout << "Sending " << count_ << " pings to " << host_ << "..." << endl;

        // Open a pipe to execute the ping command and capture its output
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            cerr << "Failed to execute command: " << cmd << endl;
            return -1;
        }

        // Read the output of the ping command line by line and store it in a string
        char buffer[128];
        string result = "";
        while (fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
            cout << buffer;
        }

        // Close the pipe and extract the packet loss information
        pclose(pipe);
        return calculatePacketLoss(result);
    }

private:
    /**
     * @brief Function that parses the output of the ping command and calculates the packet loss.
     * @param output The output of the ping command
     * @return The packet loss as a percentage
     */
    int calculatePacketLoss(const string& output) {
        int sent = 0, received = 0;
        
        // Regular expression pattern to match the output of the ping command on linux (i.e. An Arduino)
        regex pattern("(\\d+) packets transmitted, (\\d+) received, (\\d+)% packet loss");
        smatch matches;
        
        if (regex_search(output, matches, pattern)) {
            // Extract the values for sent and received packets
            sent = stoi(matches[1]);
            received = stoi(matches[2]);
        }
        
        // Calculate packet loss as a percentage
        return (sent == 0) ? 0 : (sent - received) * 100 / sent;
    }
};

int main(int argc, char* argv[]) {
    // Check that a hostname argument was provided
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <hostname>" << endl;
        return 1;
    }

    // Create a PacketLoss object for the specified host
    string host = argv[1];
    PacketLoss pl(host);

    // Calculate the packet loss and print the result
    int packetloss = pl.getPacketLoss();
    if (packetloss < 0) {
        cerr << "Failed to get packet loss information." << endl;
        return 1;
    }
    cout << "Packet loss: " << packetloss << "%" << endl;

    return 0;
}
