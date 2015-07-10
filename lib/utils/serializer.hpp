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

    /**
     * @brief Write `n` zero bytes (`\0`) to the stream.
     */
    void put_null (const size_t n)
    {
        char* buffer = new char[n];
        std::fill(buffer, buffer+n, '\0');
        outstream.write (buffer, n);
        delete[] buffer;
    }

    /**
     * @brief Write raw data, provided as a char array of length `n`, to the stream.
     */
    void put_raw (char* data, size_t n)
    {
        outstream.write (data, n);
    }

    /**
     * @brief Write raw data, provided as a string, to the stream, without writing its length.
     */
    void put_raw_string (const std::string& v)
    {
        outstream.write (v.c_str(), v.length());
    }

    /**
     * @brief Write a string, preceded by its length, to the stream. Use get_string() to read it.
     */
    void put_string (const std::string& v)
    {
        size_t len = v.length();
        put_int(len);
        put_raw_string(v);
    }

    /**
     * @brief Write plain data to the stream, by casting it to a char array.
     */
    template<typename T>
    void put_plain (const T v)
    {
        outstream.write((char*)(&v), sizeof(v));
    }

    /**
     * @brief Write an integer number to the stream.
     *
     * Currently, this simply uses put_plain(), but future versions might change this behaviour and
     * use specific conversions (litte/big endianness, signed/unsigned) before writing.
     */
    template<typename T>
    void put_int (const T v)
    {
        put_plain(v);
    }

    /**
     * @brief Write a floating point number to the stream.
     *
     * Currently, this simply uses put_plain(), but future versions might change this behaviour and
     * convert it to some machine-independent format.
     */
    template<typename T>
    void put_float (const T v)
    {
        put_plain(v);
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

    /**
     * @brief Reads `n` bytes from the stream and returns whether all of them are `\0` bytes.
     */
    bool get_null (size_t n)
    {
        char* buffer = new char[n];
        instream.read(buffer, n);

        bool ret = true;
        for (size_t i = 0; i < n; ++i) {
            ret &= (buffer[i] == '\0');
        }

        delete[] buffer;
        return ret;
    }

    /**
     * @brief Read `n` bytes from the stream and store them in the buffer.
     *
     * The buffer needs to be big enough to hold `n` bytes.
     */
    void get_raw(char* buffer, size_t n)
    {
        instream.read(buffer, n);
    }

    /**
     * @brief Read `n` bytes from the stream and return them as a string.
     */
    std::string get_raw_string(size_t n)
    {
        char* buffer = new char[n];
        instream.read(buffer, n);

        std::string str (buffer, n);
        delete[] buffer;
        return str;
    }

    /**
     * @brief Read a string from the stream, provided that its length it written preceding it, as done
     * by put_string().
     */
    std::string get_string ()
    {
        size_t len = get_int<size_t>();
        return get_raw_string(len);
    }

    // TODO maybe trailing return types is a solution to make this work without having to specify the template parameters? (also for the othter, similar methods in this class)
    /**
     * @brief Read as many bytes from the stream as the type T holds, and return them in form of
     * a value of type T.
     */
    template<typename T>
    T get_plain ()
    {
        T res;
        instream.read((char*)(&res), sizeof(T));
        return res;
    }

    /**
     * @brief Read as many bytes from the stream as the type T holds, and put them in the result
     * value of type T.
     */
    template<typename T>
    void get_plain (T& res)
    {
        instream.read((char*)(&res), sizeof(T));
    }

    /**
     * @brief Read an integer number from the stream and return it.
     */
    template<typename T>
    T get_int ()
    {
        return get_plain<T>();
    }

    /**
     * @brief Read an integer number from the stream and store it in the result.
     */
    template<typename T>
    void get_int (T& res)
    {
        res = get_plain<T>();
    }

    /**
     * @brief Read a floating point number from the stream and return it.
     */
    template<typename T>
    T get_float ()
    {
        return get_plain<T>();
    }

    /**
     * @brief Read an floating point number from the stream and store it in the result.
     */
    template<typename T>
    void get_float (T& res)
    {
        res = get_plain<T>();
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
