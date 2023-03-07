#include <iostream>
#include <fstream>
#include <chrono>
#include <curl/curl.h>

using namespace std;

int main()
{
    CURL *curl;
    CURLcode res;
    FILE *fp;
    long http_code;
    string url = "http://www.example.com/upload.php"; // change this to your server's upload script URL
    string filename = "20MB.bin"; // change this to the name of the file you want to upload
    int file_size_mb = 20; // change this to the size of the file you want to upload, in megabytes

    // generate a file with random data
    ofstream fout(filename, ios::binary);
    for (int i = 0; i < file_size_mb * 1024 * 1024; i++) {
        fout << (char)(rand() % 256);
    }
    fout.close();

    // upload the file using libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(filename.c_str(), "rb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, fp);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_size_mb * 1024 * 1024);
        auto start_time = chrono::high_resolution_clock::now(); // declare start_time here
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);
        fclose(fp);
        auto end_time = chrono::high_resolution_clock::now();
        auto elapsed_time_ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        double upload_speed_mbps = (double)file_size_mb / ((double)elapsed_time_ms / 1000.0);
        cout << "Upload speed: " << upload_speed_mbps << " Mbps" << endl;
    }
    curl_global_cleanup();

    // delete the temporary file
    // remove(filename.c_str());

    // print the result
    if (res != CURLE_OK) {
        cerr << "Error: " << curl_easy_strerror(res) << endl;
    } else if (http_code != 200) {
        cerr << "HTTP error code: " << http_code << endl;
    }

    return 0;
}
