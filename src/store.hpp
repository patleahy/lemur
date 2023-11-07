/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * The key/value store. Keys are integers and values are be strings.
 * The strings will be limited the lenght defined by VAL_LEN in types.h.
 */

#ifndef KVSTORE_H
#define KVSTORE_H

#include "memlevel.hpp"
#include <string>

namespace kv
{

/**
 * The key/value store. Keys are integers and values are be strings.
 * The strings will be limited the lenght defined by VAL_LEN in types.h.
 */
class Store
{
public:

    /**
     * Create the store by initilizeing the top level of the LSM tree.
     *
     * @param leveling true if using leveling, else use tiering.
     * @param size The number of items to store in the top level.
     * @param ratio Size ratio between adjacent levels.
     * @param bits The number of bits int the bloom filter.
     * @param hashes The number of hashes in the bloom filter.
     */
    Store(bool leveling, int size, int ratio, int bits, int hashes);

    /**
     * Put a value in the store with a key.
     */
    void Put(int key, std::string value);

    /**
     * Get a value from the store using a key.
     * @param key The key to lookup.
     * @param val Set to the value, if it is found.
     * @return true if the key was found.
     */
    bool Get(int key, std::string &value);

    /**
     * Check if a key is in the store.
     * @param key The key to lookup.
     * @return true if the key was found.
     */
    bool Contains(int key);

    /**
     * Get the number of items in the store. If a key is added twice then it will
     * be counted twice here.
     */
    int Count();

    /**
     * Return a string with a description of the store.
     * @param verbose When true include the key and values in the output.
     */
    std::string Dump(bool verbose = false);

private:
    MemLevel _next;
};

}
#endif