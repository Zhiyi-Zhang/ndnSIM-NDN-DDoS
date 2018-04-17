//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

#include <stdint.h>

namespace ns3 {
namespace ndn {


//-----------------------------------------------------------------------------

void
MurmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out );

//-----------------------------------------------------------------------------


} // namespace ndn
} // namespace ns3

#endif // _MURMURHASH3_H_
