/*
 * create the network basic tohopoly 
 * and releasize the pattern 2
 */

#include"ns3/core-module.h"
#include"ns3/network-module.h"
#include"ns3/csma-module.h"
#include"ns3/internet-module.h"
#include"ns3/point-to-point-module.h"
#include"ns3/applications-module.h"
#include"ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Pattern2");

int main(int argc, char *argv[])
{
  
  bool verbose = true;
  if(verbose)
  {
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
  }

  // create the core node the top two aggregation nodes;
  NodeContainer coreNode1;
  coreNode1.Create(2);

  // because the c1 already created in the coreNode1
  NodeContainer coreNode2;
  coreNode2.Add(coreNode1.Get(0));
  coreNode2.Create(1);

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

  // set the address for core
  Ipv4AddressHelper address;
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer coreP2PInterfaces1;
  coreP2PInterfaces1 = address.Assign(deviceCore1);
  
  address.SetBase("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer coreP2PInterfaces2;
  coreP2PInterfaces2 = address.Assign(deviceCore2);

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
  
  // set the node1 ad sinkApp
  PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(1), 8080));
  ApplicationContainer sinkApp = packetSinkHelper.Install(torNode1.Get(1));
  sinkApp.Start(Seconds(0.0));
  sinkApp.Stop(Seconds(80.0));
  csma.EnablePcap("N1 received packets", deviceTor1.Get(1), true); 

  // set the linknode from n2 to n1
  OnOffHelper client1("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(1), 8080));
  client1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client1.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client1.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp1;
  clientApp1 = client1.Install(torNode1.Get(2));
  clientApp1.Start(Seconds(1.0));
  clientApp1.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern2 n2 to n1", deviceTor1.Get(2), true);

  
  // set the linknode from n3 to n1
  OnOffHelper client2("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(1), 8080));
  client2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client2.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client2.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp2;
  clientApp2 = client2.Install(torNode2.Get(1));
  clientApp2.Start(Seconds(1.0));
  clientApp2.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern2 n3 to n1", deviceTor2.Get(1), true);

  // set the linknode from n4 to n1
  OnOffHelper client3("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(1), 8080));
  client3.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client3.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client3.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client3.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp3;
  clientApp3 = client3.Install(torNode2.Get(2));
  clientApp3.Start(Seconds(1.0));
  clientApp3.Stop(Seconds(21.0));

  csma.EnablePcap("Pattern2 n4 to n1", deviceTor2.Get(2), true);

  // set the linknode from n5 to n1
  OnOffHelper client4("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(1), 8080));
  client4.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client4.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client4.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client4.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp4;
  clientApp4 = client4.Install(torNode3.Get(1));
  clientApp4.Start(Seconds(1.0));
  clientApp4.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern2 n5 to n1", deviceTor3.Get(1), true);
  
  // set the linknode from n6 to n1
  OnOffHelper client5("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(1), 8080));
  client5.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client5.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client5.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client5.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp5;
  clientApp5 = client5.Install(torNode3.Get(2));
  clientApp5.Start(Seconds(1.0));
  clientApp5.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern2 n6 to n1", deviceTor3.Get(2), true);

  // set the linknode from n7 to n1
  OnOffHelper client6("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(1), 8080));
  client6.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client6.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client6.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client6.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp6;
  clientApp6 = client6.Install(torNode4.Get(1));
  clientApp6.Start(Seconds(1.0));
  clientApp6.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern2 n7 to n1", deviceTor4.Get(1), true);

  // set the linknode from n8 to n1
  OnOffHelper client7("ns3::TcpSocketFactory", InetSocketAddress(torCsmaInterfaces1.GetAddress(1), 8080));
  client7.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
  client7.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  client7.SetAttribute("DataRate", DataRateValue(DataRate("1.0Mbps")));
  client7.SetAttribute("PacketSize", UintegerValue(2000));
  
  ApplicationContainer clientApp7;
  clientApp7 = client7.Install(torNode4.Get(2));
  clientApp7.Start(Seconds(1.0));
  clientApp7.Stop(Seconds(21.0));
  
  csma.EnablePcap("Pattern2 n8 to n1", deviceTor4.Get(2), true);

  // record the ptp trace
  corePTP1.EnablePcapAll("Pattern2 corePTP1");
  corePTP2.EnablePcapAll("Pattern2 corePTP2");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // execeed the simulator  
  Simulator::Run();
  Simulator::Destroy();
  return 0;

}
