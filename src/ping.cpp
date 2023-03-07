#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ifaddrs.h>
#include <arpa/inet.h>

using namespace std;

int main()
{
    vector<string> hosts;

    // Get list of network interfaces and their addresses
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1)
    {
        cerr << "Error getting network interfaces" << endl;
        return 1;
    }

    // Loop through interfaces
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        // Only consider IPv4 interfaces
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            // Get interface address and netmask
            string interface_name = ifa->ifa_name;
            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
            struct sockaddr_in* netmask = (struct sockaddr_in*)ifa->ifa_netmask;

            // Calculate network address and broadcast address
            string ip_address = inet_ntoa(addr->sin_addr);
            string netmask_str = inet_ntoa(netmask->sin_addr);
            unsigned int ip = ntohl(addr->sin_addr.s_addr);
            unsigned int nm = ntohl(netmask->sin_addr.s_addr);
            unsigned int network = (ip & nm);
            unsigned int broadcast = (ip | (~nm));

            // Loop through addresses in network range
            for (unsigned int i = network+1; i < broadcast; i++)
            {
                struct in_addr address;
                address.s_addr = htonl(i);
                string ip_address = inet_ntoa(address);
                string ping_command = "ping " + ip_address + " -c 1 -w 100";
                int result = system(ping_command.c_str());
                if (result == 0)
                {
                    hosts.push_back(ip_address);
                }
            }
        }
    }

    // Free memory used by ifaddrs
    freeifaddrs(ifaddr);

    // Print connected hosts
    cout << "Connected hosts:" << endl;
    for (const auto& host : hosts)
    {
        cout << host << endl;
    }

    return 0;
}
