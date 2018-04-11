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

protected:
  // inherited from Application base class.
  virtual void
  StartApplication(); // Called at time specified by Start

  virtual void
  StopApplication(); // Called at time specified by Stop

  virtual void
  ScheduleNextChecks(); // schedule check for threshold violations

  virtual void
  CheckViolations(); // check for threshold violations

  virtual std::tuple<std::string, int>
  GetPrefix(shared_ptr<const Interest> interest); // get prefix from interest name

protected:

  EventId m_sendEvent; ///< @brief EventId of pending "send packet" event
  bool m_firstTime;


private:
  Name m_prefix;
  uint32_t m_virtualPayloadSize;

  // average tolerable fake interests before sending NACK
  int m_fakeInterestThreshold = 100;

  // total fake interests received in given time window
  int m_fakeInterestCount = 0;

  // average tolerable valid interests before sending NACK
  int m_validInterestThreshold = 200;

  // total valid interests received in given time window
  int m_validInterestCount = 0;

  // perform threshold violation checks after every checkWindow sec
  int m_checkWindow = 5;

  // create map of all fake interest prefixes 
  // and use them to NACK interest under that prefix
  std::map<std::string, int> fakePrefixMap;

  // map of all valid interest prefix
  std::map<std::string, int> validPrefixMap;


};

} // namespace ndn
} // namespace ns3

#endif // DDOS_PROD_APP_H