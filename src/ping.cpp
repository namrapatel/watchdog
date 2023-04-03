#include <iostream>
#include <cstdlib>
#include <chrono>
#include <string>
#include <regex>

/**
 * @file ping.spp
 * @brief The Ping class encapsulates the functionality for pinging a host and computing the average latency.
 * @author Namra Patel
 */

/**
 * @class Ping
 * @brief The Ping class encapsulates the functionality for pinging a host and computing the average latency.
 * 
 * @author Namra Patel
 */
class Ping {
public:
    /**
     * @brief Construct a new Ping object with the specified host and number of trials.
     * 
     * @param host The host to ping.
     * @param num_trials The number of times to ping the host.
     */
    Ping(const std::string& host, int num_trials);

    /**
     * @brief Execute the ping command and capture the output.
     * 
     * @return The output from the ping command.
     */
    std::string ping();

    /**
     * @brief Compute the average latency for the pings.
     * 
     * @return The average latency in milliseconds.
     */
    double average_latency() const;

private:
    std::string host_;      ///< The host to ping.
    int num_trials_;        ///< The number of times to ping the host.
    double total_latency_;  ///< The total latency for all pings.
};

/**
 * @brief Construct a new Ping object with the specified host and number of trials.
 * 
 * @param host The host to ping.
 * @param num_trials The number of times to ping the host.
 */
Ping::Ping(const std::string& host, int num_trials)
    : host_(host), num_trials_(num_trials), total_latency_(0.0)
{
}

/**
 * @brief Execute the ping command and capture the output.
 * 
 * @return The output from the ping command.
 */
std::string Ping::ping()
{
    std::string command = "ping -c 1 " + host_;

    auto start_time = std::chrono::steady_clock::now();

    // Execute the ping command and capture the output
    std::string output = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Error executing ping command" << std::endl;
        return "";
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        output += buffer;
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    double latency = duration.count();
    std::cout << "Ping=" << latency << "ms Host=" << host_ << std::endl;

    total_latency_ += latency;

    return output;
}

/**
 * @brief Compute the average latency for the pings.
 * 
 * @return The average latency in milliseconds.
 */
double Ping::average_latency() const
{
    return total_latency_ / num_trials_;
}

/**
 * @brief The main function prompts the user for a host to ping, pings the host multiple times, and
 *        computes the average latency for the pings.
 * 
 * @return 0 if the program executes successfully, non-zero otherwise.
 * 
 * @author Namra Patel
 */
int main()
{
    std::string host;
    std::cout << "Enter the host to ping: ";
    std::cin >> host;

    // Validate the host name using regular expressions
    std::regex pattern("^[a-zA-Z0-9]+([\\-\\.]{1}[a-zA-Z0-9]+)*\\.[a-zA-Z]{2,}$");
    if (!std::regex_match(host, pattern))
    {
        std::cerr << "Invalid host name" << std::endl;
        return 1;
    }

    std::cout << "Pinging " << host << "..." << std::endl;

    int num_trials = 5;
    Ping p(host, num_trials);
    for (int i = 0; i < num_trials; i++)
    {
        p.ping();
    }

    std::cout << "Average ping: " << p.average_latency() << " ms" << std::endl;

    return 0;
}