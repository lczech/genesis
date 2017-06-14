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
 * This is the demo "Extract Clade Placements". See the Manual for more information.
 */

#include "genesis/genesis.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace genesis;

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Contains a list of clades, each itself containing a list of taxa belonging to that clade.
 */
using CladeTaxaList = std::unordered_map<std::string, std::vector<std::string>>;

/**
 * @brief A jplace reference tree.
 */
using TreeType      = placement::PlacementTree;

/**
 * @brief Contains a list of clades, each itself containing a list of edge indices belonging to
 * that clade. We use a vector to maintain the order of the clades.
 */
using CladeEdgeList = std::vector<std::pair<std::string, std::unordered_set<size_t>>>;

// =================================================================================================
//     Get Clade Taxa Lists
// =================================================================================================

/**
 * @brief Return a list of clades, each containing a list of taxa.
 *
 * The function takes a file path as input. Each line contains a tab-separated entry that maps
 * from a taxon of the tree to the clade name that this taxon belongs to:
 *
 *     Taxon_1 <tab> clade_a
 *
 * (where the " <tab> " of course is just a single tab character).
 *
 * The return value of this function is a map from clade names to a vector of taxa names.
 */
CladeTaxaList get_clade_taxa_lists( std::string const& clade_filename )
{
    auto csv_reader = utils::CsvReader();
    csv_reader.separator_chars( "\t" );

    // Create a list of all clades and fill each clade with its taxa.
    CladeTaxaList clades;
    auto table = csv_reader.from_file( clade_filename );
    for( auto const& line : table ) {
        if( line.size() != 2 ) {
            throw std::runtime_error( "Invalid line in clade file." );
        }

        // Add the taxon to its clade.
        clades[ line[1] ].push_back( line[0] );
    }

    return clades;
}

// =================================================================================================
//     Get Clade Edges
// =================================================================================================

/**
 * @brief Return a list clades, each itself containing a list of edge indices of that clade.
 *
 * The function takes a list of clades with their taxa as input, and a reference tree.
 * It then inspects all clades and findes the edges of the tree that belong into a clade.
 * Furthermore, a clade "basal_branches" is added for those edges of the tree that do not
 * belong to any clade.
 *
 * The edges of a clade are determined by finding the smalles subtree (split) of the tree that
 * contains all nodes of the clade. That means, the clades should be monophyletic in order for this
 * algorithm to work properly.
 */
CladeEdgeList get_clade_edges( CladeTaxaList const& clades, TreeType& tree )
{
    // Prepare the result map.
    CladeEdgeList clade_edges;

    // Make a set of all edges that do not belong to any clade (the basal branches of the tree).
    // We first fill it with all edge indices, then remove the clade-edges later,
    // so that only the wanted ones remain.
    std::unordered_set<size_t> basal_branches;
    for( auto it = tree.begin_edges(); it != tree.end_edges(); ++it ) {
        basal_branches.insert( (*it)->index() );
    }

    // Process all clades.
    for( auto const& clade : clades ) {

        // Find the nodes that belong to the taxa of this clade.
        std::vector< tree::TreeNode* > node_list;
        for( auto const& taxon : clade.second ) {
            tree::TreeNode* node = find_node( tree, taxon );
            if( node == nullptr ) {
                LOG_WARN << "Cannot find taxon " << taxon;
                continue;
            }
            node_list.push_back( node );
        }

        // Find the edges that are part of the subtree of this clade.
        // This part is a bit messy and might be cleaned up in the future.
        auto bipartitions = tree::BipartitionSet( tree );
        auto smallest     = bipartitions.find_smallest_subtree( node_list );
        auto subedges     = bipartitions.get_subtree_edges( smallest->link() );

        // Add them to the clade edges list.
        clade_edges.push_back( std::make_pair( clade.first, subedges ));

        // Remove the edge indices of this clade from the basal branches (non-clade) edges list.
        for( auto edge : subedges ) {
            basal_branches.erase( edge );
        }
    }

    // Now that we have processed all clades, also add the non-clade edges (basal branches)
    // to the list as a special clade "basal_branches". This way, all edges of the reference tree
    // are used by exaclty one clade.
    clade_edges.push_back( std::make_pair( "basal_branches", basal_branches ));

    return clade_edges;
}

// =================================================================================================
//     Extract Pqueries
// =================================================================================================

/**
 * @brief Take a list of edges per clade and a Sample and return a SampleSet with single samples
 * for all given clades, where each sample contains those pqueries that fell into the clade with
 * a given threshold.
 *
 * This is the main extraaction method. The returned SampleSet also contains an additional Sample
 * "uncertain", where all pqueries of the provided sample end up which do not have more than
 * `threshold` of their placement mass in a certain clade.
 */
placement::SampleSet extract_pqueries(
    CladeEdgeList const&     clade_edges,
    placement::Sample const& sample,
    double                   threshold
) {
    using namespace ::genesis::placement;

    // Create a sample set, with one sample per clade.
    // We will then copy the pqueries of the provided sample into the correct sample of this set.
    SampleSet sample_set;
    for( auto const& clade : clade_edges ) {
        sample_set.add( Sample( sample.tree() ), clade.first);
    }

    // Also add a special `uncertain` sample, that will collect all pqueries for which there is no
    // clade with more than `threshold` percent of the placement mass.
    sample_set.add( Sample( sample.tree() ), "uncertain" );

    // Process all pqueries of the given sample.
    for( auto const& pquery : sample ) {

        // Prepare an accumulator that collects the mass per clade for this pquery.
        // The indices in the vector are the same as the ones in the clade_edge vector.
        std::vector<double> mass_per_clade ( clade_edges.size(), 0.0 );

        // For each placement, find its edge and the clade that this edge belongs to.
        // For this clade, accumulate the placement's like weight ratio.
        for( auto const& placement : pquery.placements() ) {

            // Find the correct clade for the edge of this placement by scanning them all
            // (this is a bit inefficient, but for now, it works).
            for( size_t i = 0; i < clade_edges.size(); ++i ) {
                if( clade_edges[i].second.count( placement.edge().index() ) > 0 ) {

                    // If we found the correct clade, accumulate to its mass.
                    mass_per_clade[i] += placement.like_weight_ratio;
                }
            }
        }

        // Now check whether there is a clade that has euqal or more than `threshold` percent
        // of the placement's weight ratio. If so, this is the one we assign the pquery to.
        bool found_clade = false;
        for( size_t i = 0; i < mass_per_clade.size(); ++i ) {
            // Only execute the main body of this loop when we found a fitting clade.
            if( mass_per_clade[i] < threshold ) {
                continue;
            }
            found_clade = true;

            // Get the sample from the set that has the current clade's name.
            auto sample_ptr = find_sample( sample_set, clade_edges[i].first );
            if( sample_ptr == nullptr ) {
                throw std::runtime_error( "Sample disappeared! Magic." );
            }
            auto& clade_sample = *sample_ptr;

            // Add a copy of the pquery to the sample.
            clade_sample.add( pquery );
        }

        // If there is no sure assignment ( < threshold ) for this pquery, we copy it into the
        // special `uncertain` sample.
        if( ! found_clade ) {
            auto& uncertain_sample = *find_sample( sample_set, "uncertain" );
            uncertain_sample.add( pquery );
        }
    }

    return sample_set;
}

// =================================================================================================
//     Write Sample Set
// =================================================================================================

/**
 * @brief Take a SampleSet and a directory and write all Samples in the set to jplace files
 * in that directory, named after the sample name in the set.
 */
void write_sample_set( placement::SampleSet const& sample_set, std::string output_dir )
{
    using namespace ::genesis::placement;

    auto writer = JplaceWriter();
    for( auto const& named_sample : sample_set ) {
        writer.to_file( named_sample.sample, output_dir + named_sample.name + ".jplace" );
    }
}

// =================================================================================================
//     Main Function
// =================================================================================================

/**
 * @brief Main function that processes a jplace file and writes single jplace files for different
 * clades of the reference tree, each file containing the pqueries that fell into the clade with
 * more than a given threshold of accumulated likelihood weights.
 *
 * The program takes three input arguments in the following order:
 *
 *   1. A `jplace` input file. The pqueries in this file are then split into different samples. Each
 *      such sample contains all pqueries whose placements are placed in a certain clade of the
 *      reference tree with more than a cutoff threshold of their accumulated likelihood weight.
 *
 *      According to the `jplace` standard, each pquery can have multiple possible placement positions.
 *      Each position has a value `like_weight_ratio`, which can be interpreted as a measure of
 *      probability of how likely the placement belongs to the branch that it is attached to.
 *      The ratios for all branches of the tree thus sum up to 1.0.

 *      If more of this placement mass than the threshold is placed on the branches of a single
 *      clade of the tree, the according pquery is assigned to that clade. The threshold is
 *      hardcoded in this demo and set to 0.95 (but can be changed if needed, of course).
 *
 *      It is possible that the placement algorithm (e.g., EPA or pplacer) did not output placements
 *      with low like_weight_ratios, depending on the selected options (see the respective manual
 *      for more details on how to change this). This means that the provided sum might be lower
 *      than 1.0 for some pqueries. In order to compensate for this (thus, to avoid classifying
 *      those pqueries as uncertain), we normalize the like_weight_ratios first, so that their sum
 *      is 1.0 again. This step thus ignores the uncertainties resulting from the placement
 *      algorithm.
 *   2. A path to a file, which needs to contain a single line for each taxon of the reference tree.
 *      Each line needs to contain a tab-separated entry that maps from a taxon of the tree to the
 *      clade name that this taxon belongs to:
 *
 *          Taxon_1 <tab> clade_a
 *
 *      (where the " <tab> " of course is just a single tab character).
 *      The taxa names need to be the same as the node names of the reference tree in the `jplace`
 *      file.
 *
 *      If a taxon in a clade file is not found on the tree, a warning is issued, and the taxon is
 *      ignored. If the tree contains taxa which are not in any clade file, those branches are
 *      assigned to a special clade "basal_branches". This is also the case for the inner branches
 *      of the tree: all those branches which do not belong to one of the clades are collected in
 *      this special clade.
 *
 *      As a second special clade, the "uncertain" clade is used to collect all those pqueries
 *      which did not fall into any clade with more than the threshold of accumulated likelihood
 *      weights.
 *
 *      The edges that belong to a clade are determined by finding the smalles subtree (split) of
 *      the tree that contains all nodes of the clade. That means, the clades should be monophyletic
 *      in order for this algorithm to work properly. Furthermore, the user needs to make sure that
 *      each taxon is contained in at most one clade. Otherwise, the algorithm won't work properly.
 *
 *      Remark: The rooting of the tree is insignificant for this program. Even if the root
 *      coincidentally lies within one of the clades, the result is the same. The program does not
 *      change the root; thus, when visualizing the clades, be aware that the tree might look
 *      different depending on the rooting.
 *   3. An output directory path. For each clade (including the two special clades), a `jplace` file
 *      named after the clade is written to that path. Each `jplace` file then contains all pqueries
 *      that were assigned to that clade.
 *
 * A typical use case for this program is to extract pqueries that were placed in a particular
 * clade of interest in an evolutionary placement analysis. The extracted placements can then be
 * further examined in downstream analyses.
 *
 * It is also possible to do a second run of evolutionary placement with the original sequences of
 * the pqueries of one clade, using a refined reference tree for that clade with a higher resolution
 * (more reference taxa). This two-step placement approach allows for finely grained
 * placement positions while keeping the computational load relatively small.
 */
int main( int argc, char** argv )
{
    using namespace ::genesis::placement;

    // Threshold for how much placement mass needs to be in one clade
    // in order to assign a pquery to it.
    const double threshold = 0.95;

    // Activate logging, print genesis header.
    utils::Logging::log_to_stdout();
    LOG_BOLD << genesis_header();

    // Check if the command line contains the right number of arguments and store them.
    if (argc != 4) {
        throw std::runtime_error(
            "Need to provide three command line arguments:\n"
            "  * An input jplace file path.\n"
            "  * A clade file.\n"
            "  * An output directory path."
        );
    }
    auto jplace_filename = std::string( argv[1] );
    auto clade_filename  = std::string( argv[2] );
    auto output_dir      = utils::trim_right( std::string( argv[3] ), "/") + "/";

    // Some user output.
    LOG_INFO << "Using jplace file      " << jplace_filename;
    LOG_INFO << "Using clade file       " << clade_filename;
    LOG_INFO << "Using output directory " << output_dir;

    // Read the taxa of all clades.
    auto clades = get_clade_taxa_lists( clade_filename );
    LOG_INFO << "Found " << clades.size() << " clades";

    // Read the Jplace file into a Sample object.
    Sample sample = JplaceReader().from_file( jplace_filename );

    // Normalize the like_weight_ratios. This step makes sure that missing placement weights do not
    // lead to a pquery being placed in the uncertain clade. That means, we only use the provided
    // placement masses as given in the jplace files, and scale them so that they sum up to 1.0.
    // In turn, this means that uncertainties resulting from the placement algorithm are ignored.
    normalize_weight_ratios( sample );

    // Get a list of the edges per clade of the reference tree.
    auto clade_edges = get_clade_edges( clades, sample.tree() );

    // Get a sample set that contains a sample per clade.
    // Each sample then has the pqueries from the original sample that fell into that clade.
    auto sample_set = extract_pqueries( clade_edges, sample, threshold );

    // Write everything to jplace files.
    write_sample_set( sample_set, output_dir );

    LOG_INFO << "Finished.";
    return 0;
}
