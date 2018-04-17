#include "bloomfilter.hpp"
#include "murmurhash3.hpp"
#include <array>

namespace ns3 {
namespace ndn {

BloomFilter::BloomFilter(uint64_t size, uint8_t numHashes)
      : m_bits(size)
      , m_numHashes(numHashes)
{
}

static std::array<uint64_t, 2>
hash(const uint8_t *data, std::size_t len) {
  std::array<uint64_t, 2> hashValue;
  MurmurHash3_x64_128(data, len, 0, hashValue.data());

  return hashValue;
}

static uint64_t
nthHash(uint8_t n, uint64_t hashA, uint64_t hashB, uint64_t filterSize)
{
  return (hashA + n * hashB) % filterSize;
}

void BloomFilter::add(const uint8_t *data, std::size_t len) {
  auto hashValues = hash(data, len);

  for (int n = 0; n < m_numHashes; n++) {
      m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())] = true;
  }
}

bool
BloomFilter::possiblyContains(const uint8_t *data, std::size_t len) const
{
  auto hashValues = hash(data, len);

  for (int n = 0; n < m_numHashes; n++) {
      if (!m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())]) {
          return false;
      }
  }

  return true;
}

} // namespace ndn
} // namespace ns3
