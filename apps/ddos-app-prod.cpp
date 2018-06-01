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

const static int DEFAULT_ID_MAX = 10000;

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
    .AddAttribute("FakeThreshold", "Threshold for fake interests", UintegerValue(0),
                  MakeUintegerAccessor(&DDoSProdApp::m_fakeInterestThreshold),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute("ValidThreshold", "Threshold for valid interests", UintegerValue(1000),
                  MakeUintegerAccessor(&DDoSProdApp::m_validInterestCapacity),
                  MakeUintegerChecker<uint32_t>())
    ;
  return tid;
}

DDoSProdApp::DDoSProdApp()
  : m_firstTime(true)
  , m_firstNack(true)
{
}

void
DDoSProdApp::ScheduleNextChecks()
{
  if (m_firstTime) {
    m_checkViolationEvent = Simulator::Schedule(Seconds(0.0), &DDoSProdApp::CheckViolations, this);
    // m_firstTime = false;
  }
  else if (!m_checkViolationEvent.IsRunning()) {
    m_checkViolationEvent = Simulator::Schedule(Seconds(m_checkWindow),
                                                &DDoSProdApp::CheckViolations, this);
  }

}

void
DDoSProdApp::ScheduleNextReply()
{
  if (m_firstTime) {
    m_replyEvent = Simulator::Schedule(Seconds(0.01), &DDoSProdApp::ProcessValidInterest, this);
    m_firstTime = false;
  }
  else if (!m_replyEvent.IsRunning()) {
    m_replyEvent = Simulator::Schedule(Seconds(0.1), &DDoSProdApp::ProcessValidInterest, this);
  }
}

void
DDoSProdApp::ProcessValidInterest()
{
  int totalNumber = m_validInterestCapacity/10;
  for (int i = 0; i < totalNumber; i++) {
    if (!m_validInterestQueue.empty()) {
      auto data = std::make_shared<ndn::Data>(m_validInterestQueue.front());
      data->setFreshnessPeriod(ndn::time::milliseconds(5000));
      data->setContent(std::make_shared< ::ndn::Buffer>(1024));
      ndn::StackHelper::getKeyChain().sign(*data);

      m_appLink->onReceiveData(*data);
      m_validInterestQueue.pop_front();
    }
  }

  ScheduleNextReply();
}


void
DDoSProdApp::CheckViolations()
{
  NS_LOG_INFO("Periodic CheckViolations: fake:" << m_fakeInterestCount
              << " valid:" << m_validInterestCount);

  // fake interest/time unit increases threshold

  NS_LOG_DEBUG("Fake interests per unit: " << m_fakeInterestCount);
  NS_LOG_DEBUG("Valid interests per unit: " << m_validInterestCount);

  std::cout << "Fake interests per unit: " << m_fakeInterestCount
            << " Valid interests per unit: " << m_validInterestCount
            << " Good interests per unit: " << m_goodInterests << std::endl;

  if (static_cast<double>(m_fakeInterestCount) > m_fakeInterestThreshold * m_checkWindow) {
    NS_LOG_INFO("Violate FAKE INTERST threshold!!!");
    std::cout << "Violate FAKE INTERST threshold!!!" << std::endl;

    for (auto it = m_fakePrefixMap.begin(); it != m_fakePrefixMap.end(); ++it) {
      ndn::lp::Nack nack(*m_nackFakeInterest);
      lp::NackHeader nackHeader;
      nackHeader.m_reason = lp::NackReason::DDOS_FAKE_INTEREST;
      nackHeader.m_prefixLen = it->first.size();
      nackHeader.m_tolerance = static_cast<int>(m_fakeInterestThreshold * 0.9);
      nackHeader.m_fakeInterestNames = it->second;
      nackHeader.m_nackId = rand() % DEFAULT_ID_MAX;
      nack.setHeader(nackHeader);

      // send to nack to app link service
      m_appLink->onReceiveNack(nack);
      NS_LOG_INFO("send out FAKE INTERST NACK!!!");
    }

  }

  if (static_cast<double>(m_validInterestCount) > m_validInterestCapacity * m_checkWindow) {
    NS_LOG_INFO("Violate VALID INTEREST capacity!!!");
    std::cout << "Violate VALID INTERST threshold!!!" << std::endl;

    for (auto it = m_validPrefixSet.begin(); it != m_validPrefixSet.end(); ++it) {
      ndn::lp::Nack nack(*m_nackValidInterest);
      lp::NackHeader nackHeader;
      nackHeader.m_reason = lp::NackReason::DDOS_VALID_INTEREST_OVERLOAD;
      nackHeader.m_prefixLen = it->size();
      nackHeader.m_tolerance = static_cast<int>(m_validInterestCapacity * 0.95);
      nackHeader.m_nackId = rand() % DEFAULT_ID_MAX;
      nack.setHeader(nackHeader);

      // send to nack to app link service
      m_appLink->onReceiveNack(nack);
      NS_LOG_INFO("send out VALID OVERLOAD NACK!!!");
    }
  }

  // reset the counter
  m_fakeInterestCount = 0;
  m_validInterestCount = 0;
  m_goodInterests = 0;
  m_nackFakeInterest = nullptr;
  m_nackValidInterest = nullptr;

  // reset maps
  m_fakePrefixMap.clear();
  m_validPrefixSet.clear();

  ScheduleNextChecks();
}

// inherited from Application base class.
void
DDoSProdApp::StartApplication()
{
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);

  ScheduleNextChecks();
  ScheduleNextReply();
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
    // NS_LOG_INFO("Receive Fake Interest " << interest->getName());
    m_fakePrefixMap[interestName.getPrefix(-1)].push_back(interestName);
    m_fakeInterestCount += 1;

    if (m_nackFakeInterest == nullptr) {
      m_nackFakeInterest = interest;
    }

  }
  // valid interest
  else {
    // NS_LOG_INFO("Receive Valid Interest " << interest->getName());

    if (lastComponent[0] == 'g') {
      ++m_goodInterests;
    }
    m_validPrefixSet.insert(interestName.getPrefix(-1));
    m_validInterestCount += 1;

    m_nackValidInterest = interest;

    m_validInterestQueue.push_back(interest->getName());

  }
}

} // namespace ndn
} // namespace ns3
