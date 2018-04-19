#ifndef DDOS_PROD_APP_H
#define DDOS_PROD_APP_H

#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/ndnSIM/apps/ndn-app.hpp"

#include "ns3/nstime.h"
#include "ns3/ptr.h"

namespace ns3 {
namespace ndn {

class DDoSProdApp : public App {
public:
  static TypeId
  GetTypeId(void);

  DDoSProdApp();

  // inherited from NdnApp
  virtual void
  OnInterest(shared_ptr<const Interest> interest);

  void
  ScheduleNextChecks(); // schedule check for threshold violations

  void
  CheckViolations(); // check for threshold violations

  void
  ScheduleNextReply();

  void
  ProcessValidInterest();

protected:
  // inherited from Application base class.
  virtual void
  StartApplication(); // Called at time specified by Start

  virtual void
  StopApplication(); // Called at time specified by Stop

protected:

  EventId m_checkViolationEvent; ///< @brief EventId of pending "send packet" event
  EventId m_replyEvent;
  bool m_firstTime;

private:
  Name m_prefix;
  uint32_t m_virtualPayloadSize;

  // average tolerable fake interests before sending NACK
  int m_fakeInterestThreshold = 500;

  // total fake interests received in given time window
  int m_fakeInterestCount = 0;

  // average tolerable valid interests before sending NACK
  int m_validInterestCapacity = 200;

  // total valid interests received in given time window
  int m_validInterestCount = 0;

  // perform threshold violation checks after every checkWindow sec
  int m_checkWindow = 1;

  // nack timer
  int m_timer = 5;

  // store fake interest prefixes and corresponding interest names
  // TODO: we need remove this and use per prefix threshold check
  std::map<Name, std::list<Name>> m_fakePrefixMap;
  shared_ptr<const Interest> m_nackFakeInterest;

  // store valid interest prefixes
  // TODO: we need remove this and use per prefix threshold check
  std::set<Name> m_validPrefixSet;
  shared_ptr<const Interest> m_nackValidInterest;

  // buffer the valid interest for processing
  std::list<Name> m_validInterestQueue;
};

} // namespace ndn
} // namespace ns3

#endif // DDOS_PROD_APP_H
