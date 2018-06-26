/*
 * create the network basic tohopoly 
 * and releasize the pattern 3
 * the pattern 3 add one more core
 */

#include"ns3/core-module.h"
#include"ns3/network-module.h"
#include"ns3/csma-module.h"
#include"ns3/internet-module.h"
#include"ns3/point-to-point-module.h"
#include"ns3/applications-module.h"
#include"ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Pattern3");

int main(int argc, char *argv[])
{
  
  bool verbose = true;
  if(verbose)
  {
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
  }
  
  // here we should set how things are routed at the ipv4gloabal routing module
  // TODO
  Config::SetDefault("ns3::Ipv4GlobalRouting::RandomEcmpRouting", BooleanValue("True"));
   
  // create the core node the top two aggregation nodes;
  NodeContainer coreNode1;
  coreNode1.Create(2);

  // because the c1 already created in the coreNode1
  NodeContainer coreNode2;
  coreNode2.Add(coreNode1.Get(0));
  coreNode2.Create(1);

  // create the added core
  // TODO
  NodeContainer coreNode3;
  coreNode3.Create(1);
  coreNode3.Add(coreNode1.Get(1));
  
  NodeContainer coreNode4;
  coreNode4.Add(coreNode3.Get(0));
  coreNode4.Add(coreNode2.Get(1));

  // create the full aggregation nodes
  NodeContainer aggNode1;
  aggNode1.Add(coreNode1.Get(1));
  aggNode1.Create(2);

  NodeContainer aggNode2;
  aggNode2.Add(coreNode2.Get(1));
  aggNode2.Create(2);

  // create the tor nodes in cluster1 and cluser2
  NodeContainer torNode1, torNode2, torNode3, torNode4;

  torNode1.Add(aggNode1.Get(1));
  torNode1.Create(2);

  torNode2.Add(aggNode1.Get(2));
  torNode2.Create(2);

  torNode3.Add(aggNode2.Get(1));
  torNode3.Create(2);
  
  torNode4.Add(aggNode2.Get(2));
  torNode4.Create(2);

  // create the core tohopoly
  PointToPointHelper corePTP1, corePTP2;
  corePTP1.SetDeviceAttribute("DataRate", StringValue("1.5Mbps"));
  corePTP1.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
  corePTP2.SetDeviceAttribute("DataRate", StringValue("1.5Mbps"));
  corePTP2.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));

  NetDeviceContainer deviceCore1, deviceCore2;
  deviceCore1 = corePTP1.Install(coreNode1);
  deviceCore2 = corePTP2.Install(coreNode2);
  deviceCore2 = corePTP2.Install(coreNode2);

  
  // create the added  core tohopoly
  // TODO
  PointToPointHelper corePTP3, corePTP4;
  corePTP3.SetDeviceAttribute("DataRate", StringValue("1.5Mbps"));
  corePTP3.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
  corePTP4.SetDeviceAttribute("DataRate", StringValue("1.5Mbps"));
  corePTP4.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));

  NetDeviceContainer deviceCore3, deviceCore4;
  deviceCore3 = corePTP3.Install(coreNode3);
  deviceCore4 = corePTP4.Install(coreNode4);

  // define the csma attribute
  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("1Mbps"));
  csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));

  // create the aggregation tohopoly
  NetDeviceContainer deviceAgg1, deviceAgg2;
  deviceAgg1 = csma.Install(aggNode1);
  deviceAgg2 = csma.Install(aggNode2);
  
  // create the tor tohopoly
  NetDeviceContainer deviceTor1, deviceTor2, deviceTor3, deviceTor4;
  deviceTor1 = csma.Install(torNode1);
  deviceTor2 = csma.Install(torNode2);
  deviceTor3 = csma.Install(torNode3);
  deviceTor4 = csma.Install(torNode4);

  // set the protocol stack
  InternetStackHelper stack;
  stack.Install(coreNode1);
  stack.Install(coreNode2.Get(1));
  stack.Install(torNode1);
  stack.Install(torNode2);
  stack.Install(torNode3);
  stack.Install(torNode4);

  // set the protocol stack for the added network
  // TODO
  stack.Install(coreNode3.Get(0));

  // set the address for core
  Ipv4AddressHelper address;
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer coreP2PInterfaces1;
  coreP2PInterfaces1 = address.Assign(deviceCore1);
  
  address.SetBase("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer coreP2PInterfaces2;
  coreP2PInterfaces2 = address.Assign(deviceCore2);

  // set the address for the added core
  // TODO
  address.SetBase("192.168.3.0", "255.255.255.0");
  Ipv4InterfaceContainer coreP2PInterfaces3;
  coreP2PInterfaces3 = address.Assign(deviceCore3);
  
  address.SetBase("192.168.4.0", "255.255.255.0");
  Ipv4InterfaceContainer coreP2PInterfaces4;
  coreP2PInterfaces4 = address.Assign(deviceCore4);

  // set the address for aggregation
  address.SetBase("10.1.1.0", "255.255.255.0"); 
  Ipv4InterfaceContainer aggCsmaInterfaces1;
  aggCsmaInterfaces1 = address.Assign(deviceAgg1);

  address.SetBase("10.2.1.0", "255.255.255.0");
  Ipv4InterfaceContainer aggCsmaInterfaces2;
  aggCsmaInterfaces2 = address.Assign(deviceAgg2);

  // set the address for tor
  address.SetBase("10.0.1.0", "255.255.255.0");
  Ipv4InterfaceContainer torCsmaInterfaces1;
  torCsmaInterfaces1 = address.Assign(deviceTor1);
 
  address.SetBase("10.0.2.0", "255.255.255.0");
  Ipv4InterfaceContainer torCsmaInterfaces2;
  torCsmaInterfaces2 = address.Assign(deviceTor2);
   
  address.SetBase("10.0.3.0", "255.255.255.0");
  Ipv4InterfaceContainer torCsmaInterfaces3;
  torCsmaInterfaces3 = address.Assign(deviceTor3);

  address.SetBase("10.0.4.0", "255.255.255.0");
  Ipv4InterfaceContainer torCsmaInterfaces4;
  torCsmaInterfaces4 = address.Assign(deviceTor4);
 
  // set the linknode from n1 to n5
  PacketSinkHelper packetSinkHelper1("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces3.GetAddress(1), 8080));
  ApplicationContainer sinkApp1;
  sinkApp1 = packetSinkHelper1.Install(torNode3.Get(1));
  sinkApp1.Start(Seconds(0.0));
  sinkApp1.Stop(Seconds(20.0));

  OnOffHelper client1("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces3.GetAddress(1), 8080));
  client1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client1.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client1.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp1;
  clientApp1 = client1.Install(torNode1.Get(1));
  clientApp1.Start(Seconds(1.0));
  clientApp1.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern3 n1 to n5", deviceTor3.Get(1), true);

  
  // set the linknode from n6 to n2
  PacketSinkHelper packetSinkHelper2("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(2), 8080));
  ApplicationContainer sinkApp2;
  sinkApp2 = packetSinkHelper2.Install(torNode1.Get(2));
  sinkApp2.Start(Seconds(0.0));
  sinkApp2.Stop(Seconds(20.0));

  OnOffHelper client2("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(2), 8080));
  client2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client2.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client2.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp2;
  clientApp2 = client2.Install(torNode3.Get(2));
  clientApp2.Start(Seconds(1.0));
  clientApp2.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern3 n6 to n2", deviceTor1.Get(2), true);

  // set the linknode from n3 to n7
  PacketSinkHelper packetSinkHelper3("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces4.GetAddress(1), 8080));
  ApplicationContainer sinkApp3;
  sinkApp3 = packetSinkHelper3.Install(torNode4.Get(1));
  sinkApp3.Start(Seconds(0.0));
  sinkApp3.Stop(Seconds(20.0));

  OnOffHelper client3("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces4.GetAddress(1), 8080));
  client3.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client3.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client3.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client3.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp3;
  clientApp3 = client3.Install(torNode2.Get(1));
  clientApp3.Start(Seconds(1.0));
  clientApp3.Stop(Seconds(21.0));

  csma.EnablePcap("Pattern3 n3 to n7", deviceTor4.Get(1), true);

  // set the linknode from n8 to n4
  PacketSinkHelper packetSinkHelper4("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces2.GetAddress(2), 8080));
  ApplicationContainer sinkApp4;
  sinkApp4 = packetSinkHelper4.Install(torNode2.Get(2));
  sinkApp4.Start(Seconds(0.0));
  sinkApp4.Stop(Seconds(20.0));

  OnOffHelper client4("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces2.GetAddress(2), 8080));
  client4.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client4.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client4.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client4.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp4;
  clientApp4 = client4.Install(torNode4.Get(2));
  clientApp4.Start(Seconds(1.0));
  clientApp4.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern3 n8 to n4", deviceTor2.Get(2), true);
  
  // record the ptp trace
  corePTP1.EnablePcapAll("Pattern3 corePTP1");
  corePTP2.EnablePcapAll("Pattern3 corePTP2");
  corePTP3.EnablePcapAll("Pattern3 corePTP3");
  corePTP4.EnablePcapAll("Pattern3 corePTP4");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // execeed the simulator  
  Simulator::Run();
  Simulator::Destroy();
  return 0;

}
