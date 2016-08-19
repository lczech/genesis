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

#include "sequence/functions/codes.hpp"

#include "utils/text/string.hpp"

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

/**
 * @brief Return all plain nucleic acid codes. Those are "ACGTU".
 */
std::string nucleic_acid_codes_plain()
{
    return "ACGT";
}

/**
 * @brief Return all degenerated nucleic acid codes. Those are "WSMKRYBDHV".
 */
std::string nucleic_acid_codes_degenerated()
{
    return "WSMKRYBDHV";
}

/**
 * @brief Return all undetermined nucleic acid codes. Those are "NOX.-?".
 */
std::string nucleic_acid_codes_undetermined()
{
    return "NOX.-?";
}

/**
 * @brief Return all valid nucleic acid codes. Those are "ACGTUWSMKRYBDHVNOX.-?".
 */
std::string nucleic_acid_codes_all()
{
    return nucleic_acid_codes_plain()
         + nucleic_acid_codes_degenerated()
         + nucleic_acid_codes_undetermined();
}

// ---------------------------------------------------------------------
//     Amino Acids
// ---------------------------------------------------------------------

/**
 * @brief Return all plain amino acid codes. Those are "ACDEFGHIKLMNOPQRSTUVWY".
 */
std::string amino_acid_codes_plain()
{
    return "ACDEFGHIKLMNOPQRSTUVWY";
}

/**
 * @brief Return all degenerated amino acid codes. Those are "BJZ".
 */
std::string amino_acid_codes_degenerated()
{
    return "BJZ";
}

/**
 * @brief Return all undetermined amino acid codes. Those are "X*-?".
 */
std::string amino_acid_codes_undetermined()
{
    return "X*-?";
}

/**
 * @brief Return all valid amino acid codes. Those are "ACDEFGHIKLMNOPQRSTUVWYBJZX*-?".
 */
std::string amino_acid_codes_all()
{
    return amino_acid_codes_plain()
         + amino_acid_codes_degenerated()
         + amino_acid_codes_undetermined();
}

// =================================================================================================
//     Color Codes
// =================================================================================================

/**
 * @brief Return a map of colors for each nucleic acid code.
 *
 * This function gives a color name usable for utils::Style for each nucleic acid code.
 * The return value of this function can for example be used in sequence::print_color() function.
 */
std::map<char, std::string> nucleic_acid_text_colors()
{
    return nucleic_acid_text_colors_map;
}

/**
 * @brief Return a map of colors for each amino acid code.
 *
 * This function gives a color name usable for utils::Style for each amino acid code.
 * The return value of this function can for example be used in sequence::print_color() function.
 */
std::map<char, std::string> amino_acid_text_colors()
{
    return amino_acid_text_colors_map;
}

// =================================================================================================
//     Translate Codes
// =================================================================================================

/**
 * @brief Get the name of a nucleic acid given its IUPAC code.
 *
 * The codes are translated as follows:
 *
 *     A Adenine
 *     C Cytosine
 *     G Guanine
 *     T Thymine
 *     U Uracil
 *     W Weak
 *     S Strong
 *     M aMino
 *     K Keto
 *     R puRine
 *     Y pYrimidine
 *     B not A
 *     D not C
 *     H not G
 *     V not T
 *     N any
 *     O omitted
 *     X masked
 *     . gap
 *     - gap
 *     ? gap
 *
 * The code char is treated case-insensitive. If the given code char is not valid, an
 * `std::out_of_range` exception is thrown.
 */
std::string translate_nucleic_acid( char code )
{
    auto ucode = toupper(code);
    if( nucleic_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid code '" + std::string( 1, code ) + "'." );
    }
    return nucleic_acid_code_to_name.at( ucode );
}

/**
 * @brief Get the name of a amino acid given its IUPAC code.
 *
 * The codes are translated as follows:
 *
 *     A Alanine
 *     B Aspartic acid or Asparagine
 *     C Cysteine
 *     D Aspartic acid
 *     E Glutamic acid
 *     F Phenylalanine
 *     G Glycine
 *     H Histidine
 *     I Isoleucine
 *     J Leucine or Isoleucine
 *     K Lysine
 *     L Leucine
 *     M Methionine
 *     N Asparagine
 *     O Pyrrolysine
 *     P Proline
 *     Q Glutamine
 *     R Arginine
 *     S Serine
 *     T Threonine
 *     U Selenocysteine
 *     V Valine
 *     W Tryptophan
 *     Y Tyrosine
 *     Z Glutamic acid or Glutamine
 *     X any
 *     * translation stop
 *     - gap
 *     ? gap
 *
 * The code char is treated case-insensitive. If the given code char is not valid, an
 * `std::out_of_range` exception is thrown.
 */
std::string translate_amino_acid( char code )
{
    auto ucode = toupper(code);
    if( amino_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid amino acid code '" + std::string( 1, code ) + "'." );
    }
    return amino_acid_code_to_name.at( ucode );
}

/**
 * @brief Return the possible ambiguous nucleic acid codes for a given code char.
 *
 * The codes are resolved as follows:
 *
 *    'A' ==> "A"
 *    'C' ==> "C"
 *    'G' ==> "G"
 *    'T' ==> "T"
 *    'U' ==> "T"
 *
 *    'W' ==> "AT"
 *    'S' ==> "CG"
 *    'M' ==> "AC"
 *    'K' ==> "GT"
 *    'R' ==> "AG"
 *    'Y' ==> "CT"
 *
 *    'B' ==> "CGT"
 *    'D' ==> "AGT"
 *    'H' ==> "ACT"
 *    'V' ==> "ACG"
 *
 *    'N' ==> "ACGT"
 *    'O' ==> "-"
 *    'X' ==> "-"
 *    '.' ==> "-"
 *    '-' ==> "-"
 *    '?' ==> "-"
 *
 * The code char is treated case-insensitive. If the given code char is not valid, an
 * `std::out_of_range` exception is thrown.
 *
 * See nucleic_acid_ambiguity_code() for a reverse version of this function. It is however not
 * exactly the reverse, as some degenerated codes are mapped to the gap char. Thus, this function
 * is not injective.
 */
std::string nucleic_acid_ambiguities( char code )
{
    auto ucode = toupper(code);
    if( nucleic_acid_code_to_name.count( ucode ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid code '" + std::string( 1, code ) + "'." );
    }
    return nucleic_acid_ambiguity_char_map.at( ucode );
}

/**
 * @brief Return the nucleic acid code that represents all given `codes`.
 *
 * The codes are resolved as follows:
 *
 *    "A" ==> 'A'
 *    "C" ==> 'C'
 *    "G" ==> 'G'
 *    "T" ==> 'T'
 *
 *    "AT" ==> 'W'
 *    "CG" ==> 'S'
 *    "AC" ==> 'M'
 *    "GT" ==> 'K'
 *    "AG" ==> 'R'
 *    "CT" ==> 'Y'
 *
 *    "CGT" ==> 'B'
 *    "AGT" ==> 'D'
 *    "ACT" ==> 'H'
 *    "ACG" ==> 'V'
 *
 *    "ACGT" ==> 'N'
 *    "-" ==> '-'
 *
 * The given codes are treated case-insensitive and order-independent. For example, given `"tCgG"`,
 * the function still returns `'B'`. However, if any of the given codes is not valid, an
 * `std::out_of_range` exception is thrown.
 *
 * See nucleic_acid_ambiguities() for the reverse of this function.
 */
char nucleic_acid_ambiguity_code( std::string codes )
{
    // Uppercase, sort, uniq the codes.
    auto tmp = utils::to_upper_ascii( codes );
    std::sort( tmp.begin(), tmp.end() );
    tmp.erase( std::unique( tmp.begin(), tmp.end() ), tmp.end() );

    if( nucleic_acid_ambiguity_string_map.count( tmp ) == 0 ) {
        throw std::out_of_range( "Invalid nucleic acid codes '" + tmp + "'." );
    }
    return nucleic_acid_ambiguity_string_map.at( tmp );
}

} // namespace sequence
} // namespace genesis
