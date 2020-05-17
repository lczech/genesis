#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_OUTPUT_ITERATOR_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_OUTPUT_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
 * @ingroup sequence
 */

#include "genesis/sequence/sequence.hpp"
#include "genesis/sequence/formats/fasta_writer.hpp"

#include "genesis/utils/io/output_target.hpp"

#include <iterator>
#include <iostream>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Fasta Output Iterator
// =================================================================================================

/**
 * @brief Write Fasta data, sequentially.
 *
 * This class allows to write Sequence data to an output target, using Fasta format, without
 * the need to have a full SequenceSet containing all Sequence%s in memory.
 *
 * Exemplary usage:
 *
 *     auto out_it = FastaOutputIterator( utils::to_file( "path/to/out.fasta" ));
 *     while( ... ) {
 *         Sequence seq = ...
 *         out_it << seq;
 *     }
 *
 * See the output target convenience functions utils::to_file(), utils::to_stream(), and
 * utils::to_string() for examples of how to obtain a suitable output target.
 */
class FastaOutputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type = FastaOutputIterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FastaOutputIterator() = delete;

    explicit FastaOutputIterator( std::shared_ptr<utils::BaseOutputTarget> target )
        : target_( target )
        , writer_()
    {}

    FastaOutputIterator( std::shared_ptr<utils::BaseOutputTarget> target, FastaWriter const& writer )
        : target_( target )
        , writer_( writer )
    {}

    ~FastaOutputIterator() = default;

    FastaOutputIterator( self_type const& ) = default;
    FastaOutputIterator( self_type&& )      = default;

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

    /**
     * @brief Return the FastaWriter used for this iterator.
     *
     * Use this to change the settings and writing behaviour of the iterator.
     * See FastaWriter for details.
     */
    FastaWriter& writer()
    {
        return writer_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::shared_ptr<utils::BaseOutputTarget> target_;
    FastaWriter writer_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
