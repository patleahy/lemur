/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * The memory level (level 0) in the LSM tree.
 *
 * At this level there is no bloom filter or fence posts. The key array always
 * is searched for values.
 */

#include "memlevel.hpp"
#include "keyvalues.hpp"
#include "algorithms.hpp"
#include <sstream>
#include <cstring>

namespace kv
{

/**
 * Create the memory level.
 * @param leveling true if using leveling, else use tiering.
 * @param size The number of items to store in this level.
 * @param ratio Size ratio between adjacent levels.
 * @param bits The number of bits int the bloom filter.
 * @param hashes The number of hashes in the bloom filter.
 */
MemLevel::MemLevel(bool leveling, int size, int ratio, int bits, int hashes)
    : _leveling(leveling),
      _size(size),
      _ratio(ratio),
      _count(0),
      _keys(new kv_key_t[size]),
      _vals(new kv_val_t[size]),
      _next(NULL),
      _bits(bits),
      _hashes(hashes)
{
}

MemLevel::~MemLevel()
{
    delete [] _keys;
    delete [] _vals;
    if (_next)
        delete _next;
}

/**
 * Add a key/value to this level.
 * If necessery this will merge values into lower levels as the store grows.
 */
void MemLevel::Put(kv_key_t key, kv_val_t val)
{
    // If this level is full then flush to the next level.
    if (_count == _size)
        flush();

    if (_leveling)
        put_leveling(key, val);
    else
        put_tiering(key, val);

    _count++;
}
/**
 * Get a value from this level or the levels below it.
 * @param key The key to lookup.
 * @param val Set to the value, if it is found.
 * @return true if the key was found.
 */
bool MemLevel::Get(kv_key_t key, kv_val_t &val)
{
    // First look at this level.
    if (_leveling)
    {
        if (get_leveling(key, val))
            return true;
    }
    else
    {
        if (get_tiering(key, val))
            return true;
    }

     // The key is not at this level so try the next level.
    if (_next)
    {
        return _next->Get(key, val);
    }

    // The key wasn't at this level or any level below it.
    return false;
}
/**
 * Return this count of this level plus the levels below it.
 */
int MemLevel::Count()
{
    return _count + (_next ? _next->Count() : 0);
}

/**
 * Put a value in this level using the tiering algorthm when merging.
 */
void MemLevel::put_tiering(kv_key_t key, kv_val_t val)
{
    _keys[_count] = key;
    _vals[_count] = val;
}

/**
 * Put a value in this level using the tiering algorthm when merging.
 */
void MemLevel::put_leveling(kv_key_t key, kv_val_t val)
{
    // The key is is sorted. Insert the new key before the first value which it
    // is less than or equal to. Put the value at the same position in the
    // values array.
    for (int i = 0; i < _count; i++)
    {
        if (key <= _keys[i])
        {
            // Shift all the remaining values to make space for the new value.
            std::memmove(&(_keys[i+1]), &(_keys[i]), (_count-i)*sizeof(kv_key_t));
            std::memmove(&(_vals[i+1]), &(_vals[i]), (_count-i)*sizeof(kv_val_t));
            _keys[i] = key;
            _vals[i] = val;
            return;
        }
    }
    // We got to the end of the current values without finding a key which was
    // greather than this key. Put his key and value after the last items.
    _keys[_count] = key;
    _vals[_count] = val;
}

/**
 * Look in the current level for a key using a linear seach because this
 * level is not sorted, i.e. tiering, not leveling.
 */
bool MemLevel::get_tiering(kv_key_t key, kv_val_t &val)
{
    MemKeyValues kvs(_keys, _vals, _count);
    return linearSearch(&kvs, key, val);
}

/**
 * Look in the current level for a key using a binary seach because this
 * level is sorted, i.e. leveling, not tiering.
 */
bool MemLevel::get_leveling(kv_key_t key, kv_val_t &val)
{
    MemKeyValues kvs(_keys, _vals, _count);
    return binarySearch(&kvs, key, val);
}
/**
 * Flush the current level to the next level.
 */
void MemLevel::flush()
{
    // If the next level hasn't been created yet then create it.
    if (!_next)
    {
        _next = new FileLevel(
            _leveling, _size, _size * _ratio, _ratio,
            _bits, _hashes);
    }

    // Merge the files fromt this level to the next level.
    _next->Merge(_keys, _vals, _size);

    _count = 0;
}

/**
 * Return a string with a description of this level and all the levels below it.
 * @param verbose When true include the key and values in the output.
 */
std::string MemLevel::Dump(bool verbose)
{
    std::stringstream output;
    output << "MemLevel  "
           << 0
           << " leveling " << _leveling
           << " size: " << _size
           << " ratio: " << _ratio
           << " count: " << _count
           << " Count: " << Count()
           << "\n";

    if (verbose)
    {
        std::string str;
        for (int i = 0; i < _count; i++)
        {
            value2string(_vals[i], str);
            output << "  " << _keys[i] << "=\"" << str << "\"\n";
        }
    }

    if (_next)
        output << _next->Dump(verbose, 1);

    return output.str();
}

}