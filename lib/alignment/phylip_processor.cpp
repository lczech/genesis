/**
 * @brief Implementation of functions for parsing and printing Phylip documents.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/phylip_processor.hpp"

#include <sstream>

#include "alignment/phylip_lexer.hpp"
#include "alignment/sequence_set.hpp"
#include "utils/core/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Parsing
// =============================================================================

/**
 * @brief
 */
bool PhylipProcessor::from_file (const std::string  fn, SequenceSet& aln)
{
    if (!file_exists(fn)) {
        LOG_WARN << "Phylip file '" << fn << "' does not exist.";
        return false;
    }
    return from_string(file_read(fn), aln);
}

/**
 * @brief
 */
bool PhylipProcessor::from_string (const std::string& fs, SequenceSet& aln)
{
    // do stepwise lexing
    PhylipLexer lexer;
    lexer.from_string(fs);

    // basic checks
    if (lexer.empty()) {
        LOG_INFO << "Phylip document is empty.";
        return false;
    }
    if (lexer.has_error()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }

    // delete tailing tokens immediately, produce tokens intime.
    PhylipLexer::iterator it = lexer.begin();

    std::string        label;
    std::ostringstream seq;

    // read number of sequences
    if (it == lexer.end() || it->has_new_lines() > 0) {
        LOG_WARN << "Phylip document begins with invalid new line(s).";
        return false;
    }
    if (it->is_white()) {
        ++it;
    }
    if (it == lexer.end() || !it->is_number()) {
        LOG_WARN << "Phylip document does not state a number of sequences at " << it->at() << ".";
        return false;
    }
    long num_seq = std::stoi(it->value());
    ++it;

    // read length of sequences
    if (it == lexer.end() || it->has_new_lines() > 0) {
        LOG_WARN << "Phylip document begins with invalid new line(s).";
        return false;
    }
    if (it->is_white()) {
        ++it;
    }
    if (it == lexer.end() || !it->is_number()) {
        LOG_WARN << "Phylip document does not state a length of the sequences.";
        return false;
    }
    long len_seq = std::stoi(it->value());
    ++it;

    // sanity check
    if (num_seq <= 0 || len_seq <= 0) {
        LOG_WARN << "Phylip document specified to contain no sequences or sequences of length zero."
                 << " Nothing to do here.";
        return false;
    }

    // go to first sequence
    if (it == lexer.end() || it->has_new_lines() != 1) {
        LOG_WARN << "Phylip document invalid at " << it->at() << ".";
        return false;
    }
    ++it;

    LOG_DBG << "Not yet fully implemented.";
    return false;

    // prepare alignment and sequences
    aln.clear();
    for (long i = 0; i < num_seq; ++i) {
        Sequence* nseq = new Sequence(label, seq.str());
        aln.sequences.push_back(nseq);
    }

    // process the first batch of lines, which contain the labels.
    while (it != lexer.end() && it->has_new_lines() != 2) {
        //
    }

    // process rest of file with sequences
    while (it != lexer.end()) {
        //
    }
    return true;
}

// =============================================================================
//     Printing
// =============================================================================

/**
 * @brief
 */
bool PhylipProcessor::to_file (const SequenceSet& sset, const std::string fn)
{
    if (file_exists(fn)) {
        LOG_WARN << "Phylip file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string fs;
    to_string(sset, fs);
    return file_write(fn, fs);
}

/**
 * @brief
 */
void PhylipProcessor::to_string (const SequenceSet& sset, std::string& fs)
{
    fs = to_string(sset);
}

/**
 * @brief
 */
std::string PhylipProcessor::to_string (const SequenceSet& sset)
{
    size_t length = 0;
    for (Sequence* s : sset.sequences) {
        if (length == 0) {
            length = s->length();
            continue;
        }
        if (s->length() != length) {
            LOG_WARN << "Sequences in alignment have different lengths.";
            return "";
        }
    }
    if (length == 0) {
        LOG_WARN << "No sequences in alignment.";
        return "";
    }

    std::ostringstream seq("");
    seq << sset.sequences.size() << " " << length << "\n";
    for (Sequence* s : sset.sequences) {
        // print label and sequence
        seq << s->label() << " " << s->sites() << "\n";
    }
    return seq.str();
}

} // namespace genesis
