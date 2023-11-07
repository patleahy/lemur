/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * KeyValues allows for random access to keys and values in either in-memory
 * arrays or files.
 *
 * The searching alogritms use this type to seach both the memory level and disk
 * levels.
 */

#ifndef KVKEYVALUES_H
#define KVKEYVALUES_H

#include "types.hpp"
#include "file.hpp"

namespace kv
{

/**
 * KeyValues allows for random access to keys and values in either in-memory
 * arrays or files.
 */
class KeyValues
{
public:
    virtual kv_key_t Key(int i) { return k; }
    virtual kv_val_t Val(int i) { return v; }
    virtual int Count() { return 0; }

private:
    kv_key_t k;
    kv_val_t v;
};


/**
 * Random access to the memory arrays.
 */
class MemKeyValues : public KeyValues
{
public:
    MemKeyValues(kv_key_t* keys, kv_val_t* vals, int count);
    kv_key_t Key(int i) override;
    kv_val_t Val(int i) override;
    int Count() override;

private:
    kv_key_t* _keys;
    kv_val_t* _vals;
    int _count;
};


/**
 * Random access to the keys and values in files.
 */
class FileKeyValues : public KeyValues
{
public:
    FileKeyValues(File<kv_key_t> &keys, File<kv_val_t> &vals, int count);
    kv_key_t Key(int i) override;
    kv_val_t Val(int i) override;
    int Count();

private:
    File<kv_key_t>& _keys;
    File<kv_val_t>& _vals;
    int _count;
};

}
#endif