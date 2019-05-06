/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
#include <cassert>
#include <cctype>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Reading
// =================================================================================================

Sample JplaceReader::read(
    std::shared_ptr<utils::BaseInputSource> source
) const {
    auto doc = utils::JsonReader().read( source );
    return read( doc );
}

Sample JplaceReader::read(
    utils::JsonDocument& doc
) const {
    Sample smp;

    if( ! doc.is_object() ) {
        throw std::runtime_error( "Json value is not a Json document." );
    }

    process_jplace_version_( doc );
    process_jplace_metadata_( doc, smp );

    process_jplace_tree_( doc, smp );
    auto const fields = process_jplace_fields_( doc );
    process_jplace_placements_( doc, smp, fields );

    return smp;
}

SampleSet JplaceReader::read(
    std::vector<std::shared_ptr<utils::BaseInputSource>> sources
) const {
    SampleSet target;
    read( sources, target );
    return target;
}

void JplaceReader::read(
    std::vector<std::shared_ptr<utils::BaseInputSource>> sources,
    SampleSet& target
) const {

    // Make a vector of default-constructed Samples of the needed size.
    // We do this so that the order of input jplace files is kept
    // when reading with OpenMP.
    auto tmp = std::vector<Sample>( sources.size() );

    // Parallel parsing.
    #pragma omp parallel for
    for( size_t i = 0; i < sources.size(); ++i ) {
        tmp[ i ] = read( sources[i] );
    }

    // Move to target SampleSet.
    for( size_t i = 0; i < sources.size(); ++i ) {
        auto const bn = utils::file_basename( sources[i]->source_string() );
        auto const ex = utils::file_extension( bn );
        auto name = bn;
        if( ex == "jplace" ) {
            name = utils::file_filename( bn );
        }

        target.add( std::move( tmp[i] ), name );
    }
}

// =================================================================================================
//     Processing
// =================================================================================================

// -------------------------------------------------------------------------
//     Get Version
// -------------------------------------------------------------------------

int JplaceReader::get_jplace_version_( utils::JsonDocument const& doc ) const
{
    // Check if there is a version key.
    auto v_it = doc.find( "version" );
    if( v_it == doc.end() ) {
        return -1;
    }

    // Try string and int, return -1 otherwise.
    int version = -1;
    assert( v_it != doc.end() );
    if( v_it->is_string() ) {
        try {
            version = std::stoi( v_it->get_string() );
        } catch(...) {
            version = -1;
        }
    } else if( v_it->is_number_unsigned() ) {
        version = v_it->get_number_unsigned();
    }
    return version;
}

// -------------------------------------------------------------------------
//     Processing Version
// -------------------------------------------------------------------------

void JplaceReader::process_jplace_version_( utils::JsonDocument const& doc ) const
{
    auto const version = get_jplace_version_( doc );

    // Check if there is a valid version key.
    if( version == -1 ) {
        LOG_WARN << "Jplace document does not contain a valid version number at key 'version'. "
                 << "Now continuing to parse in the hope that it still works.";
        return;
    }

    // Check if the version is working for us.
    if( version == 0 || version >= 4 ) {
        LOG_WARN << "Jplace document has version " << version << " specified at the 'version' key. "
                 << "This parser however is written for versions 1-3 of the Jplace standard. "
                 << "Now continuing to parse in the hope that it still works.";
    }
}

// -------------------------------------------------------------------------
//     Processing Metadata
// -------------------------------------------------------------------------

void JplaceReader::process_jplace_metadata_( utils::JsonDocument const& doc, Sample& smp ) const
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

void JplaceReader::process_jplace_tree_( utils::JsonDocument const& doc, Sample& smp ) const
{
    // Get the jplace version. Version 1 uses Newick comments in brackets [] for the edge_nums,
    // while later versions store them in "tags" in curly braces {}.
    // Prepare the Newick reader accordingly.
    auto const version = get_jplace_version_( doc );
    auto reader = PlacementTreeNewickReader();
    if( version == 1 ) {
        reader.get_edge_num_from_comments( true );
    }

    // Find and process the reference tree.
    auto tree_it = doc.find( "tree" );
    if( tree_it == doc.end() || ! tree_it->is_string() ) {
        throw std::runtime_error(
            "Jplace document does not contain a valid Newick tree at key 'tree'."
        );
    }
    smp.tree() = reader.read( utils::from_string( tree_it->get_string() ));

    // The tree reader already does all necessary checks of the tree. No need to repeat them here.
}

// -------------------------------------------------------------------------
//     Processing Fields
// -------------------------------------------------------------------------

std::vector<std::string> JplaceReader::process_jplace_fields_( utils::JsonDocument const& doc ) const
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
            if( std::any_of( fields.begin(), fields.end(), [&]( std::string const& fn ){
                return fn == field;
            })) {
                throw std::runtime_error(
                    "Jplace document contains field name '" + field
                    + "' more than once at key 'fields'."
                );
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

void JplaceReader::process_jplace_placements_(
    utils::JsonDocument&            doc,
    Sample&                         smp,
    std::vector<std::string> const& fields
) const {
    // create a map from edge nums to the actual edge pointers, for later use when processing
    // the pqueries. we do not use Sample::EdgeNumMap() here, because we need to do extra
    // checking for validity first!
    std::unordered_map<size_t, PlacementTreeEdge*> edge_num_map;
    for( auto& edge : smp.tree().edges() ) {
        auto& edge_data = edge.data<PlacementEdgeData>();
        if (edge_num_map.count( edge_data.edge_num()) > 0) {
            throw std::runtime_error(
                "Jplace document contains a tree where the edge_num tag '"
                + std::to_string( edge_data.edge_num() ) + "' is used more than once, "
                "and hence cannot be used to uniquely identify edges of the placements. "
                "This indicates a severe issue with the program that created the jplace file."
            );
        }
        edge_num_map.emplace( edge_data.edge_num(), &edge );
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
                // fields, instead of repetition for every field. If in the future (jplace 4 and
                // later) there are fields introduced with non-number type, this check
                // has to go into the single field assignments.
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
                            + std::to_string( val_int ) + "', which does not correspond to any "
                            "edge_num in the given Newick tree of the document."
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
                pqry_name.multiplicity = pqry_nm_val.at(1).get_number<double>();
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
