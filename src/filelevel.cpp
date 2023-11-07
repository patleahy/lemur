/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * A file level in the LSM tree.
 */

#include "filelevel.hpp"
#include "algorithms.hpp"
#include "inputreader.hpp"
#include "outputfilewriter.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <limits.h>

namespace kv
{

/**
 * Create a file level.
 * @param leveling true if using leveling, else use tiering.
 * @param pageSize The size of disk pages.
 * @param levelSize The number of items to store in this level.
 * @param ratio Size ratio between adjacent levels.
 * @param bits The number of bits int he bloom filter.
 * @param hashes The number of hashes in the bloom filter.
 */
FileLevel::FileLevel(bool leveling, int pageSize, int levelSize, int ratio, int bits, int hashes)
    : _leveling(leveling),      // Are we doing leveling or tiering.
      _pageSize(pageSize),      // The size of file pages.
      _levelSize(levelSize),    // The number of items we will store at this level.
      _ratio(ratio),            // Size ratio between adjacent levels.
      _count(0),                // The number of items currently in this level.
      _next(NULL),              // A pointer to the level below this one.
      _filter(bits, hashes, levelSize), // Create the bloom filter.
      _fenceCount(ratio),               // The number of fenced regions.
      _fenceSize(levelSize/ratio)       // The number of items in a fenced region.
{
    // Open the files we will use to store the keys and values for this layer.
    std::stringstream filename;
    filename << "data/lsm." << std::setfill('0') << std::setw(4) << levelSize;

    _keyFile.Open(filename.str() + ".key", true);
    _valFile.Open(filename.str() + ".dat", true);

    // Create the fence posts.
    _fenceMins = new kv_key_t[_fenceCount];
    _fenceMaxs = new kv_key_t[_fenceCount];
    initFences();
}

FileLevel::~FileLevel()
{
    delete [] _fenceMins;
    delete [] _fenceMaxs;
}

/**
 * Get a value from this level or the levels below it.
 * @param key The key to lookup.
 * @param val Set to the value, if it is found.
 * @return true if the key was found.
 */
bool FileLevel::Get(kv_key_t key, kv_val_t &val)
{
    // First check if the key is at this level.
    // Check the bloom filter first.
    if (_filter.Test(key))
    {
        // The bloom filter tells us the key might be in this level.

        // The algoritm we use to seach this level depends on where the level
        // is sorted, i.e. leveling vs tiering.
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
 * Look in the current level for a key using a linear seach because this
 * level is not sorted, i.e. tiering, not leveling.
 */
bool FileLevel::get_tiering(kv_key_t key, kv_val_t &val)
{
    FileKeyValues kvs(_keyFile, _valFile, _count);

    // For each fence region, if the key is between the fence posts then
    // seach within the region.
    for (int f = 0; f < _fenceCount; f++)
    {
        if (key >= _fenceMins[f] && key <= _fenceMaxs[f])
        {
            if (linearSearch(&kvs, key, val, f * _fenceSize, (f+1) * _fenceSize))
                return true;
        }
    }
    return false;
}

/**
 * Look in the current level for a key using a binary seach because this
 * level is sorted, i.e. leveling, not tiering.
 */
bool FileLevel::get_leveling(kv_key_t key, kv_val_t &val)
{
    // Find the fence region which this key is inside and search it.
    for (int f = 0; f < _fenceCount; f++)
    {
        if (key >= _fenceMins[f] && key <= _fenceMaxs[f])
        {
            FileKeyValues kvs(_keyFile, _valFile, _count);
            return binarySearch(&kvs, key, val, f * _fenceSize, (f+1) * _fenceSize);
        }
    }
    return false;
}

/**
 * Return this count of this level plus the levels below it.
 */
int FileLevel::Count()
{

    return _count + (_next ? _next->Count() : 0);
}

/**
 * Merge an array of keys and values into this level.
 * This is called from the memory level.
 */
void FileLevel::Merge(kv_key_t* keys, kv_val_t* vals, int count)
{
    InputMemReader input(keys, vals, count);
    merge(&input);
}

/**
 * Merge keys and values a files into this level. This is called be the level
 * above this one to merge into this level.
 */
void FileLevel::Merge(File<kv_key_t>& keys, File<kv_val_t>& vals, int count)
{
    InputFileReader input(keys, vals, count, _pageSize);
    merge(&input);
}

/**
 * Implemeting the merge using a InputReader which wrappes both arrays of
 * keys/values from the memory level and files from disk levels.
 */
void FileLevel::merge(InputReader* input)
{
    // If this level is full then flush to the next level.
    if (_count == _levelSize)
        flush();

    // Use the leveling or tiering algorthm to copy values from the level above
    // to this level.
    if (_leveling)
        level(input);
    else
        tier(input);
}


/**
 * Flush the current level to the next level.
 */
void FileLevel::flush()
{
    // If the next level hasn't been created yet then create it.
    if (!_next)
    {
        _next = new FileLevel(
            _leveling, _pageSize, _levelSize * _ratio, _ratio,
            _filter.getNumBits(), _filter.getNumHashes());
    }

    // Merge the files fromt this level to the next level.
    _next->Merge(_keyFile, _valFile, _count);

    // Now there is no values at this level. Clear the bloom filters, the
    // fence posts and the count.
    _filter.Clear();
    initFences();
    _count = 0;
}

/**
 * Copy the values from an upper level into this level using tiering.
 */
void FileLevel::tier(InputReader* input)
{
    // Just read the values in order from the input and and write the to the
    // output.
    OutputFileWriter output(_keyFile, _valFile, _pageSize, _count);
    while (input->HasNext())
    {
        kv_key_t key;
        kv_val_t val;
        input->Next(key, val);
        out(key, val, output);
    }
}

/**
 * Copy the values from an upper level into this level using leveling.
 * i.e. merge sort the values into this level.
 */
void FileLevel::level(InputReader* left)
{
    // Open temp files to write the output into.
    File<kv_key_t> keyOut;
    File<kv_val_t> valOut;
    keyOut.Open(_keyFile.getFilename() + ".tmp", true);
    valOut.Open(_valFile.getFilename() + ".tmp", true);
    OutputFileWriter output(keyOut, valOut, _pageSize, 0);

    // An easy way to keep the bloom filters and fences correct it to
    // clear them here and then read each value as it is beign written to the
    // output.
    _filter.Clear();
    initFences();

    kv_key_t lk, rk;
    kv_val_t lv, rv;

    // Check if there are any values currently in this level.
    if (_count > 0)
    {
        // Create a buffer wrapper around the files for this level.
        // Use a pointer for right value so it look the same as the left one.
        InputFileReader r(_keyFile, _valFile, _count, _pageSize);
        InputFileReader* right = &r;

        // The two sides of the merge will be called left and right.

        // Reset the count before writing out all the values.
        _count = 0;

        // Get the first keys and values.
        left->Next(lk, lv);
        right->Next(rk, rv);
        while (true)
        {
            // Write the key and value out for which ever key is smaller.
            // Then get another value from the side we consumed.
            //
            // Where there are no more values from that side then write the
            // other value out and finish this loop.
            //
            // If this the key in the left and right side match then insert the
            // left key (i.e. the one from the upper layer) first. This will
            // make this sort stable, i.e. the values for the same key will be
            // in the order they were inserted.
            if (lk <= rk)
            {
                out(lk, lv, output);
                if (left->HasNext())
                {
                    left->Next(lk, lv);
                }
                else
                {
                    out(rk, rv, output);
                    break;
                }
            }
            else
            {
                out(rk, rv, output);
                if (right->HasNext())
                {
                    right->Next(rk, rv);
                }
                else
                {
                    out(lk, lv, output);
                    break;
                }
            }
        }

        // If the right side wasn't exausted the left side must have been
        // exausted. Write out the remaining right values.
        while (right->HasNext())
        {
            right->Next(rk, rv);
            out(rk, rv, output);
        }
    }


    // If the left side wasn't exausted the right side must have been
    // exausted. Write out the remaining left values.
    // This will also be used if there are no values on the right side,
    // i.e. this level currently dorsn't have any values.
    while (left->HasNext())
    {
        left->Next(lk, lv);
        out(lk, lv, output);
    }

    // Flush all the data to disk.
    output.Flush();

    // Now close the files for this level and the temp files.
    // Rename the temp files over the files for this level.

    _keyFile.Close();
    keyOut.Close();
    std::rename(keyOut.getFilename().c_str(), _keyFile.getFilename().c_str());
    _keyFile.Open(_keyFile.getFilename(), false);

    _valFile.Close();
    valOut.Close();
    std::rename(valOut.getFilename().c_str(), _valFile.getFilename().c_str());
    _valFile.Open(_valFile.getFilename(), false);
}

/**
 * Write the key and value to the output buffers. Update the bloom filter and
 * fence posts with the key. Increment the count.
 */
void FileLevel::out(kv_key_t key, kv_val_t val, OutputFileWriter& output)
{
    output.Push(key, val);

    _filter.Add(key);

    int f = _count / _fenceSize;
    if (key < _fenceMins[f])
        _fenceMins[f] = key;
    if (key > _fenceMaxs[f])
        _fenceMaxs[f] = key;

    _count++;
}

/**
 * Set all the fence mininims to intmax and all the fence maximumns.
 */
void FileLevel::initFences()
{
    for (int i = 0; i < _fenceCount; i++)
    {
        _fenceMins[i] = INT_MAX;
        _fenceMaxs[i] = INT_MIN;
    }
}

/**
 * Return a string with a description of this level and all the levels below it.
 * @param verbose When true include the key and values in the output.
 */
std::string FileLevel::Dump(bool verbose, int depth)
{
    std::stringstream output;
    output << "FileLevel "
           << depth
           << " leveling " << _leveling
           << " size: " << _levelSize
           << " ratio: " << _ratio
           << " count: " << _count
           << " Count: " << Count()
           << std::endl;

    if (verbose)
    {
        output << "Fence ";
        for (int i = 0; i < _fenceCount; i++)
        {
            output << "[ " << _fenceMins[i] << ", " << _fenceMaxs[i] << " ]";
        }
        output << std::endl;

        kv_key_t key;
        kv_val_t val;
        std::string str;
        for (int i = 0; i < _count; i++)
        {
            _keyFile.Read(key, i);
            _valFile.Read(val, i);
            value2string(val, str);
            output << "  " << key << "=\"" << str << "\"" << std::endl;
        }
    }

    if (_next)
        output << _next->Dump(verbose, depth+1);

    return output.str();
}

}