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
 * This is the demo "Labelled Tree". See the Manual for more information.
 */

#include "genesis/genesis.hpp"

#include <string>

/**
 * @brief
 */
int main( int argc, const char* argv[] )
{
    // -----------------------------------------------------
    //     Init and Settings.
    // -----------------------------------------------------

    using namespace genesis;
    using namespace genesis::placement;

    // Activate logging, print genesis header.
    utils::Logging::log_to_stdout();
    LOG_BOLD << genesis_header();

    // Settings for producing the labelled tree. See documentation for labelled_tree() for details.
    bool const        fully_resolve = true;
    std::string const name_prefix   = "";

    // -----------------------------------------------------
    //     Read and validate input.
    // -----------------------------------------------------

    // Check if the command line contains the right number of arguments.
    if( argc != 3 && argc != 4 ) {
        LOG_ERR << "Need to provide a jplace file path, optionally a newick tree file path, "
                << "and an output Newick tree file path as command line arguments.";
        return 1;
    }
    std::string jplace_filename;
    std::string bstree_filename;
    std::string output_filename;

    if( argc == 3 ) {
        jplace_filename = std::string( argv[1] );
        output_filename = std::string( argv[2] );
    }
    if( argc == 4 ) {
        jplace_filename = std::string( argv[1] );
        bstree_filename = std::string( argv[2] );
        output_filename = std::string( argv[3] );
    }

    // Prepare a Jplace reader that reports wrong values.
    auto jplace_reader = JplaceReader();
    jplace_reader.invalid_number_behaviour(
        genesis::placement::JplaceReader::InvalidNumberBehaviour::kLogAndCorrect
    );

    // Read the files.
    LOG_INFO << "Jplace file: " << jplace_filename;
    auto sample = jplace_reader.from_file( jplace_filename );
    tree::Tree bstree;
    if( argc == 3 ) {
        bstree = sample.tree();
    }
    if( argc == 4 ) {
        LOG_INFO << "Bootstrap Tree file: " << bstree_filename;
        bstree = tree::DefaultTreeNewickReader().from_file( bstree_filename );
    }

    // Validation. The second one is also done in labelled_tree(), but we do it here again
    // to give a nicer output.
    if( ! validate( sample, true, false )) {
        LOG_WARN << "Invalid jplace sample.";
        return 1;
    }
    if( argc == 4 && ! tree::identical_topology( bstree, sample.tree() )) {
        LOG_WARN << "Provided Bootstrap Tree does not have the same topology as the tree in the "
                 << "Jplace file";
        return 1;
    }

    // -----------------------------------------------------
    //     Produce a labelled tree.
    // -----------------------------------------------------

    auto l_tree = labelled_tree( sample, bstree, fully_resolve, name_prefix );
    tree::DefaultTreeNewickWriter().to_file( l_tree, output_filename );
    LOG_INFO << "Done.";

    return 0;
}
