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

#include "placement/formats/jplace_writer.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "placement/function/operators.hpp"
#include "placement/formats/newick_writer.hpp"
#include "placement/sample_set.hpp"
#include "placement/sample.hpp"

#include "tree/formats/newick/writer.hpp"

#include "utils/core/genesis.hpp"
#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/options.hpp"
#include "utils/core/std.hpp"
#include "utils/formats/json/document.hpp"
#include "utils/formats/json/writer.hpp"

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
    newick_writer.branch_length_precision = branch_length_precision;
    os << in << "\"tree\": \"" << newick_writer.to_string( sample.tree() ) << "\",\n";

    // Write field names.
    os << in << "\"fields\": [ \"edge_num\", \"likelihood\", \"like_weight_ratio\", "
    << "\"distal_length\", \"pendant_length\" ],\n";

    // Write pqueries.
    os << in << "\"placements\": [\n";
    for( size_t i = 0; i < sample.pquery_size(); ++i ) {
        auto const& pquery = sample.pquery_at(i);
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
        if( i < sample.pquery_size() - 1 ) {
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
 * If the file already exists or cannot be written to, the function throws `std::runtime_error`.
 */
void JplaceWriter::to_file( Sample const& sample, std::string const& filename ) const
{
    if( utils::file_exists( filename )) {
        throw std::runtime_error( "Jplace file '" + filename + "' already exist." );
    }

    std::ofstream ofs( filename );
    if( ofs.fail() ) {
        throw std::runtime_error( "Cannot write to file '" + filename + "'." );
    }

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
    // Simplify the code. Specifying utils::Json... is cumbersome.
    using namespace utils;

    doc.clear();

    // set tree
    auto nwp = PlacementTreeNewickWriter();
    nwp.enable_names(true);
    nwp.enable_branch_lengths(true);
    nwp.branch_length_precision = branch_length_precision;
    doc.set("tree", new JsonValueString(nwp.to_string(smp.tree())));

    // set placements
    JsonValueArray* placements_arr = new JsonValueArray();
    for (auto& pqry : smp.pqueries()) {
        JsonValueObject* jpqry      = new JsonValueObject();
        placements_arr->push_back(jpqry);

        // set placements
        JsonValueArray* pqry_p_arr  = new JsonValueArray();
        for( auto const& pqry_place : pqry.placements() ) {
            JsonValueArray* pqry_fields = new JsonValueArray();
            pqry_fields->push_back(new JsonValueNumber(pqry_place.edge_num()));
            pqry_fields->push_back(new JsonValueNumber(pqry_place.likelihood));
            pqry_fields->push_back(new JsonValueNumber(pqry_place.like_weight_ratio));

            // convert from proximal to distal length.
            auto const& edge_data = pqry_place.edge().data<PlacementEdgeData>();
            pqry_fields->push_back(new JsonValueNumber(
                edge_data.branch_length - pqry_place.proximal_length
            ));
            pqry_fields->push_back(new JsonValueNumber(pqry_place.pendant_length));
            pqry_p_arr->push_back(pqry_fields);
        }
        jpqry->set("p", pqry_p_arr);

        // find out whether names have multiplicity
        bool has_nm = false;
        for( auto const& pqry_name : pqry.names() ) {
            has_nm |= ( pqry_name.multiplicity != 1.0 );
        }

        // set named multiplicity / name
        if (has_nm) {
            JsonValueArray* pqry_nm_arr = new JsonValueArray();
            for( auto const& pqry_name : pqry.names() ) {
                JsonValueArray* pqry_nm_val = new JsonValueArray();
                pqry_nm_val->push_back(new JsonValueString(pqry_name.name));
                pqry_nm_val->push_back(new JsonValueNumber(pqry_name.multiplicity));
                pqry_nm_arr->push_back(pqry_nm_val);
            }
            jpqry->set("nm", pqry_nm_arr);
        } else {
            JsonValueArray* pqry_n_arr  = new JsonValueArray();
            for( auto const& pqry_name : pqry.names() ) {
                pqry_n_arr->push_back(new JsonValueString(pqry_name.name));
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
    jmetadata->set("program", new JsonValueString( "genesis " + genesis_version() ));
    jmetadata->set("invocation", new JsonValueString( utils::Options::get().command_line_string() ));
    doc.set("metadata", jmetadata);
}

} // namespace placement
} // namespace genesis
