# ns-3-Wireless-Network-Simulation



- [Code](#code-description)
  - [sample.cc File](#samplecc)
  - [Client Class](#client-class)
  - [Mapper Class](#mapper-class)
  - [Master Class](#master-class)
  - [MyHeader Class](#myheader-class)
- [Report](#simulation-result-report)

## Code Description

### sample.cc
``ThroughputMonitor``:

The ThroughputMonitor function is responsible for calculating the network throughput. In our simulation, the throughput is lower than the maximum bandwidth of the channel due to the periodic sending of packets. Since the channel is not utilized to its full capacity, the throughput is reduced. We determine the throughput by dividing the total number of bits received by the channel by the elapsed time between the first transmission (tx) and the last reception (rx). It's important to note that the sum of the throughput from the master to mappers is approximately equal to the throughput from the client to the master.

``AverageDelayMonitor``:

The AverageDelayMonitor function calculates the average delay of each packet in the network. It measures the time taken for a packet to travel from its source to its destination. By analyzing the delays of multiple packets, we can determine the average delay experienced by the network. This information is valuable in assessing the overall performance and efficiency of the network.

### Client Class
In the client program, we provide the IP address and port of the master as parameters. This allows the client to establish a connection with the master using a UDP socket. Additionally, the client binds to a specific IP address and port combination to listen for incoming UDP socket messages from the mappers, which will contain the results of their computations.

### Mapper Class
For the mapper, we pass the IP address and port pair to which the master will connect using TCP. The mapper then binds to this specific (IP, port) pair to establish a TCP connection with the master.

Additionally, we pass the client socket address to the mapper. This allows the mapper to send the results of its computations back to the client using a UDP socket. By providing the client socket address, the mapper knows where to send the result message via UDP.

### Master Class
In the Master class, we maintain a record of the IPv4 addresses of the mappers we intend to connect to using a TCP socket. To establish a TCP connection, we require the IP address and port of the destination we wish to connect to or bind/listen to.

Furthermore, we assign an IPv4 address and port to the master, which allows it to listen for incoming communication from the client through a UDP socket. By associating an IP address and port with the master, we enable it to receive messages from the client via UDP.

### MyHeader Class
- `setData()`:
This function sets the number that will be stored in the header of the packet. It takes an integer as input and assigns it to the header field.

- `getData()`:
This function retrieves the number that is stored in the header of the packet. It returns the integer value stored in the header field.

- `print()`:
This function prints the number that is set in the header of the packet. It retrieves the value from the header field and displays it on the console.

- `Serialize()`:
This function converts the 16-bit (2-byte) integer number into big-endian representation. It prepares the number to be sent over the network by using the `writeHtonU16()` function, which converts the integer from host byte order to network byte order.

- `Deserialize()`:
This function reverses the `Serialize()` function. It takes a serialized data buffer received over the network and converts the 16-bit (2-byte) integer from network byte order to host byte order. It uses the `readNtohU16()` function for this conversion.

## Simulation Result Report

### Throughput and Average Delay
In the throughput monitor function, the throughput is calculated by dividing the total number of bits sent through the channel by the time elapsed between the first transmission (tx) and the last reception (rx). When packets are sent continuously, the throughput can reach the maximum bandwidth of the channel. However, in our simulation, packets are sent periodically, resulting in a lower throughput compared to the channel's bandwidth.

It's important to note that for large amounts of data, the influence of propagation delay becomes negligible, allowing the throughput to approach the bandwidth of the channel. In such cases, the throughput will be nearly equal to the channel's bandwidth.
<p align="center">
<img src="https://user-images.githubusercontent.com/102898527/233477173-c6be5636-64a9-4f5d-9a26-55376a52f787.png">
</p>


### Result Massage

In our system, we send random characters from the client to the master using the UDP protocol. The result, containing the processed data, is then transmitted from the mappers to the client via UDP. However, due to the nature of the UDP protocol, packets can be lost during transmission, and they may also arrive out of order. Additionally, the probability of packet loss can vary as the client moves.

As a result, the order in which the packets are received may not precisely match the order in which they were sent. It is important to consider the possibility of lost or interleaved packets when processing the received data.
<p align="center">
<img src="https://user-images.githubusercontent.com/102898527/233477104-0994b79d-ed39-474f-80a4-7604f42bfe3a.png">
</p>
