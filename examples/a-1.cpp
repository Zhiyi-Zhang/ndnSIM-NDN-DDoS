// a-1.cpp
// ./waf --run=a-1
// A-1: Attacking locally reachable is hard

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int
main(int argc, char* argv[]) {
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/meshed-ases.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  // ndnHelper.setCsSize(200);
  // ndnHelper.SetOldContentStore("ns3::ndn::cs::Nocache");
  ndnHelper.InstallAll();

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Getting consumers

  // localized attackers
  Ptr<Node> as1_cs_a0 = Names::Find<Node>("as1-cs-a0");

  // global attackers
  Ptr<Node> as1_math_a0 = Names::Find<Node>("as1-math-a0");
  Ptr<Node> as2_cs_a0 = Names::Find<Node>("as2-cs-a0");
  Ptr<Node> as2_math_a0 = Names::Find<Node>("as2-math-a0");
  Ptr<Node> as3_cs_a0 = Names::Find<Node>("as3-cs-a0");
  Ptr<Node> as4_hw_a0 = Names::Find<Node>("as4-hw-a0");
  Ptr<Node> as4_sm_a0 = Names::Find<Node>("as4-sm-a0");

  ndn::AppHelper consumerHelper("ConsApp");
  consumerHelper.SetAttribute("Names", StringValue("/edu/u1/cs/backend"));
  consumerHelper.SetAttribute("Frequency", StringValue("20"));
  consumerHelper.SetAttribute("Randomize", StringValue("uniform"));
  consumerHelper.Install(as1_cs_a0);

  consumerHelper.SetAttribute("Names", StringValue("/edu/u1/cs/something"));
  consumerHelper.Install(as1_math_a0);
  consumerHelper.Install(as2_cs_a0);
  consumerHelper.Install(as2_math_a0);
  consumerHelper.Install(as3_cs_a0);
  consumerHelper.Install(as4_hw_a0);
  consumerHelper.Install(as4_sm_a0);

  // Getting producers
  Ptr<Node> as1_cs_server = Names::Find<Node>("as1-cs-server");
  Ptr<Node> as2_cs_backend = Names::Find<Node>("as1-cs-backend"); // target

  ndn::AppHelper producerHelper("ProdApp");

  ndnGlobalRoutingHelper.AddOrigins("/edu/u1/cs/server", as1_cs_server);
  producerHelper.SetPrefix("/edu/u1/cs/server");
  producerHelper.Install(as1_cs_server);

  producerHelper.SetPrefix("/edu/u1/cs/backend");
  producerHelper.Install(as2_cs_backend);

  Ptr<Node> as1_cs = Names::Find<Node>("as1-cs");
  ndnGlobalRoutingHelper.AddOrigins("/edu/u1/cs", as1_cs);

  ndnGlobalRoutingHelper.CalculateRoutes();

  ndn::FibHelper::AddRoute("as1-cs", "/edu/u1/cs/backend", "as1-cs-backend", 1);

  Simulator::Stop(Seconds(20.0));

  ndn::L3RateTracer::InstallAll("src/ndnSIM/Results/a-1.txt", Seconds(0.5));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[]) {
  return ns3::main(argc, argv);
}
