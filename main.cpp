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
    const char* url = "https://sabnzbd.org/tests/internetspeed/20MB.bin";

    // Set the number of repetitions for each data size
    const int repetitions = 2;

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
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 1024 * 1024 * 10000L); 
        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, 0L);

        // Create a vector to store the measured bandwidths
        double bandwidths[5];

        // Download the 512MB file five times
        for (int i = 0; i < 5; i++)
        {
            // Measure the time it takes to transfer the data
            double totalElapsed = 0;
            for (int j = 0; j < repetitions; j++)
            {
                // Measure the time it takes to download the data
                auto startDownload = high_resolution_clock::now();
                printf("downloading\n");
                curl_easy_perform(curl);
                printf("downloaded\n");
                auto endDownload = high_resolution_clock::now();

                // Compute the elapsed time in milliseconds
                auto elapsed = duration_cast<milliseconds>(endDownload - startDownload).count();
                printf("elapsed: %d\n", elapsed);


                totalElapsed += elapsed;
            }

            // Compute the average elapsed time in milliseconds
            double avgElapsed = totalElapsed / repetitions;

            // Compute the bandwidth in bytes per second
            double bandwidthBytesPerSec = (double)20 * 1024 * 1024 * 8 / (double)avgElapsed * 1000; // 

            // Compute the bandwidth in megabytes per second
            double bandwidthMegabytesPerSec = bandwidthBytesPerSec / 1000000;

            // Add the bandwidth to the array
            bandwidths[i] = bandwidthMegabytesPerSec;
        }

        // Compute the average and standard deviation of the measured bandwidths
        double sum = 0;
        for (double bandwidth : bandwidths)
        {
            sum += bandwidth;
        }
        double mean = sum / 5;

        double variance = 0;
        for (double bandwidth : bandwidths)
        {
            variance += pow(bandwidth - mean, 2);
        }
        variance /= 5;

        double stddev = sqrt(variance);

        // Output the results
        cout << "Network bandwidth: " << mean << " +/- " << stddev << " MB/s" << endl;

        // Cleanup libcurl
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    return 0;
}