#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char* argv[])
{
    // Standard checks to make sure proper arguments are provided.
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " website" << endl;
        return 1;
    }

    // Build the ping command with the given argument.
    // The command will ping the website, and use grep + awk to retreieve the RTT (round-trip-time)
    // from the output. The second value represents the jitter.
    string command = "ping -q -c 10 " + string(argv[1]) + " | grep 'rtt' | awk -F '/' '{print $5}'";
    cout << "Running command: " << command << endl;

    // Run the ping command using popen and get the response
    char buffer[128];
    string response = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cerr << "Failed to execute command." << endl;
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        response += buffer;
    }
    pclose(pipe);

    // Print the network jitter as long as a response was given.
    if (response.empty()) {
        cerr << "Failed to get response." << endl;
        return 1;
    }
    response.erase(response.find_last_not_of("\n\r") + 1);
    cout << "Network Jitter: " << response << " ms." << endl;

    return 0;
}

