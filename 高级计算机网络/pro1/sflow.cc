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
// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                1.5 Mbps, 10 ms
//
//===========================================================================

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SingleFlow");

int
main (int argc, char *argv[])
{
  LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
  LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("10ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  
  int16_t sinkPort = 8080;
  Address sinkAddress(InetSocketAddress(interfaces.GetAddress(1),sinkPort));
  PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(),sinkPort));
  ApplicationContainer sinkApp = packetSinkHelper.Install(nodes.Get(1));
  sinkApp.Start(Seconds(0.));
  sinkApp.Stop(Seconds(50.));
  
  OnOffHelper client("ns3::TcpSocketFactory", InetSocketAddress(interfaces.GetAddress (1), sinkPort));
  client.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client.SetAttribute ("DataRate", DataRateValue (DataRate ("1.5Mbps")));
  client.SetAttribute ("PacketSize", UintegerValue (2000));
  ApplicationContainer clientApp = client.Install (nodes.Get (0));
  clientApp.Start(Seconds (1.0 ));
  clientApp.Stop (Seconds (51.0));
  
  pointToPoint.EnablePcapAll ("sflow");
  
  Simulator::Stop (Seconds(50));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
