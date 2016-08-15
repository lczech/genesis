/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/formats/jplace_reader.hpp"

#include "placement/function/helper.hpp"
#include "placement/formats/newick_reader.hpp"
#include "placement/sample_set.hpp"
#include "placement/sample.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/options.hpp"
#include "utils/core/std.hpp"
#include "utils/formats/json/document.hpp"
#include "utils/formats/json/reader.hpp"
#include "utils/io/input_stream.hpp"
#include "utils/io/parser.hpp"
#include "utils/io/scanner.hpp"
#include "utils/text/string.hpp"

#include <assert.h>
#include <cctype>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Version
// =================================================================================================

/**
 * @brief Returns the version number that this class is written for. Currently, this is "3".
 */
std::string JplaceReader::version ()
{
    return "3";
}

/**
 * @brief Checks whether the version of the jplace format works with this parser.
 *
 * This parser is intended for `jplace` versions 2 and 3. If while reading a different version tag
 * is found, the reader will trigger a warning and try to continue anyway.
 */
bool JplaceReader::check_version ( std::string const& version )
{
    auto v = utils::trim( version );
    return v == "2" || v == "3";
}

/**
 * @brief Checks whether the version of the jplace format works with this parser.
 *
 * This parser is intended for `jplace` versions 2 and 3. If while reading a different version tag
 * is found, the reader will trigger a warning and try to continue anyway.
 */
bool JplaceReader::check_version ( size_t version )
{
    return version == 2 || version == 3;
}

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Read `jplace` data from a stream into a Sample.
 *
 * This implementation is currenlty not yet fully implemented. Don't use it yet!
 */
void JplaceReader::from_stream ( std::istream& is, Sample& smp ) const
{
    using namespace utils;
    InputStream it( utils::make_unique< StreamInputSource >( is ));

    // We need to check whether all important keys of the document appeared.
    bool found_version    = false;
    bool found_tree       = false;
    bool found_fields     = false;
    bool found_placements = false;

    // Check for data.
    if( ! it ) {
        throw std::runtime_error(
            "Malformed Jplace file: Expecting begin of Json structure at " + it.at() + "."
        );
    }

    // Find beginning of json document.
    skip_while( it, isspace );
    read_char_if( it, '{' );

    // Read all elemetns of the document.
    do {

        // Get key of the json element.
        skip_while( it, isspace );
        expect_char( it, '"' );
        std::string key = parse_quoted_string( it );
        skip_while( it, isspace );
        read_char_if( it, ':' );
        skip_while( it, isspace );

        if( key == "version" ) {
            /* code */
        }
        if( key == "metadata" ) {
            /* code */
        }
        if( key == "tree" ) {
            /* code */
        }
        if( key == "fields" ) {
            /* code */
        }
        if( key == "placements" ) {
            /* code */
        }

    } while( it && *it == ',' );


    read_char_if( it, ',' );

    (void) smp;

    // Check whether all needed keys were found.
    if( ! found_version ) {
        throw std::runtime_error( "Json document does not provide mandatory key 'version'." );
    }
    if( ! found_tree ) {
        throw std::runtime_error( "Json document does not provide mandatory key 'tree'." );
    }
    if( ! found_fields ) {
        throw std::runtime_error( "Json document does not provide mandatory key 'fields'." );
    }
    if( ! found_placements ) {
        throw std::runtime_error( "Json document does not provide mandatory key 'placements'." );
    }

    // TODO
    throw std::domain_error( "Not yet fully implemented." );
}

/**
 * @brief Read a file and parse it as a Jplace document into a Sample object.
 */
void JplaceReader::from_file( std::string const& fn, Sample& smp ) const
{
    if ( ! utils::file_exists(fn) ) {
        throw std::runtime_error( "Jplace file '" + fn + "' does not exist." );
    }
    return from_string( utils::file_read(fn), smp );
}

/**
 * @brief Parse a string as a Jplace document into a Sample object.
 */
void JplaceReader::from_string( std::string const& jplace, Sample& smp ) const
{
    utils::JsonDocument doc;
    utils::JsonReader().from_string( jplace, doc );
    return from_document(doc, smp);
}

/**
 * @brief Take a JsonDocument object and parse it as a Jplace document into a Sample object.
 */
void JplaceReader::from_document( utils::JsonDocument const& doc, Sample& smp ) const
{
    process_json_version( doc );
    process_json_metadata( doc, smp );

    smp.clear();
    process_json_tree( doc, smp );
    auto fields = process_json_fields( doc );
    process_json_placements( doc, smp, fields);
}

/**
 * @brief Read a list of files and parse them as a Jplace document into a SampleSet object.
 */
void JplaceReader::from_files (const std::vector<std::string>& fns, SampleSet& set) const
{
    for (auto fn : fns) {
        auto map = Sample();
        from_file (fn, map);
        std::string name = utils::file_filename( utils::file_basename(fn) );
        set.add( map, name );
    }
}

/**
 * @brief Parse a list of strings as a Jplace document into a SampleSet object.
 */
void JplaceReader::from_strings (const std::vector<std::string>& jps, SampleSet& set) const
{
    size_t cnt = 0;
    for (auto jplace : jps) {
        auto map = Sample();
        from_string (jplace, map);
        set.add( map, std::string("jplace_") + std::to_string(cnt) );
        ++cnt;
    }
}

// =================================================================================================
//     Parsing
// =================================================================================================

void JplaceReader::parse_version( utils::InputStream& input_stream ) const
{
    auto ver = utils::parse_unsigned_integer<size_t>( input_stream );
    if( ! check_version( ver ) ) {
        LOG_WARN << "Jplace document has version " << utils::to_string( ver ) << ", however "
                 << "this parser is written for version " << version() <<  " of the Jplace format. "
                 << "Now continuing to parse in the hope that it still works.";
    }
}

std::unordered_map<std::string, std::string> JplaceReader::parse_metadata(
    utils::InputStream& input_stream
) const {
    using namespace utils;
    auto& it = input_stream;

    auto res = std::unordered_map<std::string, std::string>();

    // Go into the metadata field.
    skip_while( it, isspace );
    read_char_if( it, '{' );
    skip_while( it, isspace );

    // Read metadata while there is.
    bool get_more = it && *it == '\"';
    while( get_more ) {
        get_more = false;

        // Get key of the field.
        expect_char( it, '"' );
        std::string key = parse_quoted_string( it );
        skip_while( it, isspace );
        read_char_if( it, ':' );

        // Get the value of the field.
        skip_while( it, isspace );
        expect_char( it, '"' );
        std::string value = parse_quoted_string( it );

        // Store and go to next element.
        res[ key ] = value;
        skip_while( it, isspace );
        if( it && *it == ',' ) {
            ++it;
            get_more = true;
        }
        skip_while( it, isspace );
    }

    // Expected end of metatdata field.
    read_char_if( it, '}' );
    return res;
}

std::vector<std::string> JplaceReader::parse_fields( utils::InputStream& input_stream ) const
{
    using namespace utils;
    auto& it = input_stream;

    auto fields = std::vector<std::string>();

    // Go into the fields field.
    skip_while( it, isspace );
    read_char_if( it, '[' );
    skip_while( it, isspace );

    // Read fields while there are.
    bool get_more = it && *it == '\"';
    while( get_more ) {
        get_more = false;

        // Get key of the field.
        expect_char( it, '"' );
        std::string field = parse_quoted_string( it );
        fields.push_back( field );

        // Go to next element.
        skip_while( it, isspace );
        if( it && *it == ',' ) {
            ++it;
            get_more = true;
        }
        skip_while( it, isspace );
    }
    read_char_if( it, ']' );

    // Check field validity.
    bool has_edge_num        = false;
    bool has_distal_length   = false;
    bool has_proximal_length = false;
    for( size_t i = 0; i < fields.size(); ++i ) {
        auto const& field = fields[i];
        if (field == "edge_num"      || field == "likelihood"     || field == "like_weight_ratio" ||
            field == "distal_length" || field == "pendant_length" || field == "proximal_length"   ||
            field == "parsimony"
        ) {

            // Simple check for fields that appear multiple times. It's certainly not a nice algo,
            // but the vector is small, so this suffices.
            for( size_t j = i + 1; j < fields.size(); ++j ) {
                if( fields[i] == fields[j] ) {
                    throw std::runtime_error(
                        "Jplace document contains field name '" + fields[i]
                        + "' more than once at key 'fields'."
                    );
                }
            }

        } else {

            LOG_WARN << "Jplace document contains a field name '" << field << "' "
                     << "at key 'fields', which is not used by this parser and thus ignored.";
        }

        has_edge_num        |= (field == "edge_num");
        has_distal_length   |= (field == "distal_length");
        has_proximal_length |= (field == "proximal_length");
    }
    if( ! has_edge_num ) {
        throw std::runtime_error(
            "Jplace document does not contain necessary field 'edge_num' at key 'fields'."
        );
    }
    if( has_distal_length && has_proximal_length ) {
        LOG_WARN << "Jplace document contains both fields 'distal_length', and 'proximal_length'. "
                 << "Currently, only one value is used internally to represent both, which might "
                 << "lead to inconsistency if the sum of both is not equal to the branch length.";
    }

    return fields;
}

void JplaceReader::parse_tree( utils::InputStream& input_stream ) const
{
    using namespace utils;
    auto& it = input_stream;

    auto tree_string = parse_quoted_string( it );
}

// =================================================================================================
//     Processing
// =================================================================================================

/**
 * @brief Internal helper function that checks whether the `version` key in a JsonDocument
 * corresponds to a valid version number for the JplaceReader.
 */
void JplaceReader::process_json_version( utils::JsonDocument const& doc ) const
{
    // check if the version is correct
    utils::JsonValue* val = doc.get("version");
    if (!val) {
        LOG_WARN << "Jplace document does not contain a valid version number at key 'version'. "
                 << "Now continuing to parse in the hope that it still works.";
    }
    if (!check_version(val->to_string())) {
        LOG_WARN << "Jplace document has version '" << val->to_string() << "', however this parser "
                 << "is written for version " << version() << " of the Jplace format. "
                 << "Now continuing to parse in the hope that it still works.";
    }
}

/**
 * @brief Internal helper function that processes the `metadata` key of a JsonDocument and stores
 * its value in the Sample metadata member.
 */
void JplaceReader::process_json_metadata( utils::JsonDocument const& doc, Sample& smp ) const
{
    // Check if there is metadata.
    auto val = doc.get("metadata");
    if (val && val->is_object()) {
        utils::JsonValueObject* meta_obj = json_value_to_object(val);
        for( utils::JsonValueObject::ObjectPair meta_pair : *meta_obj ) {
            smp.metadata[meta_pair.first] = meta_pair.second->to_string();
        }
    }
}

/**
 * @brief Internal helper function that processes the `tree` key of a JsonDocument and stores it as
 * the Tree of a Sample.
 */
void JplaceReader::process_json_tree( utils::JsonDocument const& doc, Sample& smp ) const
{
    // find and process the reference tree
    auto val = doc.get("tree");
    if (!val || !val->is_string() ||
        !PlacementTreeNewickReader().from_string(val->to_string(), smp.tree())
    ) {
        throw std::runtime_error(
            "Jplace document does not contain a valid Newick tree at key 'tree'."
        );
    }
    if( ! has_correct_edge_nums( smp.tree() )) {
        LOG_WARN << "Jplace document has a Newick tree where the edge_num tags are non standard. "
                 << "They are expected to be assigned in ascending order via postorder traversal. "
                 << "Now continuing to parse, as we can cope with this.";
    }
}

/**
 * @brief Internal helper function that processes the `fields` key of a JsonDocument and returns
 * its values.
 */
std::vector<std::string> JplaceReader::process_json_fields( utils::JsonDocument const& doc ) const
{
    // get the field names and store them in array fields
    auto val = doc.get("fields");
    if (!val || !val->is_array()) {
        throw std::runtime_error( "Jplace document does not contain field names at key 'fields'." );
    }
    utils::JsonValueArray* fields_arr = json_value_to_array(val);
    std::vector<std::string> fields;
    bool has_edge_num = false;
    for( utils::JsonValue* fields_val : *fields_arr ) {
        if (!fields_val->is_string()) {
            throw std::runtime_error(
                "Jplace document contains a value of type '" + fields_val->type_to_string()
                + "' instead of a string with a field name at key 'fields'."
            );
        }

        // check field validity
        std::string field = fields_val->to_string();
        if (field == "edge_num"      || field == "likelihood"     || field == "like_weight_ratio" ||
            field == "distal_length" || field == "pendant_length" || field == "proximal_length"   ||
            field == "parsimony"
        ) {
            for (std::string fn : fields) {
                if (fn == field) {
                    throw std::runtime_error(
                        "Jplace document contains field name '" + field
                        + "' more than once at key 'fields'."
                    );
                }
            }
        } else {
            LOG_WARN << "Jplace document contains a field name '" << field << "' "
                     << "at key 'fields', which is not used by this parser and thus ignored.";
        }
        fields.push_back(field);
        has_edge_num |= (field == "edge_num");
    }
    if (!has_edge_num) {
        throw std::runtime_error(
            "Jplace document does not contain necessary field 'edge_num' at key 'fields'."
        );
    }
    if (
        std::end(fields) != std::find(std::begin(fields), std::end(fields), "distal_length") &&
        std::end(fields) != std::find(std::begin(fields), std::end(fields), "proximal_length")
    ) {
        LOG_WARN << "Jplace document contains both fields 'distal_length', and 'proximal_length'. "
                 << "Currently, only one value is used internally to represent both, which might "
                 << "lead to inconsistency if the sum of both is not equal to the branch length.";
    }

    return fields;
}

/**
 * @brief Internal helper function that processes the `placements` key of a JsonDocument and stores
 * the contained pqueries in the Sample.
 */
void JplaceReader::process_json_placements(
    utils::JsonDocument const& doc,
    Sample&              smp,
    std::vector<std::string>   fields
) const {
    // create a map from edge nums to the actual edge pointers, for later use when processing
    // the pqueries. we do not use Sample::EdgeNumMap() here, because we need to do extra
    // checking for validity first!
    std::unordered_map<int, PlacementTreeEdge*> edge_num_map;
    for (
        PlacementTree::ConstIteratorEdges it = smp.tree().begin_edges();
        it != smp.tree().end_edges();
        ++it
    ) {
        auto& edge = *it;
        auto& edge_data = edge->data<PlacementEdgeData>();
        if (edge_num_map.count( edge_data.edge_num()) > 0) {
            throw std::runtime_error(
                "Jplace document contains a tree where the edge_num tag '"
                + std::to_string( edge_data.edge_num() ) + "' is used more than once."
            );
        }
        edge_num_map.emplace( edge_data.edge_num(), edge.get() );
    }

    // Find and process the pqueries.
    auto val = doc.get("placements");
    if (!val || !val->is_array()) {
        throw std::runtime_error(
            "Jplace document does not contain pqueries at key 'placements'."
        );
    }
    utils::JsonValueArray* placements_arr = json_value_to_array(val);
    for( utils::JsonValue* pqry_val : *placements_arr ) {
        if (!pqry_val->is_object()) {
            throw std::runtime_error(
                "Jplace document contains a value of type '" + pqry_val->type_to_string()
                + "' instead of an object with a pquery at key 'placements'."
            );
        }
        utils::JsonValueObject* pqry_obj = json_value_to_object(pqry_val);
        if (!pqry_obj->has("p") || !pqry_obj->get("p")->is_array()) {
            throw std::runtime_error(
                "Jplace document contains a pquery at key 'placements' that does not contain an "
                + std::string( "array of placements at sub-key 'p'." )
            );
        }

        // Create new pquery.
        auto pqry = Pquery();

        // Process the placements and store them in the pquery.
        utils::JsonValueArray* pqry_p_arr = json_value_to_array(pqry_obj->get("p"));
        for( utils::JsonValue* pqry_p_val : *pqry_p_arr ) {
            if (!pqry_p_val->is_array()) {
                throw std::runtime_error(
                    "Jplace document contains a pquery with invalid placement at key 'p'."
                );
            }
            utils::JsonValueArray* pqry_fields = json_value_to_array(pqry_p_val);
            if (pqry_fields->size() != fields.size()) {
                throw std::runtime_error(
                    "Jplace document contains a placement fields array with different size "
                    + std::string( "than the fields name array." )
                );
            }

            // Process all fields of the placement.
            auto pqry_place = PqueryPlacement();
            double distal_length = -1.0;
            for (size_t i = 0; i < pqry_fields->size(); ++i) {
                // Up to version 3 of the jplace specification, the p-fields in a jplace document
                // only contain numbers (float or int), so we can do this check here once for all
                // fields, instead of repetition for every field. If in the future there are fields
                // with non-number type, this check has to go into the single field assignments.
                if (!pqry_fields->at(i)->is_number()) {
                    throw std::runtime_error(
                        "Jplace document contains pquery where field " + fields[i]
                        + " is of type '" + pqry_fields->at(i)->type_to_string()
                        + "' instead of a number."
                    );
                }

                // Switch on the field name to set the correct value.
                double pqry_place_val = json_value_to_number(pqry_fields->at(i))->value;
                if (fields[i] == "edge_num") {
                    int val_int = static_cast<int>( pqry_place_val );

                    if (edge_num_map.count( val_int ) == 0) {
                        throw std::runtime_error(
                            "Jplace document contains a pquery where field 'edge_num' has value '"
                            + std::to_string( val_int ) + "', which is not marked in the "
                            + "given tree as an edge_num."
                        );
                    }
                    pqry_place.reset_edge( *edge_num_map.at( val_int ) );

                } else if (fields[i] == "likelihood") {
                    pqry_place.likelihood = pqry_place_val;

                } else if (fields[i] == "like_weight_ratio") {
                    pqry_place.like_weight_ratio = pqry_place_val;

                } else if (fields[i] == "distal_length") {
                    distal_length = pqry_place_val;

                } else if (fields[i] == "proximal_length") {
                    pqry_place.proximal_length = pqry_place_val;

                } else if (fields[i] == "pendant_length") {
                    pqry_place.pendant_length = pqry_place_val;

                } else if (fields[i] == "parsimony") {
                    pqry_place.parsimony = pqry_place_val;
                }
            }

            // The jplace format uses distal length, but we use proximal, so we need to convert here.
            // We have to do this here (unlike all the other values, which are set in the loop
            // above), because it may happen that the edge_num field was not yet set while
            // processing. Also, we only set it if it was actually available in the fields and not
            // overwritten by the (more appropriate) field for the proximal length.
            if (distal_length >= 0.0 && pqry_place.proximal_length == 0.0) {
                auto const& edge_data = pqry_place.edge().data<PlacementEdgeData>();
                pqry_place.proximal_length = edge_data.branch_length - distal_length;
            }

            auto invalid_number_checker = [this] (
                double&                              actual,
                std::function<bool (double, double)> comparator,
                double                               expected,
                std::string                          error_message
            ) {
                if(
                    comparator( actual, expected ) && (
                    invalid_number_behaviour() == InvalidNumberBehaviour::kLog ||
                    invalid_number_behaviour() == InvalidNumberBehaviour::kLogAndCorrect
                )) {
                    LOG_WARN << error_message;
                }

                if(
                    comparator( actual, expected ) && (
                    invalid_number_behaviour() == InvalidNumberBehaviour::kCorrect ||
                    invalid_number_behaviour() == InvalidNumberBehaviour::kLogAndCorrect
                )) {
                    actual = expected;
                }

                if(
                    comparator( actual, expected ) && (
                    invalid_number_behaviour() == InvalidNumberBehaviour::kThrow
                )) {
                    throw std::runtime_error( error_message );
                }
            };

            // Check validity of placement values.
            invalid_number_checker(
                pqry_place.like_weight_ratio,
                std::less<double>(),
                0.0,
                "Invalid placement with like_weight_ratio < 0.0."
            );
            invalid_number_checker(
                pqry_place.like_weight_ratio,
                std::greater<double>(),
                1.0,
                "Invalid placement with like_weight_ratio > 1.0."
            );
            invalid_number_checker(
                pqry_place.pendant_length,
                std::less<double>(),
                0.0,
                "Invalid placement with pendant_length < 0.0."
            );
            invalid_number_checker(
                pqry_place.proximal_length,
                std::less<double>(),
                0.0,
                "Invalid placement with proximal_length < 0.0."
            );
            invalid_number_checker(
                pqry_place.proximal_length,
                std::greater<double>(),
                pqry_place.edge().data<PlacementEdgeData>().branch_length,
                "Invalid placement with proximal_length > branch_length."
            );

            // Add the placement to the query and vice versa.
            pqry.add_placement( pqry_place );
        }

        // Check name/named multiplicity validity.
        if (pqry_obj->has("n") && pqry_obj->has("nm")) {
            throw std::runtime_error(
                "Jplace document contains a pquery with both an 'n' and an 'nm' key."
            );
        }
        if (!pqry_obj->has("n") && !pqry_obj->has("nm")) {
            throw std::runtime_error(
                "Jplace document contains a pquery with neither an 'n' nor an 'nm' key."
            );
        }

        // Process names.
        if (pqry_obj->has("n")) {
            if (!pqry_obj->get("n")->is_array()) {
                throw std::runtime_error(
                    "Jplace document contains a pquery with key 'n' that is not array."
                );
            }

            utils::JsonValueArray* pqry_n_arr = json_value_to_array(pqry_obj->get("n"));
            for( utils::JsonValue* pqry_n_val : *pqry_n_arr ) {
                if (!pqry_n_val->is_string()) {
                    throw std::runtime_error(
                        "Jplace document contains a pquery where key 'n' has a non-string field."
                    );
                }

                // Add the name with a default multiplicity.
                pqry.add_name( pqry_n_val->to_string() );
            }
        }

        // Process named multiplicities.
        if (pqry_obj->has("nm")) {
            if (!pqry_obj->get("nm")->is_array()) {
                throw std::runtime_error(
                    "Jplace document contains a pquery with key 'nm' that is not array."
                );
            }

            utils::JsonValueArray* pqry_nm_arr = json_value_to_array(pqry_obj->get("nm"));
            for( utils::JsonValue* pqry_nm_val : *pqry_nm_arr ) {
                if (!pqry_nm_val->is_array()) {
                    throw std::runtime_error(
                        "Jplace document contains a pquery where key 'nm' has a non-array field."
                    );
                }

                utils::JsonValueArray * pqry_nm_val_arr = json_value_to_array(pqry_nm_val);
                if (pqry_nm_val_arr->size() != 2) {
                    throw std::runtime_error(
                        std::string( "Jplace document contains a pquery where key 'nm' has an " )
                        + "array field with size != 2 (one for the name, one for the multiplicity)."
                    );
                }
                if (!pqry_nm_val_arr->at(0)->is_string()) {
                    throw std::runtime_error(
                        std::string( "Jplace document contains a pquery where key 'nm' has an " )
                        + "array whose first value is not a string for the name."
                    );
                }
                if (!pqry_nm_val_arr->at(1)->is_number()) {
                    throw std::runtime_error(
                        std::string( "Jplace document contains a pquery where key 'nm' has an " )
                        + "array whose second value is not a number for the multiplicity."
                    );
                }

                auto pqry_name = PqueryName();
                pqry_name.name         = pqry_nm_val_arr->at(0)->to_string();
                pqry_name.multiplicity = json_value_to_number(pqry_nm_val_arr->at(1))->value;
                if (pqry_name.multiplicity < 0.0) {
                    LOG_WARN << "Jplace document contains pquery with negative multiplicity at "
                             << "name '" << pqry_name.name << "'.";
                }

                pqry.add_name( pqry_name );
            }
        }

        // Finally, add the pquery to the smp object.
        smp.add_pquery( pqry );
    }
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Return the currenlty set InvalidNumberBehaviour.
 */
JplaceReader::InvalidNumberBehaviour JplaceReader::invalid_number_behaviour() const
{
    return invalid_number_behaviour_;
}

/**
 * @brief Set the InvalidNumberBehaviour.
 *
 * This setter controls the InvalidNumberBehaviour of the JplaceReader. The default value is
 * InvalidNumberBehaviour::kIgnore.
 *
 * The function returns the JplaceReader object to allow for a fluent interface.
 */
JplaceReader& JplaceReader::invalid_number_behaviour( InvalidNumberBehaviour val )
{
    invalid_number_behaviour_ = val;
    return *this;
}

} // namespace placement
} // namespace genesis
