/**
 * @brief Implementation of SequenceSet class.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/sequence_set.hpp"

#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "alignment/sequence.hpp"
#include "utils/core/logging.hpp"

namespace genesis {

// =============================================================================
//     Constructor and Destructor
// =============================================================================

/**
 * @brief Destructor. Calls clear().
 */
SequenceSet::~SequenceSet()
{
    clear();
}

/**
 * @brief Deletes all sequences from the alignment.
 */
void SequenceSet::clear()
{
    for (Sequence* s : sequences) {
        delete s;
    }
    sequences.clear();
}

// =============================================================================
//     Accessors
// =============================================================================

/**
 * @brief Returns a pointer to a sequence with a specific label (or `nullptr`, if not found).
 */
Sequence* SequenceSet::find_sequence(std::string label) const
{
    for (Sequence* s : sequences) {
        if (s->label() == label) {
            return s;
        }
    }
    return nullptr;
}

// =============================================================================
//     Modifiers
// =============================================================================

/**
 * @brief Removes and deletes all those sequences from the Aligment whose labels are in the given
 * list. If `invert` is set to true, it does the same inverted: it removes all except those in the
 * list.
 *
 * We cannot use standard algorithms like std::remove here, as those do not delete the elements
 * (call their destructor).
 */
void SequenceSet::remove_list(std::vector<std::string> labels, bool invert)
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

/**
 * @brief Calls remove_gaps() for every Sequence.
 */
void SequenceSet::remove_gaps()
{
    for (Sequence* s : sequences) {
        s->remove_gaps();
    }
}

/**
 * @brief Calls replace() for every Sequence.
 */
void SequenceSet::replace(char search, char replace)
{
    for (Sequence* s : sequences) {
        s->replace(search, replace);
    }
}

// =============================================================================
//     Dump and Debug
// =============================================================================

/**
 * @brief Gives a summary of the sequences names and their lengths for this alignment.
 */
std::string SequenceSet::dump() const
{
    std::ostringstream out;
    for (Sequence* s : sequences) {
        out << s->label() << " [" << s->length() << "]\n";
    }
    return out.str();
}

} // namespace genesis
