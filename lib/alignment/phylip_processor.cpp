/**
 * @brief Implementation of functions for parsing and printing Phylip documents.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/phylip_processor.hpp"

#include <sstream>

#include "alignment/sequence_set.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Parsing
// =============================================================================

/**
 * @brief Determines the length of the label in front of the sequences.
 *
 * Phylip has the weird property that labels are written in front of sequences and do not need to
 * have a delimiter, but instead are simply the first n characters of the string. This value
 * determines after how many chars the label ends and the actual sequence begins.
 *
 * If set to 0 (default), a relaxed version of Phylip is used, where the sequence begin is
 * automatically detected. However, in this case, there has to be at least one whitespace character
 * (except new lines like CR or LF) between the label and the sequence.
 */
size_t PhylipProcessor::label_length = 0;

/**
 * @brief
 */
bool PhylipProcessor::FromFile (const std::string  fn, SequenceSet& aln)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Phylip file '" << fn << "' does not exist.";
        return false;
    }
    return FromString(FileRead(fn), aln);
}

/**
 * @brief
 */
bool PhylipProcessor::FromString (const std::string& fs, SequenceSet& aln)
{
    // do stepwise lexing
    PhylipLexer lexer;
    lexer.ProcessString(fs, true);

    // basic checks
    if (lexer.empty()) {
        LOG_INFO << "Phylip document is empty.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }

    // delete tailing tokens immediately, produce tokens intime.
    PhylipLexer::iterator it = lexer.begin();
    it.ConsumeWithTail(0);
    it.ProduceWithHead(0);

    std::string        label;
    std::ostringstream seq;

    // read number of sequences
    if (it == lexer.end() || it->HasNewLines() > 0) {
        LOG_WARN << "Phylip document begins with invalid new line(s).";
        return false;
    }
    if (it->IsWhite()) {
        ++it;
    }
    if (it == lexer.end() || !it->IsNumber()) {
        LOG_WARN << "Phylip document does not state a number of sequences at " << it->at() << ".";
        return false;
    }
    long num_seq = std::stoi(it->value());
    ++it;

    // read length of sequences
    if (it == lexer.end() || it->HasNewLines() > 0) {
        LOG_WARN << "Phylip document begins with invalid new line(s).";
        return false;
    }
    if (it->IsWhite()) {
        ++it;
    }
    if (it == lexer.end() || !it->IsNumber()) {
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
    if (it == lexer.end() || it->HasNewLines() != 1) {
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
    while (it != lexer.end() && it->HasNewLines() != 2) {
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
bool PhylipProcessor::ToFile (const std::string fn, const SequenceSet& aln)
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
void PhylipProcessor::ToString (std::string& fs, const SequenceSet& aln)
{
    fs = ToString(aln);
}

/**
 * @brief
 */
std::string PhylipProcessor::ToString (const SequenceSet& aln)
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
