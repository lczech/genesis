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

std::string translate_nucleic_acid( char code );
std::string translate_amino_acid( char code );

} // namespace sequence
} // namespace genesis

#endif // include guard
