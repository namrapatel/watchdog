#include <iostream>
#include <chrono>
#include <cmath>
#include <curl/curl.h>

using namespace std;
using namespace std::chrono;

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    // This function is called by libcurl when it receives data from the server
    // We don't need to do anything with the data, so we just return the size
    return size * nmemb;
}

int main()
{
    // Set the URL of the file to transfer
    const char* url = "http://ipv4.download.thinkbroadband.com/512MB.zip";

    // Set the minimum and maximum data sizes to transfer
    const int minDataSize = 1024 * 1024; // 1 MB
    const int maxDataSize = 1024 * 1024 * 512; // 512 MB

    // Set the number of repetitions for each data size
    const int repetitions = 10;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    if (curl)
    {
        // Set the URL of the server to transfer the data to and from
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the write callback function to receive data from the server
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Disable libcurl's built-in speed limits to allow maximum bandwidth usage
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 0L);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 0L);

        // Set the buffer size and the CURL buffer size to handle large data sizes
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 1024 * 1024 * 10L);
        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, 0L); 

        // Create a vector to store the measured bandwidths
        double bandwidths[30];

        // Iterate over the different data sizes
        for (int i = 0, dataSize = minDataSize; dataSize <= maxDataSize; dataSize *= 2, i++)
        {
            // Create a buffer of the specified size
            char* buffer = new char[dataSize];

            // Measure the time it takes to transfer the data
            double totalElapsed = 0;
            for (int j = 0; j < repetitions; j++)
            {
                // Set the size of the file to transfer
                string range = "0-" + to_string(dataSize - 1);
                curl_easy_setopt(curl, CURLOPT_RANGE, range.c_str());

                // Measure the time it takes to download the data
                auto startDownload = high_resolution_clock::now();
                printf("Downloading\n");
                curl_easy_perform(curl); 
                printf("Downloaded\n");
                auto endDownload = high_resolution_clock::now();

                // Compute the elapsed time in milliseconds
                auto elapsed = duration_cast<milliseconds>(endDownload - startDownload).count();

                totalElapsed += elapsed;
            }

            // Compute the average elapsed time in milliseconds
            double avgElapsed = totalElapsed / repetitions;

            // Compute the bandwidth in bytes per second
            double bandwidthBytesPerSec = (double)dataSize * 8 / (double)avgElapsed * 1000;

            // Compute the bandwidth in megabytes per second
            double bandwidthMegabytesPerSec = bandwidthBytesPerSec / 1000000;

            // Add the bandwidth to the array
            bandwidths[i] = bandwidthMegabytesPerSec;

            // Free
        // Free the buffer
        delete[] buffer;
    }

    // Compute the average and standard deviation of the measured bandwidths
    double sum = 0;
    for (double bandwidth : bandwidths)
    {
        sum += bandwidth;
    }
    double mean = sum / 30;

    double variance = 0;
    for (double bandwidth : bandwidths)
    {
        variance += pow(bandwidth - mean, 2);
    }
    variance /= 30;

    double stddev = sqrt(variance);

    // Output the results
    cout << "Network bandwidth: " << mean << " +/- " << stddev << " MB/s" << endl;

    // Cleanup libcurl
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

return 0;
}