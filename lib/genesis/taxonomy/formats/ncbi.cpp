/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/formats/ncbi.hpp"

#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/formats/csv/input_iterator.hpp"
#include "genesis/utils/text/char.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Local Helper Functions
// =================================================================================================

utils::CsvReader get_ncbi_file_reader_()
{
    // Prepare a reader for the stupid NCBI table specifications.
    // Why can't they use normal csv files like everyone else?
    auto reader = utils::CsvReader();
    reader.separator_chars( "|" );
    reader.trim_chars( "\t" );
    reader.quotation_chars( "" );
    return reader;
}

// =================================================================================================
//     Tables
// =================================================================================================

NcbiNodeLookup read_ncbi_node_table(
    std::shared_ptr<utils::BaseInputSource> source
) {
    // Use default params.
    NcbiTableParameters params;
    return read_ncbi_node_table( source, params );
}

NcbiNodeLookup read_ncbi_node_table(
    std::shared_ptr<utils::BaseInputSource> source,
    NcbiTableParameters const& params
) {
    NcbiNodeLookup result;

    // Param checks.
    if(
        params.node_table_tax_id_pos == params.node_table_parent_tax_id_pos ||
        params.node_table_tax_id_pos == params.node_table_rank_pos ||
        params.node_table_parent_tax_id_pos == params.node_table_rank_pos
    ) {
        throw std::invalid_argument( "Invalid field positions for reading NCBI node table" );
    }

    // Helper to get a field or throw.
    auto get_field = [](
        utils::CsvReader::Line& line, size_t pos, std::string const& field_name
    ) -> std::string& {
        if( pos >= line.size() ) {
            throw std::runtime_error(
                "NCBI node table line does not contain position " + std::to_string( pos ) +
                " for field " + field_name
            );
        }

        assert( pos < line.size() );
        return line[ pos ];
    };

    // Set up the csv reader iterator.
    auto csv_iterator = utils::CsvInputIterator( source, get_ncbi_file_reader_() );

    // Iterate lines and get all fields into the result lookup table.
    while( csv_iterator ) {
        auto& line = *csv_iterator;

        // Extract the relevant fields.
        NcbiNode node_entry;
        node_entry.tax_id = std::stoull(
            get_field( line, params.node_table_tax_id_pos, "tax_id" )
        );
        node_entry.parent_tax_id = std::stoull(
            get_field( line, params.node_table_parent_tax_id_pos, "parent_tax_id" )
        );
        node_entry.rank = std::move(
            get_field( line, params.node_table_rank_pos, "rank" )
        );

        // We expect unique entries.
        if( result.count( node_entry.tax_id ) > 0 ) {
            throw std::runtime_error(
                "Multiple entries for NCBI node with tax_id " + std::to_string( node_entry.tax_id )
            );
        }

        // Add the node to the lookup, and move to the next entry
        result[ node_entry.tax_id ] = std::move( node_entry );
        ++csv_iterator;
    }

    return result;
}

NcbiNameLookup read_ncbi_name_table(
    std::shared_ptr<utils::BaseInputSource> source
) {
    // Use default params.
    NcbiTableParameters params;
    return read_ncbi_name_table( source, params );
}

NcbiNameLookup read_ncbi_name_table(
    std::shared_ptr<utils::BaseInputSource> source,
    NcbiTableParameters const& params
) {
    NcbiNameLookup result;

    // Param checks.
    if(
        params.name_table_tax_id_pos == params.name_table_name_pos ||
        params.name_table_tax_id_pos == params.name_table_name_class_pos ||
        params.name_table_name_pos == params.name_table_name_class_pos
    ) {
        throw std::invalid_argument( "Invalid field positions for reading NCBI name table" );
    }

    // Helper to get a field or throw.
    auto get_field = [](
        utils::CsvReader::Line& line, size_t pos, std::string const& field_name
    ) -> std::string& {
        if( pos >= line.size() ) {
            throw std::runtime_error(
                "NCBI name table line does not contain position " +
                std::to_string( pos ) + " for field " + field_name
            );
        }

        assert( pos < line.size() );
        return line[ pos ];
    };

    // Set up the csv reader iterator.
    auto csv_iterator = utils::CsvInputIterator( source, get_ncbi_file_reader_() );

    // Iterate lines and get all fields into the result lookup table.
    while( csv_iterator ) {
        auto& line = *csv_iterator;

        // Extract the relevant fields.
        NcbiName name_entry;
        name_entry.tax_id = std::stoull(
            get_field( line, params.name_table_tax_id_pos, "tax_id" )
        );
        name_entry.name = std::move(
            get_field( line, params.name_table_name_pos, "name" )
        );
        name_entry.name_class = std::move(
            get_field( line, params.name_table_name_class_pos, "name_class" )
        );

        // Do not add if the name class does not fit.
        if( !params.name_class_filter.empty() && name_entry.name_class != params.name_class_filter ) {
            ++csv_iterator;
            continue;
        }

        // Check name validity
        if( params.validate_name_characters ) {
            auto const all_print = std::all_of(
                name_entry.name.begin(), name_entry.name.end(), utils::is_print
            );
            if( !all_print ) {
                throw std::runtime_error(
                    "NCBI name table contains entries with non-printable characters: "
                    "tax_id = " + std::to_string( name_entry.tax_id ) + ", name = \"" +
                    name_entry.name + "\""
                );
            }
        }

        // We expect unique entries.
        if( result.count( name_entry.tax_id ) > 0 ) {
            throw std::runtime_error(
                "Multiple entries for NCBI name with tax_id " + std::to_string( name_entry.tax_id ) +
                ": '" + result[ name_entry.tax_id ].name + "' vs '" + name_entry.name + "'"
            );
        }

        // Add the name to the lookup, and move to the next entry
        result[ name_entry.tax_id ] = std::move( name_entry );
        ++csv_iterator;
    }

    return result;
}

// =================================================================================================
//     Taxonomy
// =================================================================================================

Taxonomy convert_ncbi_tables(
    NcbiNodeLookup const& nodes,
    NcbiNameLookup const& names
) {
    Taxonomy result;

    // Recursive function that addes a taxon to the taxonomy, and its parents first, if needed.
    std::function<void( NcbiNode const& node )> add_taxon = [&]( NcbiNode const& node ){

        // Due to the recursion, it can happen that we already added the node before.
        // In that case, we can simply skip.
        if( node.taxon != nullptr ) {
            return;
        }

        // Safety
        auto parent_node_it = nodes.find( node.parent_tax_id );
        if( parent_node_it == nodes.end() ) {
            throw std::runtime_error(
                "Cannot find parent tax_id " + std::to_string( node.parent_tax_id ) + " for node " +
                std::to_string( node.tax_id ) + " in the NCBI nodes."
            );
        }

        // Get the parent. We need it a few times, so only do the lookup once.
        auto& parent_node = parent_node_it->second;

        // We have two bases cases: either the immediate parent already exists in the taxonomy,
        // or is the root. In both cases, we can simply add the taxon to this parent and are done.
        Taxonomy* parent_tax = nullptr;

        // Base case: the parent exists.
        if( parent_node.taxon != nullptr ) {
            parent_tax = parent_node.taxon;
        }

        // Base case: the parent is the root.
        if( parent_node.tax_id == node.tax_id ) {
            parent_tax = &result;
        }

        // If we didn't find the parent, we have to recurse and create it first.
        if( ! parent_tax ) {
            add_taxon( parent_node );

            // Now, the parent has a taxon assigned, so use it.
            assert( parent_node.taxon != nullptr );
            parent_tax = parent_node.taxon;
        }

        // Now we are sure that we have a valid parent.
        assert( parent_tax );

        // Get the name of this taxon.
        auto name_it = names.find( node.tax_id );
        if( name_it == names.end() ) {
            throw std::runtime_error(
                "No name found for tax_id " + std::to_string( node.tax_id )
            );
        }
        auto const& name = name_it->second.name;

        // Add the taxon to the parent.
        auto& added = parent_tax->add_child( name );
        added.rank( node.rank );
        added.id( node.tax_id );

        // Done. Store the taxon in the lookup for later.
        node.taxon = &added;
    };

    // Add all taxa to the taxonomy.
    for( auto const& node_it : nodes ) {
        auto const& node = node_it.second;

        // Add the taxon, recursively if needed.
        add_taxon( node );
    }

    return result;
}


Taxonomy read_ncbi_taxonomy(
    std::string const& node_file,
    std::string const& name_file
) {
    // Use default params.
    NcbiTableParameters params;
    return read_ncbi_taxonomy(
        utils::from_file( node_file ), utils::from_file( name_file ), params
    );
}

Taxonomy read_ncbi_taxonomy(
    std::string const& node_file,
    std::string const& name_file,
    NcbiTableParameters const& params
) {
    return read_ncbi_taxonomy(
        utils::from_file( node_file ), utils::from_file( name_file ), params
    );
}

Taxonomy read_ncbi_taxonomy(
    std::shared_ptr<utils::BaseInputSource> node_source,
    std::shared_ptr<utils::BaseInputSource> name_source
) {
    // Use default params.
    NcbiTableParameters params;
    return read_ncbi_taxonomy(
        node_source, name_source, params
    );
}

Taxonomy read_ncbi_taxonomy(
    std::shared_ptr<utils::BaseInputSource> node_source,
    std::shared_ptr<utils::BaseInputSource> name_source,
    NcbiTableParameters const& params
) {
    // Read data into lookup tables.
    auto const nodes = read_ncbi_node_table( node_source, params );
    auto const names = read_ncbi_name_table( name_source, params );

    // Do the table untangling.
    return convert_ncbi_tables( nodes, names );
}

} // namespace taxonomy
} // namespace genesis
