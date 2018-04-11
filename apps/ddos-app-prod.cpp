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

NS_LOG_COMPONENT_DEFINE("DDoSApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(DDoSApp);

TypeId
DDoSApp::GetTypeId(void)
{
  static TypeId tid =
    TypeId("DDoSApp")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<DDoSApp>()
      .AddAttribute("Prefix", "Prefix, for which producer has the data", StringValue("/"),
                    MakeNameAccessor(&DDoSApp::m_prefix), MakeNameChecker())
      .AddAttribute("PayloadSize", "Virtual payload size for Content packets", UintegerValue(1024),
                    MakeUintegerAccessor(&DDoSApp::m_virtualPayloadSize),
                    MakeUintegerChecker<uint32_t>())

    ;
  return tid;
}

DDoSApp::DDoSApp()
  : m_fakeInterestCount(0)
{
}

// inherited from Application base class.
void
DDoSApp::StartApplication()
{
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
}

void
DDoSApp::StopApplication()
{
  App::StopApplication();
}

void
DDoSApp::OnInterest(shared_ptr<const Interest> interest)
{
  ndn::App::OnInterest(interest);

  if (!isdigit(interest->getName().toUri().at(interest->getName().toUri().length() - 1))){
    m_fakeInterestCount += 1;

    // send nack if reached threshold
    if (m_fakeInterestCount == m_fakeInterestThreshold){

      auto nack = std::make_shared<ndn::lp::Nack>(*interest);
      lp::NackHeader nackHeader;
      nackHeader.setReason(lp::NackReason::FAKE_INTEREST_OVERLOAD);

      // send to nack to app link service
      m_appLink->onReceiveNack(*nack);

      // reset the counter
      m_fakeInterestCount = 0;
    }
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
