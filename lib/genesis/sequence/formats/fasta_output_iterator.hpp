#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_OUTPUT_ITERATOR_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_OUTPUT_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include <iterator>
#include <iostream>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Fasta Output Iterator
// =================================================================================================

/**
 * @brief
 */
class FastaOutputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = FastaOutputIterator;
    using iterator_category = std::output_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FastaOutputIterator() = default;

    FastaOutputIterator( std::ostream& out )
        : writer_()
        , output_stream_( out )
    {}

    FastaOutputIterator( std::ostream& out, FastaWriter const& writer )
        : writer_( writer )
        , output_stream_( out )
    {}

    ~FastaOutputIterator() = default;

    FastaOutputIterator( self_type const& ) = default;
    FastaOutputIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    self_type& operator = ( Sequence const& seq )
    {
        writer_.write_sequence( seq, output_stream_ );
        return *this;
    }

    self_type& operator * ()
    {
        return *this;
    }

    /**
     * @brief Return the FastaWrtier used for this iterator.
     *
     * Use this to change the writing behaviour of the iterator. See FastaWriter for details.
     */
    FastaWriter& writer()
    {
        return writer_;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    self_type& operator ++ ()
    {
        return *this;
    }

    self_type& operator ++ ( int )
    {
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    FastaWriter   writer_;
    std::ostream& output_stream_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
