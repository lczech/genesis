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

#include "genesis/placement/formats/jplace_writer.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/formats/newick_writer.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/tree/formats/newick/writer.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/core/version.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/writer.hpp"
#include "genesis/utils/io/output_stream.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Write a Sample to a stream, using the Jplace format.
 */
void JplaceWriter::to_stream( Sample const& sample, std::ostream& os ) const
{
    // Indent. Might be replaced by some setting for the class in the future.
    std::string in = "    ";

    // Open json document.
    os << "{\n";

    // Write version.
    os << in << "\"version\": 3,\n";

    // Write metadata.
    os << in << "\"metadata\": {\n";
    os << in << in << "\"program\": \"" << "genesis " << genesis_version() << "\",\n";
    os << in << in << "\"invocation\": \"" << utils::Options::get().command_line_string() << "\"\n";
    os << in << "},\n";

    // Write tree.
    auto newick_writer = PlacementTreeNewickWriter();
    newick_writer.enable_names(true);
    newick_writer.enable_branch_lengths(true);
    newick_writer.branch_length_precision( branch_length_precision_ );
    os << in << "\"tree\": \"" << newick_writer.to_string( sample.tree() ) << "\",\n";

    // Write field names.
    os << in << "\"fields\": [ \"edge_num\", \"likelihood\", \"like_weight_ratio\", "
    << "\"distal_length\", \"pendant_length\" ],\n";

    // Write pqueries.
    os << in << "\"placements\": [\n";
    for( size_t i = 0; i < sample.size(); ++i ) {
        auto const& pquery = sample.at(i);
        os << in << in << "{\n";

        // Write placements.
        os << in << in << in << "\"p\": [\n";
        for( size_t j = 0; j < pquery.placement_size(); ++j ) {
            auto const& placement = pquery.placement_at(j);
            os << in << in << in << in << "[ ";

            os << placement.edge_num() << ", ";
            os << placement.likelihood << ", ";
            os << placement.like_weight_ratio << ", ";

            auto const& edge_data = placement.edge().data<PlacementEdgeData>();
            os << edge_data.branch_length - placement.proximal_length << ", ";
            os << placement.pendant_length;

            os << " ]";
            if( j < pquery.placement_size() - 1 ) {
                os << ",";
            }
            os << "\n";
        }
        os << in << in << in << "],\n";

        // Find out whether names have multiplicity.
        bool has_nm = false;
        for( auto const& pqry_name : pquery.names() ) {
            has_nm |= ( pqry_name.multiplicity != 1.0 );
        }

        // Write names.
        if( has_nm ) {

            // With multiplicity.
            os << in << in << in << "\"nm\": [\n";
            for( size_t j = 0; j < pquery.name_size(); ++j ) {
                os << in << in << in << in << "[ \"" << pquery.name_at(j).name << "\", ";
                os << pquery.name_at(j).multiplicity << " ]";

                if( j < pquery.name_size() - 1 ) {
                    os << ", ";
                }
                os << "\n";
            }
            os << in << in << in << "]\n";

        } else {

            // Without multiplicity.
            os << in << in << in << "\"n\": [ ";
            for( size_t j = 0; j < pquery.name_size(); ++j ) {
                os << "\"" << pquery.name_at(j).name << "\"";

                if( j < pquery.name_size() - 1 ) {
                    os << ", ";
                }
            }
            os << " ]\n";

        }

        // Write end of placement stuff.
        os << in << in << "}";
        if( i < sample.size() - 1 ) {
            os << ",";
        }
        os << "\n";
    }
    os << in << "]\n";

    // Close json document.
    os << "}\n";
}

/**
 * @brief Write the data of a Sample to a file in `Jplace` format.
 *
 * If the file cannot be written to, the function throws an exception. Also, by default, if the file
 * already exists, an exception is thrown.
 * See @link utils::Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink to
 * change this behaviour.
 */
void JplaceWriter::to_file( Sample const& sample, std::string const& filename ) const
{
    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    to_stream( sample, ofs );
}

/**
 * @brief Store the data of a Sample in a string in `Jplace` format.
 */
void JplaceWriter::to_string( Sample const& sample, std::string& output ) const
{
    std::ostringstream oss;
    to_stream( sample, oss );
    output = oss.str();
}

/**
 * @brief Return the data of a Sample as a string in `Jplace` format.
 */
std::string JplaceWriter::to_string( Sample const& sample ) const
{
    std::ostringstream oss;
    to_stream( sample, oss );
    return oss.str();
}

/**
 * @brief Store the data of a Sample in a JsonDocument object.
 *
 * This method is not really useful anymore, as we can now directly write to files, strings and
 * streams. It is however kept here for reference and in case someone wants to work with Json files
 * directly.
 */
void JplaceWriter::to_document( Sample const& smp, utils::JsonDocument& doc ) const
{
    using namespace utils;
    doc = JsonDocument::object();

    // set tree
    auto nwp = PlacementTreeNewickWriter();
    nwp.enable_names(true);
    nwp.enable_branch_lengths(true);
    nwp.branch_length_precision( branch_length_precision_ );
    doc[ "tree" ] = nwp.to_string( smp.tree() );

    // set placements
    auto& placements_arr = doc[ "placements" ];
    for( auto const& pqry : smp.pqueries() ) {
        auto jpqry = JsonDocument::object();

        // set placements
        auto pqry_p_arr = JsonDocument::array();
        for( auto const& pqry_place : pqry.placements() ) {
            auto pqry_fields = JsonDocument::array();

            pqry_fields.push_back( JsonDocument::number_unsigned( pqry_place.edge_num() ));
            pqry_fields.push_back( JsonDocument::number_float( pqry_place.likelihood ));
            pqry_fields.push_back( JsonDocument::number_float( pqry_place.like_weight_ratio ));

            // convert from proximal to distal length.
            auto const& edge_data = pqry_place.edge().data<PlacementEdgeData>();
            pqry_fields.push_back( JsonDocument::number_float(
                edge_data.branch_length - pqry_place.proximal_length
            ));
            pqry_fields.push_back( JsonDocument::number_float( pqry_place.pendant_length ));

            pqry_p_arr.push_back( pqry_fields );
        }
        jpqry[ "p" ] = pqry_p_arr;

        // find out whether names have multiplicity
        bool has_nm = false;
        for( auto const& pqry_name : pqry.names() ) {
            has_nm |= ( pqry_name.multiplicity != 1.0 );
        }

        // set named multiplicity / name
        if (has_nm) {
            auto pqry_nm_arr = JsonDocument::array();
            for( auto const& pqry_name : pqry.names() ) {
                auto pqry_nm_val = JsonDocument::array();
                pqry_nm_val.push_back( pqry_name.name );
                pqry_nm_val.push_back( JsonDocument::number_float( pqry_name.multiplicity ));
                pqry_nm_arr.push_back( pqry_nm_val );
            }
            jpqry[ "nm" ] = pqry_nm_arr;
        } else {
            auto pqry_n_arr  = JsonDocument::array();
            for( auto const& pqry_name : pqry.names() ) {
                pqry_n_arr.push_back( pqry_name.name );
            }
            jpqry[ "n" ] = pqry_n_arr;
        }

        placements_arr.push_back( jpqry );
    }

    // set fields
    auto jfields = JsonDocument::array();
    jfields.push_back( "edge_num" );
    jfields.push_back( "likelihood" );
    jfields.push_back( "like_weight_ratio" );
    jfields.push_back( "distal_length" );
    jfields.push_back( "pendant_length" );
    doc[ "fields" ] = jfields;

    // set version
    doc[ "version" ] = 3;

    // set metadata
    auto jmetadata = JsonDocument::object();
    jmetadata[ "program" ] = "genesis " + genesis_version();
    jmetadata[ "invocation" ] = utils::Options::get().command_line_string();
    doc[ "metadata" ] = jmetadata;
}

} // namespace placement
} // namespace genesis
