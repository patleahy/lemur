/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * A bloom filter implementation. This is based on this blogpost:
 *     https://findingprotopia.org/posts/how-to-write-a-bloom-filter-cpp/
 * and this paper:
 *     https://doi.org/10.1002/rsa.20208
 */

#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <stddef.h>
#include <vector>
#include <array>

class BloomFilter
{
public:
    /**
     * Create a bloom filter with a given number of bits and hashes.
     * seed is passed to the MurmurHash3 algorthm to generate hashes.
     * Using a different seed at different levels of the LSM tree prevents a
     * false positive happening at every level of the tree.
     */
    BloomFilter(uint64_t numBits, uint64_t numHashes, uint32_t seed);

    /**
     * Add an item to the filter.
     */
    void Add(int data);

    /**
     * Test an item in the filter.
     */
    bool Test(int data);

    /**
     * Forget all the items which were added to the filter.
     */
    void Clear();

    uint64_t getNumBits() { return _bits.size(); }
    uint64_t getNumHashes() { return _numHashes; }

private:
    std::array<uint64_t, 2> hash(int data);
    inline uint64_t nthHash(
        uint8_t n, uint64_t hashA, uint64_t hashB, uint64_t filterSize);

    uint64_t _numHashes;
    std::vector<bool> _bits;
    uint32_t _seed;
};

#endif