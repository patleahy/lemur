/**
 * Wrappers for reading values from levels during leveling and tiering.
 *
 * These wrappers are designed for sequencal access to the level data.
 * For random access see FileKeyValues and MemKeyValues.
 */

#include "inputreader.hpp"

namespace kv
{

/**
 * An input reader which reads from keys and values in memory.
 * Used when merging from the memory level (level 0) to the first disk level.
 * For sequental access see MemKeyValues.
 *
 * @param keyFile The key file to read from.
 * @param valFile The value file to read from.
 * @param count. The number of values in the files.
 * @param pageSize the size of disk pages to use.
 */
InputFileReader::InputFileReader(File<kv_key_t> &keyFile, File<kv_val_t> &valFile, int count, int pageSize)
    : _pageSize(pageSize),
      _count(count),
      _keyFile(keyFile),
      _valFile(valFile),
      _f(0),    // The last position read from the files.
      _i(0)     // The number of items read from the file.
{
    // Create two arrays to act as memory buffers.
    _keys = new kv_key_t[pageSize];
    _vals = new kv_val_t[pageSize];

    // Get the first values.
    fetch();
}

InputFileReader::~InputFileReader()
{
    delete [] _keys;
    delete [] _vals;
}

void InputFileReader::Next(kv_key_t &key, kv_val_t &val)
{
    // If we read all items in the arrays get more.
    if (_b == _pageSize)
        fetch();

    key = _keys[_b];
    val = _vals[_b];
    _b++;
    _i++;
}

bool InputFileReader::HasNext() { return _i < _count; }

void InputFileReader::fetch()
{
    _keyFile.Read(_keys, _f, _pageSize);
    _valFile.Read(_vals, _f, _pageSize);
    _f += _pageSize;
    _b = 0;
}

/**
 * An input reader which reads from files.
 * Used when merging from a disk level to another disk level.
 * For sequental access see FileKeyValues.
 */
InputMemReader::InputMemReader(kv_key_t* keys, kv_val_t* vals, int count)
    : _keys(keys),
      _vals(vals),
      _count(count),
      _i(0)
{
}

void InputMemReader::Next(kv_key_t &key, kv_val_t &val)
{
    key = _keys[_i];
    val = _vals[_i];
    _i++;
}

bool InputMemReader::HasNext() { return _i < _count; }

}