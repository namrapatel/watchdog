# Watchdog

Real-time network monitoring right from your command-line.

### Features
- [x] Network bandwidth monitoring: Download and upload bandwidth testing
- [x] Latency between computer and inputted IP/hostname
- [ ] List connected devices
- [ ] Bandwidth usage per device
    - Filter packets by port number (HTTP or other)
    - Filter by protocol
    - Filter by IP address range
    - Filter by packet content

### Usage

Ensure that you have `g++` installed, alongside any of the libraries that might be used in the program that you're trying to run. As long as you have those, from the root folder you can run the following bash commands.


#### Use bandwidth_usage.cpp
```bash
make bandwidth
```

#### Use download.cpp
```bash
make download
```

#### Use upload.cpp
```bash
make upload
```

#### Use ping.cpp
```bash
make ping
```