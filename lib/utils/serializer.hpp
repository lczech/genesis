#ifndef GENESIS_UTILS_SERIALIZER_H_
#define GENESIS_UTILS_SERIALIZER_H_

/**
 * @brief Header of Serializer and Deserializer class.
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

#include "utils/logging.hpp"

namespace genesis {

// =================================================================================================
//     Serializer
// =================================================================================================

/**
 * @brief
 */
class Serializer
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Destructor
    // -------------------------------------------------------------------------

    Serializer (const std::string& file_name) :
        outfile   (file_name, std::ofstream::out | std::ofstream::binary),
        outstream (outfile)
    {
        if (!outfile) {
            LOG_ERR << "Cannot create Deserializer: " << strerror(errno);
        }
    };

    Serializer (std::ostream& outstream) :
        outstream (outstream)
    {};

    ~Serializer()
    {
        outfile.close();
    }

    // -------------------------------------------------------------------------
    //     Stream Status
    // -------------------------------------------------------------------------

    inline operator bool() const
    {
        return outstream;
    }

    inline bool good() const
    {
        return outstream.good();
    }

    inline bool eof() const
    {
        return outstream.eof();
    }

    inline bool fail() const
    {
        return outstream.fail();
    }

    inline bool bad() const
    {
        return outstream.bad();
    }

    // -------------------------------------------------------------------------
    //     File Status
    // -------------------------------------------------------------------------

    inline bool is_open() const
    {
        return outfile.is_open();
    }

    inline void flush()
    {
        outfile.flush();
    }

    inline void close()
    {
        outfile.close();
    }

    // -------------------------------------------------------------------------
    //     Serialization
    // -------------------------------------------------------------------------

    void put_null (const size_t n)
    {
        char* buffer = new char[n];
        std::fill(buffer, buffer+n, 0);
        outstream.write (buffer, n);
        delete[] buffer;
    }

    void put_raw (char* data, size_t n)
    {
        outstream.write (data, n);
    }

    void put_string (const std::string& v)
    {
        size_t len = v.length();
        put_plain(len);
        outstream.write (v.c_str(), len);
    }

    template<typename T>
    void put_plain (const T v)
    {
        outstream.write((char*)(&v), sizeof(v));
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

protected:
    std::ofstream outfile;
    std::ostream& outstream;
};

// =================================================================================================
//     Deserializer
// =================================================================================================

/**
 * @brief
 */
class Deserializer
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Destructor
    // -------------------------------------------------------------------------

    Deserializer (const std::string& file_name) :
        infile   (file_name, std::ifstream::in | std::ifstream::binary),
        instream (infile)
    {
        if (!infile) {
            LOG_ERR << "Cannot create Deserializer: " << strerror(errno);
        }
    };

    Deserializer (std::istream& instream) :
        instream(instream)
    {};

    ~Deserializer()
    {
        infile.close();
    }

    // -------------------------------------------------------------------------
    //     Stream Status
    // -------------------------------------------------------------------------

    inline operator bool() const
    {
        return instream;
    }

    inline bool good() const
    {
        return instream.good();
    }

    inline bool eof() const
    {
        return instream.eof();
    }

    inline bool fail() const
    {
        return instream.fail();
    }

    inline bool bad() const
    {
        return instream.bad();
    }

    // -------------------------------------------------------------------------
    //     File Status
    // -------------------------------------------------------------------------

    inline bool is_open() const
    {
        return infile.is_open();
    }

    inline void close()
    {
        infile.close();
    }

    // -------------------------------------------------------------------------
    //     Serialization
    // -------------------------------------------------------------------------

    void get_null (size_t n)
    {
        char* buffer = new char[n];
        instream.read(buffer, n);
        delete[] buffer;
    }

    void get_raw(char* buffer, size_t n)
    {
        instream.read(buffer, n);
    }

    std::string get_string ()
    {
        size_t len = get_plain<size_t>();
        char* buffer = new char[len];
        instream.read(buffer, len);

        std::string str (buffer, len);
        delete[] buffer;
        return str;
    }

    template<typename T>
    T get_plain ()
    {
        T res;
        instream.read((char*)(&res), sizeof(T));
        return res;
    }

    template<typename T>
    void get_plain (T& res)
    {
        instream.read((char*)(&res), sizeof(T));
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

protected:
    std::ifstream infile;
    std::istream& instream;
};

} // namespace genesis

#endif // include guard
