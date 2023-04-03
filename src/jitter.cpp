#include <iostream>
#include <cstring>
#include <cstdio>

using namespace std;

/**
 * @brief Class to measure network jitter for a given website using the ping command.
 * 
 * This class provides a method to run a ping command to a website and retrieve the RTT (round-trip-time) 
 * and the network jitter. The network jitter is calculated by taking the average of the 
 * round-trip-time for 10 packets sent to the website.
 * 
 * @author Ryan O'Callaghan <rocalla@uwo.ca>
 */
class NetworkJitter {
private:
    string m_website;
public:
    /**
     * @brief Constructor for NetworkJitter class.
     * 
     * Initializes the m_website member variable with the given website.
     * 
     * @param website The website to ping.
     */
    NetworkJitter(const string& website) {
    	m_website = website;
    }

    /**
     * @brief Runs the ping command to the website and retrieves the network jitter.
     * 
     * This method builds the ping command with the given argument. The command will ping the website, 
     * and use grep + awk to retrieve the RTT (round-trip-time) from the output. The second value 
     * represents the jitter. The method then uses popen to execute the command and retrieves the response.
     * 
     * @param response The output of the ping command.
     * 
     * @return true if the command was successfully executed and a response was retrieved, false otherwise.
     */
    bool runPingCommand(string& response) const {
        string command = "ping -q -c 10 " + m_website + " | grep 'rtt' | awk -F '/' '{print $5}'";
        char buffer[128];
        response = "";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            cerr << "Failed to execute command." << endl;
            return false;
        }
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            response += buffer;
        }
        pclose(pipe);

        if (response.empty()) {
            cerr << "Failed to get response." << endl;
            return false;
        }

        response.erase(response.find_last_not_of("\n\r") + 1);
        return true;
    }

};

/**
 * @brief Main function to run the network jitter measurement.
 * 
 * The main function takes a command-line argument for the website to ping. Since we are looking for general
 * network jitter, we are pinging a website that is reliable (google.ca). It then initializes 
 * a NetworkJitter object with the given website and runs the ping command. If the command was 
 * successfully executed, the network jitter is printed.
 * 
 * @param argc The number of arguments passed to the program.
 * @param argv An array of the arguments passed to the program.
 * 
 * @return 0 if the program ran successfully, 1 otherwise.
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " website" << endl;
        return 1;
    }

    NetworkJitter networkJitter(argv[1]);

    string response;
    if (networkJitter.runPingCommand(response)) {
        cout << "Network Jitter: " << response << " ms." << endl;
    }

    return 0;
}

