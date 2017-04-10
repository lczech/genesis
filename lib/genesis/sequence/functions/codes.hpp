#ifndef GENESIS_SEQUENCE_FUNCTIONS_CODES_H_
#define GENESIS_SEQUENCE_FUNCTIONS_CODES_H_

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

#include <map>
#include <string>

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace genesis {
namespace utils {

    class Color;

}
}

namespace genesis {
namespace sequence {

// =================================================================================================
//     Codes
// =================================================================================================

// ---------------------------------------------------------------------
//     Nucleic Acids
// ---------------------------------------------------------------------

/**
 * @brief Return all plain nucleic acid codes. Those are "ACGTU".
 */
std::string nucleic_acid_codes_plain();

/**
 * @brief Return all degenerated nucleic acid codes. Those are "WSMKRYBDHV".
 */
std::string nucleic_acid_codes_degenerated();

/**
 * @brief Return all undetermined nucleic acid codes. Those are "NOX.-?".
 */
std::string nucleic_acid_codes_undetermined();

/**
 * @brief Return all valid nucleic acid codes. Those are "ACGTUWSMKRYBDHVNOX.-?".
 */
std::string nucleic_acid_codes_all();

// ---------------------------------------------------------------------
//     Amino Acids
// ---------------------------------------------------------------------

/**
 * @brief Return all plain amino acid codes. Those are "ACDEFGHIKLMNOPQRSTUVWY".
 */
std::string amino_acid_codes_plain();

/**
 * @brief Return all degenerated amino acid codes. Those are "BJZ".
 */
std::string amino_acid_codes_degenerated();

/**
 * @brief Return all undetermined amino acid codes. Those are "X*-?".
 */
std::string amino_acid_codes_undetermined();

/**
 * @brief Return all valid amino acid codes. Those are "ACDEFGHIKLMNOPQRSTUVWYBJZX*-?".
 */
std::string amino_acid_codes_all();

// ---------------------------------------------------------------------
//     Misc
// ---------------------------------------------------------------------

/**
 * @brief Normalize a set of Sequence codes, i.e., make them upper case, sort them, and remove
 * duplicates.
 *
 * For example, when given a set of nucleic acid codes like "aGtc", the function returns "ACGT".
 * This is useful to get consistent codes in functions that accept a user defined code alphabet.
 */
std::string normalize_codes( std::string const& alphabet );

// =================================================================================================
//     Color Codes
// =================================================================================================

/**
 * @brief Return a map of text colors for each nucleic acid code.
 *
 * This function gives a color name usable for utils::Style for each nucleic acid code.
 * The return value of this function can for example be used in sequence::print_color() function.
 */
std::map<char, std::string> nucleic_acid_text_colors();

/**
 * @brief Return a map of text colors for each amino acid code.
 *
 * This function gives a color name usable for utils::Style for each amino acid code.
 * The return value of this function can for example be used in sequence::print_color() function.
 */
std::map<char, std::string> amino_acid_text_colors();

/**
 * @brief Return a map of Color%s for each nucleic acid code.
 *
 * This function gives a Color for each nucleic acid code.
 */
std::map<char, utils::Color> nucleic_acid_colors();

/**
 * @brief Return a map of Color%s for each amino acid code.
 *
 * This function gives a Color for each amino acid code.
 */
std::map<char, utils::Color> amino_acid_colors();

// =================================================================================================
//     Code Names
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
std::string nucleic_acid_name( char code );

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
std::string amino_acid_name( char code );

/**
 * @brief Return the possible ambiguous nucleic acid codes for a given code char.
 *
 * The codes are resolved as follows:
 *
 *     'A' ==> "A"
 *     'C' ==> "C"
 *     'G' ==> "G"
 *     'T' ==> "T"
 *     'U' ==> "T"
 *
 *     'W' ==> "AT"
 *     'S' ==> "CG"
 *     'M' ==> "AC"
 *     'K' ==> "GT"
 *     'R' ==> "AG"
 *     'Y' ==> "CT"
 *
 *     'B' ==> "CGT"
 *     'D' ==> "AGT"
 *     'H' ==> "ACT"
 *     'V' ==> "ACG"
 *
 *     'N' ==> "ACGT"
 *     'O' ==> "-"
 *     'X' ==> "-"
 *     '.' ==> "-"
 *     '-' ==> "-"
 *     '?' ==> "-"
 *
 * The code char is treated case-insensitive. If the given code char is not valid, an
 * `std::out_of_range` exception is thrown.
 *
 * See nucleic_acid_ambiguity_code() for a reverse version of this function. It is however not
 * exactly the reverse, as some degenerated codes are mapped to the gap char. Thus, this function
 * is not injective.
 */
std::string nucleic_acid_ambiguities( char code );

/**
 * @brief Return the nucleic acid code that represents all given `codes`.
 *
 * The codes are resolved as follows:
 *
 *     "A"    ==> 'A'
 *     "C"    ==> 'C'
 *     "G"    ==> 'G'
 *     "T"    ==> 'T'
 *
 *     "AT"   ==> 'W'
 *     "CG"   ==> 'S'
 *     "AC"   ==> 'M'
 *     "GT"   ==> 'K'
 *     "AG"   ==> 'R'
 *     "CT"   ==> 'Y'
 *
 *     "CGT"  ==> 'B'
 *     "AGT"  ==> 'D'
 *     "ACT"  ==> 'H'
 *     "ACG"  ==> 'V'
 *
 *     "ACGT" ==> 'N'
 *     "-"    ==> '-'
 *
 * The given codes are treated case-insensitive and order-independent. For example, given `"tCgG"`,
 * the function still returns `'B'`. However, if any of the given codes is not valid, an
 * `std::out_of_range` exception is thrown.
 *
 * See nucleic_acid_ambiguities() for the reverse of this function.
 */
char        nucleic_acid_ambiguity_code( std::string codes );

} // namespace sequence
} // namespace genesis

#endif // include guard
