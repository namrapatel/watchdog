#include <iostream>
#include <cstdlib>
#include <string>
#include <regex>

using namespace std;

// Function to parse the output of the ping command and calculate packet loss
int getPacketLoss(const string& output)
{
    int sent = 0, received = 0;
    // Regular expression pattern to match the output of the ping command
    regex pattern("(\\d+) packets transmitted, (\\d+) received, (\\d+)% packet loss");
    smatch matches;
    if (regex_search(output, matches, pattern))
    {
        // Extract the values for sent and received packets
        sent = stoi(matches[1]);
        received = stoi(matches[2]);
    }
    // Calculate packet loss as a percentage
    return (sent == 0) ? 0 : (sent - received) * 100 / sent;
}

int main(int argc, char* argv[])
{
    // Check that a hostname argument was provided
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <hostname>" << endl;
        return 1;
    }

    // Get the hostname argument and set the number of packets to send
    string host = argv[1];
    int count = 10;

    // Construct the ping command
    string cmd = "ping -c " + to_string(count) + " " + host;

    // Print some information about the test
    cout << "Sending " << count << " pings to " << host << "..." << endl;

    // Open a pipe to execute the ping command and capture its output
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        cerr << "Failed to execute command: " << cmd << endl;
        return 1;
    }

    // Read the output of the ping command line by line and store it in a string
    char buffer[128];
    string result = "";
    while (fgets(buffer, 128, pipe) != NULL)
    {
        result += buffer;
        cout << buffer;
    }

    // Close the pipe and extract the packet loss information
    pclose(pipe);
    int packetloss = getPacketLoss(result);

    // Print the packet loss percentage
    cout << "Packet loss: " << packetloss << "%" << endl;

    return 0;
}


