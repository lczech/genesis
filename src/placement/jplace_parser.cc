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

bool JplaceParser::ProcessFile (const std::string& fn, Placements& placements)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Jplace file '" << fn << "' does not exist.";
        return false;
    }
    return ProcessString(FileRead(fn), placements);
}

bool JplaceParser::ProcessString (const std::string& jplace, Placements& placements)
{
    JsonLexer lexer;
    if (!lexer.ProcessString(jplace)) {
        return false;
    }
    return ProcessLexer(lexer, placements);
}

bool JplaceParser::ProcessLexer (const JsonLexer& lexer, Placements& placements)
{
    JsonDocument doc;
    if (!JsonParser::ProcessLexer(lexer, doc)) {
        return false;
    }
    return ProcessDocument(doc, placements);
}

bool JplaceParser::ProcessDocument (const JsonDocument& doc, Placements& placements)
{
    // TODO clean

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
    if (!val || !val->IsString() || !placements.tree.FromNewickString(val->ToString())) {
        LOG_WARN << "Jplace document does not contain a valid Newick tree at key 'tree'.";
        return false;
    }

    // get the field names and store them in array fields
    val = doc.Get("fields");
    if (!val || !val->IsArray()) {
        LOG_WARN << "Jplace document does not contain field names at key 'fields'.";
        return false;
    }
    JsonValueArray* fields_arr = JsonValueToArray(val);
    std::vector<std::string> fields;
    fields.reserve(fields_arr->size());
    for (JsonValue* fields_val : *fields_arr) {
        if (!fields_val->IsString()) {
            LOG_WARN << "Jplace document contains a value of type '" << fields_val->TypeToString()
                     << "' instead of a string with a field name at key 'fields'.";
            return false;
        }
        std::string field = fields_val->ToString();
        if (field == "edge_num"      || field == "likelihood"     || field == "like_weight_ratio" ||
            field == "distal_length" || field == "pendant_length" || field == "parsimony"
        ) {
            fields.push_back(field);
        } else {
            LOG_WARN << "Jplace document contains a field name '" << field << "' "
                     << "at key 'fields', which is not used by this parser and thus skipped.";
        }
    }

    // find and process the pqueries
    val = doc.Get("placements");
    if (!val || !val->IsArray()) {
        LOG_WARN << "Jplace document does not contain pqueries at key 'placements'.";
        return false;
    }
    JsonValueArray* placements_arr = JsonValueToArray(val);
    for (JsonValue* pqry_val : *placements_arr) {
        if (!pqry_val->IsObject()) {
            LOG_WARN << "Jplace document contains a value of type '" << pqry_val->TypeToString()
                     << "' instead of an object with a pquery at key 'placements'.";
            return false;
        }
        JsonValueObject* pqry_obj = JsonValueToObject(pqry_val);
        if (!pqry_obj->Has("p") || !pqry_obj->Get("p")->IsArray()) {
            LOG_WARN << "Jplace document contains a pquery at key 'placements' that does not "
                     << "contain an array of placements at sub-key 'p'.";
            return false;
        }

        // create new pquery
        Pquery pqry;

        // process the placements and store them in the pquery
        JsonValueArray* pqry_p_arr = JsonValueToArray(pqry_obj->Get("p"));
        for (JsonValue* pqry_p_val : *pqry_p_arr) {
            if (!pqry_p_val->IsArray()) {
                LOG_WARN << "Jplace document contains a pquery with invalid placement at key 'p'.";
                return false;
            }
            JsonValueArray* pqry_fields = JsonValueToArray(pqry_p_val);
            if (pqry_fields->size() != fields.size()) {
                LOG_WARN << "Jplace document contains placement fields array with different size "
                         << "than the fields name array.";
                return false;
            }

            // process all fields of the placement
            Pquery::Placement pqry_place;
            for (size_t i = 0; i < pqry_fields->size(); ++i) {
                // so far, the p-fields only contain numbers, so we can do this check here for all
                // fields, instead of repetition for every field.
                // if in the future there are fields with non-number type, this check has to go
                // into the single field assignments.
                if (!pqry_fields->at(i)->IsNumber()) {
                    LOG_WARN << "Jplace document contains pquery where field " << fields[i]
                             << " is of type '" << pqry_fields->at(i)->TypeToString()
                             << "' instead of a number.";
                    return false;
                }

                double pqry_place_val = JsonValueToNumber(pqry_fields->at(i))->value;
                if        (fields[i] == "edge_num") {
                    pqry_place.edge_num          = pqry_place_val;
                } else if (fields[i] == "likelihood") {
                    pqry_place.likelihood        = pqry_place_val;
                } else if (fields[i] == "like_weight_ratio") {
                    pqry_place.like_weight_ratio = pqry_place_val;
                } else if (fields[i] == "distal_length") {
                    pqry_place.distal_length     = pqry_place_val;
                } else if (fields[i] == "pendant_length") {
                    pqry_place.pendant_length    = pqry_place_val;
                } else if (fields[i] == "parsimony") {
                    pqry_place.parsimony         = pqry_place_val;
                }
            }

            pqry.placements.push_back(pqry_place);
        }

        // process names

        // process named multiplicities
    }

    // check if there is metadata
    val = doc.Get("metadata");
    if (val && val->IsObject()) {
        JsonValueObject* meta_obj = JsonValueToObject(val);
        for (JsonValueObject::ObjectPair meta_pair : *meta_obj) {
            placements.metadata[meta_pair.first] = meta_pair.second->ToString();
        }
    }

    LOG_DBG << placements.tree.DumpAll();
    return true;
}

} // namespace genesis
