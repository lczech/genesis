/**
 * @brief Implementation of Jplace Parser functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/jplace_parser.hh"

#include <string>

#include "tree/tree.hh"
#include "utils/json_document.hh"
#include "utils/json_lexer.hh"
#include "utils/json_parser.hh"
#include "utils/logging.hh"
#include "utils/utils.hh"

namespace genesis {

bool JplaceParser::ProcessFile (const std::string& fn, PlacementTree& tree)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Jplace file '" << fn << "' does not exist.";
        return false;
    }
    return ProcessString(FileRead(fn), tree);
}

bool JplaceParser::ProcessString (const std::string& jplace, PlacementTree& tree)
{
    JsonLexer lexer;
    lexer.ProcessString(jplace);
    return ProcessLexer(lexer, tree);
}

bool JplaceParser::ProcessLexer (const JsonLexer& lexer, PlacementTree& tree)
{
    JsonDocument doc;
    if (!JsonParser::ProcessLexer(lexer, doc)) {
        return false;
    }
    return ProcessDocument(doc, tree);
}

bool JplaceParser::ProcessDocument (const JsonDocument& doc, PlacementTree& tree)
{
    LOG_DBG << doc.Dump();
    tree.FromNewickString("");
    return true;
}

} // namespace genesis
