#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <cstdlib>
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
    // Set the size of the data to transfer
    const int dataSize = 1024 * 1024 * 10; // 10 MB

    // Generate a random filename for the data to transfer
    string filename = "transfer-" + to_string(rand()) + ".bin";

    // Create a buffer of the specified size
    char* buffer = new char[dataSize];

    // Write random data to the buffer
    srand(time(NULL));
    for (int i = 0; i < dataSize; i++)
    {
        buffer[i] = rand() % 256;
    }

    // Write the buffer to a file
    FILE* file = fopen(filename.c_str(), "wb");
    fwrite(buffer, 1, dataSize, file);
    fclose(file);

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    if (curl)
    {
        // Set the URL of the server to transfer the data to
        curl_easy_setopt(curl, CURLOPT_URL, "http://speedtest.tele2.net/upload.php");

        // Set the write callback function to receive data from the server
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Set the file to transfer
        curl_easy_setopt(curl, CURLOPT_READDATA, filename.c_str());

        // Set the size of the file to transfer
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)dataSize);

        // Measure the time it takes to transfer the data (upload)
        auto startUpload = high_resolution_clock::now();
        curl_easy_perform(curl);
        auto endUpload = high_resolution_clock::now();

        // Compute the elapsed time in milliseconds (upload)
        auto elapsedUpload = duration_cast<milliseconds>(endUpload - startUpload).count();

        // Compute the bandwidth in bytes per second (upload)
        double bandwidthBytesPerSecUpload = (double)dataSize * 8 / (double)elapsedUpload * 1000;

        // Compute the bandwidth in megabytes per second (upload)
        double bandwidthMegabytesPerSecUpload = bandwidthBytesPerSecUpload / 1000000;

        // Set the URL of the server to transfer the data from
        curl_easy_setopt(curl, CURLOPT_URL, "http://speedtest.tele2.net/10MB.zip");

        // Reset the write callback function to receive data from the server
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Measure the time it takes to transfer the data (download)
        auto startDownload = high_resolution_clock::now();
        curl_easy_perform(curl);
        auto endDownload = high_resolution_clock::now();

        // Compute the elapsed time in milliseconds (download)
        auto elapsedDownload = duration_cast<milliseconds>(endDownload - startDownload).count();

        // Compute the bandwidth in bytes per second (download)
        double bandwidthBytesPerSecDownload = (double)dataSize * 8 / (double)elapsedDownload * 1000;

           // Compute the bandwidth in megabytes per second (download)
        double bandwidthMegabytesPerSecDownload = bandwidthBytesPerSecDownload / 1000000;

        // Output the results
        cout << "Network upload bandwidth: " << bandwidthMegabytesPerSecUpload << " MB/s" << endl;
        cout << "Network download bandwidth: " << bandwidthMegabytesPerSecDownload << " MB/s" << endl;

        // Remove the transferred file
        remove(filename.c_str());

        // Cleanup libcurl
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    // Free the buffer
    delete[] buffer;

    return 0;
}