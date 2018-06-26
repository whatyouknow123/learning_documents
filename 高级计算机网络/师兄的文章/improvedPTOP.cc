/* 
*SA15011113 Àî×ÔÇ¿
*¸ß¼¶¼ÆËã»úÍøÂçÊµÑé
*/

#include <stdio.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int
main(int argc, char *argv[])
{
	//define an enable log  module
	LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
	LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
	
//**********Create network********//
// create bottom network
	NodeContainer Nodes1,Nodes2,Nodes3,Nodes4;
	Nodes1.Create(3);
	Nodes2.Create(3);
	Nodes3.Create(3);
	Nodes4.Create(3);
//  create middle network
	NodeContainer Nodes5;
	Nodes5.Add(Nodes1.Get(2));
	Nodes5.Add(Nodes2.Get(2));
	Nodes5.Create(1);

	NodeContainer Nodes6;
	Nodes6.Add(Nodes1.Get(2));
	Nodes6.Add(Nodes2.Get(2));
	Nodes6.Create(1);
	

	NodeContainer Nodes7;
	Nodes7.Add(Nodes3.Get(2));
	Nodes7.Add(Nodes4.Get(2));
	Nodes7.Create(1);

	NodeContainer Nodes8;
	Nodes8.Add(Nodes3.Get(2));
	Nodes8.Add(Nodes4.Get(2));	
	Nodes8.Create(1);


	NodeContainer Nodes9;
	Nodes9.Add(Nodes5.Get(2));
	Nodes9.Add(Nodes7.Get(2));
	Nodes9.Create(1);

	NodeContainer Nodes10;
	Nodes10.Add(Nodes6.Get(2));
	Nodes10.Add(Nodes8.Get(2));
	Nodes10.Create(1);
	

	/*NodeContainer Nodes11;
	Nodes11.Add(Nodes6.Get(2));
	Nodes11.Add(Nodes8.Get(2));
	Nodes11.Create(1);

	NodeContainer Nodes12;
	Nodes12.Add(Nodes8.Get(2));
	Nodes12.Add(Nodes11.Get(1));*/

	//************************Define Network****************//

	printf("**********1*************\n");
	CsmaHelper csma1;
	csma1.SetChannelAttribute("DataRate", StringValue("1.0Mbps"));
	csma1.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices1;
	csmaDevices1 = csma1.Install(Nodes1);

	CsmaHelper csma2;
	csma2.SetChannelAttribute("DataRate", StringValue("1.0Mbps"));
	csma2.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices2;
	csmaDevices2 = csma2.Install(Nodes2);

	CsmaHelper csma3;
	csma3.SetChannelAttribute("DataRate", StringValue("1.0Mbps"));
	csma3.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices3;
	csmaDevices3 = csma3.Install(Nodes3);

	CsmaHelper csma4;
	csma4.SetChannelAttribute("DataRate", StringValue("1.0Mbps"));
	csma4.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices4;
	csmaDevices4 = csma4.Install(Nodes4);

	CsmaHelper csma5;
	csma5.SetChannelAttribute("DataRate", StringValue("1.0Mbps"));
	csma5.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices5;
	csmaDevices5 = csma5.Install(Nodes5);

	CsmaHelper csma6;
	csma6.SetChannelAttribute("DataRate", StringValue("1.0Mbps"));
	csma6.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices6;
	csmaDevices6 = csma6.Install(Nodes6);

	CsmaHelper csma7;
	csma7.SetChannelAttribute("DataRate", StringValue("1.0Mbps"));
	csma7.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices7;
	csmaDevices7 = csma7.Install(Nodes7);

	CsmaHelper csma8;
	csma8.SetChannelAttribute("DataRate", StringValue("1.0Mbps"));
	csma8.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices8;
	csmaDevices8 = csma8.Install(Nodes8);

	CsmaHelper csma9;
	csma9.SetChannelAttribute("DataRate", StringValue("1.5Mbps"));
	csma9.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices9;
	csmaDevices9 = csma9.Install(Nodes9);

	CsmaHelper csma10;
	csma10.SetChannelAttribute("DataRate", StringValue("1.5Mbps"));
	csma10.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices10;
	csmaDevices10 = csma10.Install(Nodes10);

	/*CsmaHelper csma11;
	csma11.SetChannelAttribute("DataRate", StringValue("1.5Mbps"));
	csma11.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices11;
	csmaDevices11 = csma11.Install(Nodes11);

	CsmaHelper csma12;
	csma12.SetChannelAttribute("DataRate", StringValue("1.5Mbps"));
	csma12.SetChannelAttribute("Delay", TimeValue(NanoSeconds(500)));
	NetDeviceContainer csmaDevices12;
	csmaDevices12 = csma12.Install(Nodes12);*/

printf("**********2*************\n");

	//Install protecal stack.
	InternetStackHelper stack;
	stack.Install(Nodes1);
	stack.Install(Nodes2);
	stack.Install(Nodes3);
	stack.Install(Nodes4);
	printf("-------1------\n");
/*	stack.Install(Nodes5);
	stack.Install(Nodes6);
	stack.Install(Nodes7);
	stack.Install(Nodes8);*/
	stack.Install(Nodes9);
	printf("-------2------\n");
	stack.Install(Nodes10);
	printf("-------3------\n");

printf("**********3*************\n");
	// dispatch network address
	Ipv4AddressHelper address;
	address.SetBase("10.0.0.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces1;
	csmaInterfaces1 = address.Assign(csmaDevices1);
	
	address.SetBase("10.0.1.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces2;
	csmaInterfaces2 = address.Assign(csmaDevices2);

	address.SetBase("10.1.0.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces3;
	csmaInterfaces3 = address.Assign(csmaDevices3);

	address.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces4;
	csmaInterfaces4 = address.Assign(csmaDevices4);

	address.SetBase("10.0.2.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces5;
	csmaInterfaces5 = address.Assign(csmaDevices5);

	address.SetBase("10.0.3.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces6;
	csmaInterfaces6 = address.Assign(csmaDevices6);

	address.SetBase("10.1.2.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces7;
	csmaInterfaces7 = address.Assign(csmaDevices7);

	address.SetBase("10.1.3.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces8;
	csmaInterfaces8 = address.Assign(csmaDevices8);


	address.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces9;
	csmaInterfaces9 = address.Assign(csmaDevices9);

	address.SetBase("192.168.2.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces10;
	csmaInterfaces10 = address.Assign(csmaDevices10);

	/*address.SetBase("192.168.3.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces11;
	csmaInterfaces11 = address.Assign(csmaDevices11);

	address.SetBase("192.168.4.0", "255.255.255.0");
	Ipv4InterfaceContainer csmaInterfaces12;
	csmaInterfaces12 = address.Assign(csmaDevices12);*/


printf("**********4*************\n");
	//TCP part
	int16_t sinkPort = 8080;
	Address sinkAddress_1_5(InetSocketAddress(csmaInterfaces1.GetAddress(0), sinkPort));
	PacketSinkHelper packetSinkHelper_1_5("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
	ApplicationContainer sinkApp_1_5 = packetSinkHelper_1_5.Install(Nodes1.Get(0));
	sinkApp_1_5.Start(Seconds(0.));
	sinkApp_1_5.Stop(Seconds(50.));

	OnOffHelper client_1_5("ns3::TcpSocketFactory", InetSocketAddress(csmaInterfaces1.GetAddress(0), sinkPort));
	client_1_5.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client_1_5.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client_1_5.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
	client_1_5.SetAttribute("PacketSize", UintegerValue(2000));
	ApplicationContainer clientApp_1_5 = client_1_5.Install(Nodes3.Get(0));
	clientApp_1_5.Start(Seconds(1.0));
	clientApp_1_5.Stop(Seconds(51.0));
	
	Address sinkAddress_6_2(InetSocketAddress(csmaInterfaces3.GetAddress(1), sinkPort));
	PacketSinkHelper packetSinkHelper_6_2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
	ApplicationContainer sinkApp_6_2 = packetSinkHelper_6_2.Install(Nodes3.Get(1));
	sinkApp_6_2.Start(Seconds(0.));
	sinkApp_6_2.Stop(Seconds(50.));
	
	OnOffHelper client_6_2("ns3::TcpSocketFactory", InetSocketAddress(csmaInterfaces3.GetAddress(1), sinkPort));
	client_6_2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client_6_2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client_6_2.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
	client_6_2.SetAttribute("PacketSize", UintegerValue(2000));
	ApplicationContainer clientApp_6_2 = client_6_2.Install(Nodes1.Get(1));
	clientApp_6_2.Start(Seconds(1.0));
	clientApp_6_2.Stop(Seconds(51.0));
	
	Address sinkAddress_3_7(InetSocketAddress(csmaInterfaces2.GetAddress(0), sinkPort));
	PacketSinkHelper packetSinkHelper_3_7("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
	ApplicationContainer sinkApp_3_7 = packetSinkHelper_3_7.Install(Nodes2.Get(0));
	sinkApp_3_7.Start(Seconds(0.));
	sinkApp_3_7.Stop(Seconds(50.));
	
	OnOffHelper client_3_7("ns3::TcpSocketFactory", InetSocketAddress(csmaInterfaces2.GetAddress(0), sinkPort));
	client_3_7.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client_3_7.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client_3_7.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
	client_3_7.SetAttribute("PacketSize", UintegerValue(2000));
	ApplicationContainer clientApp_3_7 = client_3_7.Install(Nodes4.Get(0));
	clientApp_3_7.Start(Seconds(1.0));
	clientApp_3_7.Stop(Seconds(51.0));
	
	Address sinkAddress_8_4(InetSocketAddress(csmaInterfaces4.GetAddress(1), sinkPort));
	PacketSinkHelper packetSinkHelper_8_4("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
	ApplicationContainer sinkApp_8_4 = packetSinkHelper_8_4.Install(Nodes4.Get(1));
	sinkApp_8_4.Start(Seconds(0.));
	sinkApp_8_4.Stop(Seconds(50.));

	OnOffHelper client_8_4("ns3::TcpSocketFactory", InetSocketAddress(csmaInterfaces4.GetAddress(1), sinkPort));
	client_8_4.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=50]"));
	client_8_4.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	client_8_4.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
	client_8_4.SetAttribute("PacketSize", UintegerValue(2000));
	ApplicationContainer clientApp_8_4 = client_8_4.Install(Nodes2.Get(1));
	clientApp_8_4.Start(Seconds(1.0));
	clientApp_8_4.Stop(Seconds(51.0));
	//------------------------TCP LINK-------------------------------------
	
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
	
	/*csma12.EnablePcap("P-T-P", csmaDevices12.Get(1), true);
	csma12.EnablePcap("P-T-P", csmaDevices12.Get(0), true);

	csma11.EnablePcap("P-T-P", csmaDevices11.Get(1), true);
	csma11.EnablePcap("P-T-P", csmaDevices11.Get(0), true);*/

	csma10.EnablePcap("P-T-P", csmaDevices10.Get(1), true);
	csma10.EnablePcap("P-T-P", csmaDevices10.Get(0), true);



	csma9.EnablePcap("P-T-P", csmaDevices9.Get(1), true);
	csma9.EnablePcap("P-T-P", csmaDevices9.Get(0), true);



	csma8.EnablePcap("P-T-P", csmaDevices8.Get(2), true);
	csma8.EnablePcap("P-T-P", csmaDevices8.Get(1), true);
	csma8.EnablePcap("P-T-P", csmaDevices8.Get(0), true);


	csma7.EnablePcap("P-T-P", csmaDevices7.Get(2), true);
	csma7.EnablePcap("P-T-P", csmaDevices7.Get(1), true);
	csma7.EnablePcap("P-T-P", csmaDevices7.Get(0), true);

	csma6.EnablePcap("P-T-P", csmaDevices6.Get(2), true);
	csma6.EnablePcap("P-T-P", csmaDevices6.Get(1), true);
	csma6.EnablePcap("P-T-P", csmaDevices6.Get(0), true);

	csma5.EnablePcap("P-T-P", csmaDevices5.Get(2), true);
	csma5.EnablePcap("P-T-P", csmaDevices5.Get(1), true);
	csma5.EnablePcap("P-T-P", csmaDevices5.Get(0), true);

	csma4.EnablePcap("P-T-P", csmaDevices4.Get(2), true);
	csma4.EnablePcap("P-T-P", csmaDevices4.Get(1), true);
	csma4.EnablePcap("P-T-P", csmaDevices4.Get(0), true);

	csma3.EnablePcap("P-T-P", csmaDevices3.Get(2), true);
	csma3.EnablePcap("P-T-P", csmaDevices3.Get(1), true);
	csma3.EnablePcap("P-T-P", csmaDevices3.Get(0), true);

	csma2.EnablePcap("P-T-P", csmaDevices2.Get(2), true);
	csma2.EnablePcap("P-T-P", csmaDevices2.Get(1), true);
	csma2.EnablePcap("P-T-P", csmaDevices2.Get(0), true);

	csma1.EnablePcap("P-T-P", csmaDevices1.Get(2), true);
	csma1.EnablePcap("P-T-P", csmaDevices1.Get(1), true);
	csma1.EnablePcap("P-T-P", csmaDevices1.Get(0), true);


	Simulator::Stop(Seconds(50));

	Simulator::Run();
	Simulator::Destroy();
	return 0;
}
