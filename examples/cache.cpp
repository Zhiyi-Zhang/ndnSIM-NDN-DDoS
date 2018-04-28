// cache.cpp
// Please make sure each time to set a different RngRun
// ./waf --run "cache --RngRun=2 --cacheSize=200 --maxRange=500 --frequency=20 --output=test"
// Cache: Cache withhold much traffic

#include "helper.hpp"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int
main(int argc, char* argv[]) {
  // parameters
  std::string cacheSize = "200";
  std::string maxRange = "10000";
  std::string frequency = "200";
  std::string topo = "fake-interest-ddos";
  std::string outFile = "test";


  CommandLine cmd;
  cmd.AddValue("cacheSize", "Cache Size", cacheSize);
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
  ndnHelper.setCsSize(std::stoi(cacheSize));
  ndnHelper.InstallAll();

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Install attackers
  NodeContainer attackers;
  fillAttackerContainer(attackers);
  ndn::AppHelper consumerHelper("ConsApp");
  consumerHelper.SetAttribute("Names", StringValue("/u1"));
  consumerHelper.SetAttribute("Frequency", StringValue(frequency));
  consumerHelper.SetAttribute("MaxSeq", StringValue(maxRange));
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  for (int i = 0; i < attackers.size(); i++) {
    ApplicationContainer evilApp;
    int init = static_cast<int>(x->GetValue()*(std::stoi(maxRange) - 1));
    consumerHelper.SetAttribute("InitSeq", IntegerValue(init));
    evilApp.Add(consumerHelper.Install(attackers[i]));
    evilApp.Start(Seconds (3.0));
  }

  // Getting producers
  Ptr<Node> as1_cs_server = Names::Find<Node>("as1-cs-server");
  ndn::AppHelper producerHelper("ProdApp");
  producerHelper.SetPrefix("/u1");
  producerHelper.Install(as1_cs_server);

  ndnGlobalRoutingHelper.AddOrigins("/u1", as1_cs_server);
  ndnGlobalRoutingHelper.CalculateRoutes();

  Simulator::Stop(Seconds(20.0));
  ndn::L3RateTracer::InstallAll("src/ndnSIM/Results/cache/" + outFile + ".txt",
                                Seconds(0.5));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
}

int
main(int argc, char* argv[]) {
  return ns3::main(argc, argv);
}
