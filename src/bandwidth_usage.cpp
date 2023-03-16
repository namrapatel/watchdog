#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <map>
#include <string>
#include <netinet/if_ether.h>
#include <netdb.h>
#include <thread>
#include <mutex>
#include <chrono>

void process_packet(u_char *user_data, const struct pcap_pkthdr *header, const u_char *packet);
void print_bandwidth_usage(std::map<std::string, long long> &device_bandwidth);

std::mutex bandwidth_mutex;

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // Get network device
    char *dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        std::cerr << "Device not found: " << errbuf << std::endl;
        return 1;
    }

    // Open device
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        std::cerr << "Error opening device: " << errbuf << std::endl;
        return 1;
    }

    // Map to store device IP and bandwidth usage
    std::map<std::string, long long> device_bandwidth;

    // Start a separate thread to print bandwidth usage every second
    std::thread print_thread([&device_bandwidth]() mutable { print_bandwidth_usage(device_bandwidth); });

    // Capture packets using pcap_loop()
    pcap_loop(handle, -1, process_packet, reinterpret_cast<u_char *>(&device_bandwidth));

    // Close pcap handle
    pcap_close(handle);

    // Join the print thread
    print_thread.join();

    return 0;
}


void print_bandwidth_usage(std::map<std::string, long long> &device_bandwidth) {
    while (true) {
        // Clear console
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        // Print devices and bandwidth usage
        {
            std::lock_guard<std::mutex> lock(bandwidth_mutex);
            std::cout << "Devices connected and bandwidth usage:" << std::endl;
            for (const auto &device : device_bandwidth) {
                std::cout << device.first << ": " << device.second << " bytes" << std::endl;
            }
        }

        // Sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


void process_packet(u_char *user_data, const struct pcap_pkthdr *header, const u_char *packet) {
    // Get the device_bandwidth map from user_data
    std::map<std::string, long long> &device_bandwidth = *reinterpret_cast<std::map<std::string, long long> *>(user_data);

    // Get IP header
    struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));

    // Get source IP
    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);

    // Resolve hostname
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    int status = getaddrinfo(src_ip, NULL, &hints, &res);
    std::string hostname;
    if (status == 0) { 
        hostname = res->ai_canonname ? res->ai_canonname : src_ip;
        freeaddrinfo(res);
    } else {
        // Handle error in resolving hostname
        std::cerr << "Error resolving hostname: " << gai_strerror(status) << std::endl;
        hostname = src_ip;
    }

    // Update bandwidth usage
    {
        std::lock_guard<std::mutex> lock(bandwidth_mutex);
        device_bandwidth[hostname] += header->len;
    }
}
