// ddos-app.cpp

#include "ddos-app-prod.hpp"
#include "bloomfilter.hpp"

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
  NS_LOG_INFO("Periodic CheckViolations: fake:" << m_fakeInterestCount
              << " valid:" << m_validInterestCount);

  // fake interest/time unit increases threshold
  int fakeInterestPerSec = m_fakeInterestCount/m_checkWindow;
  int validInterestPerSec = m_validInterestCount/m_checkWindow;

  if (fakeInterestPerSec > m_fakeInterestThreshold) {
    NS_LOG_INFO("Violate FAKE INTERST threshold!!!");

    for (auto it = fakePrefixMap.begin(); it != fakePrefixMap.end(); ++it) {
      // Bloomfilter bf;
      // for (const auto& name : it->second) {
      //   const auto& block = name.wireEncode();
      //   bf.add(block.value(), block)
      // }

      ndn::lp::Nack nack(*m_nackFakeInterest);
      lp::NackHeader nackHeader;
      nackHeader.m_reason = lp::NackReason::DDOS_FAKE_INTEREST;
      nackHeader.m_prefixLen = it->first.size();
      nackHeader.m_fakeTolerance = m_fakeInterestThreshold;
      nackHeader.m_timer = m_timer;
      nackHeader.m_fakeInterestNames = it->second;
      nack.setHeader(nackHeader);

      // send to nack to app link service
      m_appLink->onReceiveNack(nack);
      NS_LOG_INFO("send out FAKE INTERST NACK!!!");
    }

  }
  else if (validInterestPerSec > m_validInterestThreshold) {
    for (std::set<Name>::iterator it = validPrefixSet.begin();
      it != validPrefixSet.end(); ++it){
      auto nack = std::make_shared<ndn::lp::Nack>(Interest(*it));
      lp::NackHeader nackHeader;
      nackHeader.setReason(lp::NackReason::DDOS_VALID_INTEREST_OVERLOAD);
      nackHeader.m_prefixLen = it->size();
      nack->setHeader(nackHeader);

      // send to nack to app link service
      m_appLink->onReceiveNack(*nack);
      NS_LOG_INFO("send out VALID OVERLOAD NACK!!!");
    }
  }

  // reset the counter
  m_fakeInterestCount = 0;
  m_validInterestCount = 0;
  m_nackFakeInterest = nullptr;

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
  std::string lastComponent = interest->getName().get(-1).toUri();
  if (lastComponent[0] == 'a') {
    NS_LOG_INFO("Receive Fake Interest " << interest->getName());
    fakePrefixMap[interestName.getPrefix(-1)].push_back(interestName);
    m_fakeInterestCount += 1;
    if (m_nackFakeInterest == nullptr) {
      m_nackFakeInterest = interest;
    }

    // ndn::lp::Nack nack(*interest);
    // lp::NackHeader nackHeader;
    // nackHeader.m_reason = lp::NackReason::DDOS_FAKE_INTEREST;
    // nackHeader.m_prefixLen = 1;
    // nackHeader.m_fakeTolerance = m_fakeInterestThreshold;
    // nack.setHeader(nackHeader);

    // m_appLink->onReceiveNack(nack);
  }
  // valid interest
  else {
    NS_LOG_INFO("Receive Valid Interest " << interest->getName());

    validPrefixSet.insert(interestName.getPrefix(-1));
    m_validInterestCount += 1;

    auto data = std::make_shared<ndn::Data>(interest->getName());
    data->setFreshnessPeriod(ndn::time::milliseconds(5000));
    data->setContent(std::make_shared< ::ndn::Buffer>(1024));
    ndn::StackHelper::getKeyChain().sign(*data);

    m_appLink->onReceiveData(*data);
    NS_LOG_INFO("Sending Data packet with name " << data->getName());
  }
}

} // namespace ndn
} // namespace ns3
