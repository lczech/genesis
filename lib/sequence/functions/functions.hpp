#ifndef GENESIS_SEQUENCE_FUNCTIONS_FUNCTIONS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_FUNCTIONS_H_

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

#include <iosfwd>
#include <map>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Accessors
// =================================================================================================

Sequence const* find_sequence( SequenceSet const& set, std::string const& label );

// =================================================================================================
//     Characteristics
// =================================================================================================

std::map<char, size_t> site_histogram( Sequence    const& seq );
std::map<char, size_t> site_histogram( SequenceSet const& set );

std::map<char, double> base_frequencies( Sequence    const& seq, std::string const& plain_chars );
std::map<char, double> base_frequencies( SequenceSet const& set, std::string const& plain_chars );

size_t count_chars( SequenceSet const& set, std::string const& chars );

double gapyness( SequenceSet const& set, std::string const& undetermined_chars );

bool validate_chars( SequenceSet const& set, std::string const& chars );

size_t total_length( SequenceSet const& set );

bool is_alignment( SequenceSet const& set );

// =================================================================================================
//     Print and Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Sequence    const& seq );
std::ostream& operator << ( std::ostream& out, SequenceSet const& set );

std::string print(
    Sequence const&                    seq,
    bool                               print_label  = true,
    size_t                             length_limit = 100
);

std::string print(
    SequenceSet const&                 set,
    bool                               print_label    = true,
    size_t                             length_limit   = 100,
    size_t                             sequence_limit = 10
);

std::string print_color(
    Sequence const&                    seq,
    std::map<char, std::string> const& colors,
    bool                               print_label  = true,
    size_t                             length_limit = 100,
    bool                               background   = true
);

std::string print_color(
    SequenceSet const&                 set,
    std::map<char, std::string> const& colors,
    bool                               print_label    = true,
    size_t                             length_limit   = 100,
    size_t                             sequence_limit = 10,
    bool                               background     = true
);

// =================================================================================================
//     Modifiers
// =================================================================================================

/*

normalize --> to upper
normalize_chars
normalize_gaps

void remove_list(SequenceSet& set, std::vector<std::string> const& labels, bool invert);

// =================================================================================================
//     Sequence Modifiers
// =================================================================================================

void remove_gaps();
void replace(char search, char replace);

remove_chars
compress_chars
replace_chars

*/

} // namespace sequence
} // namespace genesis

#endif // include guard
