#ifndef GENESIS_SEQUENCE_CODES_H_
#define GENESIS_SEQUENCE_CODES_H_

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
