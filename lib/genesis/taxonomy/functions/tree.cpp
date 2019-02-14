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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/functions/tree.hpp"

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"
#include "genesis/taxonomy/formats/taxopath_generator.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/functions/taxopath.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"

#include "genesis/tree/tree.hpp"
#include "genesis/tree/formats/newick/broker.hpp"
#include "genesis/tree/formats/newick/element.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Tree
// =================================================================================================

/**
 * @brief Recursive local helper function to add taxa to the tree broker.
 */
void add_subtaxonomy_(
    Taxonomy const& taxonomy,
    bool drop_singleton_inner_nodes,
    bool no_inner_node_names,
    int max_level,
    int parent_level,
    tree::NewickBroker& broker
) {
    for( auto const& taxon : taxonomy ) {
        auto const level = parent_level + 1;

        if( max_level > -1 && taxon_level( taxon ) >= static_cast<size_t>( max_level )) {
            // Here, we are at a taxon that is at the max level.
            // Do not recurse, but add the name of the taxon to the tree. The taxon is possibly
            // an inner one (phylum, class, etc), which is what we want to happen if there is
            // a max level set.
            broker.push_bottom({ taxon.name(), level });

        } else  if( drop_singleton_inner_nodes && taxon.size() == 1 ) {
            // Here, we are at a taxon that only contains a single child taxon, which would
            // add a branch to the tree that does not furcate in any we. If the option to drop
            // such entities is set, we simply recurse to the single child, without adding it
            // to the tree.
            add_subtaxonomy_(
                taxon, drop_singleton_inner_nodes, no_inner_node_names, max_level,
                parent_level, broker
            );

        } else {
            // Here, we are in the default case: A taxon to be added to the tree.
            // Whether we set a name or not depends on whether it is an inner one or has children.
            std::string const tax_name = (
                ( no_inner_node_names && taxon.size() > 0 )
                ? ""
                : taxon.name()
            );
            broker.push_bottom({ tax_name, level });
            add_subtaxonomy_(
                taxon, drop_singleton_inner_nodes, no_inner_node_names, max_level,
                level, broker
            );
        }
    }
}

tree::Tree taxonomy_to_tree(
    Taxonomy const& taxonomy,
    bool drop_singleton_inner_nodes,
    bool no_inner_node_names,
    int  max_level
) {
    using namespace genesis::tree;

    // Make a broker. Add a dummy root node if the top leven of the taxonomy contains multiple
    // elements.
    NewickBroker broker;
    if( taxonomy.size() > 1 ) {
        broker.push_bottom({0});
    }

    // Recursively add taxa. We could
    add_subtaxonomy_(
        taxonomy, drop_singleton_inner_nodes, no_inner_node_names, max_level, 1, broker
    );
    broker.assign_ranks();

    // Non-recursive way of adding taxa, that however does not easily support
    // to drop inner branches that do not have any furcation.
    // for( auto const& taxon_it : preorder( taxonomy )) {
    //     auto const lvl = 1 + static_cast<int>( taxon_level( taxon_it.taxon() ));
    //     broker.push_bottom({ taxon_it.taxon().name(), lvl });
    // }

    // We (mis-)use a Common Tree Newick Reader here. It supports to turn a broker into a tree,
    // and takes names into account. This is what we want. It will create branch lengths, too.
    // We can live with that. Otherwise we'd have to make a new Tree data type for just this use
    // case, so not really nice.
    auto const nr = CommonTreeNewickReader();
    return nr.broker_to_tree( broker );
}

tree::Tree taxonomy_to_tree(
    Taxonomy const& taxonomy,
    std::unordered_map<std::string, Taxopath> const& extra_taxa,
    bool drop_singleton_inner_nodes,
    bool no_inner_node_names,
    int  max_level,
    bool add_extra_taxa_parents
) {
    // We are lazy. Just make a copy of the whole taxonomy (they are usually not that big),
    // and add the extra taxa to it. Then, run the standard procedure.
    auto copy = taxonomy;
    for( auto const& tp : extra_taxa ) {
        // Add the necessary parents of the extra taxon, if wanted.
        if( add_extra_taxa_parents ) {
            add_from_taxopath( copy, tp.second );
        }

        // Find the taxon to add the extra one to.
        auto const tax = find_taxon_by_taxopath( copy, tp.second );
        if( ! tax ) {
            // If we added the parent before, we cannot fail, so assure that a failure
            // only happens if the adding of taxon parents is disabled.
            assert( ! add_extra_taxa_parents );
            auto const path = TaxopathGenerator().to_string( tp.second );
            throw std::runtime_error( "Taxopath " + path + " not found in Taxonomy." );
        }

        // Add the extra taxon
        assert( tax );
        tax->add_child( tp.first );
    }

    return taxonomy_to_tree( copy, drop_singleton_inner_nodes, no_inner_node_names, max_level );
}

tree::Tree taxonomy_to_tree(
    std::unordered_map<std::string, Taxopath> const& taxon_map,
    bool drop_singleton_inner_nodes,
    bool no_inner_node_names,
    int  max_level
) {
    // Use an empty dummy taxonomy that gets filled as needed.
    Taxonomy tmp;
    return taxonomy_to_tree(
        tmp, taxon_map, drop_singleton_inner_nodes, no_inner_node_names, max_level, true
    );
}

} // namespace taxonomy
} // namespace genesis
