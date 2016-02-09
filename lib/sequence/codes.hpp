#ifndef GENESIS_SEQUENCE_CODES_H_
#define GENESIS_SEQUENCE_CODES_H_

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
//     Codes
// =================================================================================================

std::string nucleic_acid_codes_plain();
std::string nucleic_acid_codes_ambiguous();
std::string nucleic_acid_codes_determined();
std::string nucleic_acid_codes_undetermined();
std::string nucleic_acid_codes_all();

std::string amino_acid_codes_plain();
std::string amino_acid_codes_ambiguous();
std::string amino_acid_codes_determined();
std::string amino_acid_codes_undetermined();
std::string amino_acid_codes_all();

// =================================================================================================
//     Translate Codes
// =================================================================================================

std::string translate_nucleic_acid( char code );
std::string translate_amino_acid( char code );

} // namespace sequence
} // namespace genesis

#endif // include guard
