# Watchdog

Real-time network monitoring right from your command-line.

### Features
- [x] Network bandwidth monitoring: Download and upload bandwidth testing
- [x] Latency between computer and inputted IP/hostname
- [ ] List connected devices
- [ ] Bandwidth usage per device

### Usage

Ensure that you have `g++` installed, alongside any of the libraries that might be used in the program that you're trying to run. As long as you have those, from the root folder you can run the following bash commands.

#### Use download.cpp
```bash
make cdown 
make rdown
```

#### Use upload.cpp
```bash
make cup
make rup
```

#### Use ping.cpp
```bash
make cping 
make rping
```