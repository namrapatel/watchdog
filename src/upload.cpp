#include <iostream>
#include <fstream>
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

    std::string file_path = "./data/5MB.bin";
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

    curl_easy_setopt(curl, CURLOPT_URL, "http://speedtest.tele2.net/upload.php");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, file_size);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, upload_callback);

    CURLcode res = curl_easy_perform(curl);
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
            std::cout << "Upload completed successfully" << std::endl;
        }
        else
        {
            std::cerr << "Error: unexpected response code " << response_code << std::endl;
        }
    }

    curl_easy_cleanup(curl);
    delete[] buffer;

    return 0;
}
