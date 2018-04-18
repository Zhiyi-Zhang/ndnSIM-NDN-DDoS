#ifndef DDOS_SIM_HELPER_HPP
#define DDOS_SIM_HELPER_HPP

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

static void
fillAttackerContainer(NodeContainer& container)
{
  std::vector<std::string> asName = {"as1", "as2", "as3", "as4"};
  std::vector<std::string> middleName = {"cs", "math", "sm", "hw"};

  std::string target = "";
  for (auto as : asName) {
    for (auto middle : middleName) {
      for (int i = 0; i <= 9; i++) {
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
fillConsumerContainer(NodeContainer& container)
{
  std::vector<std::string> asName = {"as1", "as2", "as3", "as4"};
  std::vector<std::string> middleName = {"cs", "math", "sm", "hw"};

  std::string target = "";
  for (auto as : asName) {
    for (auto middle : middleName) {
      for (int i = 0; i < 5; i++) {
        target = as + "-" + middle + "-" + "g" + std::to_string(i);
        auto cons = Names::Find<Node>(target);
        if (cons != nullptr) {
          container.Add(cons);
        }
      }
    }
  }
}

static void
fillRouterContainer(NodeContainer& container)
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

} // ns3

#endif
