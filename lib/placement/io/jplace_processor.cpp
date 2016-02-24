/**
 * @brief Implementation of Jplace Processor functions.
 *
 * @file
 * @ingroup placement
 */

#include "placement/io/jplace_processor.hpp"

#include <memory>
#include <string>
#include <vector>

#include "placement/io/newick_processor.hpp"
#include "placement/operators.hpp"
#include "placement/placement_map_set.hpp"
#include "placement/placement_map.hpp"
#include "tree/io/newick/processor.hpp"
#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/options.hpp"
#include "utils/core/std.hpp"
#include "utils/io/json_document.hpp"
#include "utils/io/json_processor.hpp"

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

// =================================================================================================
//     Parsing
// =================================================================================================

/**
 * @brief Reads a list of files and parses them as a Jplace document into a PlacementMapSet object.
 *
 * Returns true iff successful.
 */
bool JplaceProcessor::from_files (const std::vector<std::string>& fns, PlacementMapSet& set)
{
    for (auto fn : fns) {
        auto map = std::make_shared<PlacementMap>();
        if (!from_file (fn, *map)) {
            return false;
        }
        std::string name = utils::file_filename( utils::file_basename(fn) );
        set.add(name, map);
    }
    return true;
}

/**
 * @brief Parses a list of strings as a Jplace document into a PlacementMapSet object.
 *
 * Returns true iff successful.
 */
bool JplaceProcessor::from_strings (const std::vector<std::string>& jps, PlacementMapSet& set)
{
    size_t cnt = 0;
    for (auto jplace : jps) {
        auto map = std::make_shared<PlacementMap>();
        if (!from_string (jplace, *map)) {
            return false;
        }
        set.add(std::string("jplace_") + std::to_string(cnt++), map);
    }
    return true;
}

/**
 * @brief Reads a file and parses it as a Jplace document into a PlacementMap object.
 *
 * Returns true iff successful.
 */
bool JplaceProcessor::from_file (const std::string& fn, PlacementMap& placements)
{
    if ( ! utils::file_exists(fn) ) {
        LOG_WARN << "Jplace file '" << fn << "' does not exist.";
        return false;
    }
    return from_string( utils::file_read(fn), placements );
}

/**
 * @brief Parses a string as a Jplace document into a PlacementMap object.
 *
 * Returns true iff successful.
 */
bool JplaceProcessor::from_string (const std::string& jplace, PlacementMap& placements)
{
    JsonDocument doc;
    if (!JsonProcessor().from_string(jplace, doc)) {
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
    JsonValue* val = doc.get("version");
    if (!val) {
        LOG_WARN << "Jplace document does not contain a valid version number at key 'version'. "
                 << "Now continuing to parse in the hope that it still works.";
    }
    if (!check_version(val->to_string())) {
        LOG_WARN << "Jplace document has version '" << val->to_string() << "', however this parser "
                 << "is written for version " << get_version() << " of the Jplace format. "
                 << "Now continuing to parse in the hope that it still works.";
    }

    // find and process the reference tree
    val = doc.get("tree");
    if (!val || !val->is_string() ||
        !PlacementTreeNewickProcessor().from_string(val->to_string(), placements.tree())
    ) {
        LOG_WARN << "Jplace document does not contain a valid Newick tree at key 'tree'.";
        return false;
    }
    if (!has_correct_edge_nums(placements)) {
        LOG_WARN << "Jplace document has a Newick tree where the edge_num tags are non standard. "
                 << "They are expected to be assigned in ascending order via postorder traversal. "
                 << "Now continuing to parse, as we can cope with this.";
    }

    // create a map from edge nums to the actual edge pointers, for later use when processing
    // the pqueries. we do not use PlacementMap::EdgeNumMap() here, because we need to do extra
    // checking for validity first!
    std::unordered_map<int, PlacementTree::EdgeType*> edge_num_map;
    for (
        PlacementTree::ConstIteratorEdges it = placements.tree().begin_edges();
        it != placements.tree().end_edges();
        ++it
    ) {
        PlacementTree::EdgeType* edge = it->get();
        if (edge_num_map.count(edge->data.edge_num) > 0) {
            LOG_WARN << "Jplace document contains a tree where the edge_num tag '"
                     << edge->data.edge_num << "' is used more than once.";
            return false;
        }
        edge_num_map.emplace(edge->data.edge_num, edge);
    }

    // get the field names and store them in array fields
    val = doc.get("fields");
    if (!val || !val->is_array()) {
        LOG_WARN << "Jplace document does not contain field names at key 'fields'.";
        return false;
    }
    JsonValueArray* fields_arr = json_value_to_array(val);
    std::vector<std::string> fields;
    bool has_edge_num = false;
    for (JsonValue* fields_val : *fields_arr) {
        if (!fields_val->is_string()) {
            LOG_WARN << "Jplace document contains a value of type '" << fields_val->type_to_string()
                     << "' instead of a string with a field name at key 'fields'.";
            return false;
        }

        // check field validity
        std::string field = fields_val->to_string();
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

    // Find and process the pqueries.
    val = doc.get("placements");
    if (!val || !val->is_array()) {
        LOG_WARN << "Jplace document does not contain pqueries at key 'placements'.";
        return false;
    }
    JsonValueArray* placements_arr = json_value_to_array(val);
    for (JsonValue* pqry_val : *placements_arr) {
        if (!pqry_val->is_object()) {
            LOG_WARN << "Jplace document contains a value of type '" << pqry_val->type_to_string()
                     << "' instead of an object with a pquery at key 'placements'.";
            return false;
        }
        JsonValueObject* pqry_obj = json_value_to_object(pqry_val);
        if (!pqry_obj->has("p") || !pqry_obj->get("p")->is_array()) {
            LOG_WARN << "Jplace document contains a pquery at key 'placements' that does not "
                     << "contain an array of placements at sub-key 'p'.";
            return false;
        }

        // Create new pquery.
        auto pqry = make_unique<Pquery>();

        // Process the placements and store them in the pquery.
        JsonValueArray* pqry_p_arr = json_value_to_array(pqry_obj->get("p"));
        for (JsonValue* pqry_p_val : *pqry_p_arr) {
            if (!pqry_p_val->is_array()) {
                LOG_WARN << "Jplace document contains a pquery with invalid placement at key 'p'.";
                return false;
            }
            JsonValueArray* pqry_fields = json_value_to_array(pqry_p_val);
            if (pqry_fields->size() != fields.size()) {
                LOG_WARN << "Jplace document contains a placement fields array with different size "
                         << "than the fields name array.";
                return false;
            }

            // Process all fields of the placement.
            auto pqry_place = make_unique<PqueryPlacement>();
            double distal_length = -1.0;
            for (size_t i = 0; i < pqry_fields->size(); ++i) {
                // Up to version 3 of the jplace specification, the p-fields in a jplace document
                // only contain numbers (float or int), so we can do this check here once for all
                // fields, instead of repetition for every field. If in the future there are fields
                // with non-number type, this check has to go into the single field assignments.
                if (!pqry_fields->at(i)->is_number()) {
                    LOG_WARN << "Jplace document contains pquery where field " << fields[i]
                             << " is of type '" << pqry_fields->at(i)->type_to_string()
                             << "' instead of a number.";
                    return false;
                }

                // Switch on the field name to set the correct value.
                double pqry_place_val = json_value_to_number(pqry_fields->at(i))->value;
                if (fields[i] == "edge_num") {
                    if (edge_num_map.count(pqry_place_val) == 0) {
                        LOG_WARN << "Jplace document contains a pquery where field 'edge_num' "
                                 << "has value '" << pqry_place_val << "', which is not marked "
                                 << "in the given tree as an edge_num.";
                        return false;
                    }
                    pqry_place->edge_num = pqry_place_val;
                    pqry_place->edge = edge_num_map.at(pqry_place_val);
                    pqry_place->edge->data.placements.push_back(pqry_place.get());

                } else if (fields[i] == "likelihood") {
                    pqry_place->likelihood = pqry_place_val;

                } else if (fields[i] == "like_weight_ratio") {
                    pqry_place->like_weight_ratio = pqry_place_val;

                } else if (fields[i] == "distal_length") {
                    distal_length = pqry_place_val;

                } else if (fields[i] == "proximal_length") {
                    pqry_place->proximal_length = pqry_place_val;

                } else if (fields[i] == "pendant_length") {
                    pqry_place->pendant_length = pqry_place_val;

                } else if (fields[i] == "parsimony") {
                    pqry_place->parsimony = pqry_place_val;
                }
            }

            // The jplace format uses distal length, but we use proximal, so we need to convert here.
            // We have to do this here (unlike all the other values, which are set in the loop
            // above), because it may happen that the edge_num field was not yet set while
            // processing. Also, we only set it if it was actually available in the fields and not
            // overwritten by the (more appropriate) field for the proximal length.
            if (distal_length >= 0.0 && pqry_place->proximal_length == 0.0) {
                pqry_place->proximal_length = pqry_place->edge->data.branch_length - distal_length;
            }

            // Check validity of placement values.
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
                if (pqry_place->proximal_length > pqry_place->edge->data.branch_length) {
                    if (report_invalid_numbers) {
                        LOG_INFO << "Invalid placement with proximal_length > branch_length.";
                    }
                    if (correct_invalid_numbers) {
                        pqry_place->proximal_length = pqry_place->edge->data.branch_length;
                    }
                }
            }

            // Add the placement to the query and vice versa.
            pqry_place->pquery = pqry.get();
            pqry->placements.push_back(std::move(pqry_place));
        }

        // Check name/named multiplicity validity.
        if (pqry_obj->has("n") && pqry_obj->has("nm")) {
            LOG_WARN << "Jplace document contains a pquery with both an 'n' and an 'nm' key.";
            return false;
        }
        if (!pqry_obj->has("n") && !pqry_obj->has("nm")) {
            LOG_WARN << "Jplace document contains a pquery with neither an 'n' nor an 'nm' key.";
            return false;
        }

        // Process names.
        if (pqry_obj->has("n")) {
            if (!pqry_obj->get("n")->is_array()) {
                LOG_WARN << "Jplace document contains a pquery with key 'n' that is not array.";
                return false;
            }

            JsonValueArray* pqry_n_arr = json_value_to_array(pqry_obj->get("n"));
            for (JsonValue* pqry_n_val : *pqry_n_arr) {
                if (!pqry_n_val->is_string()) {
                    LOG_WARN << "Jplace document contains a pquery where key 'n' has a "
                             << "non-string field.";
                    return false;
                }

                auto pqry_name = make_unique<PqueryName>();
                pqry_name->name         = pqry_n_val->to_string();
                pqry_name->multiplicity = 0.0;
                pqry_name->pquery = pqry.get();
                pqry->names.push_back(std::move(pqry_name));
            }
        }

        // Process named multiplicities.
        if (pqry_obj->has("nm")) {
            if (!pqry_obj->get("nm")->is_array()) {
                LOG_WARN << "Jplace document contains a pquery with key 'nm' that is not array.";
                return false;
            }

            JsonValueArray* pqry_nm_arr = json_value_to_array(pqry_obj->get("nm"));
            for (JsonValue* pqry_nm_val : *pqry_nm_arr) {
                if (!pqry_nm_val->is_array()) {
                    LOG_WARN << "Jplace document contains a pquery where key 'nm' has a "
                             << "non-array field.";
                    return false;
                }

                JsonValueArray * pqry_nm_val_arr = json_value_to_array(pqry_nm_val);
                if (pqry_nm_val_arr->size() != 2) {
                    LOG_WARN << "Jplace document contains a pquery where key 'nm' has an array "
                             << "field with size != 2 (one for the name, one for the multiplicity).";
                    return false;
                }
                if (!pqry_nm_val_arr->at(0)->is_string()) {
                    LOG_WARN << "Jplace document contains a pquery where key 'nm' has an array "
                             << "whose first value is not a string for the name.";
                    return false;
                }
                if (!pqry_nm_val_arr->at(1)->is_number()) {
                    LOG_WARN << "Jplace document contains a pquery where key 'nm' has an array "
                             << "whose second value is not a number for the multiplicity.";
                    return false;
                }

                auto pqry_name = make_unique<PqueryName>();
                pqry_name->name         = pqry_nm_val_arr->at(0)->to_string();
                pqry_name->multiplicity = json_value_to_number(pqry_nm_val_arr->at(1))->value;
                if (pqry_name->multiplicity < 0.0) {
                    LOG_WARN << "Jplace document contains pquery with negative multiplicity at "
                             << "name '" << pqry_name->name << "'.";
                }
                pqry_name->pquery = pqry.get();
                pqry->names.push_back(std::move(pqry_name));
            }
        }

        // Finally, add the pquery to the placements object.
        placements.pqueries().push_back(std::move(pqry));
    }

    // Check if there is metadata.
    val = doc.get("metadata");
    if (val && val->is_object()) {
        JsonValueObject* meta_obj = json_value_to_object(val);
        for (JsonValueObject::ObjectPair meta_pair : *meta_obj) {
            placements.metadata[meta_pair.first] = meta_pair.second->to_string();
        }
    }

    return true;
}

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief
 */
bool JplaceProcessor::to_file (const PlacementMap& placements, const std::string fn)
{
    if( utils::file_exists(fn) ) {
        LOG_WARN << "Jplace file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string ts;
    to_string(placements, ts);
    return utils::file_write( fn, ts );
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
    return JsonProcessor().to_string(json);
}

/**
 * @brief
 */
void JplaceProcessor::to_document (const PlacementMap& placements, JsonDocument& doc)
{
    doc.clear();

    // set tree
    auto nwp = PlacementTreeNewickProcessor();
    nwp.enable_names(true);
    nwp.enable_branch_lengths(true);
    doc.set("tree", new JsonValueString(nwp.to_string(placements.tree())));

    // set placements
    JsonValueArray* placements_arr = new JsonValueArray();
    for (auto& pqry : placements.pqueries()) {
        JsonValueObject* jpqry      = new JsonValueObject();
        placements_arr->push_back(jpqry);

        // set placements
        JsonValueArray* pqry_p_arr  = new JsonValueArray();
        for (auto& pqry_place : pqry->placements) {
            JsonValueArray* pqry_fields = new JsonValueArray();
            pqry_fields->push_back(new JsonValueNumber(pqry_place->edge_num));
            pqry_fields->push_back(new JsonValueNumber(pqry_place->likelihood));
            pqry_fields->push_back(new JsonValueNumber(pqry_place->like_weight_ratio));

            // convert from proximal to distal length.
            pqry_fields->push_back(new JsonValueNumber(
                pqry_place->edge->data.branch_length - pqry_place->proximal_length
            ));
            pqry_fields->push_back(new JsonValueNumber(pqry_place->pendant_length));
            pqry_p_arr->push_back(pqry_fields);
        }
        jpqry->set("p", pqry_p_arr);

        // find out whether names have multiplicity
        bool has_nm = false;
        for (auto& pqry_name : pqry->names) {
            has_nm |= pqry_name->multiplicity != 0.0;
        }

        // set named multiplicity / name
        if (has_nm) {
            JsonValueArray* pqry_nm_arr = new JsonValueArray();
            for (auto& pqry_name : pqry->names) {
                JsonValueArray* pqry_nm_val = new JsonValueArray();
                pqry_nm_val->push_back(new JsonValueString(pqry_name->name));
                pqry_nm_val->push_back(new JsonValueNumber(pqry_name->multiplicity));
                pqry_nm_arr->push_back(pqry_nm_val);
            }
            jpqry->set("nm", pqry_nm_arr);
        } else {
            JsonValueArray* pqry_n_arr  = new JsonValueArray();
            for (auto& pqry_name : pqry->names) {
                pqry_n_arr->push_back(new JsonValueString(pqry_name->name));
            }
            jpqry->set("n", pqry_n_arr);
        }
    }
    doc.set("placements", placements_arr);

    // set fields
    JsonValueArray* jfields = new JsonValueArray();
    jfields->push_back(new JsonValueString("edge_num"));
    jfields->push_back(new JsonValueString("likelihood"));
    jfields->push_back(new JsonValueString("like_weight_ratio"));
    jfields->push_back(new JsonValueString("distal_length"));
    jfields->push_back(new JsonValueString("pendant_length"));
    doc.set("fields", jfields);

    // set version
    doc.set("version", new JsonValueNumber(3));

    // set metadata
    JsonValueObject* jmetadata = new JsonValueObject();
    jmetadata->set("invocation", new JsonValueString(Options::get().command_line_string()));
    doc.set("metadata", jmetadata);
}

} // namespace genesis
