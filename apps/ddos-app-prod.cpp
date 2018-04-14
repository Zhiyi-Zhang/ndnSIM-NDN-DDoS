// ddos-app.cpp

#include "ddos-app-prod.hpp"

#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"

#include "ns3/ndnSIM/helper/ndn-stack-helper.hpp"
#include "ns3/ndnSIM/helper/ndn-fib-helper.hpp"

#include "ns3/random-variable-stream.h"

#include <memory>

NS_LOG_COMPONENT_DEFINE("DDoSProdApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(DDoSProdApp);

TypeId
DDoSProdApp::GetTypeId(void)
{
  static TypeId tid =
    TypeId("DDoSProdApp")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<DDoSProdApp>()
      .AddAttribute("Prefix", "Prefix, for which producer has the data", StringValue("/"),
                    MakeNameAccessor(&DDoSProdApp::m_prefix), MakeNameChecker())
      .AddAttribute("PayloadSize", "Virtual payload size for Content packets", UintegerValue(1024),
                    MakeUintegerAccessor(&DDoSProdApp::m_virtualPayloadSize),
                    MakeUintegerChecker<uint32_t>())

    ;
  return tid;
}

DDoSProdApp::DDoSProdApp()
  : m_firstTime(true)
{
}

void
DDoSProdApp::ScheduleNextChecks()
{
  if (m_firstTime){
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &DDoSProdApp::CheckViolations, this);
    m_firstTime = false;
  } else if (!m_sendEvent.IsRunning()) {
      m_sendEvent = Simulator::Schedule(Seconds(m_checkWindow),
                                      &DDoSProdApp::CheckViolations, this);
  }

}

void
DDoSProdApp::CheckViolations()
{
  // fake interest/sec icnreases threshold
  int fakeInterestPerSec = m_fakeInterestCount/m_checkWindow;
  int validInterestPerSec = m_validInterestCount/m_checkWindow;

  if (fakeInterestPerSec > m_fakeInterestThreshold){
    for (auto it = fakePrefixMap.begin(); it != fakePrefixMap.end(); ++it) {
      auto nack = std::make_shared<ndn::lp::Nack>(Interest(it->first));
      lp::NackHeader nackHeader;
      nackHeader.setReason(lp::NackReason::DDOS_FAKE_INTEREST);
      nackHeader.m_fakeTolerance = m_fakeInterestThreshold;
      nackHeader.m_fakeInterestNames = it->second;
      nack->setHeader(nackHeader);

      // send to nack to app link service
      m_appLink->onReceiveNack(*nack);
    }

  } else if (validInterestPerSec > m_validInterestThreshold){
    for (std::set<Name>::iterator it = validPrefixSet.begin();
      it != validPrefixSet.end(); ++it){
      auto nack = std::make_shared<ndn::lp::Nack>(Interest(*it));
      lp::NackHeader nackHeader;
      nackHeader.setReason(lp::NackReason::DDOS_VALID_INTEREST_OVERLOAD);
      nackHeader.m_prefixLen = it->size();
      nack->setHeader(nackHeader);

      // send to nack to app link service
      m_appLink->onReceiveNack(*nack);
    }
  }

  // reset the counter
  m_fakeInterestCount = 0;
  m_validInterestCount = 0;

  // reset maps
  fakePrefixMap.clear();
  validPrefixSet.clear();

  ScheduleNextChecks();
}

// inherited from Application base class.
void
DDoSProdApp::StartApplication()
{
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);

  ScheduleNextChecks();
}

void
DDoSProdApp::StopApplication()
{
  App::StopApplication();
}

void
DDoSProdApp::OnInterest(shared_ptr<const Interest> interest)
{
  ndn::App::OnInterest(interest);

  Name interestName = interest->getName();

  // fake interest
  if (!isdigit(interest->getName().toUri().at(interestName.toUri().length() - 1))){
    fakePrefixMap[interestName.getPrefix(-1)].push_back(interestName);
    m_fakeInterestCount += 1;
  }

  // valid interest
  else {
    validPrefixSet.insert(interestName.getPrefix(-1));
    m_validInterestCount += 1;

    auto data = std::make_shared<ndn::Data>(interest->getName());
    data->setFreshnessPeriod(ndn::time::milliseconds(5000));
    data->setContent(std::make_shared< ::ndn::Buffer>(1024));
    ndn::StackHelper::getKeyChain().sign(*data);
    // std::cout << "Sending Data packet for " << data->getName() << std::endl;

    m_appLink->onReceiveData(*data);
  }
}

} // namespace ndn
} // namespace ns3
