/**
 * @brief Implementation of Alignment class.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/alignment.hpp"

#include <sstream>

namespace genesis {

// =============================================================================
//     Constructor and Destructor
// =============================================================================

/**
 * @brief Destructor.
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
