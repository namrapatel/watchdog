#include <iostream>
#include <cstdlib>
#include <chrono>
#include <string>

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

        // Execute the ping command and discard the output
        std::system(command.c_str());

        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        double latency = duration.count();
        std::cout << "Trial " << i + 1 << " latency: " << latency << " ms" << std::endl;
        total_latency += latency;
    }

    double average_latency = total_latency / num_trials;
    std::cout << "Average latency: " << average_latency << " ms" << std::endl;

    return 0;
}