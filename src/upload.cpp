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

    std::string file_path = "./data/20MB.bin";
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

    printf("1");
    curl_easy_setopt(curl, CURLOPT_URL, "ftp://tele2.net/upload/");
    curl_easy_setopt(curl, CURLOPT_USERPWD, "anonymous:anonymous");
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, buffer);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, file_size);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, upload_callback);
    printf("2");
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "Error uploading file: " << curl_easy_strerror(res) << std::endl;
    }
    printf("3");
    curl_easy_cleanup(curl);
    delete[] buffer;

    return 0;
}