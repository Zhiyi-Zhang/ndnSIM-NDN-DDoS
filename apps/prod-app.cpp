// prod-app.cpp

#include "prod-app.hpp"

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

NS_LOG_COMPONENT_DEFINE("ProdApp");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(ProdApp);

TypeId
ProdApp::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ProdApp")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<ProdApp>()
      .AddAttribute("Prefix", "Prefix, for which producer has the data", StringValue("/"),
                    MakeNameAccessor(&ProdApp::m_prefix), MakeNameChecker())
      .AddAttribute("PayloadSize", "Virtual payload size for Content packets", UintegerValue(1024),
                    MakeUintegerAccessor(&ProdApp::m_virtualPayloadSize),
                    MakeUintegerChecker<uint32_t>())

    ;
  return tid;
}

ProdApp::ProdApp()
{
}

// inherited from Application base class.
void
ProdApp::StartApplication()
{
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
}

void
ProdApp::StopApplication()
{
  App::StopApplication();
}

void
ProdApp::OnInterest(shared_ptr<const Interest> interest)
{
  ndn::App::OnInterest(interest);

  if (!isdigit(interest->getName().toUri().at(interest->getName().toUri().length() - 1))){
    // std::cout << "Fake interest received: " << interest->getName() << std::endl;
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
