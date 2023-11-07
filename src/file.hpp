/**
 * Pat Leahy patleahy@uw.edu - CSEP 544 SP21 - Mini-project - Lemur
 *
 * A simple wrapper around a file to simplify reading a writing keys and values.
 */

#ifndef KVFILE_H
#define KVFILE_H

#include <string>
#include <fstream>

namespace kv
{

/**
 * This is templitized to make it easy to read and write a given data type.
 * E.g. to read and write keys create an instance of File<kv_key_t>
 */
template<class T>
class File
{
public:
    /**
     * Creating a file does nothing.
     * You need to call Open.
     */
    File()
    {
    }

    /**
     * Close the file is it is open.
     */
    ~File()
    {
        Close();
    }

    /**
     * Open the file for reading and writing.
     * @param filename The file path.
     * @param trunc If true, truncate the file if it already exists.
     */
    void Open(std::string filename, bool trunc)
    {
        _filename = filename;
        auto mode = std::fstream::in | std::fstream::out | std::fstream::binary;
        if (trunc)
            mode |= std::fstream::trunc;
        _stream = std::fstream(filename, mode);
    }

    /**
     * Close the file if it is open.
     */
    void Close()
    {
        if (_stream.is_open())
        {
            _stream.flush();
            _stream.close();
        }
    }

    /**
     * Read an item at the current read position.
     */
    void Read(T &data)
    {
        _stream.read((char*)(&data), sizeof(T));
    }

    /**
     * Read an item at a given position.
     */
    void Read(T &data, int pos)
    {
        _stream.seekg(pos * sizeof(T), _stream.beg);
        Read(data);
    }

    /**
     * Read an array of items from a given position in the file.
     */
    void Read(T *data, int pos, int count)
    {
        _stream.seekg(pos * sizeof(T), _stream.beg);
        _stream.read((char*)data, count * sizeof(T));
    }

    /**
     * Write an item at the current write position.
     * This will overrite the current contents.
     */
    void Write(T data)
    {
        _stream.write((char*)(&data), sizeof(T));
        _stream.flush();
    }

    /**
     * Write an array of items at a given position in the file.
     * This will overrite the current contents.
     */
    void Write(T data, int pos)
    {
        Write(&data, pos, 1);
    }

    /**
     * Write an array of items from a given position in the file.
     * This will overrite the current contents.
     */
    void Write(T *data, int pos, int count)
    {
        _stream.seekp(pos * sizeof(T), _stream.beg);
        _stream.write((char*)data, count*sizeof(T));
        _stream.flush();
    }

    /**
     * Rewind the read and write positions to the start of the file.
     */
    void Rewind()
    {
        _stream.seekg(0, _stream.beg);
        _stream.seekp(0, _stream.beg);
    }

    std::string getFilename() { return _filename; }

private:
    std::string _filename;
    std::fstream _stream;
};

}
#endif