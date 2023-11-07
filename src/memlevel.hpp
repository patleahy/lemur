/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * The memory level (level 0) in the LSM tree.
 *
 * At this level there is no bloom filter or fence posts. The key array always
 * is searched for values.
 */

#ifndef KVMEMLEVEL_H
#define KVMEMLEVEL_H

#include "types.hpp"
#include "filelevel.hpp"
#include "bloomfilter.hpp"

namespace kv
{

/*
 * The memory level (level 0) in the LSM tree.
 */
class MemLevel
{
public:

    /**
     * Create the memory level.
     * @param leveling true if using leveling, else use tiering.
     * @param size The number of items to store in this level.
     * @param ratio Size ratio between adjacent levels.
     * @param bits The number of bits int the bloom filter.
     * @param hashes The number of hashes in the bloom filter.
     */
    MemLevel(bool leveling, int size, int ratio, int bits, int hashes);
    ~MemLevel();

    /*
     * Add a key/value to this level.
     * If necessery this will merge values into lower levels as the store grows.
     */
    void Put(kv_key_t key, kv_val_t val);

    /**
     * Get a value from this level or the levels below it.
     * @param key The key to lookup.
     * @param val Set to the value, if it is found.
     * @return true if the key was found.
     */
    bool Get(kv_key_t key, kv_val_t &val);

    /**
     * Return this count of this level plus the levels below it.
     */
    int Count();

    /**
     * Return a string with a description of this level and all the levels below it.
     * @param verbose When true include the key and values in the output.
     */
    std::string Dump(bool verbose);

private:

    /**
     * Put a value in this level using the tiering algorthm when merging.
     */
    void put_tiering(kv_key_t key, kv_val_t val);

    /**
     * Put a value in this level using the leveling algorthm when merging.
     */
    void put_leveling(kv_key_t key, kv_val_t val);

    /**
     * Look in the current level for a key using a linear seach because this
     * level is not sorted, i.e. tiering, not leveling.
     */
    bool get_tiering(kv_key_t key, kv_val_t &val);

    /**
     * Look in the current level for a key using a binary seach because this
     * level is sorted, i.e. leveling, not tiering.
     */
    bool get_leveling(kv_key_t key, kv_val_t &val);

    /**
     * Flush the current level to the next level.
     */
    void flush();

    bool _leveling;     // Are we doing leveling or tiering.
    int _size;          // The number of items we will store at this level.
    int _ratio;         // Size ratio between adjacent levels.
    int _count;         // The number of items currently in this level.
    int _bits;          // The number of bits int the bloom filter.
    int _hashes;        // The number of hashes in the bloom filter.
    kv_key_t *_keys;    // The array of keys.
    kv_val_t *_vals;    // The array of values.
    FileLevel *_next;   // The level below this one.
};

}
#endif