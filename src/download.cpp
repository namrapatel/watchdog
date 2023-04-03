/**
 * @file download.cpp
 * @brief Measures the network bandwidth by downloading a file and measuring the time it takes.
 * @details Uses libcurl to download a 20 MB file and measure the time it takes to download it.
 * The average and standard deviation of the measured bandwidths are then computed and printed to the console.
 * @author Namra Patel
 */

#include <iostream>
#include <chrono>
#include <cmath>
#include <curl/curl.h>
using namespace std;
using namespace std::chrono;

/**
 * @class Download
 * @brief A class that measures the network bandwidth by downloading a file and measuring the time it takes.
 * @details Uses libcurl to download a file and measure the time it takes to download it.
 * The average and standard deviation of the measured bandwidths are then computed and printed to the console.
 */
class Download {
public:
    /**
     * @brief Initializes the Download object.
     * @details Initializes libcurl and sets the default options.
     */
    Download() {
        // Initialize libcurl
        curl_global_init(CURL_GLOBAL_ALL);
        m_curl = curl_easy_init();

        // Set default options
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(m_curl, CURLOPT_LOW_SPEED_LIMIT, 0L);
        curl_easy_setopt(m_curl, CURLOPT_LOW_SPEED_TIME, 0L);
        curl_easy_setopt(m_curl, CURLOPT_BUFFERSIZE, 1024 * 1024 * 10000L);
        curl_easy_setopt(m_curl, CURLOPT_MAX_RECV_SPEED_LARGE, 0L);
    }

    /**
     * @brief Cleans up the Download object.
     * @details Cleans up libcurl.
     */
    ~Download() {
        curl_easy_cleanup(m_curl);
        curl_global_cleanup();
    }

    /**
     * @brief Runs the download test.
     * @details Downloads the file and measures the time it takes.
     * The bandwidths are then computed and stored in the m_bandwidths array.
     */
    void runTest() {
        // Print "Testing..."
        cout << "Testing your download speeds..." << endl;

        // Set the URL of the file to transfer
        const char* url = "https://sabnzbd.org/tests/internetspeed/20MB.bin";

        // Set the number of repetitions for each data size
        const int repetitions = 2;

        // Set the URL of the server to transfer the data to and from
        curl_easy_setopt(m_curl, CURLOPT_URL, url);

        // Create a vector to store the measured bandwidths
        double bandwidths[5];

        // Download the 20 MB file five times
        for (int i = 0; i < 5; i++)
        {
            // Measure the time it takes to transfer the data
            double totalElapsed = 0;
            for (int j = 0; j < repetitions; j++)
            {
                // Measure the time it takes to download the data
                auto startDownload = high_resolution_clock::now();
                curl_easy_perform(m_curl);
                auto endDownload = high_resolution_clock::now();

                // Compute the elapsed time in milliseconds
                auto elapsed = duration_cast<milliseconds>(endDownload - startDownload).count();
                totalElapsed += elapsed;
            }

            // Compute the average elapsed time in milliseconds
            double avgElapsed = totalElapsed / repetitions;

            // Compute the bandwidth in bytes per second
            double bandwidthBytesPerSec = (double)20 * 1024 * 1024 * 8 / (double)avgElapsed * 1000;

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
        std::cout << "Network bandwidth: " << mean << " +/- " << stddev << " MB/s" << std::endl;
    }

private:
    /**
    * @brief Callback function for libcurl to receive data from the server.
    * @details This function is called by libcurl when it receives data from the server.
    * We don't need to do anything with the data, so we just return the size.
    * @param ptr Pointer to the received data.
    * @param size Size of each element of data.
    * @param nmemb Number of elements of data.
    * @param userdata User-defined data.
    * @return The size of the received data.
    */
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        return size * nmemb;
    }

CURL* m_curl; /**< Pointer to the libcurl object. */
};

/**
 * @brief Main function.
 * @details Initializes the Download object and runs the download test.
 * @return Exit code.
 * @author Namra Patel
*/
int main()
{
    Download download;
    download.runTest();

    return 0;
}