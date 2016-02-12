/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/codes.hpp"

#include <cctype>
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
//     Codes
// =================================================================================================

/**
 * @brief Return all plain nucleic acid codes. Those are "ACGTU".
 */
std::string nucleic_acid_codes_plain()
{
    return "ACGTU";
}

/**
 * @brief Return all ambiguous nucleic acid codes. Those are "WSMKRYBDHV".
 */
std::string nucleic_acid_codes_ambiguous()
{
    return "WSMKRYBDHV";
}

/**
 * @brief Return all determined nucleic acid codes. Those are "ACGTUWSMKRYBDHV".
 */
std::string nucleic_acid_codes_determined()
{
    return nucleic_acid_codes_plain()
         + nucleic_acid_codes_ambiguous();
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
         + nucleic_acid_codes_ambiguous()
         + nucleic_acid_codes_undetermined();
}

/**
 * @brief Return all plain amino acid codes. Those are "ACDEFGHIKLMNOPQRSTUVWY".
 */
std::string amino_acid_codes_plain()
{
    return "ACDEFGHIKLMNOPQRSTUVWY";
}

/**
 * @brief Return all ambiguous amino acid codes. Those are "BJZ".
 */
std::string amino_acid_codes_ambiguous()
{
    return "BJZ";
}

/**
 * @brief Return all determined amino acid codes. Those are "ACDEFGHIKLMNOPQRSTUVWYBJZ".
 */
std::string amino_acid_codes_determined()
{
    return amino_acid_codes_plain()
         + amino_acid_codes_ambiguous();
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
         + amino_acid_codes_ambiguous()
         + amino_acid_codes_undetermined();
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
    return nucleic_acid_code_to_name.at( toupper(code) );
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
    return amino_acid_code_to_name.at( toupper(code) );
}

} // namespace sequence
} // namespace genesis
