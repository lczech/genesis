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

#include "genesis/genesis.hpp"

#include <stdexcept>

int main()
{
    using namespace genesis::placement;
    using namespace genesis::tree;
    using namespace genesis::utils;

    // Get all jplace files in a directory and read them
    auto const jplace_files = dir_list_files( "/home/lucas/Projects/bacardi/03_bv/03_epa/orig_queries_jplace_clean", true, ".*\\.jplace$" );

    // auto const jplace_files = dir_list_files( "path/to/files", true, "\\.jplace$" );
    SampleSet sample_set = JplaceReader().read( from_files( jplace_files ));

    // ------------------------------------------
    //     Edge PCA
    // ------------------------------------------

    // Run Edge PCA
    EpcaData epca_data = epca( sample_set );

    // Get a reference tree and use it to visualize the first two eigenvectors.
    auto const& tree = sample_set.at(0).tree();
    for( size_t c = 0; c < 2; ++c ) {

        // We use a diverging color palette that is scaled for the eigenvectors.
        auto color_map = ColorMap( color_list_spectral() );
        auto color_norm = ColorNormalizationDiverging();
        color_norm.autoscale( epca_data.eigenvectors.col( c ));
        color_norm.make_centric();

        // Get the colors for the column we are interested in. This is just a temporary list.
        auto const eigen_color_vector = color_map( color_norm, epca_data.eigenvectors.col( c ));

        // Init colors with a neutral gray, signifying that these edges do not have a value.
        // This will be used in particular for the tip edges, as they are filtered out in Edge PCA.
        std::vector<Color> color_vector( tree.edge_count(), Color( 0.9, 0.9, 0.9 ));

        // For each edge that has an eigenvector, get its color and store it.
        // We need to do this because the filtering of const columns (such as tip edges)
        // that is applied during the Edge PCAmight have removed some.
        for( size_t i = 0; i < epca_data.edge_indices.size(); ++i ) {
            auto const edge_index = epca_data.edge_indices[i];
            color_vector[ edge_index ] = eigen_color_vector[i];
        }

        // Write the tree to an svg file, including the color scale legend.
        auto const tree_fn = "path/to/edge_pca_tree_" + std::to_string( c ) + ".svg";
        write_color_tree_to_svg_file( tree, {}, color_vector, color_map, color_norm, tree_fn );
    }

    // Write out projection as a CSV file
    auto const proj_fn = "path/to/projection.csv";
    std::ofstream proj_os;
    genesis::utils::file_output_stream( proj_fn, proj_os );
    for( size_t r = 0; r < epca_data.projection.rows(); ++r ) {
        proj_os << sample_set.name_at(r);
        for( size_t c = 0; c < epca_data.projection.cols(); ++c ) {
            proj_os << "," << epca_data.projection( r, c );
        }
        proj_os << "\n";
    }
    proj_os.close();

    // ------------------------------------------
    //     Squash Clustering
    // ------------------------------------------

    // Convert to Mass Trees
    std::vector<Tree> mass_trees;
    for( auto const& sample : sample_set ) {
        mass_trees.push_back( convert_sample_to_mass_tree( sample, true ).first );
    }

    // Run Squash Clustering
    auto squash_clustering = SquashClustering();
    squash_clustering.run( std::move( mass_trees ) );

    // Write out the resulting cluster tree
    std::ofstream sc_tree_os;
    file_output_stream( "path/to/cluster.newick", sc_tree_os );
    sc_tree_os << squash_clustering.tree_string( sample_set.names() );
    sc_tree_os.close();

    // ------------------------------------------
    //     Edge Dispersion and Edge Correlation
    // ------------------------------------------

    // Get empty matrices of the correct size.
    auto edge_masses     = Matrix<double>( sample_set.size(), tree.edge_count() );
    auto edge_imbalances = Matrix<double>( sample_set.size(), tree.edge_count() );

    // Fill them with the data.
    for( size_t i = 0; i < sample_set.size(); ++i ) {
        auto const& sample = sample_set[i];
        edge_masses.row( i )     = placement_mass_per_edges_with_multiplicities( sample );
        edge_imbalances.row( i ) = epca_imbalance_vector( sample );
    }

    auto means_deviations = std::vector<MeanStddevPair>( edge_imbalances.cols(), { 0.0, 0.0 } );
    for( size_t c = 0; c < edge_imbalances.cols(); ++c ) {
        means_deviations[c] = mean_stddev( edge_imbalances.col(c).begin(), edge_imbalances.col(c).end() );
    }

    // ------------------------------------------
    //     Phylogenetic k-means and Imbalance k-means
    // ------------------------------------------

    // Run the clustering
    size_t const k = 10;
    auto phylogenetic_kmeans = MassTreeKmeans();
    phylogenetic_kmeans.run( mass_trees, k );

    // Get results
    auto const cluster_info = phylogenetic_kmeans.cluster_info( mass_trees );
    auto const assignments = phylogenetic_kmeans.assignments();

    // Write results
    std::cout << "Sample\tCluster\tDistance\n";
    for( size_t i = 0; i < assignments.size(); ++i ) {
        std::cout << sample_set.name_at(i);
        std::cout << "\t" << assignments[i];
        std::cout << "\t" << cluster_info.distances[i];
        std::cout << "\n";
    }
}
