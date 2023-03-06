#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <boost/asio.hpp>

void measure_network_performance() {
    boost::asio::io_context io_context;

    boost::asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("your_network_address", "your_network_port");

    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    std::string request = "your_request";
    std::array<char, 128> buffer;

    auto start_time = std::chrono::steady_clock::now();
    boost::asio::write(socket, boost::asio::buffer(request));
    size_t length = socket.read_some(boost::asio::buffer(buffer));
    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> latency = end_time - start_time;
    std::cout << "Latency: " << latency.count() << "ms\n";

    constexpr int packet_size = 1024 * 1024; // 1 MB
    constexpr int packet_count = 10;
    std::string packet(packet_size, 'X');

    start_time = std::chrono::steady_clock::now();
    for (int i = 0; i < packet_count; i++) {
        boost::asio::write(socket, boost::asio::buffer(packet));
    }
    size_t total_bytes = packet_size * packet_count;
    size_t bytes_transferred = 0;
    while (bytes_transferred < total_bytes) {
        bytes_transferred += socket.read_some(boost::asio::buffer(buffer));
    }
    end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> transfer_time = end_time - start_time;
    double transfer_rate = total_bytes / transfer_time.count() / 1024 / 1024;
    std::cout << "Bandwidth: " << transfer_rate << " MB/s\n";

    socket.close();
}

void discover_network_hosts() {
    boost::asio::io_context io_context;

    boost::asio::ip::udp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(boost::asio::ip::udp::v4(), "0.0.0.0", "0");

    boost::asio::ip::udp::socket socket(io_context);
    socket.open(boost::asio::ip::udp::v4());
    socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket.bind(*endpoints.begin());

    std::string request = "\xff\xff\xff\xff\xff\xff\x00\x06\x4d\x61\x68\x6f\x6e\x65";
    request.append(18, '\0');
    std::array<char, 128> buffer;
    boost::asio::ip::udp::endpoint sender_endpoint;
    std::cout << "Scanning network...\n";
    for (int i = 1; i <= 254; i++) {
        std::stringstream ss;
        ss << "192.168.0." << i;
        boost::asio::ip::udp::endpoint receiver_endpoint(boost::asio::ip::address::from_string(ss.str()), 0);
        socket.send_to(boost::asio::buffer(request), receiver_endpoint);
        try {
            size_t length = socket.receive_from(boost::asio::buffer(buffer), sender_endpoint);
            if (length == 28 && buffer[0] == 0
        && buffer[1] == 1 && buffer[2] == 8 && buffer[3] == 0 && buffer[4] == 6 && buffer[5] == 4) {
            std::cout << "Host found: " << sender_endpoint.address().to_string() << "\n";
        }
    }
        catch (...) {
            // ignore timeouts and other errors
        }
    }

socket.close();
}

int main() {
    measure_network_performance();
    discover_network_hosts();
    return 0;
}