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

#include "genesis/sequence/functions/codes.hpp"

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/color.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <unordered_map>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Name Lists
// =================================================================================================

static const std::unordered_map<char, std::string> nucleic_acid_code_to_name = {
    { 'A', "Adenine" },
    { 'C', "Cytosine" },
    { 'G', "Guanine" },
    { 'T', "Thymine" },
    { 'U', "Uracil" },

    { 'W', "Weak" },
    { 'S', "Strong" },
    { 'M', "aMino" },
    { 'K', "Keto" },
    { 'R', "puRine" },
    { 'Y', "pYrimidine" },

    { 'B', "not A" },
    { 'D', "not C" },
    { 'H', "not G" },
    { 'V', "not T" },

    { 'N', "any" },
    { 'O', "omitted" },
    { 'X', "masked" },
    { '.', "gap" },
    { '-', "gap" },
    { '?', "gap" }
};

static const std::unordered_map<char, std::string> amino_acid_code_to_name = {
    { 'A', "Alanine" },
    { 'B', "Aspartic acid or Asparagine" },
    { 'C', "Cysteine" },
    { 'D', "Aspartic acid" },
    { 'E', "Glutamic acid" },
    { 'F', "Phenylalanine" },
    { 'G', "Glycine" },
    { 'H', "Histidine" },
    { 'I', "Isoleucine" },
    { 'J', "Leucine or Isoleucine" },
    { 'K', "Lysine" },
    { 'L', "Leucine" },
    { 'M', "Methionine" },
    { 'N', "Asparagine" },
    { 'O', "Pyrrolysine" },
    { 'P', "Proline" },
    { 'Q', "Glutamine" },
    { 'R', "Arginine" },
    { 'S', "Serine" },
    { 'T', "Threonine" },
    { 'U', "Selenocysteine" },
    { 'V', "Valine" },
    { 'W', "Tryptophan" },
    { 'Y', "Tyrosine" },
    { 'Z', "Glutamic acid or Glutamine" },

    { 'X', "any" },
    { '*', "translation stop" },
    { '-', "gap" },
    { '?', "gap" }
};

// =================================================================================================
//     Color Lists
// =================================================================================================

static const std::map<char, std::string> nucleic_acid_text_colors_map = {
    { 'A', "Red" },
    { 'C', "Green" },
    { 'G', "Yellow" },
    { 'T', "Blue" },
    { 'U', "Blue" },

    { 'W', "DarkGray" },
    { 'S', "DarkGray" },
    { 'M', "DarkGray" },
    { 'K', "DarkGray" },
    { 'R', "DarkGray" },
    { 'Y', "DarkGray" },

    { 'B', "DarkGray" },
    { 'D', "DarkGray" },
    { 'H', "DarkGray" },
    { 'V', "DarkGray" },

    { 'N', "DarkGray" },
    { 'O', "DarkGray" },
    { 'X', "DarkGray" },
    { '.', "DarkGray" },
    { '-', "DarkGray" },
    { '?', "DarkGray" }
};

static const std::map<char, std::string> amino_acid_text_colors_map = {
    { 'A', "Blue" },
    { 'B', "DarkGray" },
    { 'C', "LightMagenta" },
    { 'D', "Magenta" },
    { 'E', "Magenta" },
    { 'F', "Blue" },
    { 'G', "LightRed" },
    { 'H', "Cyan" },
    { 'I', "Blue" },
    { 'J', "DarkGray" },
    { 'K', "Red" },
    { 'L', "Blue" },
    { 'M', "Blue" },
    { 'N', "Green" },
    { 'O', "DarkGray" },
    { 'P', "Yellow" },
    { 'Q', "Green" },
    { 'R', "Red" },
    { 'S', "Green" },
    { 'T', "Green" },
    { 'U', "DarkGray" },
    { 'V', "Blue" },
    { 'W', "Blue" },
    { 'Y', "Cyan" },
    { 'Z', "DarkGray" },

    { 'X', "DarkGray" },
    { '*', "DarkGray" },
    { '-', "DarkGray" },
    { '?', "DarkGray" }
};

static const std::map<char, utils::Color> nucleic_acid_colors_map = {
    { 'A', { 255,   0,   0 } },
    { 'C', {   0, 255,   0 } },
    { 'G', { 255, 255,   0 } },
    { 'T', {   0,   0, 255 } },
    { 'U', {   0,   0, 255 } },

    { 'W', {  96,  96,  96 } },
    { 'S', {  96,  96,  96 } },
    { 'M', {  96,  96,  96 } },
    { 'K', {  96,  96,  96 } },
    { 'R', {  96,  96,  96 } },
    { 'Y', {  96,  96,  96 } },

    { 'B', { 128, 128, 128 } },
    { 'D', { 128, 128, 128 } },
    { 'H', { 128, 128, 128 } },
    { 'V', { 128, 128, 128 } },

    { 'N', { 128, 128, 128 } },
    { 'O', { 128, 128, 128 } },
    { 'X', { 128, 128, 128 } },
    { '.', { 128, 128, 128 } },
    { '-', { 128, 128, 128 } },
    { '?', { 128, 128, 128 } }
};

static const std::map<char, utils::Color> amino_acid_colors_map = {
    { 'A', {  25, 128, 255 } },
    { 'B', {  96,  96,  96 } },
    { 'C', { 230, 128, 128 } },
    { 'D', { 204,  77, 204 } },
    { 'E', { 204,  77, 204 } },
    { 'F', {  25, 128, 255 } },
    { 'G', { 230, 153,  77 } },
    { 'H', {  25, 179, 179 } },
    { 'I', {  25, 128, 255 } },
    { 'J', {  96,  96,  96 } },
    { 'K', { 230,  51,  25 } },
    { 'L', {  25, 128, 255 } },
    { 'M', {  25, 128, 255 } },
    { 'N', {  25, 204,  25 } },
    { 'O', {  96,  96,  96 } },
    { 'P', { 204, 204,   0 } },
    { 'Q', {  25, 204,  25 } },
    { 'R', { 230,  51,  25 } },
    { 'S', {  25, 204,  25 } },
    { 'T', {  25, 204,  25 } },
    { 'U', {  96,  96,  96 } },
    { 'V', {  25, 128, 255 } },
    { 'W', {  25, 128, 255 } },
    { 'Y', {  25, 179, 179 } },
    { 'Z', {  96,  96,  96 } },

    { 'X', { 128, 128, 128 } },
    { '*', { 128, 128, 128 } },
    { '-', { 128, 128, 128 } },
    { '?', { 128, 128, 128 } }
};

// =================================================================================================
//     Ambiguity Lists
// =================================================================================================

static const std::unordered_map<char, std::string> nucleic_acid_ambiguity_char_map = {
    { 'A', "A" },
    { 'C', "C" },
    { 'G', "G" },
    { 'T', "T" },
    { 'U', "T" },

    { 'W', "AT" },
    { 'S', "CG" },
    { 'M', "AC" },
    { 'K', "GT" },
    { 'R', "AG" },
    { 'Y', "CT" },

    { 'B', "CGT" },
    { 'D', "AGT" },
    { 'H', "ACT" },
    { 'V', "ACG" },

    { 'N', "ACGT" },
    { 'O', "-" },
    { 'X', "-" },
    { '.', "-" },
    { '-', "-" },
    { '?', "-" }
};

static const std::unordered_map< std::string, char > nucleic_acid_ambiguity_string_map = {
    { "A", 'A' },
    { "C", 'C' },
    { "G", 'G' },
    { "T", 'T' },

    { "AT", 'W' },
    { "CG", 'S' },
    { "AC", 'M' },
    { "GT", 'K' },
    { "AG", 'R' },
    { "CT", 'Y' },

    { "CGT", 'B' },
    { "AGT", 'D' },
    { "ACT", 'H' },
    { "ACG", 'V' },

    { "ACGT", 'N' },
    { "-", '-' }
};

// =================================================================================================
//     Codes
// =================================================================================================

// ---------------------------------------------------------------------
//     Nucleic Acids
// ---------------------------------------------------------------------

std::string nucleic_acid_codes_plain()
{
    return "ACGT";
}

std::string nucleic_acid_codes_degenerated()
{
    return "WSMKRYBDHV";
}

std::string nucleic_acid_codes_undetermined()
{
    return "NOX.-?";
}

std::string nucleic_acid_codes_all()
{
    return nucleic_acid_codes_plain()
         + nucleic_acid_codes_degenerated()
         + nucleic_acid_codes_undetermined();
}

// ---------------------------------------------------------------------
//     Amino Acids
// ---------------------------------------------------------------------

std::string amino_acid_codes_plain()
{
    return "ACDEFGHIKLMNOPQRSTUVWY";
}

std::string amino_acid_codes_degenerated()
{
    return "BJZ";
}

std::string amino_acid_codes_undetermined()
{
    return "X*-?";
}

std::string amino_acid_codes_all()
{
    return amino_acid_codes_plain()
         + amino_acid_codes_degenerated()
         + amino_acid_codes_undetermined();
}

// ---------------------------------------------------------------------
//     Misc
// ---------------------------------------------------------------------

std::string normalize_codes( std::string const& alphabet )
{
    // Uppercase, sort, uniq the alphabet.
    auto normalized = utils::to_upper_ascii( alphabet );
    std::sort( normalized.begin(), normalized.end() );
    normalized.erase( std::unique( normalized.begin(), normalized.end() ), normalized.end() );
    return normalized;
}

// =================================================================================================
//     Color Codes
// =================================================================================================

std::map<char, std::string> nucleic_acid_text_colors()
{
    return nucleic_acid_text_colors_map;
}

std::map<char, std::string> amino_acid_text_colors()
{
    return amino_acid_text_colors_map;
}

std::map<char, utils::Color> nucleic_acid_colors()
{
    return nucleic_acid_colors_map;
}

std::map<char, utils::Color> amino_acid_colors()
{
    return amino_acid_colors_map;
}

// =================================================================================================
//     Translate Codes
// =================================================================================================

std::string nucleic_acid_name( char code )
{
    auto ucode = toupper(code);
    if( nucleic_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid code '" + std::string( 1, code ) + "'." );
    }
    return nucleic_acid_code_to_name.at( ucode );
}

std::string amino_acid_name( char code )
{
    auto ucode = toupper(code);
    if( amino_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid amino acid code '" + std::string( 1, code ) + "'." );
    }
    return amino_acid_code_to_name.at( ucode );
}

std::string nucleic_acid_ambiguities( char code )
{
    auto ucode = toupper(code);
    if( nucleic_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid code '" + std::string( 1, code ) + "'." );
    }
    return nucleic_acid_ambiguity_char_map.at( ucode );
}

char nucleic_acid_ambiguity_code( std::string codes )
{
    // Uppercase, sort, uniq the codes.
    auto tmp = utils::to_upper_ascii( codes );
    std::sort( tmp.begin(), tmp.end() );
    tmp.erase( std::unique( tmp.begin(), tmp.end() ), tmp.end() );

    if( nucleic_acid_ambiguity_string_map.count( tmp ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid codes '" + codes + "'." );
    }
    return nucleic_acid_ambiguity_string_map.at( tmp );
}

} // namespace sequence
} // namespace genesis
