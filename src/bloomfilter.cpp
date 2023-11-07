/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * A bloom filter implementation. This is based on this blogpost:
 *     https://findingprotopia.org/posts/how-to-write-a-bloom-filter-cpp/
 * and this paper:
 *     https://doi.org/10.1002/rsa.20208
 */

#include "bloomfilter.hpp"
#include "lib/MurmurHash3.h"
#include <sstream>

BloomFilter::BloomFilter(uint64_t numBits, uint64_t numHashes, uint32_t seed)
    : _bits(numBits),
      _numHashes(numHashes),
      _seed(seed)
{
}

void BloomFilter::Add(int data)
{
    // Create one multi-bit hash of the value.
    // Then use individual bits from the hash to set the bit vector.
    // See https://findingprotopia.org/posts/how-to-write-a-bloom-filter-cpp/
    auto hashValues = hash(data);
    for (int n = 0; n < _numHashes; n++)
    {
        _bits[nthHash(n, hashValues[0], hashValues[1], _bits.size())] = true;
    }
}

bool BloomFilter::Test(int data)
{
    // Use the same technique as Add to take bits from a multi-bit hash.
    auto hashValues = hash(data);
    for (int n = 0; n < _numHashes; n++)
    {
        if (!_bits[nthHash(n, hashValues[0], hashValues[1], _bits.size())])
        {
            return false;
        }
    }

    return true;
}

void BloomFilter::Clear()
{
    std::fill(_bits.begin(), _bits.end(), false);
}

/**
 * Get a milti-byte hash.
 */
std::array<uint64_t, 2> BloomFilter::hash(int data)
{
    std::array<uint64_t, 2> hashValue;
    MurmurHash3_x64_128(&data, sizeof(int), _seed, hashValue.data());
    return hashValue;
}

/**
 * Determine of the nth bit of the filter sould be set.
 * See https://doi.org/10.1002/rsa.20208
 */
inline uint64_t BloomFilter::nthHash(
    uint8_t n, uint64_t hashA, uint64_t hashB, uint64_t bits)
{
    return (hashA + n * hashB) % bits;
}
