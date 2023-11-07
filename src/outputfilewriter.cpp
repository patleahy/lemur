/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * Buffered wrapper to write keys and values to files. Used when merging levels.
 */

#include "outputfilewriter.hpp"

namespace kv
{

/**
 * Create a writer over key and value files.
 *
 * @param keys An open key file.
 * @param vals An open values file.
 * @param pageSize The size of buffer to use.
 * @param pos The postion to start writing to in the files. This us used
 *            to append values to an existing file.
 */
OutputFileWriter::OutputFileWriter(
    File<kv_key_t>& keyFile, File<kv_val_t>& valFile,
    int pageSize, int pos)
    : _keyFile(keyFile),
      _valFile(valFile),
      _pageSize(pageSize),
      _f(pos),
      _b(0)
{
    _keys = new kv_key_t[pageSize];
    _vals = new kv_val_t[pageSize];
}

OutputFileWriter::~OutputFileWriter()
{
    Flush();
    delete [] _keys;
    delete [] _vals;
}

/**
 * Add a key and value to the end of the files.
 */
void OutputFileWriter::Push(kv_key_t key, kv_val_t val)
{
    _keys[_b] = key;
    _vals[_b] = val;
    _b++;

    if (_b == _pageSize)
        Flush();
}

/**
 * Flush all keys and values to disk.
 */
void OutputFileWriter::Flush()
{
    if (_b == 0)
        return;
    _keyFile.Write(_keys, _f, _b);
    _valFile.Write(_vals, _f, _b);
    _f += _b;
    _b = 0;
}

}