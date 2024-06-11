#ifndef GENESIS_SEQUENCE_FORMATS_FASTQ_OUTPUT_STREAM_H_
#define GENESIS_SEQUENCE_FORMATS_FASTQ_OUTPUT_STREAM_H_

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
#include "genesis/sequence/formats/fastq_writer.hpp"

#include "genesis/utils/io/output_target.hpp"

#include <iterator>
#include <iostream>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Fastq Output Stream
// =================================================================================================

/**
 * @brief Write Fastq data, sequentially.
 *
 * This class allows to write Sequence data to an output target, using Fastq format, without
 * the need to have a full SequenceSet containing all Sequence%s in memory.
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
class FastqOutputStream
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type = FastqOutputStream;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FastqOutputStream() = delete;

    explicit FastqOutputStream( std::shared_ptr<utils::BaseOutputTarget> target )
        : target_( target )
        , writer_()
    {}

    FastqOutputStream( std::shared_ptr<utils::BaseOutputTarget> target, FastqWriter const& writer )
        : target_( target )
        , writer_( writer )
    {}

    ~FastqOutputStream() = default;

    FastqOutputStream( self_type const& ) = default;
    FastqOutputStream( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    self_type& operator<< ( Sequence const& seq )
    {
        writer_.write( seq, target_ );
        return *this;
    }

    self_type& write( Sequence const& seq )
    {
        writer_.write( seq, target_ );
        return *this;
    }

    self_type& write( Sequence const& seq, std::string const& quality_string )
    {
        writer_.write( seq, quality_string, target_ );
        return *this;
    }

    /**
     * @brief Return the FastqWriter used for this iterator.
     *
     * Use this to change the settings and writing behaviour of the iterator.
     * See FastqWriter for details.
     */
    FastqWriter& writer()
    {
        return writer_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::shared_ptr<utils::BaseOutputTarget> target_;
    FastqWriter writer_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
