// b-1-1.cpp
// Please make sure each time to set a different RngRun
// ./waf --run "valid-interest-aggregation --RngRun=2 --maxRange=200 --frequency=20 --topo=meshed-bad --output=200-20-bad"
// B-1: Interest Aggregation with valid Interests

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int
main(int argc, char* argv[]) {

  // parameters
  std::string maxRange = "10000";
  std::string frequency = "200";
  std::string topo = "meshed-bad";
  std::string outFile = "raw";

  CommandLine cmd;
  cmd.AddValue("maxRange", "Max Data Range", maxRange);
  cmd.AddValue("frequency", "Sending Frequency", frequency);
  cmd.AddValue("topo", "Topology File", topo);
  cmd.AddValue("output", "Output File Name", outFile);
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/" + topo + ".txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  // ndnHelper.setCsSize(200);
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Nocache");
  ndnHelper.InstallAll();

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Install attackers
  Ptr<Node> attackers[35] = {Names::Find<Node>("as1-cs-a0"),
                             Names::Find<Node>("as1-cs-a1"),
                             Names::Find<Node>("as1-cs-a2"),
                             Names::Find<Node>("as1-cs-a3"),
                             Names::Find<Node>("as1-cs-a4"),
                             Names::Find<Node>("as1-math-a0"),
                             Names::Find<Node>("as1-math-a1"),
                             Names::Find<Node>("as1-math-a2"),
                             Names::Find<Node>("as1-math-a3"),
                             Names::Find<Node>("as1-math-a4"),
                             Names::Find<Node>("as2-cs-a0"),
                             Names::Find<Node>("as2-cs-a1"),
                             Names::Find<Node>("as2-cs-a2"),
                             Names::Find<Node>("as2-cs-a3"),
                             Names::Find<Node>("as2-cs-a4"),
                             Names::Find<Node>("as2-math-a0"),
                             Names::Find<Node>("as2-math-a1"),
                             Names::Find<Node>("as2-math-a2"),
                             Names::Find<Node>("as2-math-a3"),
                             Names::Find<Node>("as2-math-a4"),
                             Names::Find<Node>("as3-cs-a0"),
                             Names::Find<Node>("as3-cs-a1"),
                             Names::Find<Node>("as3-cs-a2"),
                             Names::Find<Node>("as3-cs-a3"),
                             Names::Find<Node>("as3-cs-a4"),
                             Names::Find<Node>("as4-hw-a0"),
                             Names::Find<Node>("as4-hw-a1"),
                             Names::Find<Node>("as4-hw-a2"),
                             Names::Find<Node>("as4-hw-a3"),
                             Names::Find<Node>("as4-hw-a4"),
                             Names::Find<Node>("as4-sm-a0"),
                             Names::Find<Node>("as4-sm-a1"),
                             Names::Find<Node>("as4-sm-a2"),
                             Names::Find<Node>("as4-sm-a3"),
                             Names::Find<Node>("as4-sm-a4")};


  ndn::AppHelper consumerHelper("ConsApp");
  consumerHelper.SetAttribute("Names", StringValue("/edu/u1/cs/server"));
  consumerHelper.SetAttribute("Frequency", StringValue(frequency));
  // consumerHelper.SetAttribute("Randomize", StringValue("uniform"));
  consumerHelper.SetAttribute("MaxSeq", StringValue(maxRange));
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  for (int i = 0; i < 35; i++) {
    int init = static_cast<int>(x->GetValue()*(std::stoi(maxRange) - 1));
    consumerHelper.SetAttribute("InitSeq", IntegerValue(init));
    consumerHelper.Install(attackers[i]);
  }

  // Getting producers
  Ptr<Node> as1_cs_server = Names::Find<Node>("as1-cs-server");

  ndn::AppHelper producerHelper("ProdApp");
  ndnGlobalRoutingHelper.AddOrigins("/edu/u1/cs/server", as1_cs_server);
  producerHelper.SetPrefix("/edu/u1/cs/server");
  producerHelper.Install(as1_cs_server);

  Ptr<Node> as1_cs = Names::Find<Node>("as1-cs");
  ndnGlobalRoutingHelper.AddOrigins("/edu/u1/cs", as1_cs);
  ndnGlobalRoutingHelper.CalculateRoutes();

  Simulator::Stop(Seconds(20.0));
  ndn::L3RateTracer::InstallAll("src/ndnSIM/Results/valid-interest-aggregation/" + outFile + ".txt",
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
