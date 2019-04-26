/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
#include "genesis/utils/tools/hash/md5.hpp"
#include "genesis/utils/tools/hash/sha1.hpp"
#include "genesis/utils/tools/hash/sha256.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
// #include <regex>
#include <stdexcept>

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
std::pair<std::string, size_t> guess_sequence_abundance( Sequence const& sequence )
{
    return guess_sequence_abundance( sequence.label() );
}

std::pair<std::string, size_t> guess_sequence_abundance( std::string const& label )
{
    std::string res_name = label;
    size_t      res_abun = 1;

    // We only look for a simple number, no sign oder decimal points etc
    auto is_digits_ = []( std::string const& s )
    {
        return s.find_first_not_of( "0123456789" ) == std::string::npos;
    };

    // Try to find ";size=123;", using label attributes.
    try{
        auto const la = label_attributes( label );
        res_name = la.label;
        if( la.attributes.count( "size" ) > 0 && is_digits_( la.attributes.at( "size" ))) {
            res_abun = std::stoull( la.attributes.at( "size" ));
        }
    } catch( ... ) {
        res_name = label;
        res_abun = 1;
    }

    // Try to find "_123" at the end.
    auto const upos = label.find_last_of( "_" );
    if( upos != std::string::npos && upos + 1 < label.size() && ::isdigit( label[ upos + 1 ]) ) {

        // The rest of the label needs to be a number.
        auto const sub = label.substr( upos + 1 );
        if( is_digits_( sub ) ) {
            res_name =  label.substr( 0, upos );
            res_abun = std::stoull( sub );
        }
    }

    return { res_name, res_abun };

    // Try to find "size=123".
    // This is the old version that directly parses the label.
    // auto spos = label.find( "size=" );
    // if( spos != std::string::npos && spos + 5 < label.size() && ::isdigit( label[ spos + 5 ]) ) {
    //
    //     // Parse the substring as far as possible, that is, get all digits.
    //     auto const sub = label.substr( spos + 5 );
    //     try{
    //         res_abun = std::stoull( sub );
    //
    //         // If the number parsing above succeeds, also change the name/label.
    //         // Here, we need to take care of a semicolon (or other non-alpha char)
    //         // that might appear in front of the "size=" part. If there is one, ignore it.
    //         if( spos > 0 && ::ispunct( label[ spos - 1 ] )) {
    //             --spos;
    //         }
    //         res_name = label.substr( 0, spos );
    //     } catch( ... ){
    //         res_name = label;
    //         res_abun = 1;
    //     }
    // }

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

LabelAttributes label_attributes( Sequence const& sequence )
{
    return label_attributes( sequence.label() );
}

LabelAttributes label_attributes( std::string const& label )
{
    // Set the label to the first part (before the first semicolon).
    // This is always correct, even if there are no semicola.
    LabelAttributes result;
    auto const attribs = utils::split( label, ";" );
    assert( attribs.size() > 0 );
    result.label = attribs.front();

    // Set the other parts. We here require that the attribs follow the needed structure.
    for( size_t i = 1; i < attribs.size(); ++i ) {
        auto const ap = utils::split( attribs[i], "=" );
        if( ap.size() != 2 ) {
            throw std::runtime_error( "Invalid Sequence label for extracting label attributes." );
        }
        result.attributes[ ap[0] ] = ap[1];
    }

    return result;
}

// =================================================================================================
//     Uniqueness
// =================================================================================================

bool has_unique_labels( SequenceSet const& set, bool case_sensitive )
{
    std::unordered_set< std::string > label_set;
    std::string label;

    for( auto const& seq : set ) {
        if( case_sensitive ) {
            label = seq.label();
        } else {
            label = utils::to_lower( seq.label() );
        }

        if( label_set.count( label ) > 0 ) {
            return false;
        } else {
            label_set.insert( label );
        }
    }
    return true;
}

void relabel_with_hash( Sequence& seq, utils::HashingFunctions hash_function )
{
    auto const digest = utils::hash_hex( utils::from_string( seq.sites() ), hash_function );
    seq.label( digest );
}

void relabel_with_hash( SequenceSet& set, utils::HashingFunctions hash_function )
{
    for( auto& seq : set ) {
        relabel_with_hash( seq, hash_function );
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
