#ifndef _BLOOMFILTER_H_
#define _BLOOMFILTER_H_

#include <vector>

namespace ns3 {
namespace ndn {

class BloomFilter
{
public:
  BloomFilter(uint64_t size, uint8_t numHashes);

  void
  add(const uint8_t *data, std::size_t len);

  bool
  possiblyContains(const uint8_t *data, std::size_t len) const;

private:
  std::vector<bool> m_bits;
  uint8_t m_numHashes;
};

} // namespace ndn
} // namespace ns3

#endif // BLOOMFILTER
