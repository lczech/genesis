/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
//     Local Helpers
// =================================================================================================

/**
 * @brief Recursive local helper function to add taxa to the tree broker.
 */
static void add_subtaxonomy_(
    Taxonomy const& taxonomy,
    TaxonomyToTreeParams params,
    long parent_level,
    tree::NewickBroker& broker,
    std::vector<Taxon const*>& added_taxa
) {
    using namespace genesis::tree;

    // Helper function to add an element to the broker
    auto add_element_to_broker_ = [&]( Taxon const& taxon, std::string const& name, long level )
    {
        // We add an element to the broker, and set its identifier to the position in the added_taxa
        // vector where we add its corresponding Taxon to. This allows us later to keep track of
        // the broker elements in the order of their taxa.
        auto elem = NewickBrokerElement{ name, level };
        elem.identifier = added_taxa.size();
        added_taxa.push_back( &taxon );
        broker.push_bottom( std::move( elem ));
    };

    for( auto const& taxon : taxonomy ) {
        auto const level = parent_level + 1;

        if( params.max_level > -1 && taxon_level( taxon ) >= static_cast<size_t>( params.max_level )) {
            // Here, we are at a taxon that is at the max level.
            // Do not recurse, but add the name of the taxon to the tree. The taxon is possibly
            // an inner one (phylum, class, etc), which is what we want to happen if there is
            // a max level set.
            add_element_to_broker_( taxon, taxon.name(), level );

        } else if( ! params.keep_singleton_inner_nodes && taxon.size() == 1 ) {
            // Here, we are at a taxon that only contains a single child taxon, which would
            // add a branch to the tree that does not furcate in any way. If the option to drop
            // such entities is set, we simply recurse to the single child, without adding it
            // to the tree.
            add_subtaxonomy_( taxon, params, parent_level, broker, added_taxa );

        } else {
            // Here, we are in the default case: A taxon to be added to the tree.
            // Whether we set a name or not depends on whether it is an inner one or has children.
            std::string const tax_name = (
                ( ! params.keep_inner_node_names && taxon.size() > 0 )
                ? ""
                : taxon.name()
            );
            add_element_to_broker_( taxon, tax_name, level );
            add_subtaxonomy_( taxon, params, level, broker, added_taxa );
        }
    }
}

// =================================================================================================
//     Basic
// =================================================================================================

tree::Tree taxonomy_to_tree(
    Taxonomy const& taxonomy,
    TaxonomyToTreeParams params
) {
    // Forward to the advanced function, with a taxon to node value function that does nothing.
    std::vector<Taxon const*> per_node_taxa;
    return taxonomy_to_tree( taxonomy, per_node_taxa, params );
}

tree::Tree taxonomy_to_tree(
    Taxonomy const& taxonomy,
    std::unordered_map<std::string, Taxopath> const& extra_taxa,
    TaxonomyToTreeParams params,
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

    return taxonomy_to_tree( copy, params );
}

tree::Tree taxonomy_to_tree(
    std::unordered_map<std::string, Taxopath> const& taxon_map,
    TaxonomyToTreeParams params
) {
    // Use an empty dummy taxonomy that gets filled as needed.
    Taxonomy tmp;
    return taxonomy_to_tree( tmp, taxon_map, params, true );
}

// =================================================================================================
//     Advanced
// =================================================================================================

tree::Tree taxonomy_to_tree(
    Taxonomy const& taxonomy,
    std::vector<Taxon const*>& per_node_taxa,
    TaxonomyToTreeParams params
) {
    using namespace genesis::tree;

    // We expect an empty input, to avoid overwriting stuff by accident.
    if( !per_node_taxa.empty() ) {
        throw std::invalid_argument( "Expecting empty per_node_taxa for taxonomy_to_tree()" );
    }

    // Make a broker.
    // Add a dummy root node if the top leven of the taxonomy contains multiple elements.
    // This is needed as we otherwise do not have a root node in the broker, but multiple,
    // which is not allowed.
    NewickBroker broker;
    if( taxonomy.size() > 1 ) {
        broker.push_bottom( NewickBrokerElement{0} );
    }

    // Recursively add taxa. We could try something non-recursive here (see below for an example),
    // but this makes early stopping for the max level a bit difficult.
    // Taxonomies are not that deep, so this should not yield any trouble at all.
    std::vector<Taxon const*> added_taxa;
    add_subtaxonomy_( taxonomy, params, 1, broker, added_taxa );
    broker.assign_ranks();

    // Non-recursive way of adding taxa, that however does not easily support
    // to drop inner branches that do not have any furcation.
    // for( auto const& taxon_it : preorder( taxonomy )) {
    //     auto const lvl = 1 + static_cast<int>( taxon_level( taxon_it.taxon() ));
    //     broker.push_bottom({ taxon_it.taxon().name(), lvl });
    // }

    // We (mis-)use a Common Tree Newick Reader here. It supports to turn a broker into a tree,
    // and takes names into account. This is what we want. It will create branch lengths, too.
    // We can live with that - they are just set to 1. Otherwise we'd have to make a new Tree data
    // type for just this use case, so not really nice, and would probably also lead to problems
    // in typical tree viewing tools, which might expect branch lengths to be present.
    auto newick_reader = CommonTreeNewickReader();

    // Set up the reader such that it uses the NewickBrokerElement identifier to create our taxon
    // map from nodes to taxa. We simply re-assign the taxa we collected above to the correct nodes.
    per_node_taxa.resize( broker.size() );
    newick_reader.element_to_node_plugins.push_back(
        [&]( NewickBrokerElement const& element, TreeNode& node )
        {
            // Check that all indices are in order
            assert( node.index() < per_node_taxa.size() );
            assert( element.identifier < added_taxa.size() );
            assert( !per_node_taxa[node.index()] );
            assert( added_taxa[element.identifier] );

            // The way that the taxa in our taxonomy are added to the broker, and then from there
            // created in the tree is currently implemented in a way where the order is preserved.
            // Hence, the indicies are actually the same, and we could in practice just return
            // the `added_taxa` list as our result here. However, this relies on implementation
            // details in the Newick processing, and we do not want that. So instead, we build the
            // list here with the correct mapping, making this future proof. The assertion here could
            // be used to check the assumption though.
            // assert( node.index() == element.identifier );

            // Set the taxon for the current node.
            per_node_taxa[node.index()] = added_taxa[element.identifier];
        }
    );
    return newick_reader.broker_to_tree( broker );
}

} // namespace taxonomy
} // namespace genesis
