/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Morteza Kheirkhah <m.kheirkhah@sussex.ac.uk>
 */

// Network topology
//
//       n0 ----------- n1
// - Flow from n0 to n1 using MpTcpBulkSendApplication.

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/csma-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MpTcpBulkSendExample");

int
main(int argc, char *argv[])
{
  LogComponentEnable("MpTcpSocketBase", LOG_INFO);


  //Configure mptcp
  Config::SetDefault("ns3::Ipv4GlobalRouting::FlowEcmpRouting", BooleanValue(true));
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1400));
  Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(0));
  Config::SetDefault("ns3::DropTailQueue::Mode", StringValue("QUEUE_MODE_PACKETS"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(100));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(MpTcpSocketBase::GetTypeId()));
  Config::SetDefault("ns3::MpTcpSocketBase::MaxSubflows", UintegerValue(8)); // Sink
  //Below, we can set the congestion control algorithm
  //Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("RTT_Compensator"));
  //Below, we set the path management algorithm used. N diff ports uses one IP address and different port numbers while the one we should use later when are topology is compatible, is full mesh where multiple IPs are used
  Config::SetDefault("ns3::MpTcpSocketBase::PathManagement", StringValue("NdiffPorts"));

  //Enable logging for udp echo client and server
  //LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  //LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  //create the 18 nodes that are part of topology
  NodeContainer nodes;  
  nodes.Create(18);
  
  //have one access point
  NodeContainer accessPoint;
  accessPoint.Create(1);

  //create the channel and physical layer to be used by WifiHelper
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetChannel(channel.Create());


  //set up wifi settings
  WifiHelper wifi = WifiHelper::Default();
  wifi.SetRemoteStationManager("ns3::AarfWifiManager");


  //what type of MAC is being used
  NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
  
  //the ssid of the AP
  Ssid ssid = Ssid("AP1");
  //configure mac for mobile hosts
  mac.SetType("ns3::StaWifiMac",
              "Ssid", SsidValue(ssid),
              "ActiveProbing", BooleanValue(false));


  //create interfaces
  NetDeviceContainer staDevices;
  staDevices = wifi.Install(phy,mac,nodes);

  //configure mac for access point
  mac.SetType("ns3::ApWifiMac","Ssid", SsidValue(ssid));


  //create interface for accesspoint
  NetDeviceContainer apDevices;
  apDevices = wifi.Install(phy,mac, accessPoint);
  


  //this section sets up mobility for mobile hosts
  MobilityHelper mobility;

  mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(0.0), "MinY", DoubleValue(0.0), "DeltaX", DoubleValue(5.0), "DeltaY", DoubleValue(10.0), "GridWidth", UintegerValue(3), "LayoutType", StringValue("RowFirst"));

  mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Bounds", RectangleValue(Rectangle(-50,50,-50,50)));

  mobility.Install(nodes);

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(accessPoint);



  
  

  //install internet stack on hosts and assign addresses
  InternetStackHelper internet;
  internet.Install(nodes);
  internet.Install(accessPoint);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign(staDevices);
  ipv4.Assign(apDevices);




  
  //create the udp echo server on one device
  UdpEchoServerHelper echoServer(9);
  ApplicationContainer serverApps = echoServer.Install(nodes.Get(10));
  serverApps.Start(Seconds(1.0));
  serverApps.Start(Seconds(10.0));

  //create the udp echo client helper and set attributes
  UdpEchoClientHelper echoClient (i.GetAddress(10), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue(10000000));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(0.1)));
  echoClient.SetAttribute("PacketSize", UintegerValue(1024));
  ApplicationContainer clientApps;
  
  
  //install udp echo client on machines
  for(int i = 0; i < 18; i++)
  {
	if(i == 0 || i == 17 || i == 10)
        {
		continue;
	}

        clientApps.Add(echoClient.Install(nodes.Get(i)));
        

	
  }
  //clientApps = echoClient.Install(nodes.Get(1));
  clientApps.Start(Seconds(1.0));
  clientApps.Stop(Seconds(10.0));
  uint16_t port = 9;
  

  //set up mptcp packet sink and bulk sender. same as mptcp.cc
  MpTcpPacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps = sink.Install(nodes.Get(17));
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(10.0));

  MpTcpBulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(i.GetAddress(17)), port));
  source.SetAttribute("MaxBytes", UintegerValue(0));
  ApplicationContainer sourceApps = source.Install(nodes.Get(0));
  sourceApps.Start(Seconds(0.0));
  sourceApps.Stop(Seconds(10.0));
  
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(20.0));
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO ("Done.");

}
