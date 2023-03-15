#include <iostream>
#include <pcap.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>
#define __FAVOR_BSD
#include <netinet/ip.h>
#undef __FAVOR_BSD


void process_packet(u_char* args, const struct pcap_pkthdr* header, const u_char* buffer)
{
    // Extract the IP header from the packet
    const struct iphdr* ip_header = reinterpret_cast<const struct iphdr*>(buffer + sizeof(struct ether_header));

    // Extract the TCP header from the packet
    const struct tcphdr* tcp_header = reinterpret_cast<const struct tcphdr*>(buffer + sizeof(struct ether_header) + sizeof(struct iphdr));

    // Print the source and destination IP addresses and port numbers
    std::cout << "Source IP: " << inet_ntoa(*(reinterpret_cast<const struct in_addr*>(&ip_header->saddr))) << std::endl;
    std::cout << "Destination IP: " << inet_ntoa(*(reinterpret_cast<const struct in_addr*>(&ip_header->daddr))) << std::endl;
    std::cout << "Source port: " << ntohs(tcp_header->th_sport) << std::endl;
    std::cout << "Destination port: " << ntohs(tcp_header->th_dport) << std::endl;
    std::cout << "Packet length: " << header->len << " bytes" << std::endl;
}

int main()
{
    char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_t* pcap_handle = nullptr;

    // Open the network interface for capturing
    pcap_handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, error_buffer);
    if (pcap_handle == nullptr)
    {
        std::cerr << "Error opening network interface: " << error_buffer << std::endl;
        return 1;
    }

    // Set a filter to capture TCP packets
    struct bpf_program filter;
    const char* filter_expression = "tcp";
    if (pcap_compile(pcap_handle, &filter, filter_expression, 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        std::cerr << "Error compiling filter expression" << std::endl;
        pcap_close(pcap_handle);
        return 1;
    }
    if (pcap_setfilter(pcap_handle, &filter) == -1)
    {
        std::cerr << "Error setting filter" << std::endl;
        pcap_close(pcap_handle);
        return 1;
    }

    // Capture packets until the user presses a key
    std::cout << "Capturing packets. Press any key to stop." << std::endl;
    pcap_loop(pcap_handle, -1, process_packet, nullptr);

    // Clean up and exit
    pcap_close(pcap_handle);
    return 0;
}
