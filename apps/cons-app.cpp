// cons-app.cpp

#include "cons-app.hpp"

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

#include <ndn-cxx/lp/tags.hpp>


#include "ns3/random-variable-stream.h"

NS_LOG_COMPONENT_DEFINE("ConsApp");

namespace ns3 {
namespace ndn{

NS_OBJECT_ENSURE_REGISTERED(ConsApp);

// register NS-3 type
TypeId
ConsApp::GetTypeId()
{
  static TypeId tid = TypeId("ConsApp")
    .SetParent<ndn::App>()
    .AddConstructor<ConsApp>()
    .AddAttribute("Frequency", "Frequency of interest packets", StringValue("1"),
                  MakeIntegerAccessor(&ConsApp::m_frequency), MakeIntegerChecker<int32_t>())

    .AddAttribute("Randomize",
                  "Type of send time randomization: none (default), uniform, exponential",
                  StringValue("none"),
                  MakeStringAccessor(&ConsApp::SetRandomize, &ConsApp::GetRandomize),
                  MakeStringChecker())

    .AddAttribute("Name",
                  "Name of interest",
                  StringValue("/"),
                  MakeStringAccessor(&ConsApp::m_interestName), MakeStringChecker())

    .AddAttribute("ValidInterest",
                  "Auto append sequence number: true (default), false",
                  BooleanValue(true),
                  MakeBooleanAccessor(&ConsApp::m_validInterest), MakeBooleanChecker())

    .AddAttribute("InitSeq",
                  "The first seq to send",
                  IntegerValue(-1),
                  MakeIntegerAccessor(&ConsApp::m_lastSeq), MakeIntegerChecker<int32_t>())

    .AddAttribute("MaxSeq",
                  "The max seq to send",
                  StringValue("200"),
                  MakeIntegerAccessor(&ConsApp::m_maxSeq), MakeIntegerChecker<int32_t>());;

  return tid;
}

ConsApp::ConsApp()
  : m_frequency(1.0)
  , m_firstTime(true)
  , m_interestName("/")
{
}

ConsApp::~ConsApp()
{
}


void
ConsApp::ScheduleNextPacket()
{
  // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
  // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";

  if (m_firstTime) {
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &ConsApp::SendInterest, this);
    m_firstTime = false;
  }
  else if (!m_sendEvent.IsRunning())
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &ConsApp::SendInterest, this);
}

void
ConsApp::SetRandomize(const std::string& value)
{
  if (value == "uniform") {
    m_random = CreateObject<UniformRandomVariable>();
    m_random->SetAttribute("Min", DoubleValue(0.0));
    m_random->SetAttribute("Max", DoubleValue(2 * 1.0 / m_frequency));
  }
  else if (value == "exponential") {
    m_random = CreateObject<ExponentialRandomVariable>();
    m_random->SetAttribute("Mean", DoubleValue(1.0 / m_frequency));
    m_random->SetAttribute("Bound", DoubleValue(50 * 1.0 / m_frequency));
  }
  else
    m_random = 0;

  m_randomType = value;
}

std::string
ConsApp::GetRandomize() const
{
  return m_randomType;
}

// Processing upon start of the application
void
ConsApp::StartApplication()
{
  // initialize ndn::App
  ndn::App::StartApplication();
  // std::cout << "Interest Name is " << m_interestName << std::endl;

  NS_LOG_INFO("Current Frequency: " << m_frequency);
  NS_LOG_INFO("Current Max Range: " << m_maxSeq);

  ScheduleNextPacket();
}

// Processing when application is stopped
void
ConsApp::StopApplication()
{
  // cleanup ndn::App
  ndn::App::StopApplication();
}

void
ConsApp::SendInterest()
{
  /////////////////////////////////////
  // Sending one Interest packet out //
  /////////////////////////////////////
  Name interest_copy(m_interestName);

  if (m_validInterest){
    interest_copy.append(std::to_string(m_lastSeq + 1));
    m_lastSeq += 1;

    if (m_lastSeq == m_maxSeq){
      m_lastSeq = -1;
    }
  }
  else{
    interest_copy.append("a" + std::to_string(rand()%200));
  }

  // Create and configure ndn::Interest
  auto interest = std::make_shared<ndn::Interest>(interest_copy);
  Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
  interest->setNonce(rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
  interest->setMustBeFresh(true);

  interest->setTag(make_shared<lp::HopCountTag>(0));

  // Call trace (for logging purposes)
  m_transmittedInterests(interest, this, m_face);

  m_appLink->onReceiveInterest(*interest);
  NS_LOG_INFO("Sending Interest packet for: " << interest->getName());

  ScheduleNextPacket();
}

// Callback that will be called when Data arrives
void
ConsApp::OnData(std::shared_ptr<const ndn::Data> data)
{
  NS_LOG_INFO("DATA received for name: " << data->getName());
}

} // namespace ndn
} // namespace ns3
