// ndn-simple.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/NFD/daemon/fw/ddos-strategy.hpp"
// #include "ns3/ndnSIM/apps/ddos-app-prod.hpp"

namespace ns3 {

int
main(int argc, char* argv[])
{
  // setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::Mtu", StringValue("9000"));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Creating nodes
  NodeContainer nodes;
  nodes.Create(3);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/ndn-simple.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/ddos");

  // Consumer
  ndn::AppHelper consumerHelper("ConsApp");
  // Consumer will request /prefix/0, /prefix/1, ...
  consumerHelper.SetAttribute("Name", StringValue("/prefix"));
  consumerHelper.SetAttribute("Frequency", StringValue("15"));
  consumerHelper.SetAttribute("MaxSeq", StringValue("1000"));
  consumerHelper.SetAttribute("ValidInterest", BooleanValue(false));
  consumerHelper.Install(Names::Find<Node>("node1"));

  // Producer
  ndn::AppHelper producerHelper("DDoSProdApp");
  producerHelper.SetPrefix("/prefix");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.SetAttribute("ValidThreshold", StringValue("10"));
  producerHelper.SetAttribute("FakeThreshold", StringValue("10"));
  producerHelper.Install(Names::Find<Node>("node3"));

  Simulator::Stop(Seconds(5.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
