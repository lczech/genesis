/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/md5.hpp"
#include "genesis/utils/tools/sha1.hpp"
#include "genesis/utils/tools/sha256.hpp"

#include <algorithm>
#include <cctype>
// #include <regex>

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
size_t guess_sequence_abundance( Sequence const& sequence )
{
    return guess_sequence_abundance( sequence.label() );
}

size_t guess_sequence_abundance( std::string const& label )
{
    size_t result = 1;

    // We only look for a simple number, no sign oder decimal points etc
    auto is_digits = []( std::string const& s )
    {
        return s.find_first_not_of( "0123456789" ) == std::string::npos;
    };

    // Try to find "size=123"
    auto const spos = label.find( "size=" );
    if( spos != std::string::npos && spos + 5 < label.size() && isdigit( label[ spos + 5 ]) ) {

        // Parse the substring as far as possible, that is, get all digits.
        auto const sub = label.substr( spos + 5 );
        try{
            result = std::stoull( sub );
        } catch( ... ){
            result = 1;
        }
    }

    // Try to find "_123" at the end
    auto const upos = label.find_last_of( "_" );
    if( upos != std::string::npos && upos + 1 < label.size() && isdigit( label[ upos + 1 ]) ) {

        // The rest of the label needs to be a number.
        auto const sub = label.substr( upos + 1 );
        if( is_digits( sub ) ) {
            result = std::stoull( sub );
        }
    }

    return result;

    // Slow regex version
    // Prepare static regex (no need to re-compile it on every function call).
    // Matches either ";size=123;" or "_123"
    // static const std::string expr = "(?:[;]?size=([0-9]+)[;]?)|(?:_([0-9]+)$)";
    // static std::regex pattern( expr );
    //
    // // Run the expression.
    // std::smatch matches;
    // if( std::regex_search( label, matches, pattern )) {
    //     size_t res;
    //     std::string const num = ( matches[1].str().empty() ? matches[2].str() : matches[1].str() );
    //     sscanf( num.c_str(), "%zu", &res );
    //     return res;
    // } else {
    //     return 1;
    // }
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

void relabel_sha256( Sequence& seq )
{
    auto digest = utils::SHA256::from_string_hex( seq.sites() );
    seq.label( digest );
}

void relabel_sha256( SequenceSet& set )
{
    for( auto& seq : set ) {
        relabel_sha256( seq );
    }
}

void relabel_md5( Sequence& seq )
{
    auto digest = utils::MD5::from_string_hex( seq.sites() );
    seq.label( digest );
}

void relabel_md5( SequenceSet& set )
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
    result.reserve( label.size() );

    std::string const invalid_chars = ":,();[]'";
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
