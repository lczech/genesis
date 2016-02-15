#ifndef GENESIS_SEQUENCE_FUNCTIONS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_H_

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

// ================================================================================================
//     Forwad Declarations
// ================================================================================================

class Sequence;
class SequenceSet;

// ================================================================================================
//     Accessors
// ================================================================================================

Sequence const* find_sequence( SequenceSet const& set, std::string const& label );

// ================================================================================================
//     Characteristics
// ================================================================================================

std::map<char, size_t> site_histogram( Sequence    const& seq );
std::map<char, size_t> site_histogram( SequenceSet const& set );

std::map<char, double> base_frequencies( Sequence    const& seq, std::string const& plain_chars );
std::map<char, double> base_frequencies( SequenceSet const& set, std::string const& plain_chars );

size_t count_chars( SequenceSet const& set, std::string const& chars );

double gapyness( SequenceSet const& set, std::string const& undetermined_chars );

bool validate_chars( SequenceSet const& set, std::string const& chars );

size_t total_length( SequenceSet const& set );

bool is_alignment( SequenceSet const& set );

// ================================================================================================
//     Print and Output
// ================================================================================================

std::ostream& operator << ( std::ostream& out, Sequence    const& seq );
std::ostream& operator << ( std::ostream& out, SequenceSet const& set );

std::string print_color(
    Sequence const&                    seq,
    std::map<char, std::string> const& colors,
    size_t                             length_limit = 100,
    bool                               label = false,
    bool                               background = true
);

std::string print_color(
    SequenceSet const&                 set,
    std::map<char, std::string> const& colors,
    size_t                             length_limit = 100,
    size_t                             sequence_limit = 10,
    bool                               label = false,
    bool                               background = true
);

// ================================================================================================
//     Modifiers
// ================================================================================================

/*

normalize --> to upper
normalize_chars
normalize_gaps

void remove_list(SequenceSet& set, std::vector<std::string> const& labels, bool invert);

// ================================================================================================
//     Sequence Modifiers
// ================================================================================================

void remove_gaps();
void replace(char search, char replace);

remove_chars
compress_chars
replace_chars

*/

} // namespace sequence
} // namespace genesis

#endif // include guard
