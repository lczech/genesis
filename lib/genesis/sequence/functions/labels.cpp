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

#include "genesis/sequence/functions/labels.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"

#include "genesis/utils/math/sha1.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>

namespace genesis {
namespace sequence {

// =================================================================================================
//     General
// =================================================================================================

Sequence const* find_sequence( SequenceSet const& set, std::string const& label )
{
    for (Sequence const& s : set) {
        if (s.label() == label) {
            return &s;
        }
    }
    return nullptr;
}

std::unordered_set<std::string> labels( SequenceSet const& set )
{
    std::unordered_set<std::string> result;
    for( auto const& seq : set ) {
        result.insert( seq.label() );
    }
    return result;
}

// =================================================================================================
//     Uniqueness
// =================================================================================================

bool has_unique_labels( SequenceSet const& set, bool case_sensitive )
{
    std::unordered_set< std::string > labels;
    std::string label;

    for( auto const& seq : set ) {
        if( case_sensitive ) {
            label = seq.label();
        } else {
            label = utils::to_lower( seq.label() );
        }

        if( labels.count( label ) > 0 ) {
            return false;
        } else {
            labels.insert( label );
        }
    }
    return true;
}

void relabel_sha1( Sequence& seq )
{
    auto digest = utils::SHA1::from_string_hex( seq.sites() );
    seq.label( digest );
}

void relabel_sha1( SequenceSet& set )
{
    for( auto& seq : set ) {
        relabel_sha1( seq );
    }
}

// =================================================================================================
//     Validity
// =================================================================================================

bool is_valid_label( std::string const& label )
{
    std::string invalid_chars = ":,();[]'";
    for( auto c : label ) {
        if( ! isgraph(c) || invalid_chars.find( c ) != std::string::npos ) {
            return false;
        }
    }
    return true;
}

bool has_valid_label(  Sequence const& seq )
{
    return is_valid_label( seq.label() );
}

bool has_valid_labels( SequenceSet const& set )
{
    for( auto const& seq : set ) {
        if( ! has_valid_label( seq )) {
            return false;
        }
    }
    return true;
}

std::string sanitize_label( std::string const& label )
{
    std::string result;
    std::string invalid_chars = ":,();[]'";
    for( auto c : label ) {
        if( ! isgraph(c) || invalid_chars.find( c ) != std::string::npos ) {
            result += "_";
        } else {
            result += c;
        }
    }
    return result;
}

void sanitize_label( Sequence& seq )
{
    seq.label( sanitize_label( seq.label() ));
}

void sanitize_labels( SequenceSet& set )
{
    for( auto& seq : set ) {
        sanitize_label( seq );
    }
}

// =================================================================================================
//     Modifiers
// =================================================================================================

void filter_by_label_list(
    SequenceSet&                           set,
    std::unordered_set<std::string> const& labels,
    bool                                   invert
) {
    auto new_last = std::remove_if(
        set.begin(),
        set.end(),
        [&] ( Sequence const& seq ) {
            return ( !invert && labels.count( seq.label() )  > 0 ) ||
                   (  invert && labels.count( seq.label() ) == 0 );
        }
    );
    set.remove( new_last , set.end() );
}

} // namespace sequence
} // namespace genesis
