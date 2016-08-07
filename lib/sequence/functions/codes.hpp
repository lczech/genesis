#ifndef GENESIS_SEQUENCE_FUNCTIONS_CODES_H_
#define GENESIS_SEQUENCE_FUNCTIONS_CODES_H_

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

#include <map>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Codes
// =================================================================================================

std::string nucleic_acid_codes_plain();
std::string nucleic_acid_codes_degenerated();
std::string nucleic_acid_codes_undetermined();
std::string nucleic_acid_codes_all();

std::string amino_acid_codes_plain();
std::string amino_acid_codes_degenerated();
std::string amino_acid_codes_undetermined();
std::string amino_acid_codes_all();

// =================================================================================================
//     Color Codes
// =================================================================================================

std::map<char, std::string> nucleic_acid_text_colors();
std::map<char, std::string> amino_acid_text_colors();

// =================================================================================================
//     Translate Codes
// =================================================================================================

std::string translate_nucleic_acid( char code );
std::string translate_amino_acid( char code );

} // namespace sequence
} // namespace genesis

#endif // include guard
