#ifndef GENESIS_SEQUENCE_FUNCTIONS_LABELS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_LABELS_H_

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

#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Uniqueness
// =================================================================================================

bool has_unique_labels( SequenceSet const& set, bool case_sensitive = true );

void relabel_sha1( Sequence&    seq );
void relabel_sha1( SequenceSet& set );

// =================================================================================================
//     Validity
// =================================================================================================

bool is_valid_label(   std::string const& label );
bool has_valid_label(  Sequence const&    seq );
bool has_valid_labels( SequenceSet const& set );

std::string sanitize_label( std::string const& label );
void sanitize_label( Sequence&     seq );
void sanitize_labels( SequenceSet& set );

} // namespace sequence
} // namespace genesis

#endif // include guard
