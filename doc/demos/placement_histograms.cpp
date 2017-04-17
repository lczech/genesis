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
 * This is the demo "Placement Histograms". See the Manual for more information.
 */

#include "genesis/genesis.hpp"

using namespace genesis;
using namespace genesis::placement;
using namespace genesis::utils;

#include <algorithm>
#include <fstream>
#include <string>

// =================================================================================================
//      Get Jplace Files From Input Path
// =================================================================================================

/**
 * @brief Helper function that returns the path to all jplace files in a directory,
 * or, if the input is a single file, returns only this file.
 *
 * This allows to call the program with either one or multiple files.
 */
std::vector<std::string> get_jplace_files( std::string const& input_path )
{
    // Prepare a vector for all jplace files we want to process.
    std::vector<std::string> jplace_files;

    if( is_dir( input_path ) ) {

        // If the provided path is a directory, find all jplace files in it.
        auto all_files = dir_list_files( input_path );
        erase_if( all_files, [](std::string const& file) {
            return ! ends_with( to_lower( file ), ".jplace" );
        });
        std::sort( all_files.begin(), all_files.end() );

        LOG_INFO << "Found " << all_files.size() << " jplace files:";
        for( auto filename : all_files ) {
            LOG_INFO << "- " << filename;

            // Add all found jplace files with the correct path to the vector.
            auto jplace_filename = trim_right( input_path, "/") + "/" + filename;
            jplace_files.push_back( jplace_filename );
        }

    } else if( is_file( input_path )) {

        // If the provided path is a file, use this (no check if it ends in .jplace,
        // because it might be storted with a differnet file name).
        jplace_files.push_back( input_path );
        LOG_INFO << "Found jplace file " << input_path << ".";

    } else {
        throw std::runtime_error( "Invalid path: " + input_path );
    }

    return jplace_files;
}

// =================================================================================================
//     Main Function
// =================================================================================================

/**
 * @brief Main function that processes jplace files and writes histograms for their LWRs and EDPLs.
 *
 * The program takes a path to either a `jplace` file, or a directory containing `jplace` files,
 * and builds histograms of the the placement mass (according to the
 * `like_weight_ratio`, LWR) of each placement and of the Expected Distance between Placement
 * Locations (EDPL).
 *
 * The program takes two arguments:
 *
 *     ./placement_histograms <path to jplace file(s)> <path to output directory>
 *
 * and writes the files `histogram_lwr.csv` and `histogram_edpl.csv` in `csv` format. Those files
 * can be opened with any spreadsheet program (e.g., Microsoft Excel, OpenOffice Calc) and
 * visualized as e.g., histogram bar charts.
 *
 * The first analysis shows the distribution of the likelihood weights for the placements of the input
 * data. In other words, it shows how probable the placement with the highest likelihood weight is,
 * how probable the one with the second highest is, etc. In order to visualize this information for
 * all query sequences, it is summarized in form of histograms.
 *
 * ![Histogram of the first three most probable Likelihood Weight Ratios.](demos/hist_lwr.png)
 *
 * Those histograms can be interpreted as measures of quality of the placements:
 * The chart above shows the histogram of the first three most probable placement positions,
 * measured by their likelihood weight ratio (LWR).
 * The most probable placement location is visualized in blue, the second most
 * probable one in orange, the third one in green. The first LWR shows a peak at the `>= 0.8` bin,
 * meaning that many placements are placed with 80% certainty or more.
 *
 * There is also a second peak in the most probable placement position, around `0.45`, indicating
 * that there are quite some sequences that have a somewhat lower certainty.
 * This does not necessarily imply that the placement of the sequences in those bins is incorrect.
 * It can also indicate that there is more than one “good” candidate for placing it – for example,
 * if the corresponding sequence is from a species that does not have a very close relative in the
 * reference, but belongs to the same family as some species in the reference. In this case, one
 * expects to observe placements with a medium probability (likelihood weight) within different
 * branches of the subtree that entails the family.
 *
 * Thus, as a second analysis, the program assesses the locality of placements distributions for each
 * sequence over the tree. If they are all located in the same part of the reference tree, for
 * example at different branches of one family, this indicates that the sequence can be
 * confidently placed into that clade.
 *
 * The Expected Distance between Placement Locations (EDPL) is one way of quantifying this locality.
 * The EDPL calculates the weighted distance between all placement positions of a single
 * sequence. For weighting, the likelihood weights are used; the distance is measured along the
 * branches of the reference tree (i.e., the patristic distance between placements). Thus, the
 * EDPL quantifies to which extent the placements of a single sequence are scattered over the
 * tree. See http://matsen.github.io/pplacer/generated_rst/guppy_edpl.html for further details.
 *
 * ![Histogram of the Expected Distance between Placement Locations.](demos/hist_edpl.png)
 *
 * The second chart shows the distribution of EDPL values for our exemplary query sequences.
 * Most of the values are below `0.2`. This means that most sequences have a weighted distance that
 * is smaller than `0.2` between alternative placements (measured in branch length units).
 * Note that, the maximum likelihood reference tree used for the example has an average branch
 * length of `0.12`. Thus, the placement scatter (or radius) for almost all query sequences is smaller
 * than two branches, on average.
 *
 * Our example can be summarized like this: Many sequences have a placement probability of 80% or
 * higher. But even those with a lower probability for the most likely placement are spread over
 * nearby, local branches of the reference tree. Hence, the vast majority of placements shows high
 * confidence with respect to a single branch or a local neighborhood of branches.
 */
int main( int argc, const char* argv[] )
{
    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    // Into how many histogram bins should the data be divided?
    size_t const histogram_bins  = 20;

    // What character to use for separating columns in the output tables.
    char const   table_separator = '\t';

    // Maximum number of columns for the Likelihood Weight Ratio
    // (only build histograms for this many of the most probable placements).
    size_t const max_lwr_cols = 5;

    // -------------------------------------------------------------------------
    //     Init
    // -------------------------------------------------------------------------

    // Activate logging, print genesis header.
    Logging::log_to_stdout();
    LOG_BOLD << genesis_header();

    // Check if the command line contains the right number of arguments.
    if (argc != 3) {
        throw std::runtime_error(
            "Need to provide two command line arguments:\n"
            "  * A valid path to either a jplace file or a directory containing jplace files.\n"
            "  * An output path."
        );
    }
    auto input_path  = std::string( argv[1] );
    auto output_path = dir_normalize_path( std::string( argv[2] ));

    // Prepare a vector for all jplace files we want to process.
    auto jplace_files = get_jplace_files( input_path );

    // Prepare a Jplace reader that reports wrong values.
    auto jplace_reader = JplaceReader();
    jplace_reader.invalid_number_behaviour(
        genesis::placement::JplaceReader::InvalidNumberBehaviour::kLogAndCorrect
    );

    // -------------------------------------------------------------------------
    //     Prepare Histogram Accumulators
    // -------------------------------------------------------------------------

    HistogramAccumulator accu_lwr_all;
    HistogramAccumulator accu_edpl;
    std::vector<HistogramAccumulator> accu_lwrs;

    // -------------------------------------------------------------------------
    //     Read all jplace files and accumulate data for the Histograms
    // -------------------------------------------------------------------------

    LOG_INFO << "Processing files.";

    // Process all provided jplace files.
    for( auto const& jplace_filename : jplace_files ) {

        // Read the Jplace file into a Sample object.
        Sample smp = jplace_reader.from_file(jplace_filename);
        sort_placements_by_weight( smp );

        // Accumulate LWRs into histogram accus.
        for( auto const& pquery : smp.pqueries() ) {
            for( auto const& place : pquery.placements() ) {
                accu_lwr_all.increment( place.like_weight_ratio );
            }

            auto const lwr_max = std::min( max_lwr_cols, pquery.placement_size() );
            for( size_t i = 0; i < lwr_max; ++i ) {
                while( i >= accu_lwrs.size() ) {
                    accu_lwrs.emplace_back();
                }
                accu_lwrs[i].increment( pquery.placement_at(i).like_weight_ratio );
            }
        }

        // Accumulate EDPL.
        auto edpl_vec = edpl(smp);
        for( auto v : edpl_vec ) {
            accu_edpl.increment(v);
        }
    }

    // -------------------------------------------------------------------------
    //     Build Histograms from the Accumulators
    // -------------------------------------------------------------------------

    auto hist_lwr_all = accu_lwr_all.build_uniform_ranges_histogram( histogram_bins, 0.0, 1.0 );
    auto hist_edpl    = accu_edpl.build_uniform_ranges_histogram( histogram_bins, true );

    auto hist_lwrs    = std::vector<Histogram>();
    for( size_t i = 0; i < accu_lwrs.size(); ++i ) {
        auto hist_lwr = accu_lwrs[i].build_uniform_ranges_histogram( histogram_bins, 0.0, 1.0 );
        hist_lwrs.push_back( hist_lwr );
    }

    // -------------------------------------------------------------------------
    //     Write Histograms to files, as tab separated data
    // -------------------------------------------------------------------------

    // Open files.
    std::ofstream file_lwrs;
    std::ofstream file_edpl;
    file_output_stream( output_path + "/histogram_lwr.csv",  file_lwrs );
    file_output_stream( output_path + "/histogram_edpl.csv", file_edpl );

    // Write table header rows.
    auto ts = table_separator;
    file_lwrs << "Bin" << ts << "Range" << ts << "Range Start" << ts << "Range End";
    file_lwrs << ts << "Bin Name" << ts << "LWR Total";
    for( size_t i = 0; i < hist_lwrs.size(); ++i ) {
        file_lwrs << ts << "LWR " << ( i + 1 );
    }
    file_lwrs << "\n";
    file_edpl << "Bin" << ts << "Range" << ts << "Range Start" << ts << "Range End" << ts;
    file_edpl << "Bin Name" << ts << "EDPL\n";

    // Write table contents. This is a bit hacky, because each table has similar, but different
    // data to be written.
    auto print_hist_line = [&]( std::ofstream& out, Histogram const& hist, size_t i ) {
        auto range = hist.bin_range( i );
        out << i;
        out << ts << "\"[" << range.first << ", " << range.second << ")\"";
        out << ts << range.first << ts << range.second;
        out << ts << ">= " << range.first;
        out << ts << hist[ i ];
    };
    auto print_lwr_line = [&]( size_t i ) {
        print_hist_line( file_lwrs, hist_lwr_all, i );
        for( size_t h = 0; h < hist_lwrs.size(); ++h ) {
            file_lwrs << ts << hist_lwrs[ h ][ i ];
        }
        file_lwrs << "\n";
    };
    for( size_t i = 0; i < histogram_bins; ++i ) {
        print_lwr_line( i );
        print_hist_line( file_edpl, hist_edpl, i );
        file_edpl << "\n";
    }

    LOG_INFO << "Finished.";
    return 0;
}
