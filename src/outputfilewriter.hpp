/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * Buffered wrapper to write keys and values to files. Used when merging levels.
 */

#ifndef OUTPUTFILEWRITER_H
#define OUTPUTFILEWRITER_H

#include "types.hpp"
#include "file.hpp"
#include "bloomfilter.hpp"

namespace kv
{

/**
 * Buffered wrapper to write keys and values to files. Used when merging levels.
 */
class OutputFileWriter
{
public:

    /**
     * Create a writer over key and value files.
     *
     * @param keys An open key file.
     * @param vals An open values file.
     * @param pageSize The size of buffer to use.
     * @param pos The postion to start writing to in the files. This us used
     *            to append values to an existing file.
     */
    OutputFileWriter(File<kv_key_t>& keys, File<kv_val_t>& vals, int pageSize, int pos);

    /**
     * Flush all keys and values to disk.
     */
    ~OutputFileWriter();

    /**
     * Add a key and value to the end of the files.
     */
    void Push(kv_key_t key, kv_val_t val);

    /**
     * Flush all keys and values to disk.
     */
    void Flush();

private:
    File<kv_key_t>& _keyFile;   // The files.
    File<kv_val_t>& _valFile;
    kv_key_t* _keys;    // The buffers.
    kv_val_t* _vals;
    int _pageSize;      // The buffer size.
    int _f;             // The position in the file to write to.
    int _b;             // The position in the buffer.
};

}
#endif