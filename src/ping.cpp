#include <iostream>
#include <cstdlib>
#include <chrono>
#include <string>
#include <regex>

int main()
{
    std::string host;
    std::cout << "Enter the host to ping: ";
    std::cin >> host;

    std::cout << "Pinging " << host << "..." << std::endl;

    int num_trials = 5;
    double total_latency = 0.0;
    for (int i = 0; i < num_trials; i++)
    {
        std::string command = "ping -c 1 " + host;

        auto start_time = std::chrono::steady_clock::now();

        // Execute the ping command and capture the output
        std::string output = "";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe)
        {
            std::cerr << "Error executing ping command" << std::endl;
            return 1;
        }

        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            output += buffer;
        }

        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        double latency = duration.count();
        std::cout << "Trial " << i + 1 << ": Ping=" << latency << "ms Host=" << host << std::endl;

        total_latency += latency;
    }

    double average_latency = total_latency / num_trials;
    std::cout << "Average ping: " << average_latency << " ms" << std::endl;

    return 0;
}
