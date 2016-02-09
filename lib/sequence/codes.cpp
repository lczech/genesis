/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/codes.hpp"

#include <cctype>
#include <unordered_map>
#include <stdexcept>

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
    { 'X', "masked" },
    { '.', "gap" },
    { '-', "gap" }
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
    { '-', "gap" }
};

// =================================================================================================
//     Functions
// =================================================================================================

/**
 * @brief Get the name of a nucleic acid given its IUPAC code.
 *
 *  The codes are translates as follows:
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
 *     X masked
 *     . gap
 *     - gap
 *
 * The code char is treated case-insensitive. If the given code char is not valid, an
 * `std::out_of_range` exception is thrown.
 */
std::string translate_nucleic_acid( char code )
{
    return nucleic_acid_code_to_name.at( toupper(code) );
}

/**
 * @brief Get the name of a amino acid given its IUPAC code.
 *
 *  The codes are translates as follows:
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
 *
 * The code char is treated case-insensitive. If the given code char is not valid, an
 * `std::out_of_range` exception is thrown.
 */
std::string translate_amino_acid( char code )
{
    return amino_acid_code_to_name.at( toupper(code) );
}

} // namespace sequence
} // namespace genesis
