/**
 * @brief Implementation of functions for parsing and printing FASTA documents.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/fasta_processor.hpp"

#include <sstream>
#include <string.h>

#include "alignment/alignment.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

namespace genesis {

// =============================================================================
//     Parsing
// =============================================================================

/**
 * @brief
 */
bool FastaProcessor::FromFile (const std::string fn, Alignment& aln)
{
    if (!FileExists(fn)) {
        LOG_WARN << "FASTA file '" << fn << "' does not exist.";
        return false;
    }
    return FromString(FileRead(fn), aln);
}

/**
 * @brief
 */
bool FastaProcessor::FromString (const std::string& fs, Alignment& aln)
{
    // do stepwise lexing
    FastaLexer lexer;
    lexer.ProcessString(fs, true);

    // basic checks
    if (lexer.empty()) {
        LOG_INFO << "FASTA document is empty.";
        return false;
    }
    if (lexer.HasError()) {
        LOG_WARN << "Lexing error at " << lexer.back().at()
                 << " with message: " << lexer.back().value();
        return false;
    }
    aln.clear();

    // delete tailing tokens immediately, produce tokens intime.
    Lexer::iterator it = lexer.begin();
    it.ConsumeWithTail(0);
    it.ProduceWithHead(0);

    std::string        label;
    std::ostringstream seq;

    // process all sequences
    while (it != lexer.end()) {
        // parse label
        if (!it->IsTag()) {
            LOG_WARN << "FASTA sequence does not start with '>' at " << it->at();
            return false;
        }
        label = it->value();
        ++it;

        // parse sequence
        seq.str("");
        seq.clear();
        while (it != lexer.end() && it->IsSymbol()) {
            seq << it->value();
            ++it;
        }

        // add to alignment
        Sequence* nseq = new Sequence(label, seq.str());
        aln.sequences.push_back(nseq);

        // there are no other lexer tokens than tag and symbol for fasta files!
        assert(it == lexer.end() || it->IsTag());
    }

    return true;
}

// =============================================================================
//     Printing
// =============================================================================

/**
 * @brief Determines after how many chars to do a line break when printing a FASTA file.
 *
 * Default is `80`. If set to `0`, no breaks are inserted.
 */
size_t FastaProcessor::line_length = 80;

/**
 * @brief
 */
bool FastaProcessor::ToFile (const std::string fn, const Alignment& aln)
{
    if (FileExists(fn)) {
        LOG_WARN << "FASTA file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string fs;
    ToString(fs, aln);
    return FileWrite(fn, fs);
}

/**
 * @brief
 */
void FastaProcessor::ToString (std::string& fs, const Alignment& aln)
{
    fs = ToString(aln);
}

/**
 * @brief
 */
std::string FastaProcessor::ToString (const Alignment& aln)
{
    std::ostringstream seq("");
    for (Sequence* s : aln.sequences) {
        // print label
        seq << ">" << s->Label() << "\n";

        // print sequence. if needed, add new line at every line_length position.
        if (line_length > 0) {
            for (size_t i = 0; i < s->Length(); i += line_length) {
                // write line_length many characters.
                // (if the string is shorter, as many characters as possible are used)
                seq << s->Sites().substr(i, line_length) << "\n";
            }
        } else {
            seq << s->Sites() << "\n";
        }
    }
    return seq.str();
}

} // namespace genesis
