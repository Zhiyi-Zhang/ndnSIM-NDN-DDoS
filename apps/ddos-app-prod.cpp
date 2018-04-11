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
    for (std::map<std::string, int>::iterator it = fakePrefixMap.begin(); 
      it != fakePrefixMap.end(); ++it){
      auto nack = std::make_shared<ndn::lp::Nack>(Interest(it->first));
      lp::NackHeader nackHeader;
      nackHeader.setReason(lp::NackReason::FAKE_INTEREST_OVERLOAD);
      nackHeader.setPrefixLen(it->second);

      // send to nack to app link service
      m_appLink->onReceiveNack(*nack);
    }

  } else if (validInterestPerSec > m_validInterestThreshold){
    for (std::map<std::string, int>::iterator it = validPrefixMap.begin();
      it != validPrefixMap.end(); ++it){
      auto nack = std::make_shared<ndn::lp::Nack>(Interest(it->first));
      lp::NackHeader nackHeader;
      nackHeader.setReason(lp::NackReason::VALID_INTEREST_OVERLOAD);
      nackHeader.setPrefixLen(it->second);

      // send to nack to app link service
      m_appLink->onReceiveNack(*nack);
    }
  }

  // reset the counter
  m_fakeInterestCount = 0;
  m_validInterestCount = 0; 

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

  // fake interest  
  if (!isdigit(interest->getName().toUri().at(interest->getName().toUri().length() - 1))){
    
    std::tuple<std::string, int> prefixInfo = GetPrefix(interest);
    fakePrefixMap[std::get<0>(prefixInfo)] = std::get<1>(prefixInfo);
    m_fakeInterestCount += 1;
  }

  // valid interest
  else {
    
    std::tuple<std::string, int> prefixInfo = GetPrefix(interest);
    validPrefixMap[std::get<0>(prefixInfo)] = std::get<1>(prefixInfo);
    m_validInterestCount += 1;

    auto data = std::make_shared<ndn::Data>(interest->getName());
    data->setFreshnessPeriod(ndn::time::milliseconds(5000));
    data->setContent(std::make_shared< ::ndn::Buffer>(1024));
    ndn::StackHelper::getKeyChain().sign(*data);
    // std::cout << "Sending Data packet for " << data->getName() << std::endl;

    m_appLink->onReceiveData(*data);
  }
}

std::tuple<std::string, int>
DDoSProdApp::GetPrefix(shared_ptr<const Interest>interest)
{
  std::string interestName = interest->getName().toUri();

  std::string prefix;
  int prefixLength = 0;
  size_t pos = 0;
  std::string token;
  while ((pos = interestName.find("/")) != std::string::npos) {
    token = interestName.substr(0, pos);
    prefix.append(token);
    prefix.append("/");
    prefixLength += 1;
    interestName.erase(0, pos + 1);
  }

  return std::make_tuple(prefix.substr(0, prefix.size() - 1), prefixLength);
}

} // namespace ndn
} // namespace ns3
