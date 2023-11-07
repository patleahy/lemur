/**
 * Wrappers for reading values from levels during leveling and tiering.
 *
 * These wrappers are designed for sequencal access to the level data.
 * For random access see FileKeyValues and MemKeyValues.
 */

#ifndef KVINPUTREADER_H
#define KVINPUTREADER_H

#include "file.hpp"
#include "types.hpp"

namespace kv
{

/**
 * Base class for an imput reader which can read from files or arrays.
 * See InputFileReader and InputMemReader. This type is passed to the
 * tier or level methods in filelevel. Thoes methods can call HasNext() and
 * Next() without knowing if the data is comming from a memory arrays or disk.
 * These wrappers are designed for sequencal access to the level data.
 * For random access see FileKeyValues and MemKeyValues.
 */
class InputReader
{
public:
    virtual void Next(kv_key_t &key, kv_val_t &val) {}
    virtual bool HasNext() { return false; }
};

/**
 * An input reader which reads from keys and values in memory.
 * Used when merging from the memory level (level 0) to the first disk level.
 * For sequental access see MemKeyValues.
 */
class InputMemReader : public InputReader
{
public:
    InputMemReader(kv_key_t* keys, kv_val_t* vals, int count);
    void Next(kv_key_t &key, kv_val_t &val) override;
    bool HasNext() override;

private:
    kv_key_t* _keys;
    kv_val_t* _vals;
    int _count;
    int _i;
};

/**
 * An input reader which reads from files.
 * Used when merging from a disk level to another disk level.
 * For sequental access see FileKeyValues.
 */
class InputFileReader : public InputReader
{
public:
    InputFileReader(File<kv_key_t>& keys, File<kv_val_t>& vals, int count, int pageSize);
    ~InputFileReader();
    void Next(kv_key_t &key, kv_val_t &val) override;
    bool HasNext() override;

private:
    void fetch();

    kv_key_t* _keys;
    kv_val_t* _vals;
    File<kv_key_t>& _keyFile;
    File<kv_val_t>& _valFile;
    int _pageSize;
    int _count;
    int _f;
    int _b;
    int _i;
};

}
#endif