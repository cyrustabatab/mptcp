
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
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/packet-sink.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int 
main (int argc, char *argv[])
{

  Config::SetDefault("ns3::Ipv4GlobalRouting::FlowEcmpRouting", BooleanValue(true));
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1400));
  Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(0));
  Config::SetDefault("ns3::DropTailQueue::Mode", StringValue("QUEUE_MODE_PACKETS"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(100));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(MpTcpSocketBase::GetTypeId()));
  Config::SetDefault("ns3::MpTcpSocketBase::PathManagement", StringValue("NdiffPorts"));
  //Config::SetDefault("ns3::Ipv4GlobalRouting::RandomEcmpRouting", BooleanValue(true));
  bool verbose = true;
  uint32_t nCsma = 18;
  uint32_t nWifi = 18;
  uint32_t nSubFlows = 8;
  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("nSubFlows", "Number of wifi STA devices", nSubFlows);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  Config::SetDefault("ns3::MpTcpSocketBase::MaxSubflows", UintegerValue(nSubFlows));

  cmd.Parse (argc,argv);

  if (nWifi > 18)
    {
      std::cout << "Number of wifi nodes " << nWifi << 
                   " specified exceeds the mobility bounding box" << std::endl;
      exit (1);
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  //NodeContainer p2pNodes;
  //p2pNodes.Create (2);

  
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  /*
  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);
  */
  
  NodeContainer csmaNodes;
  //csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  wifiStaNodes.Add(csmaNodes.Get(0));
  NodeContainer wifiApNode;
  wifiApNode.Create(1);
  
  NodeContainer secondLAN;
  secondLAN.Add(wifiApNode.Get(0));
  secondLAN.Create(nCsma);
  
  NetDeviceContainer secondLANDevices;
  secondLANDevices = csma.Install(secondLAN);

  /*
  NodeContainer p2pNodes;
  p2pNodes.Add(csmaNodes.Get(nCsma - 1));
  p2pNodes.Add(secondLAN.Get(nCsma));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install(p2pNodes);
  */
  NodeContainer thirdLAN;
  thirdLAN.Add(csmaNodes.Get(nCsma - 1));
  thirdLAN.Create(nCsma);
  thirdLAN.Add(secondLAN.Get(nCsma));
  
  NetDeviceContainer thirdLANDevices;
  thirdLANDevices = csma.Install(thirdLAN);
  
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();

  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  for(unsigned int i = 0; i < nWifi; i++)
  {
	mobility.Install(wifiStaNodes.Get(i));
  }
  //mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  mobility.Install(wifiStaNodes.Get(nWifi));

  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  
  for(unsigned int i = 0; i < nWifi; i++)
  {
	stack.Install(wifiStaNodes.Get(i));
  }

  for(unsigned int i = 1; i <= nCsma; i++)
  {
	stack.Install(secondLAN.Get(i));
  }
  
  for(unsigned int i = 1; i <= nCsma; i++)
  {
	stack.Install(thirdLAN.Get(i));
  }
  

  Ipv4AddressHelper address;

  
  /*
  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);
  */
  
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces;
  staInterfaces = address.Assign (staDevices);
  address.Assign (apDevices);

  address.SetBase("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer secondLANInterfaces;
  secondLANInterfaces = address.Assign(secondLANDevices);
  
  
  address.SetBase("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer thirdLANInterfaces;
  thirdLANInterfaces = address.Assign(thirdLANDevices);
  
  
  MpTcpPacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9));
  ApplicationContainer sinkApps = sink.Install(secondLAN.Get(10));
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(10.0));
  
  /*
  UdpEchoServerHelper echoServer (9);
  
  ApplicationContainer serverApps = echoServer.Install (secondLAN.Get(10));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  */
  
  //cout << "Here" << endl;
  MpTcpBulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(secondLANInterfaces.GetAddress(10)), 9));
  source.SetAttribute("MaxBytes", UintegerValue(0));
  ApplicationContainer sourceApps = source.Install(csmaNodes.Get(0));
  sourceApps.Start(Seconds(0.0));
  sourceApps.Stop(Seconds(10.0));
  //cout << "Here1" << endl;
  /*
  UdpEchoClientHelper echoClient (secondLANInterfaces.GetAddress(10), 9);
  //source.SetAttribute("MaxBytes", UIntegerValue(0));
  //ApplicationContainer sourceApps = source.Install(nodes.Get(0))
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = 
    echoClient.Install (thirdLAN.Get (1 ));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  */  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  Simulator::Stop (Seconds (20.0));
  //cout << "Here2" << endl;
  //pointToPoint.EnablePcapAll ("third");
  //phy.EnablePcap ("third", apDevices.Get (0));
  //csma.EnablePcap ("third", csmaDevices.Get (0), true);
  
  Simulator::Run ();
  
  Simulator::Destroy ();
  
  return 0;
}


