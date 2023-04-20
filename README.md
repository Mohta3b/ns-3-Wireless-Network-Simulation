# CN_CHomeworks_2


## document

- [Code](#code-documentation)
- [Report](#simulation-results-report)


## Code Documentation

- [sample.cc](#samplecc)
- [Client Class](#client-class)
- [Mapper Class](#mapper-class)
- [Master Class](#master-class)
- [MyHeader Class](#myheader-class)

### sample.cc

- ThroughputMonitor:
this function calculates the throughput of the network.
which much less than the bandwidth of the channel, beacuse of the priodic send of the packet we don't use the channel the maximum of the bandwidth.
and the sum of the throughput of the master to mappers is approximately equal to the throughput of the client to master. we calcualte the throughput as the total number of bit sent in by the channel divided by the total time between first transmission(tx) and last reception(rx).
- AverageDelayMonitor:
this function calculates the average delay of each packet.

### Client Class
### Mapper Class
### Master Class
### MyHeader Class
-setData():
set the number that will be in the header of the packet.
- print():
print the number set in the header of the packet.


## simulation results report

### Throughput and Average Delay
as siad above in the throughput monitor function, we calculate the throughput as the total number of bit sent in by the channel divided by the total time between first transmission(tx) and last reception(rx). if the send peckets continuously, the throughput will be the maximum of the bandwidth of the channel. but in our simulation, we send the packets periodically, so the throughput will be less than the bandwidth of the channel. and for large data the propagaiton can be ignored so that the throughput will be almost equal to the bandwidth of the channel.
![image](https://user-images.githubusercontent.com/102898527/233477173-c6be5636-64a9-4f5d-9a26-55376a52f787.png)

### result Massage
here we send random char from client to master in the udp protocol then recive the result by udp from mappers to client, beacuse of the udp protocol, packets can be lost and packets can be interleaved and because the client moves this probability of losing packts varies.
so therecieve order is not exactly the same as the send order.
![image](https://user-images.githubusercontent.com/102898527/233477104-0994b79d-ed39-474f-80a4-7604f42bfe3a.png)
