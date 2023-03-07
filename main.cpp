#include <iostream>
#include <chrono>
#include <thread>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#include <iphlpapi.h>
#include <assert.h>
#pragma comment(lib, "iphlpapi.lib")
#else
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

using namespace std;
using namespace std::chrono;

int main()
{
    // Set the size of the data to send
    const int dataSize = 1024 * 1024 * 10; // 10 MB

    // Create a buffer of the specified size
    char* buffer = new char[dataSize];

    // Measure the time it takes to send the data
    auto start = high_resolution_clock::now();
    // Send the data to a dummy IP address (in this case, localhost)
    for (int i = 0; i < dataSize; i++)
    {
        buffer[i] = 'a';
    }
    //Sleep for 10 seconds to give some time to measure the bandwidth
    this_thread::sleep_for(chrono::seconds(10));
    auto end = high_resolution_clock::now();

    // Compute the elapsed time in milliseconds
    auto elapsed = duration_cast<milliseconds>(end - start).count();

    // Compute the bandwidth in bytes per second
    double bandwidthBytesPerSec = (double)dataSize * 8 / (double)elapsed * 1000;

    // Compute the bandwidth in megabytes per second
    double bandwidthMegabytesPerSec = bandwidthBytesPerSec / 1000000;

    // Output the result
    cout << "Network bandwidth: " << bandwidthMegabytesPerSec << " MB/s" << endl;

    // Free the buffer
    delete[] buffer;

    return 0;
}
