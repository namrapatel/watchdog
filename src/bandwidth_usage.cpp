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
#include <ifaddrs.h>
#include <cstring>
using namespace std;

class BandwidthMonitor {
public:
    BandwidthMonitor();
    void startMonitoring();
    void stopMonitoring();
    void printBandwidthUsage();

private:
    void processPacket(const struct pcap_pkthdr *header, const u_char *packet);
    void printMyIpAddress();
    void printBandwidthUsageHelper();
    std::string resolveHostName(const char *ip);

    std::mutex bandwidth_mutex_;
    std::map<std::string, long long> device_bandwidth_;
    pcap_t *handle_;
    bool is_running_;
    std::thread print_thread_;
};

BandwidthMonitor::BandwidthMonitor()
    : handle_(nullptr), is_running_(false)
{
    char errbuf[PCAP_ERRBUF_SIZE];

    // Get network device
    char *dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        std::cerr << "Device not found: " << errbuf << std::endl;
        return;
    }

    // Open device
    handle_ = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle_ == NULL) {
        std::cerr << "Error opening device: " << errbuf << std::endl;
        return;
    }

    // Print your own IP address
    printMyIpAddress();
}

void BandwidthMonitor::startMonitoring() {
    if (is_running_) {
        return;
    }

    is_running_ = true;

    // Start a separate thread to print bandwidth usage every second
    print_thread_ = std::thread(&BandwidthMonitor::printBandwidthUsageHelper, this);

    // Capture packets using pcap_loop()
    pcap_loop(handle_, -1, [](u_char *user_data, const struct pcap_pkthdr *header, const u_char *packet) {
        BandwidthMonitor *monitor = reinterpret_cast<BandwidthMonitor *>(user_data);
        monitor->processPacket(header, packet);
    }, reinterpret_cast<u_char *>(this));
}

void BandwidthMonitor::stopMonitoring() {
    if (!is_running_) {
        return;
    }

    is_running_ = false;

    // Join the print thread
    if (print_thread_.joinable()) {
        print_thread_.join();
    }

    // Close pcap handle
    if (handle_ != nullptr) {
        pcap_close(handle_);
        handle_ = nullptr;
    }
}

void BandwidthMonitor::printBandwidthUsage() {
    std::lock_guard<std::mutex> lock(bandwidth_mutex_);

    // Print devices and bandwidth usage
    std::cout << "Devices connected and bandwidth usage:" << std::endl;
    for (const auto &device : device_bandwidth_) {
        std::cout << device.first << ": " << device.second << " bytes" << std::endl;
    }
}

void BandwidthMonitor::processPacket(const struct pcap_pkthdr *header, const u_char *packet) {
    // Get IP header
    struct ip *ip_header = (struct ip *)(packet + sizeof(struct ether_header));

    // Get source IP
    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);

    // Resolve hostname
    std::string hostname = resolveHostName(src_ip);

    // Update bandwidth usage
    {
        std::lock_guard<std::mutex> lock(bandwidth_mutex_);
        device_bandwidth_[hostname] += header->len;
    }
}

void BandwidthMonitor::printMyIpAddress() {
    char ip[INET_ADDRSTRLEN];
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);
    tmp = addrs;
    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            // Convert IP address to string and print
            inet_ntop(AF_INET, &(pAddr->sin_addr), ip, INET_ADDRSTRLEN);
            std::cout << "My IP address is: " << ip << std::endl;
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
}

void BandwidthMonitor::printBandwidthUsageHelper() {
    while (is_running_) {
        // Clear console
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        printBandwidthUsage();

        // Sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::string BandwidthMonitor::resolveHostName(const char *ip) {
    // Resolve hostname
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    int status = getaddrinfo(ip, NULL, &hints, &res);
    std::string hostname;
    if (status == 0) {
        hostname = res->ai_canonname ? res->ai_canonname : ip;
        freeaddrinfo(res);
    } else {
        // Handle error in resolving hostname
        std::cerr << "Error resolving hostname: " << gai_strerror(status) << std::endl;
        hostname = ip;
    }

    return hostname;
}


int main() {
    BandwidthMonitor monitor;

    monitor.startMonitoring();

    std::cout << "Monitoring bandwidth usage... Press Enter to stop." << std::endl;
    std::cin.ignore();

    monitor.stopMonitoring();

    return 0;
}
