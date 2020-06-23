#ifndef GENESIS_UTILS_IO_GZIP_BLOCK_OSTREAM_H_
#define GENESIS_UTILS_IO_GZIP_BLOCK_OSTREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/io/gzip.hpp"
#include "genesis/utils/io/gzip_stream.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     Gzip Block Output Stream
// ================================================================================================

/**
 * @brief Output stream that writes blocks of gzip-compressed data to an underlying wrapped stream,
 * using parallel compression.
 *
 * The gzip format specifies that concatenated blocks of gzip-compressed data (including the gzip
 * header) are still valid gzip files, and are equivalent to concatenating the decompressed data.
 * This is for example used in compressed vcf files (.vcf.gz, Variant Calling Format) to achieve
 * random access into compressed data, by maintaining an index table of offsets to the beginning
 * of individual compressed blocks.
 *
 * We here use a similar technique to achieve a compression speedup by using parallel threads
 * on different gzip blocks. This gives almost linear speedup, at the cost of ~3% increase in
 * resulting file size due to the additional gzip headers of each block. This downside can be
 * alleivated by using larger blocks though. By default, we use 64kB blocks.
 *
 * Exemplary usage:
 *
 *     // Wrapped output stream to write to. Use binary mode, so that compressed output works.
 *     std::ofstream ofile;
 *     ofile.open( "path/to/test.txt.gz", std::ios_base::binary );
 *
 *     // Prepare stream
 *     GzipBlockOStream ostr( ofile );
 *
 *     // Write data to stream
 *     ostr << "some data\n";
 *
 * By default, the number of threads is determined using the number of available threads in an
 * OpenMP parallel region. If set manually via @p num_threads to a value other than 0, we recommend
 * to use not more than the hardware concurrency, or fewer, if at the same time compressed data is
 * read in some other part of the program, or other computation-heavy work is done.
 *
 * Furthermore, note that some file managers might not display the original (uncompressed) file size
 * correctly when viewing the resulting gz file, as they might use only the size of one block
 * instead of the full resulting uncompressed file size. This should not affect decompression or any
 * other downstream processes though. As this class is a stream, we usually do not know beforehand
 * how lare the resulting file will be, so there is not much we can do about this.
 *
 * The class could also be extended in the future to achieve indexing similar to compressed vcf.
 * NB: We have not yet tested compatibility with the vcf format, as they might employ additional
 * tricks to achieve their goals.
 */
class GzipBlockOStream
    : public std::ostream
{
public:

    explicit GzipBlockOStream(
        std::ostream& os,
        std::size_t block_size = GZIP_DEFAULT_BLOCK_SIZE,
        GzipCompressionLevel compression_level = GzipCompressionLevel::kDefaultCompression,
        std::size_t num_threads = 0
    );

    explicit GzipBlockOStream(
        std::streambuf* sbuf_p,
        std::size_t block_size = GZIP_DEFAULT_BLOCK_SIZE,
        GzipCompressionLevel compression_level = GzipCompressionLevel::kDefaultCompression,
        std::size_t num_threads = 0
    );

    virtual ~GzipBlockOStream();

    static const std::size_t GZIP_DEFAULT_BLOCK_SIZE = 1ul << 16;
};

} // namespace utils
} // namespace genesis

#endif // include guard
