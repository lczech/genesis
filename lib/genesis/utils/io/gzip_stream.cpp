/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab,  Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35,  D-69118 Heidelberg,  Germany
*/

/*
    The code in this file as well as the according header file lib/utils/io/gzip_stream.hpp are
    adapted from the excellent zstr library (C++ header-only ZLib wrapper" classes) by Matei David,
    see https://github.com/mateidavid/zstr

    We adapted the original code by renaming all classes and variables to our standards,
    moving much of the implementation into a source file (so that the header does not clutter
    its callers with zlib-internal symbols), and refining some functionality.

    For this and the according header file, we need to include the following original license:

    The MIT License (MIT)

    Copyright (c) 2015 Matei David, Ontario Institute for Cancer Research

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/io/gzip_stream.hpp"

#include <cassert>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef GENESIS_ZLIB

#    include "zlib.h"

#    if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#       include <fcntl.h>
#       include <io.h>
#   endif

#endif // GENESIS_ZLIB

namespace genesis {
namespace utils {

// We only include all the class definitions if we actually use zlib.
// If not, we later also provide dummy implementations that throw if instanciated.
#ifdef GENESIS_ZLIB

// ================================================================================================
//     Gzip Stream Wrapper
// ================================================================================================

/**
 * @brief Internal helper class to wrap the original zlib z_stream class to be used in an
 * iostream for gzip/zlib (de)compression.
 *
 * The class is based on the internal zstr::detail::z_stream_wrapper class of the
 * [zstr library](https://github.com/mateidavid/zstr) by Matei David; see also our
 * @link supplement_acknowledgements_code_reuse_gzip_streams Acknowledgements@endlink.
 */
class GzipStreamWrapper
    : public z_stream
{
public:

    GzipStreamWrapper( bool is_input = true, int level = Z_DEFAULT_COMPRESSION )
        : is_input_(is_input)
    {
        this->zalloc = Z_NULL;
        this->zfree = Z_NULL;
        this->opaque = Z_NULL;
        int ret;

        if( is_input_ ) {
            this->avail_in = 0;
            this->next_in = Z_NULL;
            ret = inflateInit2( this, 15+32 );
        } else {
            if(
                level < static_cast<int>( GzipCompressionLevel::kDefaultCompression ) ||
                level > static_cast<int>( GzipCompressionLevel::kBestCompression )
            ) {
                throw std::invalid_argument(
                    "Compression level " + std::to_string( static_cast<int>(level )) +
                    " is invalid for usage in gzip output stream. Valid range is [ -1, 9 ]."
                );
            }
            ret = deflateInit2( this, level, Z_DEFLATED, 15+16, 8, Z_DEFAULT_STRATEGY );
        }

        if( ret != Z_OK ) {
            throw except::GzipError( this->msg, ret );
        }
    }

    ~GzipStreamWrapper()
    {
        if( is_input_ ) {
            inflateEnd(this);
        } else {
            deflateEnd(this);
        }
    }

private:

    bool is_input_;
};

// ================================================================================================
//     Gzip Input Stream Buffer
// ================================================================================================

/**
 * @brief Internal helper class that is a gzip-decompressing input stream buffer.
 *
 * GzipIStreambuf is the core decompression class. This is constructed from an existing
 * std::streambuf that contains source data. The constructor accepts explicit settings for the
 * internal buffer size (default: 1 MB) and the auto-detection option (default: on).
 * ZLib errors cause exceptions to be thrown.
 *
 * We do not expose this class to our headers, as there is currently no need, and this would
 * only clutter the namespace. However, if needed, the class declaration can trivially be moved
 * to the header, while keeping the implementation here (so that we do not spam the header and all
 * places where the header is included with the zlib internal symbols). For this to work, the
 * header additionally needs a forward declaration of GzipStreamWrapper.
 *
 * The class is based on the zstr::istreambuf class of the
 * [zstr library](https://github.com/mateidavid/zstr) by Matei David; see also our
 * @link supplement_acknowledgements_code_reuse_gzip_streams Acknowledgements@endlink.
 */
class GzipIStreambuf
    : public std::streambuf
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    GzipIStreambuf(
        std::streambuf * sbuf_p,
        bool auto_detect = true,
        std::size_t buff_size = GZIP_DEFAULT_BUFFER_SIZE
    )
        : sbuf_p_(sbuf_p)
        , zstrm_ptr_(nullptr)
        , auto_detect_(auto_detect)
        , buff_size_(buff_size)
        , auto_detect_run_(false)
        , is_text_(false)
    {
        assert(sbuf_p_);
        in_buff_ = new char [buff_size_];
        in_buff_start_ = in_buff_;
        in_buff_end_ = in_buff_;
        out_buff_ = new char [buff_size_];
        setg(out_buff_, out_buff_, out_buff_);
    }

    GzipIStreambuf( GzipIStreambuf const& ) = delete;
    GzipIStreambuf( GzipIStreambuf && ) = delete;
    GzipIStreambuf& operator = ( GzipIStreambuf const& ) = delete;
    GzipIStreambuf& operator = ( GzipIStreambuf && ) = delete;

    virtual ~GzipIStreambuf()
    {
        delete [] in_buff_;
        delete [] out_buff_;
        if( zstrm_ptr_ ) {
            delete zstrm_ptr_;
        }
    }

    // -------------------------------------------------------------
    //     Virtual functions
    // -------------------------------------------------------------

    virtual std::streambuf::int_type underflow() override
    {
        if (this->gptr() == this->egptr()) {
            // pointers for free region in output buffer
            char * out_buff_free_start = out_buff_;

            do {
                // read more input if none available
                if (in_buff_start_ == in_buff_end_) {
                    // empty input buffer: refill from the start
                    in_buff_start_ = in_buff_;
                    std::streamsize sz = sbuf_p_->sgetn(in_buff_, buff_size_);
                    in_buff_end_ = in_buff_ + sz;
                    if (in_buff_end_ == in_buff_start_) break; // end of input
                }

                // auto detect if the stream contains text or deflate data
                if (auto_detect_ && ! auto_detect_run_) {
                    auto_detect_run_ = true;
                    unsigned char b0 = *reinterpret_cast< unsigned char * >(in_buff_start_);
                    unsigned char b1 = *reinterpret_cast< unsigned char * >(in_buff_start_ + 1);

                    // Ref:
                    // http://en.wikipedia.org/wiki/Gzip
                    // http://stackoverflow.com/questions/9050260/what-does-a-zlib-header-look-like
                    is_text_ = ! (in_buff_start_ + 2 <= in_buff_end_
                                 && ((b0 == 0x1F && b1 == 0x8B)         // gzip header
                                     || (b0 == 0x78 && (b1 == 0x01      // zlib header
                                                        || b1 == 0x9C
                                                        || b1 == 0xDA))));
                }

                if (is_text_) {

                    // simply swap in_buff_ and out_buff_, and adjust pointers
                    assert(in_buff_start_ == in_buff_);
                    std::swap(in_buff_, out_buff_);
                    out_buff_free_start = in_buff_end_;
                    in_buff_start_ = in_buff_;
                    in_buff_end_ = in_buff_;

                } else {

                    // run inflate() on input
                    if (! zstrm_ptr_) {
                        zstrm_ptr_ = new GzipStreamWrapper(true);
                    }
                    zstrm_ptr_->next_in = reinterpret_cast< decltype(zstrm_ptr_->next_in) >(in_buff_start_);
                    zstrm_ptr_->avail_in = in_buff_end_ - in_buff_start_;
                    zstrm_ptr_->next_out = reinterpret_cast< decltype(zstrm_ptr_->next_out) >(out_buff_free_start);
                    zstrm_ptr_->avail_out = (out_buff_ + buff_size_) - out_buff_free_start;
                    int ret = inflate(zstrm_ptr_, Z_NO_FLUSH);

                    // process return code
                    if (ret != Z_OK && ret != Z_STREAM_END) {
                        throw except::GzipError(zstrm_ptr_->msg, ret);
                    }

                    // update in&out pointers following inflate()
                    in_buff_start_ = reinterpret_cast< decltype(in_buff_start_) >(zstrm_ptr_->next_in);
                    in_buff_end_ = in_buff_start_ + zstrm_ptr_->avail_in;
                    out_buff_free_start = reinterpret_cast< decltype(out_buff_free_start) >(zstrm_ptr_->next_out);
                    assert(out_buff_free_start + zstrm_ptr_->avail_out == out_buff_ + buff_size_);

                    // if stream ended, deallocate inflator
                    if (ret == Z_STREAM_END) {
                        delete zstrm_ptr_;
                        zstrm_ptr_ = nullptr;
                    }
                }
            } while (out_buff_free_start == out_buff_);

            // 2 exit conditions:
            // - end of input: there might or might not be output available
            // - out_buff_free_start != out_buff_: output available
            this->setg(out_buff_, out_buff_, out_buff_free_start);
        }

        return this->gptr() == this->egptr()
            ? traits_type::eof()
            : traits_type::to_int_type(*this->gptr())
        ;
    }

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

private:

    std::streambuf * sbuf_p_;
    char * in_buff_;
    char * in_buff_start_;
    char * in_buff_end_;
    char * out_buff_;
    GzipStreamWrapper * zstrm_ptr_;
    bool auto_detect_;
    std::size_t buff_size_;
    bool auto_detect_run_;
    bool is_text_;
};

// ================================================================================================
//     Gzip Output Stream Buffer
// ================================================================================================

/**
 * @brief Internal helper class that is a gzip-compressing output stream buffer.
 *
 * GzipOStreambuf is the core compression class. This is constructed from an existing
 * std::streambuf that contains sink data. The constructor accepts explicit settings for the
 * internal buffer size (default: 1 MB) and the compression option (default: ZLib default).
 * ZLib errors cause exceptions to be thrown.
 *
 * We do not expose this class to our headers, as there is currently no need, and this would
 * only clutter the namespace. However, if needed, the class declaration can trivially be moved
 * to the header, while keeping the implementation here (so that we do not spam the header and all
 * places where the header is included with the zlib internal symbols). For this to work, the
 * header additionally needs a forward declaration of GzipStreamWrapper.
 *
 * The class is based on the zstr::ostreambuf class of the
 * [zstr library](https://github.com/mateidavid/zstr) by Matei David; see also our
 * @link supplement_acknowledgements_code_reuse_gzip_streams Acknowledgements@endlink.
 */
class GzipOStreambuf
    : public std::streambuf
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    GzipOStreambuf(
        std::streambuf * sbuf_p
    )
        : GzipOStreambuf( sbuf_p, Z_DEFAULT_COMPRESSION, GZIP_DEFAULT_BUFFER_SIZE )
    {}

    GzipOStreambuf(
        std::streambuf * sbuf_p,
        int level,
        std::size_t buff_size = GZIP_DEFAULT_BUFFER_SIZE
    )
        : sbuf_p_(sbuf_p)
        , zstrm_ptr_(new GzipStreamWrapper(false, level))
        , buff_size_(buff_size)
    {
        assert(sbuf_p_);
        in_buff_ = new char [buff_size_];
        out_buff_ = new char [buff_size_];
        setp(in_buff_, in_buff_ + buff_size_);
    }

    GzipOStreambuf( GzipOStreambuf const& ) = delete;
    GzipOStreambuf( GzipOStreambuf &&) = delete;
    GzipOStreambuf& operator = ( GzipOStreambuf const& ) = delete;
    GzipOStreambuf& operator = ( GzipOStreambuf &&) = delete;

    virtual ~GzipOStreambuf()
    {
        // flush the zlib stream
        //
        // NOTE: Errors here (sync() return value not 0) are ignored, because we
        // cannot throw in a destructor. This mirrors the behaviour of
        // std::basic_filebuf::~basic_filebuf(). To see an exception on error,
        // close the ofstream with an explicit call to close(), and do not rely
        // on the implicit call in the destructor.
        try {
            sync();
        } catch (...) {}

        delete [] in_buff_;
        delete [] out_buff_;
        delete zstrm_ptr_;
    }

    // -------------------------------------------------------------
    //     Internal and Virtual Functions
    // -------------------------------------------------------------

    virtual std::streambuf::int_type overflow(std::streambuf::int_type c = traits_type::eof()) override
    {
        zstrm_ptr_->next_in = reinterpret_cast< decltype(zstrm_ptr_->next_in) >(pbase());
        zstrm_ptr_->avail_in = pptr() - pbase();
        while (zstrm_ptr_->avail_in > 0) {
            int r = deflate_loop_(Z_NO_FLUSH);
            if (r != 0) {
                setp(nullptr, nullptr);
                return traits_type::eof();
            }
        }
        setp(in_buff_, in_buff_ + buff_size_);
        return traits_type::eq_int_type(c, traits_type::eof()) ? traits_type::eof() : sputc(c);
    }

    virtual int sync() override
    {
        // first, call overflow to clear in_buff_
        overflow();
        if (! pptr()) {
            return -1;
        }

        // then, call deflate asking to finish the zlib stream
        zstrm_ptr_->next_in = nullptr;
        zstrm_ptr_->avail_in = 0;

        if (deflate_loop_(Z_FINISH) != 0) {
            return -1;
        }
        deflateReset(zstrm_ptr_);
        return 0;
    }

private:

    int deflate_loop_(int flush)
    {
        while( true ) {
            zstrm_ptr_->next_out = reinterpret_cast< decltype(zstrm_ptr_->next_out) >(out_buff_);
            zstrm_ptr_->avail_out = buff_size_;
            int ret = deflate(zstrm_ptr_, flush);
            if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) {
                throw except::GzipError( zstrm_ptr_->msg, ret );
            }

            std::streamsize sz = sbuf_p_->sputn(
                out_buff_, reinterpret_cast< decltype(out_buff_) >(zstrm_ptr_->next_out) - out_buff_
            );

            if (sz != reinterpret_cast< decltype(out_buff_) >(zstrm_ptr_->next_out) - out_buff_) {
                // there was an error in the sink stream
                return -1;
            }

            if (ret == Z_STREAM_END || ret == Z_BUF_ERROR || sz == 0) {
                break;
            }
        }
        return 0;
    }

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

private:

    std::streambuf * sbuf_p_;
    char * in_buff_;
    char * out_buff_;
    GzipStreamWrapper * zstrm_ptr_;
    std::size_t buff_size_;
};

// ================================================================================================
//     Gzip Input Stream
// ================================================================================================

// We have all the implementation here, so that we do not need to expose the stream buffers.

GzipIStream::GzipIStream( std::istream& is, bool auto_detect, std::size_t buffer_size )
    : std::istream( new GzipIStreambuf( is.rdbuf(), auto_detect, buffer_size ))
{
    exceptions(std::ios_base::badbit);
}

GzipIStream::GzipIStream( std::streambuf* sbuf_p, bool auto_detect, std::size_t buffer_size )
    : std::istream( new GzipIStreambuf( sbuf_p, auto_detect, buffer_size ))
{
    exceptions(std::ios_base::badbit);
}

GzipIStream::~GzipIStream()
{
    delete rdbuf();
}

// ================================================================================================
//     Gzip Output Stream
// ================================================================================================

// Let's make sure that our levels match the zlib levels. Pretty sure that zlib will never
// change their levels, but if they ever do, we want to know.
static_assert(
    static_cast<int>(GzipCompressionLevel::kDefaultCompression) == Z_DEFAULT_COMPRESSION &&
    static_cast<int>(GzipCompressionLevel::kNoCompression)      == Z_NO_COMPRESSION      &&
    static_cast<int>(GzipCompressionLevel::kBestSpeed)          == Z_BEST_SPEED          &&
    static_cast<int>(GzipCompressionLevel::kBestCompression)    == Z_BEST_COMPRESSION,
    "It seems that the zlib-internal compression levels values have changed "
    "compared to the values that we internally use in genesis. This needs to be fixed, "
    "please submit a bug report to https://github.com/lczech/genesis/issues"
);

// We have all the implementation here, so that we do not need to expose the stream buffers.

GzipOStream::GzipOStream( std::ostream& os, GzipCompressionLevel level, std::size_t buffer_size )
    : std::ostream( new GzipOStreambuf( os.rdbuf(), static_cast<int>(level), buffer_size ))
{
    exceptions(std::ios_base::badbit);
}

GzipOStream::GzipOStream( std::streambuf* sbuf_p, GzipCompressionLevel level, std::size_t buffer_size )
    : std::ostream( new GzipOStreambuf( sbuf_p, static_cast<int>(level), buffer_size ))
{
    exceptions(std::ios_base::badbit);
}

GzipOStream::~GzipOStream()
{
    delete rdbuf();
}

// ================================================================================================
//     Gzip Input File Stream
// ================================================================================================

GzipIFStream::GzipIFStream(
    std::string const& filename,
    std::ios_base::openmode mode,
    bool auto_detect,
    std::size_t buffer_size
)
    // Open in binary mode, which should also work for uncompressed files on Unix,
    // but might break on Windowas, as it then does not do the line ending conversions.
    // See https://github.com/mateidavid/zstr/issues/15
    : StrictFStreamHolder<StrictIFStream>( filename, mode | std::ios_base::binary )
    , std::istream( new GzipIStreambuf( file_stream_.rdbuf(), auto_detect, buffer_size ))
{
    exceptions(std::ios_base::badbit);
}

GzipIFStream::~GzipIFStream()
{
    if (rdbuf()) {
        delete rdbuf();
    }
}

// ================================================================================================
//     Gzip Output File Stream
// ================================================================================================

GzipOFStream::GzipOFStream(
    std::string const& filename,
    std::ios_base::openmode mode,
    GzipCompressionLevel level,
    std::size_t buffer_size
)
    : StrictFStreamHolder<StrictOFStream>( filename, mode | std::ios_base::binary )
    , std::ostream( new GzipOStreambuf( file_stream_.rdbuf(), static_cast<int>(level), buffer_size ))
{
    exceptions(std::ios_base::badbit);
}

GzipOFStream::~GzipOFStream()
{
    if (rdbuf()) {
        delete rdbuf();
    }
}

GzipOFStream& GzipOFStream::flush()
{
    std::ostream::flush();
    file_stream_.flush();
    return *this;
}

// Up until here, we defined all classes needed for gzip streaming.
// However, if genesis is compiled without zlib support, we instead use dummy implementations
// which throw exceptions when being used.
#else // GENESIS_ZLIB

// ================================================================================================
//     Gzip Input Stream
// ================================================================================================

GzipIStream::GzipIStream( std::istream&, bool, std::size_t )
{
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

GzipIStream::GzipIStream( std::streambuf*, bool, std::size_t )
{
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

GzipIStream::~GzipIStream()
{}

// ================================================================================================
//     Gzip Output Stream
// ================================================================================================

GzipOStream::GzipOStream( std::ostream&, GzipCompressionLevel, std::size_t )
{
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

GzipOStream::GzipOStream( std::streambuf*, GzipCompressionLevel, std::size_t )
{
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

GzipOStream::~GzipOStream()
{}

// ================================================================================================
//     Gzip Input File Stream
// ================================================================================================

GzipIFStream::GzipIFStream(
    std::string const&,
    std::ios_base::openmode,
    bool,
    std::size_t
)
    : StrictFStreamHolder<StrictIFStream>( "", std::ios_base::binary )
{
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

GzipIFStream::~GzipIFStream()
{
    // Nothing to do;
}

// ================================================================================================
//     Gzip Output File Stream
// ================================================================================================

GzipOFStream::GzipOFStream(
    std::string const&,
    std::ios_base::openmode,
    GzipCompressionLevel,
    std::size_t
)
    : StrictFStreamHolder<StrictOFStream>( "", std::ios_base::binary )
{
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

GzipOFStream::~GzipOFStream()
{
    // Nothing to do;
}

GzipOFStream& GzipOFStream::flush()
{
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

#endif // GENESIS_ZLIB

} // namespace utils
} // namespace genesis
