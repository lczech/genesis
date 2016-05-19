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
 * This is the demo "Visualize Placements". See the Manual for more information.
 */

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include "placement/formats/jplace_reader.hpp"
#include "placement/formats/newick_writer.hpp"
#include "placement/placement_tree.hpp"
#include "placement/sample.hpp"

#include "tree/formats/newick/color_writer_mixin.hpp"
#include "tree/default/functions.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"

#include "utils/formats/nexus/document.hpp"
#include "utils/formats/nexus/taxa.hpp"
#include "utils/formats/nexus/trees.hpp"
#include "utils/formats/nexus/writer.hpp"

#include "utils/text/string.hpp"
#include "utils/tools/color.hpp"
#include "utils/tools/color/gradient.hpp"
#include "utils/tools/color/operators.hpp"

using namespace genesis;

// =================================================================================================
//      Count Placement Mass Per Edge
// =================================================================================================

/**
 * @brief Examine all Placements in a Sample and add their like_weight_ratio to the branch where
 * the placement is located.
 *
 * The function loops over all Placements of all Pqueries of the given Sample. For each Placement,
 * it adds the like_weight_ratio to the given vector at the index position of the branch where
 * the Placement is located.
 */
void count_placement_mass_per_edge(
    placement::Sample const& sample,
    std::vector<double>&     placement_mass
) {
    using namespace ::genesis::placement;

    // Check whether the provided vector has the same number of elements as the tree has edges.
    if( placement_mass.size() != sample.tree().edge_count() ) {
        throw std::runtime_error( "Placement mass vector has wrong size." );
    }

    // Loop over all placements of all pqueries of the sample and accumulate the mass.
    for( auto const& pquery : sample ) {
        for( auto const& placement : pquery.placements() ) {
            auto index = placement.edge().index();
            placement_mass[index] += placement.like_weight_ratio;
        }
    }
}

// =================================================================================================
//      Counts To Colorss
// =================================================================================================

/**
 * @brief Given a vector of doubles, return a vector of Colors representing the distribution
 * of the double values.
 *
 * The resulting vector contains a color indicating how high the value of each input double is,
 * compared to the other values. This means: First, we find the highest value in the vector. Then,
 * for all values in the vector, we calculate the log-scaled relative value compared to the highest
 * value. This relative value is then turned into a color gradient.
 *
 * This way, the resulting vector has ligh blue colors for lower numbers, purple for medium numbers,
 * and darker colors up to black for higher numbers (when using the given color gradient scheme).
 */
std::vector<utils::Color> counts_to_colors(
    std::vector<double> const& count_vector
) {
    // Create a color gradient in "blue pink black".
    auto gradient   = std::map<double, utils::Color>();
    gradient[ 0.0 ] = utils::color_from_hex("#81bfff");
    gradient[ 0.5 ] = utils::color_from_hex("#c040be");
    gradient[ 1.0 ] = utils::color_from_hex("#000000");
    auto base_color = utils::color_from_hex("#81bfff");

    // Create the result vector.
    std::vector<utils::Color> color_vector( count_vector.size(), base_color );

    // Find the highest value in the input vector.
    auto mass_max = *std::max_element (count_vector.begin(), count_vector.end());

    // Calculate the resulting colors.
    for( size_t i = 0; i < count_vector.size(); ++i ) {
        if( count_vector[i] > 0.0 ) {
            double log_val = log( static_cast<double>(count_vector[i]) ) / log( mass_max );
            color_vector[i] = utils::gradient( gradient, log_val );
        }
    }

    return color_vector;
}

// =================================================================================================
//     Write Color Tree To Nexus
// =================================================================================================

/**
 * @brief Write a nexus file containing a tree with colored branches.
 *
 * The file format can be read and visualized by, e.g., FigTree.
 *
 * The nexus classes of genesis are currently only rudimentary. They do their job, but are not
 * particularly nice to use. This might change in the future.
 */
void write_color_tree_to_nexus(
    placement::PlacementTree const&  tree,
    std::vector<utils::Color> const& colors_per_branch,
    std::string const&               nexus_filename
) {
    // We use a normal Newick writer for PlacementTrees, but also wrap it in a Color Mixin
    // in order to allow for color annotated branches.
    using ColorWriter = tree::NewickColorWriterMixin<placement::PlacementTreeNewickWriter>;

    // Get the Newick representation of the tree, with color annotated branches.
    auto tree_writer = ColorWriter();
    tree_writer.enable_edge_nums(false);
    tree_writer.edge_colors(colors_per_branch);
    std::string newick_tree = tree_writer.to_string(tree);

    // Create an (empty) Nexus document.
    auto nexus_doc = utils::NexusDocument();

    // Add the taxa of the tree to the document.
    auto taxa = utils::make_unique<utils::NexusTaxa>();
    taxa->add_taxa(node_names(tree));
    nexus_doc.set_block( std::move(taxa) );

    // Add the tree itself to the document.
    auto trees = utils::make_unique<utils::NexusTrees>();
    trees->add_tree( "tree1", newick_tree );
    nexus_doc.set_block( std::move(trees) );

    // Write the document to a Nexus file.
    auto nexus_writer = utils::NexusWriter();
    nexus_writer.to_file( nexus_doc, nexus_filename );
}

// =================================================================================================
//     Main Function
// =================================================================================================

/**
 * @brief Main function that processes jplace files and writes a nexus file with a colored tree.
 *
 * The program takes a path to either a `jplace` file, or a directory containing `jplace` files,
 * reads all of them and counts the placement mass (according to the `like_weight_ratio` of each
 * placement) for each branch of the tree. Those masses are then turned into colors representing a
 * heat gradient of how much placement mass was placed on each branch, and writes a tree with this
 * color information to a given nexus file path. The resulting file can be read and visualized
 * with, e.g., FigTree.
 *
 * If a directory is given as first command line argument, all files in it that end in ".jplace"
 * are processed and their weights are accumulated. This means that all trees in the `jplace` files
 * need to have the same topology. For reasons of simplicity, we only check if they have the correct
 * number of edges. It is thus up to the user to make sure that all trees have identical topology.
 * Otherwise, the result will be meaningless. If for example EPA was run multiple times with
 * different sets of query sequences, but always the same reference tree, the resulting `jplace`
 * files can be used here.
 *
 * If a single file is given as input, all of the above is obsolete. The filename also
 * does not need to end in ".jplace" in this case. In this case, simple this file is visualized.
 *
 * Furthermore, as second command line argument, the user needs to provide a valid filename for the
 * output nexus file. That means, the path to the file needs to exist, but the file not (yet).
 */
int main( int argc, char** argv )
{
    using namespace ::genesis::placement;

    // Activate logging.
    utils::Logging::log_to_stdout();

    // Check if the command line contains the right number of arguments.
    if (argc != 3) {
        throw std::runtime_error(
            "Need to provide two command line arguments:\n"
            "  * A valid path to either a jplace file or a directory containing jplace files.\n"
            "  * An output nexus file name."
        );
    }
    auto input_path = std::string( argv[1] );
    auto nexus_file = std::string( argv[2] );

    // Prepare a vector for all jplace files we want to process.
    std::vector<std::string> jplace_files;

    if( utils::is_dir( input_path ) ) {

        // If the provided path is a directory, find all jplace files in it.
        auto all_files = utils::dir_list_files( input_path );
        utils::erase_if( all_files, [](std::string const& file) {
            return ! utils::ends_with( utils::to_lower( file ), ".jplace" );
        });

        LOG_INFO << "Using " << all_files.size() << " jplace files:";
        for( auto filename : all_files ) {
            LOG_INFO << "- " << filename;

            // Add all found jplace files with the correct path to the vector.
            auto jplace_filename = utils::trim_right( input_path, "/") + "/" + filename;
            jplace_files.push_back( jplace_filename );
        }

    } else if( utils::is_file( input_path )) {

        // If the provided path is a file, use this (no check if it ends in .jplace,
        // because it might be storted with a differnet file name).
        jplace_files.push_back( input_path );
        LOG_INFO << "Using jplace file " << input_path << ".";

    } else {
        throw std::runtime_error( "Invalid path: " + input_path );
    }

    // Prepare a vector that will contain the masses per edge, summed over all samples that we
    // want to process.
    std::vector<double> placement_mass;

    // Prepare a tree object which we will use to store one of the trees of the jplace files.
    // The topology of this tree is then used to create the final Nexus file.
    PlacementTree tree;

    // Process all provided jplace files.
    for( auto const& jplace_filename : jplace_files ) {

        // Read the Jplace file into a Sample object.
        Sample sample;
        JplaceReader().from_file(jplace_filename, sample);

        if( placement_mass.size() == 0 ) {

            // In the first iteration of this loop (which might be the only one, if we only process
            // one file), we need to initialize the vector to the actual number of edges of the tree.
            // We also make a copy of the tree, for later usage.
            placement_mass = std::vector<double>( sample.tree().edge_count(), 0.0 );
            tree = sample.tree();

        } else if( placement_mass.size() != sample.tree().edge_count() ) {

            // In later iterations (when the vector already has a size > 0), we check if it has the
            // correct size. If not, the trees of the jplace files have different size.
            throw std::runtime_error( "" );
        }

        // Call the function for counting the masses per edge.
        count_placement_mass_per_edge( sample, placement_mass );
    }

    // Once all jplace files have been processed, we can prepare and write the final color tree.
    auto colors_per_branch = counts_to_colors( placement_mass );

    LOG_INFO << "Writing to nexus file " << nexus_file;
    write_color_tree_to_nexus( tree, colors_per_branch, nexus_file );

    LOG_INFO << "Finished.";
    return 0;
}
