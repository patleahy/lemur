/**
 * Pat Leahy pat@patleahy.com - CSEP 544 SP21 - Mini-project - Lemur
 *
 * A file level in the LSM tree.
 */

#ifndef KVFILELEVEL_H
#define KVFILELEVEL_H

#include "types.hpp"
#include "keyvalues.hpp"
#include "file.hpp"
#include "inputreader.hpp"
#include "outputfilewriter.hpp"
#include "bloomfilter.hpp"

namespace kv
{

/**
 * A file level in the LSM tree.
 */
class FileLevel
{
public:

    /**
     * Create a file level.
     * @param leveling true if using leveling, else use tiering.
     * @param pageSize The size of disk pages.
     * @param levelSize The number of items to store in this level.
     * @param ratio Size ratio between adjacent levels.
     * @param bits The number of bits int he bloom filter.
     * @param hashes The number of hashes in the bloom filter.
     */
    FileLevel(bool leveling, int pageSize, int levelSize, int ratio, int bits, int hashes);
    ~FileLevel();

    /**
     * Get a value from this level or the levels below it.
     * @param key The key to lookup.
     * @param val Set to the value, if it is found.
     * @return true if the key was found.
     */
    bool Get(kv_key_t key, kv_val_t &val);

    /**
     * Return this count of this level plus the levels below it.
     */
    int Count();

    /**
     * Merge an array of keys and values into this level.
     * This is called from the memory level.
     */
    void Merge(kv_key_t* keys, kv_val_t* vals, int count);

    /**
     * Merge keys and values a files into this level. This is called be the level
     * above this one to merge into this level.
     */
    void Merge(File<kv_key_t>& keys, File<kv_val_t>& vals, int count);

    /**
     * Return a string with a description of this level and all the levels below it.
     * @param verbose When true include the key and values in the output.
     */
    std::string Dump(bool verbose, int depth);

private:
    /**
     * Look in the current level for a key using a linear seach because this
     * level is not sorted, i.e. tiering, not leveling.
     */
    bool get_tiering(kv_key_t key, kv_val_t &val);

    /**
     * Look in the current level for a key using a binary seach because this
     * level is sorted, i.e. leveling, not tiering.
     */
    bool get_leveling(kv_key_t key, kv_val_t &val);

    /**
     * Flush the current level to the next level.
     */
    void flush();

    /**
     * Implemeting the merge using a InputReader which wrappes both arrays of
     * keys/values from the memory level and files from disk levels.
     */
    void merge(InputReader* input);

    /**
     * Copy the values from an upper level into this level using tiering.
     */
    void tier(InputReader* input);

    /**
     * Copy the values from an upper level into this level using leveling.
     * i.e. merge sort the values into this level.
     */
    void level(InputReader* input);

    /**
     * Write the key and value to the output buffers. Update the bloom filter and
     * fence posts with the key. Increment the count.
     */
    void out(kv_key_t key, kv_val_t val, OutputFileWriter& output);

    /**
     * Set all the fence mininims to intmax and all the fence maximumns.
     */
    void initFences();

    bool _leveling;             // Are we doing leveling or tiering.
    int _pageSize;              // The size of file pages.
    int _levelSize;             // The number of items we will store at this level.
    int _ratio;                 // Size ratio between adjacent levels.
    int _count;                 // The number of items currently in this level.
    File<kv_key_t> _keyFile;    // The file with keys.
    File<kv_val_t> _valFile;    // The file with values.
    FileLevel *_next;           // The level below this one.
    BloomFilter _filter;        // The bloom filter.
    int _fenceCount;            // The number of fenced regions.
    int _fenceSize;             // The number of items in a fenced region.
    int* _fenceMins;            // The min feence post values.
    int* _fenceMaxs;            // The max feence post values.
};

}
#endif