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

    std::string        label = "";
    std::ostringstream seq("");

    // TODO this is neither nice nor fast...
    // TODO make it possible to transform sequence on the fly!

    // fill the alignment object
    for ( ; it != lexer.end(); ++it) {
        if (it->IsTag()) {
            // store sequence, if we already have one
            if (label != "" || seq.str() != "") {
                Sequence::SymbolType* sites = strdup(seq.str().c_str());
                aln.sequences.emplace_back(label, sites, seq.str().size());
            }

            // start new sequence
            label = it->value();
            seq.str("");
            seq.clear();
            continue;
        }
        if (it->IsSymbol()) {
            seq << it->value();
            continue;
        }
        LOG_WARN << "Invalid FASTA document.";
    }

    // write last seq
    if (label != "" || seq.str() != "") {
        Sequence::SymbolType* sites = strdup(seq.str().c_str());
        aln.sequences.emplace_back(label, sites, seq.str().size());
    }

    return true;
}

// =============================================================================
//     Printing
// =============================================================================

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
    for (Sequence s : aln.sequences) {
        seq << ">" << s.Label() << "\n";
        // TODO add \n every 80 (configurable) chars
        seq << s.Sites() << "\n";
    }
    return seq.str();
}

} // namespace genesis
