#ifndef GENESIS_POPULATION_H_
#define GENESIS_POPULATION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
 * @brief This header includes all headers in 'population'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include "genesis/population/filter/filter_stats.hpp"
#include "genesis/population/filter/filter_status.hpp"
#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/sample_counts_filter_numerical.hpp"
#include "genesis/population/filter/sample_counts_filter_positional.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/filter/variant_filter_numerical.hpp"
#include "genesis/population/filter/variant_filter_positional.hpp"
#include "genesis/population/format/bed_reader.hpp"
#include "genesis/population/format/frequency_table_input_stream.hpp"
#include "genesis/population/format/genome_region_reader.hpp"
#include "genesis/population/format/gff_reader.hpp"
#include "genesis/population/format/hts_file.hpp"
#include "genesis/population/format/map_bim_reader.hpp"
#include "genesis/population/format/sam_flags.hpp"
#include "genesis/population/format/sam_variant_input_stream.hpp"
#include "genesis/population/format/simple_pileup_common.hpp"
#include "genesis/population/format/simple_pileup_input_stream.hpp"
#include "genesis/population/format/simple_pileup_reader.hpp"
#include "genesis/population/format/sync_common.hpp"
#include "genesis/population/format/sync_input_stream.hpp"
#include "genesis/population/format/sync_reader.hpp"
#include "genesis/population/format/vcf_common.hpp"
#include "genesis/population/format/vcf_format_iterator.hpp"
#include "genesis/population/format/vcf_header.hpp"
#include "genesis/population/format/vcf_input_stream.hpp"
#include "genesis/population/format/vcf_record.hpp"
#include "genesis/population/function/diversity_pool_calculator.hpp"
#include "genesis/population/function/diversity_pool_functions.hpp"
#include "genesis/population/function/diversity_pool_processor.hpp"
#include "genesis/population/function/fst_cathedral.hpp"
#include "genesis/population/function/fst_pool_calculator.hpp"
#include "genesis/population/function/fst_pool_functions.hpp"
#include "genesis/population/function/fst_pool_karlsson.hpp"
#include "genesis/population/function/fst_pool_kofler.hpp"
#include "genesis/population/function/fst_pool_processor.hpp"
#include "genesis/population/function/fst_pool_unbiased.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/population/function/genome_locus.hpp"
#include "genesis/population/function/genome_locus_set.hpp"
#include "genesis/population/function/genome_region.hpp"
#include "genesis/population/function/subsample.hpp"
#include "genesis/population/function/variant_input_stream.hpp"
#include "genesis/population/function/window_average.hpp"
#include "genesis/population/genome_locus.hpp"
#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/genome_region_list.hpp"
#include "genesis/population/plotting/af_spectrum.hpp"
#include "genesis/population/plotting/cathedral_plot.hpp"
#include "genesis/population/plotting/genome_heatmap.hpp"
#include "genesis/population/plotting/heatmap_colorization.hpp"
#include "genesis/population/sample_counts.hpp"
#include "genesis/population/stream/variant_gapless_input_stream.hpp"
#include "genesis/population/stream/variant_input_stream_adapters.hpp"
#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/stream/variant_input_stream_sources.hpp"
#include "genesis/population/stream/variant_parallel_input_stream.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/window/base_window.hpp"
#include "genesis/population/window/base_window_stream.hpp"
#include "genesis/population/window/chromosome_window_stream.hpp"
#include "genesis/population/window/functions.hpp"
#include "genesis/population/window/genome_window_stream.hpp"
#include "genesis/population/window/interval_window_stream.hpp"
#include "genesis/population/window/position_window_stream.hpp"
#include "genesis/population/window/queue_window_stream.hpp"
#include "genesis/population/window/region_window_stream.hpp"
#include "genesis/population/window/sliding_window_generator.hpp"
#include "genesis/population/window/variant_window_stream.hpp"
#include "genesis/population/window/vcf_window.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/population/window/window_view_stream.hpp"

#endif // include guard
