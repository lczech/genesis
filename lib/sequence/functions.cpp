/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/functions.hpp"

#include "sequence/sequence.hpp"
#include "sequence/sequence_set.hpp"
#include "utils/core/logging.hpp"
#include "utils/text/string.hpp"

#include <algorithm>
#include <array>
#include <assert.h>
#include <numeric>
// #include <sstream>
#include <stdexcept>
#include <string>
// #include <unordered_map>
// #include <unordered_set>
// #include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Accessors
// =================================================================================================

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

// =================================================================================================
//     Characteristics
// =================================================================================================

// -------------------------------------------------------------------------
//     Site Histogram  &  Base Frequencies
// -------------------------------------------------------------------------

/**
 * @brief Get a histogram of the occurrences of particular sites, given a Sequence.
 *
 * This gives the raw counts of how often each site (character) appears in the Sequence.
 * See base_frequencies() for the relative version of this function.
 */
std::map<char, size_t> site_histogram( Sequence const& seq )
{
    std::map<char, size_t> sh;
    for( auto const& s : seq ) {
        ++sh[s];
    }
    return sh;
}

/**
 * @brief Get a histogram of the occurrences of particular sites, given a SequenceSet.
 *
 * This gives the raw counts of how often each site (character) appears in the whole set.
 * See base_frequencies() for the relative version of this function.
 */
std::map<char, size_t> site_histogram( SequenceSet const& set )
{
    std::map<char, size_t> sh;
    for( auto const& seq : set ) {
        for( auto const& s : seq ) {
            ++sh[s];
        }
    }
    return sh;
}

/**
 * @brief Local helper function that turns a site histogram into base frequencies.
 */
std::map<char, double> base_frequencies_accumulator(
    std::map<char, size_t> const& sitehistogram,
    std::string            const& plain_chars
) {
    // Calculate sum of raw counts of all chars given in plain_chars.
    size_t sum = 0;
    for( auto const& shp : sitehistogram ) {
        if( plain_chars.find( shp.first ) != std::string::npos ) {
            sum += shp.second;
        }
    }

    // Make relative.
    std::map<char, double> bf;
    for( auto const& pc : plain_chars ) {
        if( sitehistogram.count( pc )) {
            bf[pc] = static_cast<double>( sitehistogram.at(pc) ) / static_cast<double>( sum );
        }
    }
    return bf;
}

/**
 * @brief Get the base frequencies of the sites in a Sequence given the base chars.
 *
 * This returns the relative proportions of the given `plain_chars` to each other. Typically,
 * the given chars come from either nucleic_acid_codes_plain() or amino_acid_codes_plain(),
 * depending on the dataset.
 *
 * It is necessary to select those chars on a per-dataset basis, as it is up to the user to define
 * the meaning of those chars.
 */
std::map<char, double> base_frequencies( Sequence const& seq, std::string const& plain_chars )
{
    auto const sh = site_histogram( seq );
    return base_frequencies_accumulator( sh, plain_chars );
}

/**
 * @brief Get the base frequencies of the sites in a SequenceSet given the base chars.
 *
 * See the Sequence implementation of this function for details.
 */
std::map<char, double> base_frequencies( SequenceSet const& set, std::string const& plain_chars )
{
    auto const sh = site_histogram( set );
    return base_frequencies_accumulator( sh, plain_chars );
}

// -------------------------------------------------------------------------
//     Char counting and validation
// -------------------------------------------------------------------------

/**
 * @brief Local helper function to create a case-insensitive lookup table.
 */
std::array<bool, 128> make_lookup_table( std::string const& chars )
{
    auto lookup = std::array<bool, 128> {};
    for( char c : chars ) {
        // get rid of this check and leave it to the parser/lexer/stream iterator/lookup table
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
 * `amino_acid_codes_...`. The chars are treated case-insensitive.
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
            // get rid of this check and leave it to the parser/lexer/stream iterator
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
 * @brief Return the "gapyness" of the sequences, i.e., the proportion of gap chars
 * and other completely undetermined chars to the total length of all sequences.
 *
 * This function returns a value in the interval 0.0 (no gaps and undetermined chars at all)
 * and 1.0 (all chars are undetermined).
 * See `nucleic_acid_codes_undetermined()` and `amino_acid_codes_undetermined()` for presettings
 * of gap character that can be used here depending on the data set type.
 * The chars are treated case-insensitive.
 * In the special case that there are no sequences or sites, 0.0 is returned.
 */
double gapyness( SequenceSet const& set, std::string const& undetermined_chars )
{
    size_t gaps = count_chars( set, undetermined_chars );
    size_t len  = total_length( set );
    if( len == 0 ) {
        return 0.0;
    }

    double ret = static_cast<double>( gaps ) / static_cast<double>( len );
    assert( 0.0 <= ret && ret <= 1.0 );
    return ret;
}

/**
 * @brief Returns true iff all sequences only consist of the given `chars`.
 *
 * For presettings of usable chars, see the functions `nucleic_acid_codes_...` and
 * `amino_acid_codes_...`. For example, to check whether the sequences are nucleic acids,
 * use `nucleic_acid_codes_all()`. The chars are treated case-insensitive.
 *
 * If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`
 * exception is thrown.
 */
bool validate_chars( SequenceSet const& set, std::string const& chars )
{
    auto lookup = make_lookup_table( chars );
    for( auto& s : set ) {
        for( auto& c : s ) {
            // get rid of this check and leave it to the parser/lexer/stream iterator
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

// -------------------------------------------------------------------------
//     Length and length checks
// -------------------------------------------------------------------------

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
