// b-1-1.cpp
// Please make sure each time to set a different RngRun
// ./waf --run "fake-interest-ddos --RngRun=2 --maxRange=200 --frequency=20 --topo=meshed-bad --output=test"
// B-1: Interest Aggregation with valid Interests

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/NFD/daemon/fw/ddos-strategy.hpp"

namespace ns3 {

static void
fillAttackerContainer(NodeContainer& container)
{
  std::vector<std::string> asName = {"as1", "as2", "as3", "as4"};
  std::vector<std::string> middleName = {"cs", "math", "sm", "hw"};

  std::string target = "";
  for (auto as : asName) {
    for (auto middle : middleName) {
      for (int i = 0; i < 5; i++) {
        target = as + "-" + middle + "-" + "a" + std::to_string(i);
        auto cons = Names::Find<Node>(target);
        if (cons != nullptr) {
          container.Add(cons);
        }
      }
    }
  }
}

static void
fillRoutersContainer(NodeContainer& container)
{
  std::vector<std::string> asName = {"as1", "as2", "as3", "as4"};
  std::vector<std::string> middleName = {"1", "2", "sm", "hw", "cs", "math"};

  std::string target = "";
  for (auto as : asName) {
    for (auto middle : middleName) {
      target = as + "-" + middle;
      auto cons = Names::Find<Node>(target);
      if (cons != nullptr) {
        container.Add(cons);
      }
    }
  }
}

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
  ndnHelper.SetDefaultRoutes(false);
  ndnHelper.InstallAll();

  ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/ddos");

  NodeContainer allNodes = topologyReader.GetNodes();
  NodeContainer attackers;
  NodeContainer routers;
  fillRoutersContainer(routers);
  fillAttackerContainer(attackers);

  ndn::AppHelper consumerHelper("ConsApp");
  consumerHelper.SetAttribute("Name", StringValue("/edu/u1/cs/server"));
  consumerHelper.SetAttribute("Frequency", StringValue(frequency));
  consumerHelper.SetAttribute("MaxSeq", StringValue(maxRange));
  consumerHelper.SetAttribute("ValidInterest", BooleanValue(false));
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();

  for (int i = 0; i < attackers.size(); i++) {
    int init = static_cast<int>(x->GetValue()*(std::stoi(maxRange) - 1));
    consumerHelper.SetAttribute("InitSeq", IntegerValue(init));
    consumerHelper.Install(attackers[i]);
  }

  // Getting producers
  Ptr<Node> as1_cs_server = Names::Find<Node>("as1-cs-server");
  ndn::AppHelper producerHelper("DDoSProdApp");
  producerHelper.SetPrefix("/edu/u1/cs/server");
  producerHelper.Install(as1_cs_server);

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();
  ndnGlobalRoutingHelper.AddOrigins("/edu/u1/cs/server", as1_cs_server);
  Ptr<Node> as1_cs = Names::Find<Node>("as1-cs");
  ndnGlobalRoutingHelper.AddOrigins("/edu/u1/cs", as1_cs);
  ndnGlobalRoutingHelper.CalculateRoutes();

  Simulator::Stop(Seconds(3.0));
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
