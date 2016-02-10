/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/functions.hpp"

#include <algorithm>
#include <array>
#include <assert.h>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils/core/logging.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace sequence {

// ================================================================================================
//     Accessors
// ================================================================================================

/**
 * @brief Return a pointer to a sequence with a specific label, or `nullptr` iff not found.
 */
Sequence const* find_sequence( SequenceSet const& set, std::string const& label )
{
    for (Sequence const& s : set) {
        if (s.label() == label) {
            return &s;
        }
    }
    return nullptr;
}

// ================================================================================================
//     Characteristics
// ================================================================================================

/**
 * @brief Local helper function to create a case-insensitive lookup table.
 */
std::array<bool, 128> make_lookup_table( std::string const& chars )
{
    auto lookup = std::array<bool, 128> {};
    for( char c : chars ) {
        if( c < 0 ) {
            throw std::invalid_argument("Invalid chars provided.");
        }
        assert( c >= 0 );
        lookup[ static_cast<unsigned char>( toupper(c) ) ] = true;
        lookup[ static_cast<unsigned char>( tolower(c) ) ] = true;
    }
    return lookup;
}

/**
 * @brief Count the number of occurrences of the given `chars` within the sites of the SequenceSet.
 *
 * This function can be used to count e.g. gaps or ambiguous characters in sequences.
 * For presettings of usable chars, see the functions `nucleic_acid_codes_...` and
 * `amino_acid_codes_...`. This function is case-insensitive.
 *
 * If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`
 * exception is thrown.
 */
size_t count_chars( SequenceSet const& set, std::string const& chars )
{
    auto lookup = make_lookup_table( chars );
    size_t counter = 0;
    for( auto& s : set ) {
        for( auto& c : s ) {
            if( c < 0 ) {
                continue;
            }
            assert( c >= 0 );
            if( lookup[ static_cast<unsigned char>(c) ] ) {
                ++counter;
            }
        }
    }
    return counter;
}

/**
 * @brief Return the total length (sum) of all sequences in the set.
 */
size_t total_length( SequenceSet const& set )
{
    return std::accumulate( set.begin(), set.end(), 0,
        [] (size_t c, Sequence const& s) {
            return c + s.length();
        }
    );
}

/**
 * @brief Returns true iff all sequences only consist of the given `chars`.
 *
 * For presettings of usable chars, see the functions `nucleic_acid_codes_...` and
 * `amino_acid_codes_...`. For example, to check whether the sequences are nucleic acids,
 * use `nucleic_acid_codes_all()`. This function is case-insensitive.
 *
 * If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`
 * exception is thrown.
 */
bool validate_chars( SequenceSet const& set, std::string const& chars )
{
    auto lookup = make_lookup_table( chars );
    for( auto& s : set ) {
        for( auto& c : s ) {
            if( c < 0 ) {
                return false;
            }
            assert( c >= 0 );
            if( ! lookup[ static_cast<unsigned char>(c) ] ) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Return true iff all sequences in the set have the same length.
 */
bool is_alignment( SequenceSet const& set )
{
    if( set.size() == 0 ) {
        return true;
    }

    size_t length = set[0].length();
    for( auto& s : set ) {
        if( s.length() != length ) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Return the "gapyness" of the sequences, i.e., the proportion of gap chars
 * and other completely undetermined chars to the total length of all sequences.
 *
 * This function returns a value in the interval 0.0 (no gaps and undetermined chars at all)
 * and 1.0 (all chars are undetermined).
 * See `nucleic_acid_codes_undetermined()` and `amino_acid_codes_undetermined()` for presettings
 * of gap character that can be used here depending on the data set type.
 * This function is case-insensitive. In the special case that there are no sequences or sites,
 * 0.0 is returned.
 */
double gapyness( SequenceSet const& set, std::string const& undetermined_chars )
{
    size_t gaps = count_chars( set, undetermined_chars );
    size_t size = total_length( set );
    if( size == 0 ) {
        return 0.0;
    }

    double ret = static_cast<double>( gaps ) / static_cast<double>( size );
    assert( 0.0 <= ret && ret <= 1.0 );
    return ret;
}

// =============================================================================
//     Modifiers
// =============================================================================

/*
/ **
 * @brief Remove and delete all those sequences from a SequenceSet whose labels are in the given
 * list. If `invert` is set to true, it does the same inverted: it removes all except those in the
 * list.
 * /
void remove_list(SequenceSet& set, std::vector<std::string> const& labels, bool invert)
{
    // create a set of all labels for fast lookup.
    std::unordered_set<std::string> lmap(labels.begin(), labels.end());

    // iterate and move elements from it to re
    std::vector<Sequence*>::iterator it = sequences.begin();
    std::vector<Sequence*>::iterator re = sequences.begin();

    // this works similar to std::remove (http://www.cplusplus.com/reference/algorithm/remove/)
    while (it != sequences.end()) {
        // if the label is (not) in the map, move it to the re position, otherwise delete it.
        if ( (!invert && lmap.count((*it)->label())  > 0) ||
             ( invert && lmap.count((*it)->label()) == 0)
        ) {
            delete *it;
        } else {
            *re = std::move(*it);
            ++re;
        }
        ++it;
    }

    // delete the tail of the vector.
    sequences.erase(re, sequences.end());
}

// =============================================================================
//     Sequence Modifiers
// =============================================================================

/ **
 * @brief Calls remove_gaps() for every Sequence.
 * /
void SequenceSet::remove_gaps()
{
    for (Sequence* s : sequences) {
        s->remove_gaps();
    }
}

/ **
 * @brief Calls replace() for every Sequence.
 * /
void SequenceSet::replace(char search, char replace)
{
    for (Sequence* s : sequences) {
        s->replace(search, replace);
    }
}

// =============================================================================
//     Dump
// =============================================================================

/ **
 * @brief Gives a summary of the sequences names and their lengths for this set.
 * /
std::string SequenceSet::dump() const
{
    std::ostringstream out;
    for (Sequence* s : sequences) {
        out << s->label() << " [" << s->length() << "]\n";
    }
    return out.str();
}

// =============================================================================
//     Mutators
// =============================================================================

/ **
 * @brief Removes all occurences of `gap_char` from the sequence.
 * /
void Sequence::remove_gaps( std::string const& gap_chars )
{
    sites_.erase(std::remove(sites_.begin(), sites_.end(), gap_char), sites_.end());
}

void Sequence::compress_gaps( std::string const& gap_chars )
{
    ...
}

/ **
 * @brief Replaces all occurences of `search` by `replace`.
 * /
void Sequence::replace(char search, char replace)
{
    sites_ = text::replace_all (sites_, std::string(1, search), std::string(1, replace));
}

// =============================================================================
//     Dump and Debug
// =============================================================================

/ **
 * @brief Prints the label and the whole sequence (possibly very long!).
 * /
std::string Sequence::dump() const
{
    return label() + ": " + sites();
}


*/

} // namespace sequence
} // namespace genesis
