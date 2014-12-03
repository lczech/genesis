/**
 * @brief Implementation of Jplace Parser functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/jplace_parser.hh"

#include <string>
#include <vector>

//~ #include "placement/placement_data.hh"
#include "placement/placements.hh"
#include "tree/tree.hh"
#include "utils/json_document.hh"
#include "utils/json_lexer.hh"
#include "utils/json_parser.hh"
#include "utils/logging.hh"
#include "utils/utils.hh"

namespace genesis {

bool JplaceParser::ProcessFile (const std::string& fn, Placements& place)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Jplace file '" << fn << "' does not exist.";
        return false;
    }
    return ProcessString(FileRead(fn), place);
}

bool JplaceParser::ProcessString (const std::string& jplace, Placements& place)
{
    JsonLexer lexer;
    if (!lexer.ProcessString(jplace)) {
        return false;
    }
    return ProcessLexer(lexer, place);
}

bool JplaceParser::ProcessLexer (const JsonLexer& lexer, Placements& place)
{
    JsonDocument doc;
    if (!JsonParser::ProcessLexer(lexer, doc)) {
        return false;
    }
    return ProcessDocument(doc, place);
}

bool JplaceParser::ProcessDocument (const JsonDocument& doc, Placements& place)
{
    // check if the version is correct
    JsonValue* val = doc.Get("version");
    if (!val || !val->IsNumber()) {
        LOG_WARN << "Jplace document does not contain a valid version number at key 'version'.";
        return false;
    }
    if (JsonValueToNumber(val)->value != version) {
        LOG_WARN << "Jplace document has version number '" << val->ToString()
                 << "', however this parser is written for version " << version << " of Jplace. "
                 << "Now continuing to parse in the hope that it still works.";
    }

    // find and process the reference tree
    val = doc.Get("tree");
    if (!val || !val->IsString()) {
        LOG_WARN << "Jplace document does not contain a Newick tree at key 'tree'.";
        return false;
    }
    if (!place.tree.FromNewickString(JsonValueToString(val)->value)) {
        return false;
    }

    // get the field names and store them in array fields
    val = doc.Get("fields");
    if (!val || !val->IsArray()) {
        LOG_WARN << "Jplace document does not contain field names at key 'fields'.";
        return false;
    }
    JsonValueArray* farr = JsonValueToArray(val);
    std::vector<std::string> fields;
    fields.reserve(farr->size());
    for (JsonValue* fval : *farr) {
        if (!fval->IsString()) {
            LOG_WARN << "Jplace document contains a value of type '" << fval->TypeToString()
                     << "' with value '" << fval->ToString()
                     << "' instead of a string with a field name at key 'fields'.";
            return false;
        }
        fields.push_back(fval->ToString());
    }

    // find and process the placements
    val = doc.Get("placements");
    if (!val || !val->IsArray()) {
        LOG_WARN << "Jplace document does not contain placements at key 'placements'.";
        return false;
    }
    JsonValueArray* parr = JsonValueToArray(val);
    for (JsonValue* pval : *parr) {
        if (!pval->IsObject()) {
            LOG_WARN << "Jplace document contains a value of type '" << pval->TypeToString()
                     << "' with value '" << pval->ToString()
                     << "' instead of an object with a placement at key 'placements'.";
            return false;
        }
        JsonValueObject* pobj = JsonValueToObject(pval);
        if (!pobj->Has("p") || !pobj->Get("p")->IsArray()) {
            LOG_WARN << "Jplace document contains a placement without object key 'p'"
                     << " containing an array at key 'placements'. You get it?";
            return false;
        }
        JsonValueArray* pparr = JsonValueToArray(pobj->Get("p"));
        for (JsonValue* ppval : *pparr) {
            if (!ppval->IsArray()) {
                LOG_WARN << "Jplace document contains invalid placement.";
                return false;
            }
            JsonValueArray* ppfields = JsonValueToArray(ppval);
            if (ppfields->size() != fields.size()) {
                LOG_WARN << "Jplace document contains fields value array with different size "
                         << "then the fields name array.";
                return false;
            }
            for (size_t i = 0; i < ppfields->size(); ++i) {
                if (fields[i] == "edge_num") {
                    // do stuff here
                }
            }
        }
    }

    // check if there is metadata
    val = doc.Get("metadata");
    if (val && val->IsObject()) {
        JsonValueObject* obj = JsonValueToObject(val);
        for (JsonValueObject::ObjectPair mdp : *obj) {
            place.metadata[mdp.first] = mdp.second->ToString();
        }
    }

    LOG_DBG << place.tree.DumpAll();
    return true;
}

} // namespace genesis
