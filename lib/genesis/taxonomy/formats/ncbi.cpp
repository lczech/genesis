/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/formats/ncbi.hpp"

#include <cassert>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     NCBI Reading stuff
// =================================================================================================

NcbiNodeLookup convert_ncbi_node_table(
    utils::CsvReader::Table const& node_table,
    size_t tax_id_pos,
    size_t parent_tax_id_pos,
    size_t rank_pos
) {
    NcbiNodeLookup result;

    // Helper to get a field or throw.
    auto get_field = []( utils::CsvReader::Row const& line, size_t pos, std::string const& field_name ){
        if( pos >= line.size() ) {
            throw std::runtime_error(
                "NCBI node table line does not contain position " + std::to_string( pos ) +
                " for field " + field_name
            );
        }

        assert( pos < line.size() );
        return line[ pos ];
    };

    // Iterate lines and get all fields into the result lookup table.
    for( auto const& line : node_table ) {
        NcbiNode node;
        node.tax_id        = get_field( line, tax_id_pos, "tax_id" );
        node.parent_tax_id = get_field( line, parent_tax_id_pos, "parent_tax_id" );
        node.rank          = get_field( line, rank_pos, "rank" );

        // We expect unique entries.
        if( result.count( node.tax_id ) > 0 ) {
            throw std::runtime_error( "Multiple entries for NCBI node with tax_id " + node.tax_id );
        }

        result[ node.tax_id ] = node;
    }

    return result;
}

NcbiNameLookup convert_ncbi_name_table(
    utils::CsvReader::Table const& name_table,
    size_t tax_id_pos,
    size_t name_pos,
    size_t name_class_pos,
    std::string name_class_filter
) {
    NcbiNameLookup result;

    // Helper to get a field or throw.
    auto get_field = []( utils::CsvReader::Row const& line, size_t pos, std::string const& field_name ){
        if( pos >= line.size() ) {
            throw std::runtime_error(
                "NCBI name table line does not contain position " + std::to_string( pos ) +
                " for field " + field_name
            );
        }

        assert( pos < line.size() );
        return line[ pos ];
    };

    // Iterate lines and get all fields into the result lookup table.
    for( auto const& line : name_table ) {
        NcbiName name;
        name.tax_id     = get_field( line, tax_id_pos, "tax_id" );
        name.name       = get_field( line, name_pos, "name" );
        name.name_class = get_field( line, name_class_pos, "name_class" );

        // Do not add if the name class does not fit.
        if( name.name_class != name_class_filter ) {
            continue;
        }

        // We expect unique entries.
        if( result.count( name.tax_id ) > 0 ) {
            throw std::runtime_error( "Multiple entries for NCBI name with tax_id " + name.tax_id );
        }

        result[ name.tax_id ] = name;
    }

    return result;
}

Taxonomy convert_ncbi_tables(
    NcbiNodeLookup const& nodes,
    NcbiNameLookup const& names
) {
    Taxonomy result;

    // Recursive function that addes a taxon to the taxonomy, and its parents first, if needed.
    std::function<void( NcbiNode const& node )> add_taxon = [&]( NcbiNode const& node ){

        // Due to the recurson, it can happen that we already added the node before.
        // In that case, we can simply skip.
        if( node.taxon != nullptr ) {
            return;
        }

        // Safety
        if( nodes.count( node.parent_tax_id ) == 0 ) {
            throw std::runtime_error(
                "Cannot find parent tax_id " + node.parent_tax_id + " for node " +
                node.tax_id + " in the NCBI nodes."
            );
        }

        // Get the parent. We need it a few times, so only to the lookup once.
        auto& parent_node = nodes.at( node.parent_tax_id );

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
        if( names.count( node.tax_id ) == 0 ) {
            throw std::runtime_error( "No name found for tax_id " + node.tax_id );
        }
        auto const& name = names.at( node.tax_id ).name;

        // Add the taxon to the parent.
        // We skip the check for taxa with the same name, as otherwise the function becomes way too
        // slow. We trust the NCBI taxonomy just enough to think that there are no duplicates.
        auto& added = parent_tax->add_child( name, false );
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

Taxonomy read_ncbi_taxonomy( std::string const& node_file, std::string const& name_file )
{
    // Prepare a reader for the stupid NCBI table specifications.
    // Why can't they use normal csv files like everyone else?
    auto reader = utils::CsvReader();
    reader.separator_chars( "|" );
    reader.trim_chars( "\t" );
    reader.quotation_chars( "" );

    // Read data into lookup tables.
    auto const nodes = convert_ncbi_node_table( reader.from_file( node_file ));
    auto const names = convert_ncbi_name_table( reader.from_file( name_file ));

    // Do the table untangling.
    return convert_ncbi_tables( nodes, names );
}

} // namespace taxonomy
} // namespace genesis
