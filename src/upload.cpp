#include <iostream>
#include <fstream>
#include <chrono>
#include <curl/curl.h>

size_t upload_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    return size * nitems;
}

int main()
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Error initializing curl" << std::endl;
        return 1;
    }

    std::string file_path = "./data/10MB.zip";
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return 1;
    }

    file.seekg(0, std::ios::end);
    long long file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[file_size];
    file.read(buffer, file_size);

    // Got server IPs from: https://gist.github.com/epixoip/2b8696ed577d584a7f484c006d945051
    // This server is located in Kitchener, ON, Canada
    curl_easy_setopt(curl, CURLOPT_URL, "http://speedtest.netflash.net/speedtest/upload.php");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, file_size);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, upload_callback);

    int num_trials = 5;
    double total_speed = 0.0;
    for (int i = 0; i < num_trials; i++)
    {
        auto start_time = std::chrono::steady_clock::now();
        CURLcode res = curl_easy_perform(curl);
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        if (res != CURLE_OK)
        {
            std::cerr << "Error uploading file: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if (response_code == 200)
            {
                // upload speed = (file size / duration) * 1000 ms / 
                double upload_speed = (file_size / (double)duration.count()) * 1000.0 / (1024.0 * 1024.0); 
                std::cout << "Trial " << i + 1 << " upload speed: " << upload_speed << " MB/s" << std::endl;
                total_speed += upload_speed;
            }
            else
            {
                std::cerr << "Error: unexpected response code " << response_code << std::endl;
            }
        }
    }

    double average_speed = total_speed / num_trials;
    std::cout << "Average upload speed: " << average_speed << " MB/s" << std::endl;

    curl_easy_cleanup(curl);
    delete[] buffer;

    return 0;
}
