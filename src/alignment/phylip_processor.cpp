/**
 * @brief Implementation of functions for parsing and printing Phylip documents.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/phylip_processor.hpp"

#include <sstream>

#include "alignment/alignment.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Parsing
// =============================================================================

// =============================================================================
//     Printing
// =============================================================================

/**
 * @brief
 */
bool PhylipProcessor::ToFile (const std::string fn, const Alignment& aln)
{
    if (FileExists(fn)) {
        LOG_WARN << "Phylip file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string fs;
    ToString(fs, aln);
    return FileWrite(fn, fs);
}

/**
 * @brief
 */
void PhylipProcessor::ToString (std::string& fs, const Alignment& aln)
{
    fs = ToString(aln);
}

/**
 * @brief
 */
std::string PhylipProcessor::ToString (const Alignment& aln)
{
    size_t length = 0;
    for (Sequence* s : aln.sequences) {
        if (length == 0) {
            length = s->Length();
            continue;
        }
        if (s->Length() != length) {
            LOG_WARN << "Sequences in alignment have different lengths.";
            return "";
        }
    }
    if (length == 0) {
        LOG_WARN << "No sequences in alignment.";
        return "";
    }

    std::ostringstream seq("");
    seq << aln.sequences.size() << " " << length << "\n";
    for (Sequence* s : aln.sequences) {
        // print label and sequence
        seq << s->Label() << " " << s->Sites() << "\n";
    }
    return seq.str();
}

} // namespace genesis
