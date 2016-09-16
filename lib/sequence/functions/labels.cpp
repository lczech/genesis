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

#include "sequence/functions/labels.hpp"

#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"

#include "utils/math/sha1.hpp"
#include "utils/text/string.hpp"

#include <unordered_set>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Uniqueness
// =================================================================================================

/**
 * @brief Return true iff all labels of the Sequence%s in the SequenceSet are unique.
 *
 * The optional parameter `case_sensitive` controls how labels are compared. Default is `true`,
 * that is, Sequence%s are compared case-sensitively.
 */
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

/**
 * @brief Relabel the Sequence using the SHA1 hash digest of its sites.
 */
void relabel_sha1( Sequence& seq )
{
    auto digest = utils::SHA1::from_string( seq.sites() );
    seq.label( digest );
}

/**
 * @brief Relabel all Sequence%s in the SequenceSet using the SHA1 hash digest of the sites.
 *
 * If there are duplicate Sequence%s, this function will lead to multiple Sequences with the same
 * name, which might be an issue for downstream programs that expect unique labels.
 * See has_unique_labels() to check this.
 */
void relabel_sha1( SequenceSet& set )
{
    for( auto& seq : set ) {
        relabel_sha1( seq );
    }
}

// =================================================================================================
//     Validity
// =================================================================================================

/**
 * @brief Check whether a given string is a valid label for a Sequence.
 *
 * While we can work with any form of label (as long as it is a string), most file formats and
 * consequently most programs that read them restrict the set of valid characters for labels of
 * sequences. We thus provide this function, which uses the most common interpretation of valid
 * labels.
 *
 * A label is valid if its characters have a graphical representation (i.e., isgraph() is true) and
 * if non of these characters occurs:
 *
 *     :,();[]'
 *
 * Thus, all whitespaces, control characters, and the listed special characters are invalid.
 * See sanitize_label() for a function that replaces all invalid characters of the label by
 * underscores.
 */
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

/**
 * @brief Check whether a Sequence has a valid label.
 *
 * This might be important for printing the Sequence to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 * See sanitize_label() for a function that replaces all invalid characters of the label by
 * underscores.
 */
bool has_valid_label(  Sequence const& seq )
{
    return is_valid_label( seq.label() );
}

/**
 * @brief Check whether all Sequence%s in a SequenceSet have valid labels.
 *
 * This might be important for printing the Sequences to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 * See sanitize_labels() for a function that replaces all invalid characters of the labels by
 * underscores.
 */
bool has_valid_labels( SequenceSet const& set )
{
    for( auto const& seq : set ) {
        if( ! has_valid_label( seq )) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Sanitize a label by replacing all invalid characters with underscores.
 *
 * This might be important for printing the Sequences to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 */
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

/**
 * @brief Sanitize a label by replacing all invalid characters with underscores.
 *
 * This might be important for printing the Sequences to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 */
void sanitize_label( Sequence& seq )
{
    seq.label( sanitize_label( seq.label() ));
}

/**
 * @brief Sanitize the labels of all Sequence%s in the SequenceSet by replacing all invalid
 * characters with underscores.
 *
 * This might be important for printing the Sequences to a file that needs to be read by other
 * applications. See is_valid_label() for details on what is considered a valid label.
 */
void sanitize_labels( SequenceSet& set )
{
    for( auto& seq : set ) {
        sanitize_label( seq );
    }
}

} // namespace sequence
} // namespace genesis
