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
we pass the ip and port of the master so that the client connect to the master through a udp socket. and bind the client on a pair of ip and port to listent on udp socket from wich the mapper will send the result.
### Mapper Class
for the mapper we pass the ip and port pair to which the master will connect using tcp, so 
we will bind mapper to this (ip,port) pair.
and pass the client socketAddr so that it can send result by udp.  
### Master Class
in the master class we keep track of the ip4 addr. of the mappers that we want to connect by a tcp socket. in other to connect by the socket you should have the ip and the port of the destion you want to connect or the bind(listen) to.
we assiciate an ip4 and port to the master on which it wil listen to the client through a udp socket.
### MyHeader Class
-setData():
set the number that will be in the header of the packet.
- getData():
get the number that is in the header of the packet.
- print():
print the number set in the header of the packet.
- Serialize():
convert the 16 bits(2bytes) integer number in to the big-endian representation.
convert from host to network by using function: writeHtonU16().
- deserialize():
reverse the Serialize() function.
convert from network to host by using function: readNtohU16().
## simulation results report

### Throughput and Average Delay
as siad above in the throughput monitor function, we calculate the throughput as the total number of bit sent in by the channel divided by the total time between first transmission(tx) and last reception(rx). if the send peckets continuously, the throughput will be the maximum of the bandwidth of the channel. but in our simulation, we send the packets periodically, so the throughput will be less than the bandwidth of the channel. and for large data the propagaiton can be ignored so that the throughput will be almost equal to the bandwidth of the channel.
![image](https://user-images.githubusercontent.com/102898527/233477173-c6be5636-64a9-4f5d-9a26-55376a52f787.png)

### result Massage
here we send random char from client to master in the udp protocol then recive the result by udp from mappers to client, beacuse of the udp protocol, packets can be lost and packets can be interleaved and because the client moves this probability of losing packts varies.
so therecieve order is not exactly the same as the send order.
![image](https://user-images.githubusercontent.com/102898527/233477104-0994b79d-ed39-474f-80a4-7604f42bfe3a.png)
