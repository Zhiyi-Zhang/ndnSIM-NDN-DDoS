#ifndef _BLOOMFILTER_H_
#define _BLOOMFILTER_H_

#include "../ndn-cxx/src/encoding/buffer.hpp"
#include <vector>
#include <bitset>


namespace ns3 {
namespace ndn {

class BloomFilter
{
public:
  BloomFilter(uint8_t numHashes = 5);

  void
  add(const uint8_t *data, std::size_t len);

  bool
  possiblyContains(const uint8_t *data, std::size_t len) const;

  template<size_t N>
  ::ndn::Buffer
  bitset_to_bytes(const std::bitset<N>& bs)
  {
    ::ndn::Buffer result((N + 7) >> 3);
    for (int j=0; j<int(N); j++)
      result[j>>3] |= (bs[j] << (j & 7));
    return result;
  }

  template<size_t N>
  std::bitset<N>
  bitset_from_bytes(const ::ndn::Buffer& buf)
  {
    assert(buf.size() == ((N + 7) >> 3));
    std::bitset<N> result;
    for (int j=0; j<int(N); j++)
      result[j] = ((buf[j>>3] >> (j & 7)) & 1);
    return result;
  }

private:
  std::bitset<1400> m_bits;
  uint8_t m_numHashes;
};

} // namespace ndn
} // namespace ns3

#endif // BLOOMFILTER
