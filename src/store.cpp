/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * The key/value store. Keys are integers and values are be strings.
 * The strings will be limited the lenght defined by VAL_LEN in types.h.
 */

#include "store.hpp"
#include "types.hpp"
#include "memlevel.hpp"

namespace kv
{

/**
 * Create the store by initilizeing the top level of the LSM tree.
 *
 * @param leveling true if using leveling, else use tiering.
 * @param size The number of items to store in the top level.
 * @param ratio Size ratio between adjacent levels.
 * @param bits The number of bits int the bloom filter.
 * @param hashes The number of hashes in the bloom filter.
 */
Store::Store(bool leveling, int size, int ratio, int bits, int hashes)
    : _next(leveling, size, ratio, bits, hashes)
{
}

/**
 * Put a value in the store with a key.
 */
void Store::Put(int key, std::string val)
{
    kv_key_t k = key;
    kv_val_t v;
    string2value(val, v);
    _next.Put(k, v);
}

/**
 * Get a value from the store using a key.
 * @param key The key to lookup.
 * @param val Set to the value, if it is found.
 * @return true if the key was found.
 */
bool Store::Get(int key, std::string &val)
{
    kv_key_t k = key;
    kv_val_t v;
    if (_next.Get(k, v))
    {
        value2string(v, val);
        return true;
    }
    return false;
}

/**
 * Check if a key is in the store.
 * @param key The key to lookup.
 * @return true if the key was found.
 */
bool Store::Contains(int key)
{
    std::string value;
    return Get(key, value);
}

/**
 * Get the number of items in the store. If a key is added twice then it will
 * be counted twice here.
 */
int Store::Count()
{
    return _next.Count();
}

/**
 * Return a string with a description of the store.
 * @param verbose When true include the key and values in the output.
 */
std::string Store::Dump(bool verbose)
{
    return _next.Dump(verbose);
}

}