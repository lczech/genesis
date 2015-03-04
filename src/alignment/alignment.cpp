/**
 * @brief Implementation of Alignment class.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/alignment.hpp"

#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "utils/logging.hpp"

namespace genesis {

// =============================================================================
//     Constructor and Destructor
// =============================================================================

/**
 * @brief Destructor. Calls clear().
 */
Alignment::~Alignment()
{
    clear();
}

/**
 * @brief Deletes all sequences from the alignment.
 */
void Alignment::clear()
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
Sequence* Alignment::FindSequence(std::string label) const
{
    for (Sequence* s : sequences) {
        if (s->Label() == label) {
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
void Alignment::RemoveList(std::vector<std::string> labels, bool invert)
{
    // create a set of all labels for fast lookup.
    std::unordered_set<std::string> lmap(labels.begin(), labels.end());

    // iterate and move elements from it to re
    std::vector<Sequence*>::iterator it = sequences.begin();
    std::vector<Sequence*>::iterator re = sequences.begin();

    // this works similar to std::remove (http://www.cplusplus.com/reference/algorithm/remove/)
    while (it != sequences.end()) {
        // if the label is (not) in the map, move it to the re position, otherwise delete it.
        if ( (!invert && lmap.count((*it)->Label())  > 0) ||
             ( invert && lmap.count((*it)->Label()) == 0)
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
//     Dump and Debug
// =============================================================================

/**
 * @brief Gives a summary of the sequences names and their lengths for this alignment.
 */
std::string Alignment::Dump() const
{
    std::ostringstream out;
    for (Sequence* s : sequences) {
        out << s->Label() << " [" << s->Length() << "]\n";
    }
    return out.str();
}

} // namespace genesis
