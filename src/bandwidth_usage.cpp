/**
 * @file bandwidth_usage.cpp
 * @brief Measures the network bandwidth by monitoring the traffic on the network interface.
 * @details Uses libpcap to monitor the traffic on the network interface and compute the bandwidth usage.
 * The bandwidth usage is then printed to the console every 5 seconds.
 * @author Namra Patel
*/

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

/**
 * @class BandwidthMonitor
 * @brief A class that measures the network bandwidth by monitoring the traffic on the network interface.
 * @details Uses libpcap to monitor the traffic on the network interface and compute the bandwidth usage.
 * The bandwidth usage is then printed to the console every 5 seconds.
 * @author Namra Patel
*/
class BandwidthMonitor {
public:
    /**
     * @brief Initializes the BandwidthMonitor object.
     * @details Initializes libpcap and starts monitoring the traffic on the network interface.
     * @return void
    */
    BandwidthMonitor();

    /**
     * @brief Starts monitoring the traffic on the network interface.
     * @details Starts monitoring the traffic on the network interface and prints the bandwidth usage every 5 seconds.
     * @return void
    */
    void startMonitoring();

    /**
     * @brief Stops monitoring the traffic on the network interface.
     * @details Stops monitoring the traffic on the network interface and stops printing the bandwidth usage.
     * @return void
    */
    void stopMonitoring();

    /**
     * @brief Prints the bandwidth usage every 5 seconds.
     * @details Prints the bandwidth usage every 5 seconds.
     * @return void
    */
    void printBandwidthUsage();

private:
    /**
     * @brief Processes a packet.
     * @details Processes a packet and computes the bandwidth usage.
     * @param header The header of the packet.
     * @param packet The packet.
     * @return void
    */
    void processPacket(const struct pcap_pkthdr *header, const u_char *packet);

    /**
     * @brief Prints the user's IP address.
     * @details Prints the user's IP address.
     * @return void
    */
    void printMyIpAddress();

    /**
     * @brief Prints the bandwidth usage .
     * @details Prints the bandwidth usage using a loop that runs based on a flag.
     * @return void
    */
    void printBandwidthUsageHelper();

    /**
     * @brief Resolves the host name from the IP address.
     * @details Resolves the host name from the IP address.
     * @param ip The IP address.
     * @return The host name.
    */
    std::string resolveHostName(const char *ip);

    std::mutex bandwidth_mutex_; // Mutex to protect the bandwidth map
    std::map<std::string, long long> device_bandwidth_; // Map of device IP address to bandwidth usage
    pcap_t *handle_; // libpcap handle
    bool is_running_; // Flag to indicate if the bandwidth monitor is running
    std::thread print_thread_; // Thread to print bandwidth usage
};

/**
 * @brief BandwidthMonitor constructor.
 *
 * Initializes the handle to null and sets the is_running_ flag to false. 
 * It gets the network device by calling pcap_lookupdev and opens the device by calling pcap_open_live. 
 * If the device is not found or there is an error opening the device, it prints an error message and returns. 
 * It then calls printMyIpAddress to print the IP address of the device.
 * @param None.
 * @return None.
 */
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

/**
 * @brief Start monitoring network bandwidth usage.
 * Checks if the monitoring is already running. 
 * If it is, it returns immediately. If it's not, it sets the is_running_ flag to true and starts a new thread to print the bandwidth usage every second by calling printBandwidthUsageHelper. 
 * It then captures packets using pcap_loop, and for each packet captured, it calls processPacket to update the bandwidth usage.
 * @param None.
 * @return None.
 */
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


/**
 * @brief Stop monitoring network bandwidth usage.
 * Checks if the monitoring is already stopped. 
 * If it is, it returns immediately. If it's not, it sets the is_running_ flag to false and joins the print_thread_ to stop printing the bandwidth usage. 
 * It then closes the pcap handle by calling pcap_close.
 * @param None.
 * @return None.
 */
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


/**
 * @brief Print the bandwidth usage of devices connected to the network.
 * Acquires a lock on the bandwidth_mutex_ to ensure exclusive access to the device_bandwidth_ map. 
 * It then prints the devices and their corresponding bandwidth usage by iterating through the device_bandwidth_ map.
 * @param None.
 * @return None.
 */
void BandwidthMonitor::printBandwidthUsage() {
    std::lock_guard<std::mutex> lock(bandwidth_mutex_);

    // Print devices and bandwidth usage
    std::cout << "Devices connected and bandwidth usage:" << std::endl;
    for (const auto &device : device_bandwidth_) {
        std::cout << device.first << ": " << device.second << " bytes" << std::endl;
    }
}


/**
 * @brief Process a packet captured by pcap_loop() to update the bandwidth usage.
 *
 * Extracts the IP header from the packet and gets the source IP address. 
 * It then resolves the hostname of the IP address using resolveHostName function. 
 * Finally, it updates the bandwidth usage of the device corresponding to the hostname by acquiring a lock on the bandwidth_mutex_ and incrementing the device_bandwidth_ map with the length of the packet.
 *
 * @param header A pointer to the pcap_pkthdr structure that contains the packet header fields.
 * @param packet A pointer to the packet data.
 * @return None.
 */
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

/**
 * @brief Print the IP address of the local device.
 *
 * Uses getifaddrs() to get the network interfaces on the local device. 
 * It iterates through the list of network interfaces, and if the interface has an IPv4 address, it gets the IP address and prints it to the console.
 *
 * @param None.
 * @return None.
 */
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


/**
 * @brief Continuously print the bandwidth usage of devices connected to the network at one-second intervals.
 *
 * Runs a loop that continues to print the bandwidth usage of devices connected to the network every second as long as the is_running_ flag is true. Before printing the bandwidth usage, it clears the console to display up-to-date information. It sleeps for one second after each print to ensure that the information is updated at regular intervals.
 *
 * @param None.
 *
 * @return None.
 */
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


/**
 * @brief Resolve the hostname from an IP address.
 *
 * Uses getaddrinfo() to resolve the hostname from an IP address. It sets up a struct addrinfo to provide hints for the resolution, including specifying the IP family and socket type. It then calls getaddrinfo() to get the address information for the given IP address. If the resolution is successful, it returns the canonical hostname. If it is not successful, it returns the IP address.
 *
 * @param ip A pointer to a character array holding the IP address to resolve.
 *
 * @return A string with the hostname of the device with the given IP address, or the IP address itself if resolution fails.
 */
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



/**
 * @brief Main function to start monitoring network bandwidth usage.
 *
 * Creates a BandwidthMonitor object and calls its startMonitoring() method to begin capturing and monitoring network traffic. 
 * Then, it displays a message to the console informing the user that the program is monitoring bandwidth usage and will stop when the Enter key is pressed. It waits for the user to press Enter, and then calls the BandwidthMonitor object's stopMonitoring() method to stop the capture and monitoring of network traffic.
 * @param None.
 * @return 0 if the program terminates successfully.
 * @author Namra Patel
 */
int main() {
    BandwidthMonitor monitor;

    monitor.startMonitoring();

    std::cout << "Monitoring bandwidth usage... Press Enter to stop." << std::endl;
    std::cin.ignore();

    monitor.stopMonitoring();

    return 0;
}