# Watchdog

Real-time network monitoring right from your command-line.

### Features
1. **Download** : A file is automatically downloaded and the download speed is recorded and printed to the user.
2. **Upload** : A file is automatically uploaded to an HTTP server and the upload speed is recorded and printed to the user.
3. **Ping** : Enter a host to ping, that host is pinged and the results are printed.
4. **Bandwidth** : After escalating sudo privileges, all incoming connections are printed with their respective bandwidth usage.
5. **Jitter** : The current jitter (variation of latency on a packet flow) is calculated by connecting to a reliable host and printed to the user.
6. **Packet Loss** : The current packetloss is calculated by connecting to a reliable host and printing the packetloss to the user.

### Usage

Ensure that you have `g++` installed, alongside any of the libraries that might be used in the program that you're trying to run. As long as you have those, from the root folder you can run the following bash command. Upon exiting the program, the environment will be automatically cleaned up and ready to run again.

### Use watchdog 
```bash
make all
```

### Structure

The codebase is structured as follows:

```
watchdog/
├─ src/
│ ├─ download.cpp: test your download speed.
│ ├─ upload.cpp: test your upload speed. 
│ ├─ ping.cpp: test your ping. 
│ ├─ bandwidth_usage.cpp: display the bandwidth usage of all connections. 
│ ├─ jitter.cpp: display jitter by connecting to a reliable host. 
│ ├─ packetloss.cpp: display packetloss by connecting to a reliable host.
│ ├─ main.cpp: main entry point for the program.
├─ data/
│ ├─ 10MB.zip
│ ├─ 20MB.bin 
│ ├─ 5MB.bin 
├─ makefile
├─ README.md
```