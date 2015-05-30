/**
 * @brief Implementation of Jplace Processor functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/jplace_processor.hpp"

#include <string>
#include <vector>

#include "placement/placement_map.hpp"
#include "tree/newick_processor.hpp"
#include "utils/json_document.hpp"
#include "utils/json_processor.hpp"
#include "utils/logging.hpp"
#include "utils/options.hpp"
#include "utils/utils.hpp"

namespace genesis {

/**
 * @brief Returns the version number that this class is written for.
 */
std::string JplaceProcessor::get_version ()
{
    return "3";
}

/**
 * @brief Checks whether the version of the jplace format works with this parser.
 */
bool JplaceProcessor::check_version (const std::string version)
{
    return version == "2" || version == "3";
}

// =============================================================================
//     Parsing
// =============================================================================

bool JplaceProcessor::report_invalid_numbers  = false;
bool JplaceProcessor::correct_invalid_numbers = true;

/**
 * @brief Reads a file and parses it as a Jplace document into a PlacementMap object.
 *
 * Returns true iff successful.
 */
bool JplaceProcessor::from_file (const std::string& fn, PlacementMap& placements)
{
    if (!FileExists(fn)) {
        LOG_WARN << "Jplace file '" << fn << "' does not exist.";
        return false;
    }
    return from_string(FileRead(fn), placements);
}

/**
 * @brief Parses a string as a Jplace document into a PlacementMap object.
 *
 * Returns true iff successful.
 */
bool JplaceProcessor::from_string (const std::string& jplace, PlacementMap& placements)
{
    JsonDocument doc;
    if (!JsonProcessor::FromString(jplace, doc)) {
        return false;
    }
    return from_document(doc, placements);
}

/**
 * @brief Takes a JsonDocument object and parses it as a Jplace document into a PlacementMap object.
 *
 * Returns true iff successful.
 */
bool JplaceProcessor::from_document (const JsonDocument& doc, PlacementMap& placements)
{
    placements.clear();

    // check if the version is correct
    JsonValue* val = doc.Get("version");
    if (!val) {
        LOG_WARN << "Jplace document does not contain a valid version number at key 'version'. "
                 << "Now continuing to parse in the hope that it still works.";
    }
    if (!check_version(val->ToString())) {
        LOG_WARN << "Jplace document has version '" << val->ToString() << "', however this parser "
                 << "is written for version " << get_version() << " of the Jplace format. "
                 << "Now continuing to parse in the hope that it still works.";
    }

    // find and process the reference tree
    val = doc.Get("tree");
    if (!val || !val->IsString() || !NewickProcessor::FromString(val->ToString(), placements.tree)) {
        LOG_WARN << "Jplace document does not contain a valid Newick tree at key 'tree'.";
        return false;
    }

    // create a map from edge nums to the actual edge pointers, for later use when processing
    // the pqueries. we do not use PlacementMap::EdgeNumMap() here, because we need to do extra
    // checking for validity first!
    std::unordered_map<int, PlacementTree::EdgeType*> edge_num_map;
    for (
        PlacementTree::ConstIteratorEdges it = placements.tree.BeginEdges();
        it != placements.tree.EndEdges();
        ++it
    ) {
        PlacementTree::EdgeType* edge = *it;
        if (edge_num_map.count(edge->edge_num) > 0) {
            LOG_WARN << "Jplace document contains a tree where the edge num tag '"
                     << edge->edge_num << "' is used more than once.";
            return false;
        }
        edge_num_map.emplace(edge->edge_num, edge);
    }

    // get the field names and store them in array fields
    val = doc.Get("fields");
    if (!val || !val->IsArray()) {
        LOG_WARN << "Jplace document does not contain field names at key 'fields'.";
        return false;
    }
    JsonValueArray* fields_arr = JsonValueToArray(val);
    std::vector<std::string> fields;
    bool has_edge_num = false;
    for (JsonValue* fields_val : *fields_arr) {
        if (!fields_val->IsString()) {
            LOG_WARN << "Jplace document contains a value of type '" << fields_val->TypeToString()
                     << "' instead of a string with a field name at key 'fields'.";
            return false;
        }

        // check field validity
        std::string field = fields_val->ToString();
        if (field == "edge_num"      || field == "likelihood"     || field == "like_weight_ratio" ||
            field == "distal_length" || field == "pendant_length" || field == "proximal_length"   ||
            field == "parsimony"
        ) {
            for (std::string fn : fields) {
                if (fn == field) {
                    LOG_WARN << "Jplace document contains field name '" << field << "' more than "
                             << "once at key 'fields'.";
                    return false;
                }
            }
        } else {
            LOG_WARN << "Jplace document contains a field name '" << field << "' "
                     << "at key 'fields', which is not used by this parser and thus skipped.";
        }
        fields.push_back(field);
        has_edge_num |= (field == "edge_num");
    }
    if (!has_edge_num) {
        LOG_WARN << "Jplace document does not contain necessary field 'edge_num' at key 'fields'.";
        return false;
    }
    if (
        std::end(fields) != std::find(std::begin(fields), std::end(fields), "distal_length") &&
        std::end(fields) != std::find(std::begin(fields), std::end(fields), "proximal_length")
    ) {
        LOG_WARN << "Jplace document contains both fields 'distal_length', and 'proximal_length'. "
                 << "Currently, only one value is used internally to represent both, which might "
                 << "lead to inconsistency if the sum of both is not equal to the branch length.";
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
        Pquery* pqry = new Pquery();

        // process the placements and store them in the pquery
        JsonValueArray* pqry_p_arr = JsonValueToArray(pqry_obj->Get("p"));
        for (JsonValue* pqry_p_val : *pqry_p_arr) {
            if (!pqry_p_val->IsArray()) {
                LOG_WARN << "Jplace document contains a pquery with invalid placement at key 'p'.";
                return false;
            }
            JsonValueArray* pqry_fields = JsonValueToArray(pqry_p_val);
            if (pqry_fields->size() != fields.size()) {
                LOG_WARN << "Jplace document contains a placement fields array with different size "
                         << "than the fields name array.";
                return false;
            }

            // process all fields of the placement
            PqueryPlacement* pqry_place = new PqueryPlacement();
            for (size_t i = 0; i < pqry_fields->size(); ++i) {
                // up to version 3 of the jplace specification, the p-fields in a jplace document
                // only contain numbers (float or int),so we can do this check here once for all
                // fields, instead of repetition for every field. if in the future there are fields
                // with non-number type, this check has to go into the single field assignments.
                if (!pqry_fields->at(i)->IsNumber()) {
                    LOG_WARN << "Jplace document contains pquery where field " << fields[i]
                             << " is of type '" << pqry_fields->at(i)->TypeToString()
                             << "' instead of a number.";
                    return false;
                }

                // switch on the field name to set the correct value
                double pqry_place_val = JsonValueToNumber(pqry_fields->at(i))->value;
                if (fields[i] == "edge_num") {
                    if (edge_num_map.count(pqry_place_val) == 0) {
                        LOG_WARN << "Jplace document contains a pquery where field 'edge_num' "
                                 << "has value '" << pqry_place_val << "', which is not marked "
                                 << "in the given tree as an edge num.";
                        return false;
                    }
                    pqry_place->edge_num = pqry_place_val;
                    pqry_place->edge = edge_num_map.at(pqry_place_val);
                    pqry_place->edge->placements.push_back(pqry_place);

                } else if (fields[i] == "likelihood") {
                    pqry_place->likelihood = pqry_place_val;

                } else if (fields[i] == "like_weight_ratio") {
                    pqry_place->like_weight_ratio = pqry_place_val;

                } else if (fields[i] == "distal_length") {
                    // the jplace format uses distal length, but we use proximal,
                    // so we need to convert here.
                    pqry_place->proximal_length = pqry_place->edge->branch_length - pqry_place_val;

                } else if (fields[i] == "proximal_length") {
                    pqry_place->proximal_length = pqry_place_val;

                } else if (fields[i] == "pendant_length") {
                    pqry_place->pendant_length = pqry_place_val;

                } else if (fields[i] == "parsimony") {
                    pqry_place->parsimony = pqry_place_val;
                }
            }
            pqry_place->pquery = pqry;
            pqry->placements.push_back(pqry_place);

            // check validity of placement values
            if (report_invalid_numbers || correct_invalid_numbers) {
                if (pqry_place->like_weight_ratio < 0.0) {
                    if (report_invalid_numbers) {
                        LOG_INFO << "Invalid placement with like_weight_ratio < 0.0.";
                    }
                    if (correct_invalid_numbers) {
                        pqry_place->like_weight_ratio = 0.0;
                    }
                }
                if (pqry_place->like_weight_ratio > 1.0) {
                    if (report_invalid_numbers) {
                        LOG_INFO << "Invalid placement with like_weight_ratio > 1.0.";
                    }
                    if (correct_invalid_numbers) {
                        pqry_place->like_weight_ratio = 1.0;
                    }
                }
                if (pqry_place->pendant_length < 0.0) {
                    if (report_invalid_numbers) {
                        LOG_INFO << "Invalid placement with pendant_length < 0.0.";
                    }
                    if (correct_invalid_numbers) {
                        pqry_place->pendant_length = 0.0;
                    }
                }
                if (pqry_place->proximal_length < 0.0) {
                    if (report_invalid_numbers) {
                        LOG_INFO << "Invalid placement with proximal_length < 0.0.";
                    }
                    if (correct_invalid_numbers) {
                        pqry_place->proximal_length = 0.0;
                    }
                }
                if (pqry_place->proximal_length > pqry_place->edge->branch_length) {
                    if (report_invalid_numbers) {
                        LOG_INFO << "Invalid placement with proximal_length > branch_length.";
                    }
                    if (correct_invalid_numbers) {
                        pqry_place->proximal_length = pqry_place->edge->branch_length;
                    }
                }
            }
        }

        // check name/named multiplicity validity
        if (pqry_obj->Has("n") && pqry_obj->Has("nm")) {
            LOG_WARN << "Jplace document contains a pquery with both an 'n' and an 'nm' key.";
            return false;
        }
        if (!pqry_obj->Has("n") && !pqry_obj->Has("nm")) {
            LOG_WARN << "Jplace document contains a pquery with neither an 'n' nor an 'nm' key.";
            return false;
        }

        // process names
        if (pqry_obj->Has("n")) {
            if (!pqry_obj->Get("n")->IsArray()) {
                LOG_WARN << "Jplace document contains a pquery with key 'n' that is not array.";
                return false;
            }

            JsonValueArray* pqry_n_arr = JsonValueToArray(pqry_obj->Get("n"));
            for (JsonValue* pqry_n_val : *pqry_n_arr) {
                if (!pqry_n_val->IsString()) {
                    LOG_WARN << "Jplace document contains a pquery where key 'n' has a "
                             << "non-string field.";
                    return false;
                }

                PqueryName* pqry_name   = new PqueryName();
                pqry_name->name         = pqry_n_val->ToString();
                pqry_name->multiplicity = 0.0;
                pqry_name->pquery = pqry;
                pqry->names.push_back(pqry_name);
            }
        }

        // process named multiplicities
        if (pqry_obj->Has("nm")) {
            if (!pqry_obj->Get("nm")->IsArray()) {
                LOG_WARN << "Jplace document contains a pquery with key 'nm' that is not array.";
                return false;
            }

            JsonValueArray* pqry_nm_arr = JsonValueToArray(pqry_obj->Get("nm"));
            for (JsonValue* pqry_nm_val : *pqry_nm_arr) {
                if (!pqry_nm_val->IsArray()) {
                    LOG_WARN << "Jplace document contains a pquery where key 'nm' has a "
                             << "non-array field.";
                    return false;
                }

                JsonValueArray * pqry_nm_val_arr = JsonValueToArray(pqry_nm_val);
                if (pqry_nm_val_arr->size() != 2) {
                    LOG_WARN << "Jplace document contains a pquery where key 'nm' has an array "
                             << "field with size != 2 (one for the name, one for the multiplicity).";
                    return false;
                }
                if (!pqry_nm_val_arr->at(0)->IsString()) {
                    LOG_WARN << "Jplace document contains a pquery where key 'nm' has an array "
                             << "whose first value is not a string for the name.";
                    return false;
                }
                if (!pqry_nm_val_arr->at(1)->IsNumber()) {
                    LOG_WARN << "Jplace document contains a pquery where key 'nm' has an array "
                             << "whose second value is not a number for the multiplicity.";
                    return false;
                }

                PqueryName* pqry_name   = new PqueryName();
                pqry_name->name         = pqry_nm_val_arr->at(0)->ToString();
                pqry_name->multiplicity = JsonValueToNumber(pqry_nm_val_arr->at(1))->value;
                if (pqry_name->multiplicity < 0.0) {
                    LOG_WARN << "Jplace document contains pquery with negative multiplicity at "
                             << "name '" << pqry_name->name << "'.";
                }
                pqry_name->pquery = pqry;
                pqry->names.push_back(pqry_name);
            }
        }

        // finally, add the pquery to the placements object
        placements.pqueries.push_back(pqry);
    }

    // check if there is metadata
    val = doc.Get("metadata");
    if (val && val->IsObject()) {
        JsonValueObject* meta_obj = JsonValueToObject(val);
        for (JsonValueObject::ObjectPair meta_pair : *meta_obj) {
            placements.metadata[meta_pair.first] = meta_pair.second->ToString();
        }
    }

    return true;
}

// =============================================================================
//     Printing
// =============================================================================

/**
 * @brief
 */
bool JplaceProcessor::to_file (const PlacementMap& placements, const std::string fn)
{
    if (FileExists(fn)) {
        LOG_WARN << "Jplace file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string ts;
    to_string(placements, ts);
    return FileWrite(fn, ts);
}

/**
 * @brief
 */
void JplaceProcessor::to_string (const PlacementMap& placements, std::string&  jplace)
{
    jplace = to_string(placements);
}

/**
 * @brief
 */
std::string JplaceProcessor::to_string (const PlacementMap& placements)
{
    JsonDocument json;
    to_document(placements, json);
    return JsonProcessor::ToString(json);
}

/**
 * @brief
 */
void JplaceProcessor::to_document (const PlacementMap& placements, JsonDocument& doc)
{
    doc.clear();

    // set tree
    NewickProcessor::print_names          = true;
    NewickProcessor::print_branch_lengths = true;
    NewickProcessor::print_comments       = false;
    NewickProcessor::print_tags           = true;
    doc.Set("tree", new JsonValueString(NewickProcessor::ToString(placements.tree)));

    // set placements
    JsonValueArray* placements_arr = new JsonValueArray();
    for (Pquery* pqry : placements.pqueries) {
        JsonValueObject* jpqry      = new JsonValueObject();
        placements_arr->push_back(jpqry);

        // set placements
        JsonValueArray* pqry_p_arr  = new JsonValueArray();
        for (PqueryPlacement* pqry_place : pqry->placements) {
            JsonValueArray* pqry_fields = new JsonValueArray();
            pqry_fields->push_back(new JsonValueNumber(pqry_place->edge_num));
            pqry_fields->push_back(new JsonValueNumber(pqry_place->likelihood));
            pqry_fields->push_back(new JsonValueNumber(pqry_place->like_weight_ratio));

            // convert from proximal to distal length.
            pqry_fields->push_back(new JsonValueNumber(
                pqry_place->edge->branch_length - pqry_place->proximal_length
            ));
            pqry_fields->push_back(new JsonValueNumber(pqry_place->pendant_length));
            pqry_p_arr->push_back(pqry_fields);
        }
        jpqry->Set("p", pqry_p_arr);

        // find out whether names have multiplicity
        bool has_nm = false;
        for (PqueryName* pqry_name : pqry->names) {
            has_nm |= pqry_name->multiplicity != 0.0;
        }

        // set named multiplicity / name
        if (has_nm) {
            JsonValueArray* pqry_nm_arr = new JsonValueArray();
            for (PqueryName* pqry_name : pqry->names) {
                JsonValueArray* pqry_nm_val = new JsonValueArray();
                pqry_nm_val->push_back(new JsonValueString(pqry_name->name));
                pqry_nm_val->push_back(new JsonValueNumber(pqry_name->multiplicity));
                pqry_nm_arr->push_back(pqry_nm_val);
            }
            jpqry->Set("nm", pqry_nm_arr);
        } else {
            JsonValueArray* pqry_n_arr  = new JsonValueArray();
            for (PqueryName* pqry_name : pqry->names) {
                pqry_n_arr->push_back(new JsonValueString(pqry_name->name));
            }
            jpqry->Set("n", pqry_n_arr);
        }
    }
    doc.Set("placements", placements_arr);

    // set fields
    JsonValueArray* jfields = new JsonValueArray();
    jfields->push_back(new JsonValueString("edge_num"));
    jfields->push_back(new JsonValueString("likelihood"));
    jfields->push_back(new JsonValueString("like_weight_ratio"));
    jfields->push_back(new JsonValueString("distal_length"));
    jfields->push_back(new JsonValueString("pendant_length"));
    doc.Set("fields", jfields);

    // set version
    doc.Set("version", new JsonValueNumber(3));

    // set metadata
    JsonValueObject* jmetadata = new JsonValueObject();
    jmetadata->Set("invocation", new JsonValueString(Options::GetCommandLineString()));
    doc.Set("metadata", jmetadata);
}

} // namespace genesis
