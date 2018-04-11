#ifndef DDOS_APP_H
#define DDOS_APP_H

#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ns3/ndnSIM/apps/ndn-app.hpp"

#include "ns3/nstime.h"
#include "ns3/ptr.h"

namespace ns3 {
namespace ndn {
class DDoSApp : public App {
public:
  static TypeId
  GetTypeId(void);

  DDoSApp();

  // inherited from NdnApp
  virtual void
  OnInterest(shared_ptr<const Interest> interest);

protected:
  // inherited from Application base class.
  virtual void
  StartApplication(); // Called at time specified by Start

  virtual void
  StopApplication(); // Called at time specified by Stop

private:
  Name m_prefix;
  uint32_t m_virtualPayloadSize;

  // number of tolerable fake interests before sending NACK
  int m_fakeInterestThreshold = 100;
  int m_fakeInterestCount;
};

} // namespace ndn
} // namespace ns3

#endif // DDOS_APP_H