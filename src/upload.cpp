/**
 * @file upload.cpp
 * @brief This file contains the main function to upload a file to a server and calculate the average upload speed.
 * 
 */

#include <iostream>
#include <fstream>
#include <chrono>
#include <curl/curl.h>

/**
 * @brief This class encapsulates the functionality of uploading a file to a server and calculating the average upload speed.
 * 
 */
class FileUploader
{
public:
    /**
     * @brief Construct a new FileUploader object with the specified file path.
     * 
     * @param file_path The path to the file to upload.
     */
    FileUploader(const std::string& file_path) : file_path_(file_path), total_speed_(0.0) {}

    /**
     * @brief Upload the file to the server and return the average upload speed.
     * 
     * @return The average upload speed in MB/s.
     */
    double UploadFile()
    {
        CURL* curl = curl_easy_init();
        if (!curl)
        {
            std::cerr << "Error initializing curl" << std::endl;
            return 0.0;
        }

        std::ifstream file(file_path_, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error opening file: " << file_path_ << std::endl;
            curl_easy_cleanup(curl);
            return 0.0;
        }

        file.seekg(0, std::ios::end);
        long long file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        char* buffer = new char[file_size];
        file.read(buffer, file_size);

        curl_easy_setopt(curl, CURLOPT_URL, "http://speedtest.netflash.net/speedtest/upload.php");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, file_size);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &FileUploader::UploadCallback);

        int num_trials = 5;
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
                    double upload_speed = (file_size / (double)duration.count()) * 1000.0 / (1024.0 * 1024.0); 
                    std::cout << "Trial " << i + 1 << " upload speed: " << upload_speed << " MB/s" << std::endl;
                    total_speed_ += upload_speed;
                }
                else
                {
                    std::cerr << "Error: unexpected response code " << response_code << std::endl;
                }
            }
        }

        curl_easy_cleanup(curl);
        delete[] buffer;
        return total_speed_ / num_trials;
    }

private:
    /**
     * @brief This callback function is used to discard the upload response data.
     * 
     * @param buffer Pointer to the data buffer.
     * @param size Size of each element to be written.
     * @param nitems Number of elements to be written.
     * @param userdata Pointer to user-defined data (not used in this implementation).
     * @return The total number of bytes written.
     */
    static size_t UploadCallback(char* buffer, size_t size, size_t nitems, void* userdata)
    {
        return size * nitems;
    }

    std::string file_path_; /**< The path to the file to upload. */
    double total_speed_; /**< The total upload speed across all trials. */
};

/**
 * @brief The main function to upload a file to a server and calculate the average upload speed.
 * 
 * @return The exit status of the program.
 * 
 * @author Namra Patel
 */
int main()
{
    std::string file_path = "./data/10MB.zip";

    FileUploader uploader(file_path);
    double average_speed = uploader.UploadFile();

    std::cout << "Average upload speed: " << average_speed << " MB/s" << std::endl;
    return 0;
}