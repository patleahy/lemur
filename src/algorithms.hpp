/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * Searching algorithms.
 */

#ifndef KVALGORITHMS_H
#define KVALGORITHMS_H

#include "types.hpp"
#include "keyvalues.hpp"

namespace kv
{

/**
 * Do a binary search on a sorted list of key/value pairs.
 * @param kvs A wrapper over an in-memory array of key/values or key/value files.
 * @param key The key to lookup.
 * @param val The value found.
 * @return true if the key was found
 */
bool binarySearch(KeyValues *kvs, kv_key_t key, kv_val_t &val);

/**
 * Do a linear search on a list of key/value pairs.
 * @param kvs A wrapper over an in-memory array of key/values or key/value files.
 * @param key The key to lookup.
 * @param val The value found.
 * @return true if the key was found
 */
bool linearSearch(KeyValues *kvs, kv_key_t key, kv_val_t &val);

/**
 * Do a binary search on a sorted list of key/value pairs.
 * Limit the search to a subrange in the list. This is used when fence posts
 * limit the range in which a key could be found.
 * @param kvs A wrapper over an in-memory array of key/values or key/value files.
 * @param key The key to lookup.
 * @param val The value found.
 * @return true if the key was found
 */
bool binarySearch(KeyValues *kvs, kv_key_t key, kv_val_t &val, int lower, int upper);

/**
 * Do a linear search on a list of key/value pairs.
 * Limit the search to a subrange in the list. This is used when fence posts
 * limit the range in which a key could be found.
 * @param kvs A wrapper over an in-memory array of key/values or key/value files.
 * @param key The key to lookup.
 * @param val The value found.
 * @return true if the key was found
 */
bool linearSearch(KeyValues *kvs, kv_key_t key, kv_val_t &val, int lower, int upper);

}
#endif