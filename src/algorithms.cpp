/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * See header file for function level documentation.
 */

#include "algorithms.hpp"

namespace kv
{

bool binarySearch(KeyValues *kvs, kv_key_t key, kv_val_t &val)
{
    return binarySearch(kvs, key, val, 0, kvs->Count());
}

bool binarySearch(KeyValues *kvs, kv_key_t key, kv_val_t &val, int lower, int upper)
{
    int l = lower;
    int r = upper-1;
    kv_key_t k;
    while (l <= r)
    {
        int m = (l+r)/2;
        k = kvs->Key(m);
        if (k < key)
        {
            l = m + 1;
        }
        else if (k > key)
        {
            r = m - 1;
        }
        else
        {
            // Found a matching key.

            // The same key could have been added more than once. Check the key
            // to the left to see if it is the same key. Keep sliding to the
            // left while the key is equal. This will cause us to return the
            // most recent value for this key.
            while ((m > lower) && (key == kvs->Key(m-1))) { m--; }
  
            val = kvs->Val(m);
            return true;
        }
    }
    return false;
}

bool linearSearch(KeyValues *kvs, kv_key_t key, kv_val_t &val)
{
    return linearSearch(kvs, key, val, 0, kvs->Count());
}

bool linearSearch(KeyValues *kvs, kv_key_t key, kv_val_t &val, int lower, int upper)
{
    // Search backward so we find the most recent copy of a key.
    for (int i = upper-1; i >= lower; i--)
    {
        kv_key_t test = kvs->Key(i);
        if (test == key)
        {
            val = kvs->Val(i);
            return true;
        }
    }
    return false;
}

}