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
  //std::cout << "Checking for violations" << std::endl;
  
  // fake interest/sec icnreases threshold
  int fakeInterestPerSec = m_fakeInterestCount/m_checkWindow;

  std::cout << fakeInterestPerSec << std::endl; 

  if (fakeInterestPerSec > m_fakeInterestThreshold){
    auto nack = std::make_shared<ndn::lp::Nack>();
    lp::NackHeader nackHeader;
    nackHeader.setReason(lp::NackReason::FAKE_INTEREST_OVERLOAD);

    //std::cout << "fake interest/s " << fakeInterestPerSec << " increased threshold " 
    //  << m_fakeInterestThreshold << " send NACK" << std::endl;

    // send to nack to app link service
    m_appLink->onReceiveNack(*nack);

    // reset the counter
    m_fakeInterestCount = 0;
  }

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

  if (!isdigit(interest->getName().toUri().at(interest->getName().toUri().length() - 1))){
    m_fakeInterestCount += 1;
  }
  else {
    // std::cout << "Received Interest packet for " << interest->getName() << std::endl;

    // Note that Interests send out by the app will not be sent back to the app !

    auto data = std::make_shared<ndn::Data>(interest->getName());
    data->setFreshnessPeriod(ndn::time::milliseconds(5000));
    data->setContent(std::make_shared< ::ndn::Buffer>(1024));
    ndn::StackHelper::getKeyChain().sign(*data);
    // std::cout << "Sending Data packet for " << data->getName() << std::endl;

    // Call trace (for logging purposes)
    m_transmittedDatas(data, this, m_face);

    m_appLink->onReceiveData(*data);
  }
}

} // namespace ndn
} // namespace ns3
