#ifndef GENESIS_SEQUENCE_COUNTS_H_
#define GENESIS_SEQUENCE_COUNTS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "utils/math/matrix.hpp"
#include "utils/tools/char_lookup.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwardd Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Sequence Counts
// =================================================================================================

class SequenceCounts
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Type of uint used for internally counting the freuqencies of Sequence sites.
     *
     * We use this alias here, because in the future, we might need to adjust this value:
     * Either to save memory if many different objects of type SequenceCounts are needed, so that
     * they need to be small; or on the contrary, to allow for more Sequence&s being counted by
     * using a broader type here.
     */
    using CountsIntType = uint32_t;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SequenceCounts() = default;
    SequenceCounts( std::string const& characters, size_t length );

    ~SequenceCounts() = default;

    SequenceCounts( SequenceCounts const& ) = default;
    SequenceCounts( SequenceCounts&& )      = default;

    SequenceCounts& operator= ( SequenceCounts const& ) = default;
    SequenceCounts& operator= ( SequenceCounts&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t        length() const;
    std::string   characters() const;

    CountsIntType added_sequences_count() const;

    CountsIntType count_at( size_t site_index, char   character ) const;
    CountsIntType count_at( size_t site_index, size_t character_index ) const;

    // std::vector< CountsIntType> counts_at( size_t site_index ) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void add_sequence( Sequence const& sequence );
    void add_sequence( std::string const& sites );

    void add_sequences( SequenceSet const& sequences );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string                        characters_;
    utils::CharLookup< unsigned char > lookup_;
    utils::Matrix< CountsIntType >     counts_;
    CountsIntType                      num_seqs_ = 0;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
