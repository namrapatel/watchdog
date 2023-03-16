#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <map>
#include <string>

// Function to process packet
void process_packet(const struct pcap_pkthdr *header, const u_char *packet, std::map<std::string, long long> &device_bandwidth);

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

    // Capture packets
    struct pcap_pkthdr header;
    const u_char *packet;
    while (true) {
        packet = pcap_next(handle, &header);
        process_packet(&header, packet, device_bandwidth);
    }

    // Close pcap handle
    pcap_close(handle);

    return 0;
}

void process_packet(const struct pcap_pkthdr *header, const u_char *packet, std::map<std::string, long long> &device_bandwidth) {
    // Get IP header
    struct ip *ip_header = (struct ip *)(packet + 14);

    // Get source IP
    char src_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);

    // Update bandwidth usage
    device_bandwidth[src_ip] += header->len;

    // Print devices and bandwidth usage
    std::cout << "Devices connected and bandwidth usage:" << std::endl;
    for (const auto &device : device_bandwidth) {
        std::cout << device.first << ": " << device.second << " bytes" << std::endl;
    }
}