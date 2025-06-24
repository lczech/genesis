#ifndef GENESIS_TAXONOMY_FUNCTIONS_TREE_H_
#define GENESIS_TAXONOMY_FUNCTIONS_TREE_H_

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

#include "genesis/tree/tree.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;
class Taxopath;

// =================================================================================================
//     Parameters
// =================================================================================================

/**
 * @brief Parameters for taxonomy_to_tree()
 *
 * This struct simply collects the different parameters for the function,
 * in order to have a nicer interface. See there for details.
 */
struct TaxonomyToTreeParams
{
    /**
     *  @brief Keep singleton inner nodes.
     *
     * It might happen that a taxonomic path goes down several levels with just one taxon at each level.
     * This would create inner nodes in the tree that just connect two other nodes, that is, nodes that
     * do not furcate at all. Many downstream programs might have problems with such trees.
     * By default, such nodes are collapsed. `keep_singleton_inner_nodes` can be used to include these
     * inner nodes in the tree, instead of immediately adding their children.
     */
    bool keep_singleton_inner_nodes = false;

    /**
     * @brief Kepp inner node names.
     *
     * A Taxonomy contains names at every level, while a Tree usually does not contain
     * inner node names. Thus, inner node are not named by default. Use `keep_inner_node_names`
     * to still set the inner taxonomic labels in the tree.
     */
    bool keep_inner_node_names = false;

    /**
     * @brief Maximum level of the taxonomy to use for the tree.
     *
     * This be used to only turn the first few (highest) levels (starting at 0) of the
     * Taxonomy to the tree, and stopping after that. By default, the whole Taxonomy (all levels)
     * is turned into a Tree.
     */
    int max_level = -1;
};

// =================================================================================================
//     Basic
// =================================================================================================

/**
 * @brief Turn a Taxonomy into a (possibly multifurcating) Tree.
 *
 * A Taxonomy is a hierarchy that can be interpreted as a rooted tree. Using this function, such
 * a tree is created and returned. It can be used to construct a taxonomic constraint tree
 * for tree inference.
 */
tree::Tree taxonomy_to_tree(
    Taxonomy const& taxonomy,
    TaxonomyToTreeParams params = TaxonomyToTreeParams{}
);

/**
 * @brief Turn a Taxonomy into a (possibly multifurcating) Tree, and allow to add extra tips to it.
 *
 * This is similar to the general version of this function, see
 * @link taxonomy_to_tree( Taxonomy const&, bool, bool, int ) taxonomy_to_tree()@endlink.
 * It however allows a special feature: A mapping from extra taxon names to @link Taxon Taxa@endlink
 * in the given Taxonomy.
 *
 * This is useful if the Taxonomy is used for a set of sequences that have taxonomic assignments:
 * One might wish to build a tree where tips correspond to sequences, and the tree topology
 * reflects the taxonomy of these sequences. For such a use case, this function can use the
 * Taxonomy of the sequences, as well as a mapping of sequences names to Taxopath%s.
 * The output tree will then contain "extra taxa" that are made up of the sequences names,
 * added as children to the Taxonomy (and hence, added as tips to the tree).
 *
 * The parameter @p add_extra_taxa_parents defaults to `true`, meaning that the parent taxa of
 * the @p extra_taxa are added to the Taxonomy if not already present in the @p taxonomy.
 * If set to `false`, the @p taxonomy is expected to already contain all paths that are found in
 * the @p extra_taxa, and will throw if this is not the case.
 *
 * See also
 * @link taxonomy_to_tree( std::unordered_map<std::string, Taxopath> const&, bool, bool, int ) taxonomy_to_tree()@endlink for a simplified version of this function that also explains some
 * more details of the workings.
 */
tree::Tree taxonomy_to_tree(
    Taxonomy const& taxonomy,
    std::unordered_map<std::string, Taxopath> const& extra_taxa,
    TaxonomyToTreeParams params = TaxonomyToTreeParams{},
    bool add_extra_taxa_parents = true
);

/**
 * @brief Turn a list of Taxa into a (possibly multifurcating) Tree.
 *
 * The function is a simplified version of
 * @link taxonomy_to_tree( Taxonomy const&, std::unordered_map<std::string, Taxopath> const&, bool, bool, int, bool ) taxonomy_to_tree()@endlink, that does not take a given Taxonomy, but instead
 * just a list if (tip) Taxa and their Taxopath%s. All keys in @p taxon_map are added as new
 * tips to a Taxonomy that is created from the the Taxopath%s (mapped values) of @p taxon_map.
 *
 * This is for example again useful if one has a set of sequences with taxonomy assignment,
 * and wants to build a taxonomic constraint for inferring a tree from these sequences:
 * Given the sequence names as keys, and their taxonomic paths as mapped values, the function
 * creates a (possibly multifurcating) tree that can be used as such constraint.
 */
tree::Tree taxonomy_to_tree(
    std::unordered_map<std::string, Taxopath> const& taxon_map,
    TaxonomyToTreeParams params = TaxonomyToTreeParams{}
);

// =================================================================================================
//     Advanced
// =================================================================================================

/**
 * @brief Turn a Taxonomy into a Tree, and gather all @link Taxon Taxa@endlink in the node order
 * of the Tree.
 *
 * This function is for advanced use cases on top of the basic functionality of converting
 * a Taxonomy into a Tree. For some purposes, it might be necessary to transfer data from the
 * Taxonomy to the Tree, or otherwise be able to obtain a mapping of which taxa ended up as which
 * nodes in the tree.
 *
 * To this end, this function fills the given @p per_node_taxa vector with the @link Taxon Taxa@endlink
 * of the Taxonomy that were used to build the Tree, in the order of the Tree node indices.
 * This one-to-one mapping of nodes in the tree to the taxa those nodes were created from can be
 * used downstream. It allows to process the taxon data (via the pointers in that list) in the same
 * order as the tree (via its node indices), and make use of the taxon data, for instance when
 * visualizing the tree.
 */
tree::Tree taxonomy_to_tree(
    Taxonomy const& taxonomy,
    std::vector<Taxon const*>& per_node_taxa,
    TaxonomyToTreeParams params = TaxonomyToTreeParams{}
);

} // namespace taxonomy
} // namespace genesis

#endif // include guard
