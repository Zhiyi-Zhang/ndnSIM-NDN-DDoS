// Cons-app.hpp

#ifndef CONS_APP_H_
#define CONS_APP_H_

#include "ns3/ndnSIM/model/ndn-common.hpp"
#include "ns3/ndnSIM/apps/ndn-consumer.hpp"
#include "murmurhash3.hpp"

namespace ns3 {
namespace ndn{

class ConsApp : public ndn::App {
public:
  // register NS-3 type "ConsApp"
  static TypeId
  GetTypeId();

  ConsApp();
  virtual ~ConsApp();

  // (overridden from ndn::App) Processing upon start of the application
  virtual void
  StartApplication();

  // (overridden from ndn::App) Processing when application is stopped
  virtual void
  StopApplication();

  // (overridden from ndn::App) Callback that will be called when Data arrives
  virtual void
  OnData(std::shared_ptr<const ndn::Data> contentObject);

protected:
  virtual void
  ScheduleNextPacket();

  void
  SetRandomize(const std::string& value);

  std::string
  GetRandomize() const;

protected:
  int m_frequency;
  bool m_firstTime;
  Ptr<RandomVariableStream> m_random;
  std::string m_randomType;
  EventId m_sendEvent; ///< @brief EventId of pending "send packet" event
  std::string m_interestNames; // parameter passed in scenario
  std::vector<std::string> m_interestNameList;
  bool m_validInterest;
  std::map<std::string, int> m_lastSeq; // per prefix last seq
  int m_initSeq;
  int m_maxSeq;

private:
  void
  SendInterest();
};

} // namespace ndn
} // namespace ns3

#endif // CONS_APP_H_
