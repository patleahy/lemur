/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * KeyValues allows for random access to keys and values in either in-memory
 * arrays or files.
 *
 * The searching alogritms use this type to seach both the memory level and disk
 * levels.
 */

#include "keyvalues.hpp"

namespace kv
{

MemKeyValues::MemKeyValues(kv_key_t* keys, kv_val_t* vals, int count)
    : _keys(keys),
      _vals(vals),
      _count(count)
{
}

kv_key_t MemKeyValues::Key(int i) { return _keys[i]; }
kv_val_t MemKeyValues::Val(int i) { return _vals[i]; }
int MemKeyValues::Count() { return _count; }


FileKeyValues::FileKeyValues(File<kv_key_t> &keys, File<kv_val_t> &vals, int count)
    : _count(count),
        _keys(keys),
        _vals(vals)
{
}

kv_key_t FileKeyValues::Key(int i)
{
    kv_key_t key;
    _keys.Read(key, i);
    return key;
}

kv_val_t FileKeyValues::Val(int i)
{
    kv_val_t val;
    _vals.Read(val, i);
    return val;
}

int FileKeyValues::Count() { return _count; }

}