// b-1-1.cpp
// Please make sure each time to set a different RngRun
// ./waf --run "fake-interest-ddos --RngRun=2 --maxRange=400 --frequency=10 --withStrategy=true --topo=fake-interest-ddos --output=test"
// fake-interest-ddos topology contains: 12 good users, 60 attackers
// B-1: Interest Aggregation with valid Interests

#include "helper.hpp"
#include "ns3/ndnSIM/NFD/daemon/fw/ddos-strategy.hpp"

namespace ns3 {

int
main(int argc, char* argv[]) {

  Config::SetDefault("ns3::PointToPointNetDevice::Mtu", StringValue("9000"));

  // parameters
  std::string maxRange = "10000";
  std::string frequency = "200";
  std::string topo = "meshed-bad";
  std::string outFile = "raw";
  std::string useStrategy = "false";

  CommandLine cmd;
  cmd.AddValue("maxRange", "Max Data Range", maxRange);
  cmd.AddValue("frequency", "Sending Frequency", frequency);
  cmd.AddValue("withStrategy", "Whether use ddos strategy", useStrategy);
  cmd.AddValue("topo", "Topology File", topo);
  cmd.AddValue("output", "Output File Name", outFile);
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/" + topo + ".txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(false);
  ndnHelper.InstallAll();

  if (useStrategy == "true") {
    ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/ddos");
  }

  ndn::AppHelper consumerHelper("ConsApp");
  consumerHelper.SetAttribute("Name", StringValue("/u1/cs/server"));
  consumerHelper.SetAttribute("Frequency", StringValue(frequency));
  consumerHelper.SetAttribute("MaxSeq", StringValue(maxRange));
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();


  // NodeContainer consumers;
  // fillConsumerContainer(consumers);
  // for (int i = 0; i < consumers.size(); i++) {
  //   int init = static_cast<int>(x->GetValue()*(std::stoi(maxRange) - 1));
  //   consumerHelper.SetAttribute("InitSeq", IntegerValue(init));
  //   consumerHelper.Install(consumers[i]);
  // }

  consumerHelper.SetAttribute("ValidInterest", BooleanValue(false));
  NodeContainer attackers;
  fillAttackerContainer(attackers);
  for (int i = 0; i < attackers.size(); i++) {
    int init = static_cast<int>(x->GetValue()*(std::stoi(maxRange) - 1));
    consumerHelper.SetAttribute("InitSeq", IntegerValue(init));
    consumerHelper.Install(attackers[i]);
  }

  // Getting producers
  Ptr<Node> as1_cs_server = Names::Find<Node>("as1-cs-server");
  ndn::AppHelper producerHelper("DDoSProdApp");
  producerHelper.SetPrefix("/u1/cs/server");
  producerHelper.Install(as1_cs_server);

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();
  ndnGlobalRoutingHelper.AddOrigins("/u1/cs/server", as1_cs_server);
  Ptr<Node> as1_cs = Names::Find<Node>("as1-cs");
  ndnGlobalRoutingHelper.AddOrigins("/u1/cs", as1_cs);
  ndnGlobalRoutingHelper.CalculateRoutes();

  Simulator::Stop(Seconds(20.0));
  ndn::L3RateTracer::InstallAll("src/ndnSIM/Results/fake-interest-ddos/" + outFile + ".txt",
                                Seconds(0.5));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[]) {
  return ns3::main(argc, argv);
}
