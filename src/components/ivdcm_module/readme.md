This module provides support of the **Enable In Vehicle Distributed Connectivity Manager** functionality.
If you want to use IVDCM you need to run cmake with flag -DENABLE_IVDCM e.g.: `cmake -DENABLE_IVDCM=ON sdl_core`

Linux dependencies:
* [pcap](http://www.tcpdump.org) library

In order to install pcap on Ubunutu 14.04 and 16.04 you can run command:
```
sudo apt-get install libpcap0.8 libpcap-dev
```