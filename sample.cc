#include <cstdlib>
#include<time.h>
#include <stdio.h>
#include <string>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/error-model.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("WifiTopology");
const int MAPPER_COUNT = 1;
const float DURATION_TIME = 60.0;

void
ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, double em)
{
    uint16_t i = 0;

    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats ();

    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);

        std::cout << "Flow ID			: "<< stats->first << " ; " << fiveTuple.sourceAddress << " -----> " << fiveTuple.destinationAddress << std::endl;
        std::cout << "Tx Packets = " << stats->second.txPackets << std::endl;
        std::cout << "Rx Packets = " << stats->second.rxPackets << std::endl;
        std::cout << "Duration		: "<< (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) << std::endl;
        std::cout << "Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds () << " Seconds" << std::endl;
        std::cout << "Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) / 1024 / 1024  << " Mbps" << std::endl;
    
        i++;

        std::cout << "---------------------------------------------------------------------------" << std::endl;
    }

    Simulator::Schedule (Seconds (10),&ThroughputMonitor, fmhelper, flowMon, em);
}

void
AverageDelayMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, double em)
{
    uint16_t i = 0;

    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats ();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
        std::cout << "Flow ID			: "<< stats->first << " ; " << fiveTuple.sourceAddress << " -----> " << fiveTuple.destinationAddress << std::endl;
        std::cout << "Tx Packets = " << stats->second.txPackets << std::endl;
        std::cout << "Rx Packets = " << stats->second.rxPackets << std::endl;
        std::cout << "Duration		: "<< (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) << std::endl;
        std::cout << "Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds () << " Seconds" << std::endl;
        std::cout << "Sum of e2e Delay: " << stats->second.delaySum.GetSeconds () << " s" << std::endl;
        std::cout << "Average of e2e Delay: " << stats->second.delaySum.GetSeconds () / stats->second.rxPackets << " s" << std::endl;
    
        i++;

        std::cout << "---------------------------------------------------------------------------" << std::endl;
    }

    Simulator::Schedule (Seconds (10),&AverageDelayMonitor, fmhelper, flowMon, em);
}

class MyHeader : public Header 
{
public:
    MyHeader ();
    virtual ~MyHeader ();
    void SetData (uint16_t data);
    void SetStringData (std::string data);
    uint16_t GetData (void) const;
    std::string GetStringData (void) const;
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    virtual void Print (std::ostream &os) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
    virtual uint32_t GetSerializedSize (void) const;
private:
    uint16_t m_data;
    std::string s_data;
};

MyHeader::MyHeader ()
{
}

MyHeader::~MyHeader ()
{
}

TypeId
MyHeader::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::MyHeader")
        .SetParent<Header> ()
        .AddConstructor<MyHeader> ()
    ;
    return tid;
}

TypeId
MyHeader::GetInstanceTypeId (void) const
{
    return GetTypeId ();
}

void
MyHeader::Print (std::ostream &os) const
{
    os << "data = " << m_data << endl;
}

uint32_t
MyHeader::GetSerializedSize (void) const
{
    return 2;
}

void
MyHeader::Serialize (Buffer::Iterator start) const
{
    start.WriteHtonU16 (m_data);
}

uint32_t
MyHeader::Deserialize (Buffer::Iterator start)
{
    m_data = start.ReadNtohU16 ();

    return 2;
}

void 
MyHeader::SetData (uint16_t data)
{
    m_data = data;
}

void
MyHeader::SetStringData (std::string data)
{
    s_data = char(data[0]);
}

uint16_t 
MyHeader::GetData (void) const
{
    return m_data;
}

std::string
MyHeader::GetStringData (void) const
{
    return s_data;
}

class master : public Application
{
public:
    master (uint16_t masterClientPort, std::vector<uint16_t> masterMapperPort, Ipv4InterfaceContainer& master_ip, Ipv4InterfaceContainer& mapper_ip);
    virtual ~master ();
private:
    virtual void StartApplication (void);
    void HandleRead (Ptr<Socket> socket);

    uint16_t masterClientPort;
    std::vector<uint16_t> masterMapperPort;
    Ipv4InterfaceContainer master_ip;
    Ipv4InterfaceContainer mapper_ip;
    Ptr<Socket> socket; // between server and client
    std::vector< Ptr<Socket>> tcpSocket; // between server and mapper
    // create buffer vector to save received data
    std::vector<MyHeader> buffer;
    
};


class client : public Application
{
public:
    client (uint16_t masterClientPort, uint16_t mapperClientPort, Ipv4InterfaceContainer& client_ip, Ipv4InterfaceContainer& master_ip);
    virtual ~client ();

private:
    virtual void StartApplication (void);
    void HandleRead (Ptr<Socket> socket);
    void PrintMessage ();

    uint16_t masterClientPort;
    uint16_t mapperClientPort;
    Ipv4InterfaceContainer client_ip;
    Ipv4InterfaceContainer master_ip;
    //Ptr<Socket> serverSocket; // between client and server
    Ptr<Socket> mapperSocket; // between client and mapper
    // create buffer vector to save received data in string foramt
    std::string message = "";
    std::string input_message = "";
    
};


class mapper : public Application
{
public:
    mapper (uint16_t masterMapperPort, uint16_t mapperClientPort, Ipv4InterfaceContainer& mapper_ip, Ipv4InterfaceContainer& client_ip , int this_mapper_id);
    virtual ~mapper ();

private:
    virtual void StartApplication (void);
    void HandleAccept (Ptr<Socket> s, const Address& from);
    void HandleRead (Ptr<Socket> socket);
    char Decode (uint16_t data);

    uint16_t masterMapperPort;
    uint16_t mapperClientPort;
    Ipv4InterfaceContainer mapper_ip;
    Ipv4InterfaceContainer client_ip;
    Ptr<Socket> udpSocket; // between client and mapper
    Ptr<Socket> tcpSocket; // between server and mapper
    std::vector<MyHeader> buffer;
    // define encoder like 1 -> a, 2 -> b, 3 -> c, ...
    std::map<uint16_t, char> encoder;
    int this_mapper_id;

};


int
main (int argc, char *argv[])
{
    double error = 0.000001;
    string bandwidth = "1Mbps";
    bool verbose = true;
    double duration = DURATION_TIME;
    bool tracing = false;

    srand(time(NULL));

    CommandLine cmd (__FILE__);
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

    cmd.Parse (argc,argv);

    if (verbose)
    {
        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    NodeContainer wifiStaNodeClient;
    wifiStaNodeClient.Create (1);

    NodeContainer wifiStaNodeMaster;
    wifiStaNodeMaster.Create (1);

    // Define mapper node and create 3 nodes
    NodeContainer wifiMapperNode;
    wifiMapperNode.Create (MAPPER_COUNT);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();

    YansWifiPhyHelper phy;
    phy.SetChannel (channel.Create ());


    WifiHelper wifi;
    wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

    // Install wifi devices on client nodes
    WifiMacHelper mac;
    Ssid ssid = Ssid ("ns-3-ssid");
    mac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));

    NetDeviceContainer staDeviceClient;
    staDeviceClient = wifi.Install (phy, mac, wifiStaNodeClient);

    // Install wifi devices on master nodes
    mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));

    NetDeviceContainer staDeviceMaster;
    staDeviceMaster = wifi.Install (phy, mac, wifiStaNodeMaster);

    // Install wifi devices on mapper nodes
    mac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid));

    NetDeviceContainer staDeviceMapper;
    staDeviceMapper = wifi.Install (phy, mac, wifiMapperNode);

    mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute ("ErrorRate", DoubleValue (error));
    phy.SetErrorRateModel("ns3::YansErrorRateModel");

    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue (0.0),
                                   "MinY", DoubleValue (0.0),
                                   "DeltaX", DoubleValue (3.0),
                                   "DeltaY", DoubleValue (10.0),
                                   "GridWidth", UintegerValue (10),
                                   "LayoutType", StringValue ("RowFirst"));

    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
    mobility.Install (wifiStaNodeClient);

    // Set mobility model for mapper nodes
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiMapperNode);

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodeMaster);



    InternetStackHelper stack;
    stack.Install (wifiStaNodeClient);
    stack.Install (wifiStaNodeMaster);
    stack.Install (wifiMapperNode);
    

    Ipv4AddressHelper address;

    Ipv4InterfaceContainer staNodeClientInterface;
    Ipv4InterfaceContainer staNodesMasterInterface;
    Ipv4InterfaceContainer staNodeMapperInterface;

    address.SetBase ("10.1.3.0", "255.255.255.0");
    staNodeClientInterface = address.Assign (staDeviceClient);
    staNodesMasterInterface = address.Assign (staDeviceMaster);
    staNodeMapperInterface = address.Assign (staDeviceMapper);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint16_t masterClientPort = 1102;
    uint16_t mapperClientPort = 1103;
    std::vector <uint16_t> masterMapperPort;
    masterMapperPort.push_back(1104);
    masterMapperPort.push_back(1105);
    masterMapperPort.push_back(1106);

    // Create object for client application
    Ptr<client> clientApp = CreateObject<client> (masterClientPort, mapperClientPort, staNodeClientInterface , staNodesMasterInterface);
    wifiStaNodeClient.Get (0)->AddApplication (clientApp);
    clientApp->SetStartTime (Seconds (0.0));
    clientApp->SetStopTime (Seconds (duration));  

    // Create object for master application
    Ptr<master> masterApp = CreateObject<master> (masterClientPort, masterMapperPort, staNodesMasterInterface, staNodeMapperInterface);
    wifiStaNodeMaster.Get (0)->AddApplication (masterApp);
    masterApp->SetStartTime (Seconds (0.0));
    masterApp->SetStopTime (Seconds (duration));

    // Create object for mapper application
    for (int i = 0; i < MAPPER_COUNT; i++)
    {
        Ptr<mapper> mapperApp = CreateObject<mapper> (masterMapperPort[i], mapperClientPort, staNodeMapperInterface, staNodeClientInterface , i);
        wifiMapperNode.Get (i)->AddApplication (mapperApp);
        mapperApp->SetStartTime (Seconds (0.0));
        mapperApp->SetStopTime (Seconds (duration));
    }

    NS_LOG_INFO ("Run Simulation");

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    //ThroughputMonitor (&flowHelper, flowMonitor, error);
    //AverageDelayMonitor (&flowHelper, flowMonitor, error);

    Simulator::Stop (Seconds (duration));
    Simulator::Run ();

    return 0;
}

client::client(uint16_t masterClientPort, uint16_t mapperClientPort, Ipv4InterfaceContainer& client_ip, Ipv4InterfaceContainer& master_ip)
        : masterClientPort (masterClientPort),
          mapperClientPort (mapperClientPort),
          client_ip (client_ip),
          master_ip (master_ip)
{
    std::srand (time(0));
}

client::~client ()
{
}

static void GenerateTraffic (Ptr<Socket> socket, uint16_t data, std::string &input_message)
{
    Ptr<Packet> packet = new Packet();
    MyHeader m;
    m.SetData(data);
    packet->AddHeader (m);
    packet->Print (std::cout);
    socket->Send(packet);
    //std::cout << packet->GetSize() << std::endl;
    input_message += to_string(data);
    //std::cout << input_message << std::endl;
    Simulator::Schedule (Seconds (0.1), &GenerateTraffic, socket, rand() % 26 , input_message);
}

void
client::StartApplication (void)
{
    // connect to udp connection created by master
    Ptr<Socket> sock = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress sockAddr (master_ip.GetAddress(0), masterClientPort);
    sock->Connect (sockAddr);

    GenerateTraffic(sock, 0 , input_message);
    //std::cout << "heyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy" << std::endl;

    // create udp connection to receive traffic from mappers
    mapperSocket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), mapperClientPort);
    mapperSocket->Bind (local);
    mapperSocket->SetRecvCallback (MakeCallback (&client::HandleRead, this));


    // Print final message after end of simulation
    Simulator::Schedule (Seconds (DURATION_TIME-0.001), &client::PrintMessage, this);

}

void
client::HandleRead (Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    while ((packet = mapperSocket->Recv ()))
    {
        //std::cout << "wefiwlfnelkwf :   "<<  packet->GetSize() << std::endl;
        MyHeader m;
        packet->RemoveHeader (m);
        // convert ascii to char and append to message
        message += (char)(m.GetData() + 97);

        //std::cout << "Message: " << message << std::endl;
    }
}

void
client::PrintMessage ()
{
    NS_LOG_INFO ("Message: " << message);
    std:: cout << "Message: " << message << std::endl;
    std:: cout << "Input Message: " << input_message << std::endl;
}

master::master (uint16_t masterClientPort, std::vector<uint16_t> masterMapperPort, Ipv4InterfaceContainer& master_ip, Ipv4InterfaceContainer& mapper_ip)
        : masterClientPort (masterClientPort),
          masterMapperPort (masterMapperPort),
          master_ip (master_ip),
          mapper_ip (mapper_ip)
{
    std::srand (time(0));
}

master::~master ()
{
}

void
master::StartApplication (void)
{
    // create udp connection to receive traffic from client
    socket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress local = InetSocketAddress (master_ip.GetAddress(0), masterClientPort);
    socket->Bind (local);


    // connect to tcp socket of mappers to send message to each mapper
    for (int i = 0; i < MAPPER_COUNT ; i++)
    {
        // connect to mapper and add push it to tcpSocket vector
        tcpSocket.push_back (Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId ()));
        tcpSocket[i]->Connect (InetSocketAddress (mapper_ip.GetAddress(i), masterMapperPort[i]));
    }
    

    //std::cout << "heyyyyyy" << std::endl;

    // Send received traffic from client to mappers
    socket->SetRecvCallback (MakeCallback (&master::HandleRead, this));
}

void 
master::HandleRead (Ptr<Socket> socket)
{
    Ptr<Packet> packet;

    while ((packet = socket->Recv ()))
    {
        if (packet->GetSize () == 0)
        {
            break;
        }

        // Add header to packet
        MyHeader m;
        packet->RemoveHeader (m);
        buffer.push_back (m);

        Ptr<Packet> new_packet = new Packet();
        new_packet->AddHeader (m);
        // Send received packet to mappers
        for (int i = 0; i < MAPPER_COUNT ; ++i)
        {
            tcpSocket[i]->Send (new_packet);
        }

        // save packet in local buffer
        // MyHeader data;
        // packet->RemoveHeader(data);
        // buffer.push_back (data);

        // Print data
        // std::cout << "Data: " << data.GetData() << std::endl;
    }
}

mapper::mapper (uint16_t masterMapperPort, uint16_t mapperClientPort, Ipv4InterfaceContainer& mapper_ip, Ipv4InterfaceContainer& client_ip, int this_mapper_id)
        : masterMapperPort (masterMapperPort),
          mapperClientPort (mapperClientPort),
          mapper_ip (mapper_ip),
          client_ip (client_ip),
          this_mapper_id (this_mapper_id)
{
    std::srand (time(0));
}

mapper::~mapper ()
{
}

void
mapper::StartApplication (void)
{
    // Set Encoder to all mappers (for each mapper almost equal number of unique alphabets) : 1 -> a , 2 -> b , ... and if we have 3 mappers then first mapper would have 1 to 9, second 10 to 18 and third 19 to 26
    // if 3 mappers : first mapper 0 to 8, second 9 to 17 and third 18 to 26
    int start = (this_mapper_id * 26) / 3;
    int end = ((this_mapper_id + 1) * 26) / 3;
    int j =0;
    for (int i = start; i <= end; i++)
    {
        // {1 , a} , {2 , b} , ...
        encoder[j] = std::make_pair (i+1, (char)(i+97));
        //std::cout << "encoder[i]: " << i << "  "<< encoder[i] << std::endl;
    }

    // Creat tcp connection for each mapper to receive message from master
    tcpSocket = Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId ());
    InetSocketAddress local1 = InetSocketAddress (mapper_ip.GetAddress(this_mapper_id), masterMapperPort);
    tcpSocket->Bind (local1);
    tcpSocket->Listen ();

    //std::cout << "Handle Acceptaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
    // Connect to udp connection created by client
    udpSocket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress local2 = InetSocketAddress (Ipv4Address::GetAny (), mapperClientPort);
    udpSocket->Bind (local2);
    udpSocket->Connect (InetSocketAddress (client_ip.GetAddress(0), mapperClientPort));

    
    tcpSocket->SetAcceptCallback (MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
                                  MakeCallback (&mapper::HandleAccept, this));
}

void
mapper::HandleAccept (Ptr<Socket> s, const Address& from)
{

    //std::cout << "Handle Accept" << std::endl;
    NS_LOG_INFO ("Received one connection from " << InetSocketAddress::ConvertFrom(from).GetIpv4 ());
    //tcpSocket = s;
    s->SetRecvCallback (MakeCallback (&mapper::HandleRead, this));
}

void
mapper::HandleRead (Ptr<Socket> socket)
{
    Ptr<Packet> packet;

    while ((packet = socket->Recv ()))
    {
        if (packet->GetSize () == 0)
        {
            break;
        }

        // Decode data
        MyHeader data;
        packet->RemoveHeader(data);
        std::cout << "Received Data: " << data.GetData() << std::endl;
        char decodedData = Decode(data.GetData());
        std::cout << "decodedData : " << decodedData << std::endl;

        // Send decoded data to client
        if (decodedData != '&')
        {
            //std::cout << "Data: " << data.GetData() << std::endl;
            //std::cout << "Decoded Data: " << decodedData << std::endl;
            // convert decodedData to ASCII uint16_t
            uint16_t ascii = decodedData;
            std::cout << "ASCII: " << ascii << std::endl;
            //std::cout << "ASCII: " << ascii << std::endl;
            // Add header to packet
            MyHeader m;
            m.SetData(ascii);
            Ptr<Packet> new_packet = new Packet();
            new_packet->AddHeader (m);
            udpSocket->Send (new_packet);
        }
    }
}

char
mapper::Decode (uint16_t data)
{
    //std::cout << "Size : " << encoder.size() << std::endl;
    // Decode data using encoder : return "&" if Decode was not successful
    
    
}