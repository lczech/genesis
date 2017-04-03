/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/placement/formats/jplace_reader.hpp"

#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/formats/newick_reader.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/iterator.hpp"
#include "genesis/utils/formats/json/reader.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <cctype>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// #ifdef GENESIS_OPENMP
// #   include <omp.h>
// #endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Reading
// =================================================================================================

// -------------------------------------------------------------------------
//     Reading from Stream
// -------------------------------------------------------------------------

Sample JplaceReader::from_stream( std::istream& is ) const
{
    auto doc = utils::JsonReader().from_stream( is );
    return from_document( doc );
}

// -------------------------------------------------------------------------
//     Reading from File
// -------------------------------------------------------------------------

Sample JplaceReader::from_file( std::string const& fn ) const
{
    auto doc = utils::JsonReader().from_file( fn );
    return from_document( doc );
}

// -------------------------------------------------------------------------
//     Reading from String
// -------------------------------------------------------------------------

Sample JplaceReader::from_string( std::string const& jplace ) const
{
    auto doc = utils::JsonReader().from_string( jplace );
    return from_document( doc );
}

// -------------------------------------------------------------------------
//     Reading from Document
// -------------------------------------------------------------------------

Sample JplaceReader::from_document( utils::JsonDocument& doc ) const
{
    Sample smp;

    if( ! doc.is_object() ) {
        throw std::runtime_error( "Json value is not a Json document." );
    }

    process_json_version( doc );
    process_json_metadata( doc, smp );

    process_json_tree( doc, smp );
    auto fields = process_json_fields( doc );
    process_json_placements( doc, smp, fields);

    return smp;
}

// -------------------------------------------------------------------------
//     Reading from Files
// -------------------------------------------------------------------------

SampleSet JplaceReader::from_files (const std::vector<std::string>& fns ) const
{
    SampleSet set;
    from_files( fns, set );
    return set;
}

// -------------------------------------------------------------------------
//     Reading from Strings
// -------------------------------------------------------------------------

SampleSet JplaceReader::from_strings (const std::vector<std::string>& jps ) const
{
    SampleSet set;
    from_files( jps, set );
    return set;
}

// -------------------------------------------------------------------------
//     Reading from Files
// -------------------------------------------------------------------------

void JplaceReader::from_files (const std::vector<std::string>& fns, SampleSet& set) const
{
    // #if defined( GENESIS_OPENMP )
    //
    //     // Make a vector of default-constructored Samples of the needed size.
    //     auto tmp = std::vector<Sample>( fns.size() );
    //
    //     // Parallel parsing.
    //     #pragma omp parallel for
    //     for( size_t i = 0; i < fns.size(); ++i ) {
    //         tmp[ i ] = from_file( fns[i] );
    //     }
    //
    //     // Move to target SampleSet.
    //     for( size_t i = 0; i < fns.size(); ++i ) {
    //         std::string name = utils::file_filename( utils::file_basename( fns[i] ) );
    //         set.add( std::move( tmp[i] ), name );
    //     }
    //
    // #else

        for (auto fn : fns) {
            std::string name = utils::file_filename( utils::file_basename(fn) );
            set.add( from_file( fn ), name );
        }

    // #endif
}

// -------------------------------------------------------------------------
//     Reading from Strings
// -------------------------------------------------------------------------

void JplaceReader::from_strings (const std::vector<std::string>& jps, SampleSet& set) const
{
    size_t cnt = set.size();
    for (auto jplace : jps) {
        set.add( from_string( jplace ), std::string("jplace_") + std::to_string(cnt) );
        ++cnt;
    }
}

// =================================================================================================
//     Processing
// =================================================================================================

// -------------------------------------------------------------------------
//     Processing Version
// -------------------------------------------------------------------------

void JplaceReader::process_json_version( utils::JsonDocument const& doc ) const
{
    // Check if there is a version key.
    auto v_it = doc.find( "version" );
    if( v_it == doc.end() || !( v_it->is_string() || v_it->is_number() ) ) {
        LOG_WARN << "Jplace document does not contain a valid version number at key 'version'. "
                 << "Now continuing to parse in the hope that it still works.";
        return;
    }

    // The key can be a number (3) or a string ("3"), check both.
    std::string doc_version;
    if( v_it->is_number() ) {
        if( v_it->is_number_float() ) {
            doc_version = std::to_string( v_it->get_number_float() );
        } else {
            doc_version = std::to_string( v_it->get_number_unsigned() );
        }
    }
    if( v_it->is_string() ) {
        doc_version = v_it->get_string();
    }

    // Check if the version is correct.
    if( ! check_version( doc_version )) {
        LOG_WARN << "Jplace document has version '" << doc_version << "', however this parser "
                 << "is written for version " << version() << " of the Jplace format. "
                 << "Now continuing to parse in the hope that it still works.";
    }
}

// -------------------------------------------------------------------------
//     Processing Metadata
// -------------------------------------------------------------------------

void JplaceReader::process_json_metadata( utils::JsonDocument const& doc, Sample& smp ) const
{
    // Check if there is metadata.
    auto meta_it = doc.find( "metadata" );
    if( meta_it != doc.end() && meta_it->is_object() ) {
        for( auto it = meta_it->begin(); it != meta_it->end(); ++it ) {

            // Only use metadata that is a string. Everything else is ignored.
            if( it.value().is_string() ) {
                smp.metadata[ it.key() ] = it.value().get_string();
            }
        }
    }
}

// -------------------------------------------------------------------------
//     Processing Tree
// -------------------------------------------------------------------------

void JplaceReader::process_json_tree( utils::JsonDocument const& doc, Sample& smp ) const
{
    // Find and process the reference tree.
    auto tree_it = doc.find( "tree" );
    if( tree_it == doc.end() || ! tree_it->is_string()
    ) {
        throw std::runtime_error(
            "Jplace document does not contain a valid Newick tree at key 'tree'."
        );
    }
    smp.tree() = PlacementTreeNewickReader().from_string( tree_it->get_string() );
    if( ! has_correct_edge_nums( smp.tree() )) {
        LOG_WARN << "Jplace document has a Newick tree where the edge_num tags are non standard. "
                 << "They are expected to be assigned in ascending order via postorder traversal. "
                 << "Now continuing to parse, as we can cope with this.";
    }
}

// -------------------------------------------------------------------------
//     Processing Fields
// -------------------------------------------------------------------------

std::vector<std::string> JplaceReader::process_json_fields( utils::JsonDocument const& doc ) const
{
    // get the field names and store them in array fields
    auto fields_it = doc.find( "fields" );

    if( fields_it == doc.end() || ! fields_it->is_array() ) {
        throw std::runtime_error( "Jplace document does not contain field names at key 'fields'." );
    }
    std::vector<std::string> fields;
    bool has_edge_num = false;
    for( auto const& fields_val : *fields_it ) {
        if( ! fields_val.is_string() ) {
            throw std::runtime_error(
                "Jplace document contains a value of type '" + fields_val.type_name()
                + "' instead of a string with a field name at key 'fields'."
            );
        }

        // check field validity
        std::string const& field = fields_val.get_string();
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

// -------------------------------------------------------------------------
//     Processing Placements
// -------------------------------------------------------------------------

void JplaceReader::process_json_placements(
    utils::JsonDocument&      doc,
    Sample&                   smp,
    std::vector<std::string>  fields
) const {
    // create a map from edge nums to the actual edge pointers, for later use when processing
    // the pqueries. we do not use Sample::EdgeNumMap() here, because we need to do extra
    // checking for validity first!
    std::unordered_map<size_t, PlacementTreeEdge*> edge_num_map;
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
    auto place_it = doc.find( "placements" );
    if( place_it == doc.end() || ! place_it->is_array() ) {
        throw std::runtime_error(
            "Jplace document does not contain pqueries at key 'placements'."
        );
    }
    for( auto& pqry_obj : *place_it ) {
        if( ! pqry_obj.is_object() ) {
            throw std::runtime_error(
                "Jplace document contains a value of type '" + pqry_obj.type_name()
                + "' instead of an object with a pquery at key 'placements'."
            );
        }
        auto pqry_p_arr = pqry_obj.find( "p" );
        if( pqry_p_arr == pqry_obj.end() || ! pqry_p_arr->is_array() ) {
            throw std::runtime_error(
                "Jplace document contains a pquery at key 'placements' that does not contain an "
                + std::string( "array of placements at sub-key 'p'." )
            );
        }

        // Create new pquery.
        auto pqry = Pquery();

        // Process the placements and store them in the pquery.
        for( auto const& pqry_fields : *pqry_p_arr ) {
            if( ! pqry_fields.is_array() ) {
                throw std::runtime_error(
                    "Jplace document contains a pquery with invalid placement at key 'p'."
                );
            }

            if (pqry_fields.size() != fields.size()) {
                throw std::runtime_error(
                    "Jplace document contains a placement fields array with different size "
                    + std::string( "than the fields name array." )
                );
            }

            // Process all fields of the placement.
            auto pqry_place = PqueryPlacement();
            double distal_length = -1.0;
            for (size_t i = 0; i < pqry_fields.size(); ++i) {
                // Up to version 3 of the jplace specification, the p-fields in a jplace document
                // only contain numbers (float or int), so we can do this check here once for all
                // fields, instead of repetition for every field. If in the future there are fields
                // with non-number type, this check has to go into the single field assignments.
                if (!pqry_fields.at(i).is_number()) {
                    throw std::runtime_error(
                        "Jplace document contains pquery where field " + fields[i]
                        + " is of type '" + pqry_fields.at(i).type_name()
                        + "' instead of a number."
                    );
                }

                // Switch on the field name to set the correct value.
                double pqry_place_val = pqry_fields.at(i).get_number<double>();
                if (fields[i] == "edge_num") {
                    size_t val_int = pqry_fields.at(i).get_number<size_t>();

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
        if( pqry_obj.count("n") > 0 && pqry_obj.count("nm") > 0 ) {
            throw std::runtime_error(
                "Jplace document contains a pquery with both an 'n' and an 'nm' key."
            );
        }
        if( pqry_obj.count("n") == 0 && pqry_obj.count("nm") == 0 ) {
            throw std::runtime_error(
                "Jplace document contains a pquery with neither an 'n' nor an 'nm' key."
            );
        }

        // Process names.
        if( pqry_obj.count("n") > 0 ) {
            if( ! pqry_obj[ "n" ].is_array() ) {
                throw std::runtime_error(
                    "Jplace document contains a pquery with key 'n' that is not array."
                );
            }

            for( auto const& pqry_n_val : pqry_obj[ "n" ] ) {
                if( ! pqry_n_val.is_string() ) {
                    throw std::runtime_error(
                        "Jplace document contains a pquery where key 'n' has a non-string field."
                    );
                }

                // Add the name with a default multiplicity.
                pqry.add_name( pqry_n_val.get_string() );
            }
        }

        // Process named multiplicities.
        if( pqry_obj.count("nm") > 0 ) {
            if ( ! pqry_obj[ "nm" ].is_array() ) {
                throw std::runtime_error(
                    "Jplace document contains a pquery with key 'nm' that is not array."
                );
            }

            for( auto const& pqry_nm_val : pqry_obj[ "nm" ] ) {
                if( ! pqry_nm_val.is_array() ) {
                    throw std::runtime_error(
                        "Jplace document contains a pquery where key 'nm' has a non-array field."
                    );
                }

                if( pqry_nm_val.size() != 2 ) {
                    throw std::runtime_error(
                        std::string( "Jplace document contains a pquery where key 'nm' has an " )
                        + "array field with size != 2 (one for the name, one for the multiplicity)."
                    );
                }
                if( ! pqry_nm_val.at(0).is_string() ) {
                    throw std::runtime_error(
                        std::string( "Jplace document contains a pquery where key 'nm' has an " )
                        + "array whose first value is not a string for the name."
                    );
                }
                if( ! pqry_nm_val.at(1).is_number() ) {
                    throw std::runtime_error(
                        std::string( "Jplace document contains a pquery where key 'nm' has an " )
                        + "array whose second value is not a number for the multiplicity."
                    );
                }

                auto pqry_name = PqueryName();
                pqry_name.name         = pqry_nm_val.at(0).get_string();
                pqry_name.multiplicity = pqry_nm_val.at(1).get_number_float();
                if (pqry_name.multiplicity < 0.0) {
                    LOG_WARN << "Jplace document contains pquery with negative multiplicity at "
                             << "name '" << pqry_name.name << "'.";
                }

                pqry.add_name( pqry_name );
            }
        }

        // Finally, add the pquery to the smp object.
        smp.add( pqry );
        pqry_obj.clear();
    }
}

// =================================================================================================
//     Jplace Version
// =================================================================================================

std::string JplaceReader::version ()
{
    return "3";
}

bool JplaceReader::check_version ( size_t version )
{
    return version == 2 || version == 3;
}

bool JplaceReader::check_version ( std::string const& version )
{
    auto v = utils::trim( version );
    return v == "2" || v == "3";
}

// =================================================================================================
//     Properties
// =================================================================================================

JplaceReader::InvalidNumberBehaviour JplaceReader::invalid_number_behaviour() const
{
    return invalid_number_behaviour_;
}

JplaceReader& JplaceReader::invalid_number_behaviour( InvalidNumberBehaviour val )
{
    invalid_number_behaviour_ = val;
    return *this;
}

} // namespace placement
} // namespace genesis
