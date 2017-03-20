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
 * This is the demo "Compare Jplace Files". See the Manual for more information.
 */

#include "genesis/genesis.hpp"

#include <string>
#include <unordered_map>
#include <numeric>
#include <algorithm>

using namespace genesis;

/**
 * @brief Main function that processes two jplace files and compares them.
 *
 * This program is useful to compare two `jplace` files that were computed with different settings
 * or even different programs (EPA or pplacer). It is not meant for evaluating differences in the
 * microbial communities that are represented by the input. Instead, it is meant for files that
 * share @link Pquery Pqueries@endlink (identified by their names), and gives information about
 * differences between the Placements in those Pqueries.
 *
 * The program takes two input `jplace` file paths as input. It compares the
 * @link Pquery Pqueries@endlink and their Placements and prints two tables:
 *
 *   1. An overview table that lists all Pqueries of the two files that have a PqueryName in common.
 *      This table indicates whether the top Placement (the one with the highest `like_weight_ratio`)
 *      of both Pqueries is the same (i.e., is located at the same branch); it furthermore indicates
 *      whether all Placements (sorted by their `like_weight_ratio`) are the same, that is, if they
 *      are located on the same branches. Lastly, the difference in log-likelhood and the
 *      @link earth_movers_distance() Earth Movers Distance@endlink between the Pqueries is printed.
 *   2. A detail table that lists all Placements of the Pqueries that were marked invalid in the
 *      overview table - that is, if either the top rank or any other placement was not equally
 *      placed in a Pquery. This table lists the Placements for such Pqueries, sorted by their
 *      `like_weight_ratio`, and shows on which branches (edge_num) they are placed in the two
 *      Pqueries. If the Placements are on the same branch, they are considered correct.
 *
 * The program expects that the reference Tree%s of the input are topologically identical. In order
 * to compensate for differences in branch lengths, both Trees are normalized in the beginning, so
 * that their length (sum of branch lengths) is 1.0. This also means that the Earth Movers Distance
 * yields comparable values in the range `[ 0.0, 1.0 ]`.
 */
int main( int argc, const char* argv[] )
{
    // -----------------------------------------------------
    //     Init and Settings.
    // -----------------------------------------------------

    using namespace ::genesis::placement;

    // Activate logging, print genesis header.
    utils::Logging::log_to_stdout();
    LOG_BOLD << genesis_header();

    // Chars to be used in the output tables. Configure to your wishes.
    std::string valid_char   = "X";
    std::string invalid_char = "-";
    // auto valid_style   = utils::Style();
    // auto invalid_style = utils::Style();
    auto valid_style   = utils::Style( "Green" );
    auto invalid_style = utils::Style( "Red" );

    // -----------------------------------------------------
    //     Read and validate input.
    // -----------------------------------------------------

    // Check if the command line contains the right number of arguments.
    if (argc != 3) {
        LOG_ERR << "Need to provide two jplace file paths as command line arguments.";
        return 1;
    }
    auto jplace_filename_l = std::string( argv[1] );
    auto jplace_filename_r = std::string( argv[2] );

    // Prepare a Jplace reader that reports wrong values.
    auto jplace_reader = JplaceReader();
    jplace_reader.invalid_number_behaviour(
        genesis::placement::JplaceReader::InvalidNumberBehaviour::kLogAndCorrect
    );

    // Read the files.
    std::cout << "Jplace file 1: " << jplace_filename_l << "\n";
    Sample smpl_l = jplace_reader.from_file( jplace_filename_l );
    std::cout << "Jplace file 2: " << jplace_filename_r << "\n";
    Sample smpl_r = jplace_reader.from_file( jplace_filename_r );
    std::cout << "\n";

    // Validation.
    if( ! validate( smpl_l, true, false ) || ! validate( smpl_r, true, false ) ) {
        LOG_WARN << "Invalid jplace samples.";
        return 1;
    }
    if( ! compatible_trees( smpl_l, smpl_r )) {
        LOG_ERR << "Reference trees not compatible.";
        return 1;
    }

    // -----------------------------------------------------
    //     Collect infos and prepare results.
    // -----------------------------------------------------

    // Collect the EMD distances and other parameters to make statistics about the results.
    std::vector<double> emd_results;
    auto tree_len_l = tree::length(smpl_l.tree());
    auto tree_len_r = tree::length(smpl_r.tree());
    std::vector<std::string> names_of_invalid_pqueries;

    // Prepare samples for the emd calculation by specifiying the underlying trees
    // here we also normalize the tree branches such that the sum total is 1 in each
    scale_all_branch_lengths( smpl_l, 1.0 / tree_len_l );
    scale_all_branch_lengths( smpl_r, 1.0 / tree_len_r );
    auto emd_smpl_l = Sample( smpl_l.tree() );
    auto emd_smpl_r = Sample( smpl_r.tree() );

    std::cout << "Total tree length 1: " << tree_len_l << "\n";
    std::cout << "Total tree length 2: " << tree_len_r << "\n\n";

    // For speedup, create maps from names of the samples to its pquery indices.
    auto name_map_l = std::unordered_map< std::string, size_t >();
    auto name_map_r = std::unordered_map< std::string, size_t >();
    for( size_t i = 0; i < smpl_l.size(); ++i ) {
        for( auto const& name_l : smpl_l.at(i).names() ) {
            name_map_l[ name_l.name ] = i;
        }
    }
    for( size_t i = 0; i < smpl_r.size(); ++i ) {
        for( auto const& name_r : smpl_r.at(i).names() ) {
            name_map_r[ name_r.name ] = i;
        }
    }

    // Prepare the output overview table.
    utils::Table overview_table;
    overview_table.add_column( "Pquery" );
    overview_table.add_column( "Top equal" ).justify( utils::Table::Column::Justification::kCentered );
    overview_table.add_column( "All equal" ).justify( utils::Table::Column::Justification::kCentered );
    overview_table.add_column( "avg logLH delta" ).justify( utils::Table::Column::Justification::kRight );
    overview_table.add_column( "EMD" ).justify( utils::Table::Column::Justification::kRight );

    // -----------------------------------------------------
    //     Compare both samples and create Overview Table.
    // -----------------------------------------------------

    // Iterate all pqueries of the left sample and find the equivalent pqueries of the right sample.
    for( auto& pqry_l : smpl_l ) {
        for( auto const& name_l : pqry_l.names() ) {

            // Check whether the right sample has a pquery with that name, and get it.
            if( name_map_r.count( name_l.name ) == 0 ) {
                continue;
            }
            auto& pqry_r = smpl_r.at( name_map_r[ name_l.name ]);

            /* == some straight forward evaluation: do they place on the same branches? == */

            // first sort the placements by LWR
            sort_placements_by_weight(pqry_l);
            sort_placements_by_weight(pqry_r);

            // check if top placement of both sequences are on the same branch
            auto const& place_l = pqry_l.placement_at(0);
            auto const& place_r = pqry_r.placement_at(0);
            bool top_correct = false;
            if (place_l.edge_num() == place_r.edge_num()) {
                top_correct = true; // correct!
            }

            // check if all ranks (up to min(max of ranks)) are equal in terms of placed edge
            bool all_ranks_equal = true;
            for( size_t i = 0; i < std::min(pqry_l.placement_size(), pqry_r.placement_size()); ++i ) {
                auto const& place_l = pqry_l.placement_at(i);
                auto const& place_r = pqry_r.placement_at(i);
                if (place_l.edge_num() != place_r.edge_num()) {
                    all_ranks_equal = false;
                    // abort on first incorrect placement.
                    break;
                }
            }

            // Add to incorrect list if necessary.
            if( ! top_correct || ! all_ranks_equal ) {
                names_of_invalid_pqueries.push_back( name_l.name );
            }

            // Lets also output some actual values of likelihood differences. One for each pquery,
            // so lets take the average.
            std::vector<double> LH_deltas;
            auto num_ranks = std::min(pqry_l.placement_size(), pqry_r.placement_size());
            for( size_t i = 0; i < num_ranks; ++i ) {
                auto const& place_l = pqry_l.placement_at(i);
                auto const& place_r = pqry_r.placement_at(i);
                LH_deltas.push_back(fabs(place_l.likelihood - place_r.likelihood));
            }
            auto avg_LH_delta = std::accumulate(LH_deltas.begin(), LH_deltas.end(), 0.0) / num_ranks;

            /* == next a validity metric thats a little more adventurous:
              the Earth-Movers Distance between the two LWR distributions of the left and right pquerys
              as they are on the Tree. This is very close to the Phylogenetic Kantorovic-Rubinstein metric
              that the pplacer guys have previously used to cluster trees. Here it gives us an idea of
              how "off" two placement distributions are from each other (0.0 is not at all, 1.0 is worst
              possible case).

              Note that special emd structure was already initalized using the normalized trees (outside of loop)
              == */

            // Add the pqueries to the emd samples.
            emd_smpl_l.add( pqry_l );
            emd_smpl_r.add( pqry_r );

            // Calculate the emd.
            double emd = earth_movers_distance( emd_smpl_l, emd_smpl_r );

            // add results to the overview table
            overview_table << name_l.name;
            overview_table.append(
                top_correct ? valid_style : invalid_style,
                top_correct ? valid_char  : invalid_char
            );
            overview_table.append(
                all_ranks_equal ? valid_style : invalid_style,
                all_ranks_equal ? valid_char  : invalid_char
            );
            overview_table
                << utils::to_string_precise( avg_LH_delta, 6 )
                << utils::to_string_precise( emd, 6 );
            emd_results.push_back(emd);

            // Remove the pqueries again.
            emd_smpl_l.clear_pqueries();
            emd_smpl_r.clear_pqueries();
        }
    }

    // -----------------------------------------------------
    //     Result output.
    // -----------------------------------------------------

    // Output the overview table.
    std::cout << "Legend\n  valid:   '" + valid_char + "'\n  invalid: '" + invalid_char + "'\n\n";

    std::cout << "=================================================================================================\n";
    std::cout << "    Overview of all Pqueries that are in both Samples:\n";
    std::cout << "=================================================================================================\n\n";
    std::cout << utils::simple_grid()( overview_table );

    // get some stats
    auto total = std::accumulate( emd_results.begin(), emd_results.end(), 0.0 );
    auto n = emd_results.size();

    std::cout << "\nMean EMD: " << std::to_string(total/n) << "\n\n";

    // -----------------------------------------------------
    //     Details Table.
    // -----------------------------------------------------

    // Prepare the table.
    utils::Table details_table;
    details_table.add_column( "Name" );
    details_table.add_column( "Rank" ).justify( utils::Table::Column::Justification::kRight );
    details_table.add_column( "edge_num L" ).justify( utils::Table::Column::Justification::kRight );
    details_table.add_column( "edge_num R" ).justify( utils::Table::Column::Justification::kRight );
    details_table.add_column( "LWR L" ).justify( utils::Table::Column::Justification::kRight );
    details_table.add_column( "LWR R" ).justify( utils::Table::Column::Justification::kRight );
    details_table.add_column( "Correct?" ).justify( utils::Table::Column::Justification::kCentered );

    // Do a detailed comparison of the pqueries that were marked invalid in the overview.
    for( size_t j = 0; j < names_of_invalid_pqueries.size(); ++j ) {
        auto name   = names_of_invalid_pqueries[j];
        auto const& pqry_l = smpl_l.at( name_map_l[ name ]);
        auto const& pqry_r = smpl_r.at( name_map_r[ name ]);

        auto num_ranks = std::min(pqry_l.placement_size(), pqry_r.placement_size());
        for( size_t i = 0; i < num_ranks; ++i ) {
            auto const& place_l = pqry_l.placement_at(i);
            auto const& place_r = pqry_r.placement_at(i);

            if( i == 0 ) {
                details_table << name;
            } else {
                details_table << "";
            }

            details_table
                << utils::to_string( i+1 )
                << utils::to_string( place_l.edge_num() )
                << utils::to_string( place_r.edge_num() )
                << utils::to_string_precise( place_l.like_weight_ratio, 6 )
                << utils::to_string_precise( place_r.like_weight_ratio, 6 );
            details_table.append(
                place_l.edge_num() == place_r.edge_num() ? valid_style : invalid_style,
                place_l.edge_num() == place_r.edge_num() ? valid_char  : invalid_char
            );
        }

        // Add empty line after each pquery, unless it is the last one.
        if( j < names_of_invalid_pqueries.size() - 1 ) {
            details_table.line_break();
        }
    }

    std::cout << "=================================================================================================\n";
    std::cout << "    Details about the Pqueries that were marked invalid in the overview table:\n";
    std::cout << "=================================================================================================\n\n";
    std::cout << utils::simple_grid()( details_table );

    return 0;
}
