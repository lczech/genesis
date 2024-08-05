#ifndef GENESIS_SEQUENCE_FORMATS_FASTX_OUTPUT_STREAM_H_
#define GENESIS_SEQUENCE_FORMATS_FASTX_OUTPUT_STREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/sequence.hpp"
#include "genesis/sequence/formats/fasta_writer.hpp"
#include "genesis/sequence/formats/fastq_writer.hpp"

#include "genesis/utils/io/output_target.hpp"

#include <iterator>
#include <iostream>

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    #include <string_view>

#endif

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward declarations and typedefs
// =================================================================================================

template<class Writer>
class FastxOutputStream;

using FastaOutputStream = FastxOutputStream<FastaWriter>;
using FastqOutputStream = FastxOutputStream<FastqWriter>;

// =================================================================================================
//     Fasta and Fastq Output Stream
// =================================================================================================

/**
 * @brief Write Fasta or Fastq data, sequentially.
 *
 * This class allows to write Sequence data to an output target, using Fasta or Fastq format,
 * without the need to have a full SequenceSet containing all Sequence%s in memory.
 *
 * Exemplary usage:
 *
 *     auto out_it = FastqOutputStream( utils::to_file( "path/to/out.fastq" ));
 *     while( ... ) {
 *         Sequence seq = ...
 *         out_it << seq;
 *     }
 *
 * See the output target convenience functions utils::to_file(), utils::to_stream(), and
 * utils::to_string() for examples of how to obtain a suitable output target.
 */
template<class Writer>
class FastxOutputStream
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type = FastxOutputStream;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FastxOutputStream() = delete;

    explicit FastxOutputStream(
        std::shared_ptr<utils::BaseOutputTarget> target
    )
        : target_( target )
        , writer_()
    {}

    FastxOutputStream(
        std::shared_ptr<utils::BaseOutputTarget> target,
        Writer const& writer
    )
        : target_( target )
        , writer_( writer )
    {}

    ~FastxOutputStream() = default;

    FastxOutputStream( self_type const& ) = default;
    FastxOutputStream( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    self_type const& operator<< ( Sequence const& sequence ) const
    {
        writer_.write( sequence, target_ );
        return *this;
    }

    self_type const& write( Sequence const& sequence ) const
    {
        writer_.write( sequence, target_ );
        return *this;
    }

    /**
     * @brief Writer overload for Fastq files where the @p quality_string is provided instead
     * of taken from the @p sequence.
     */
    template<
        typename T = Writer,
        typename std::enable_if< std::is_same<T, FastqWriter>::value >::type* = nullptr
    >
    self_type const& write( Sequence const& sequence, std::string const& quality_string ) const
    {
        writer_.write( sequence, quality_string, target_ );
        return *this;
    }

    #if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    /**
     * @brief Writer overload for Fastq files where all elements are provided as std::string_view.
     *
     * The given @p quality string is assumed to be encoded correctly already, for instance in
     * phred scaling and ASCII encoding.
     */
    template<
        typename T = Writer,
        typename std::enable_if< std::is_same<T, FastqWriter>::value >::type* = nullptr
    >
    self_type const& write(
        std::string_view const& label,
        std::string_view const& sites,
        std::string_view const& quality
    ) const {
        writer_.write( label, sites, quality, target_ );
        return *this;
    }

    #endif

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    std::shared_ptr<utils::BaseOutputTarget> output_target() const
    {
        return target_;
    }

    /**
     * @brief Return the Writer used for this iterator.
     *
     * Use this to change the settings and writing behaviour of the iterator.
     * See Writer for details.
     */
    Writer& writer()
    {
        return writer_;
    }

    Writer const& writer() const
    {
        return writer_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::shared_ptr<utils::BaseOutputTarget> target_;
    Writer writer_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
