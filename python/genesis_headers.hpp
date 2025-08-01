#pragma once

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech

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

// ================================================================================================
//   placement
// ================================================================================================

// #include <genesis/placement/placement_tree.hpp>
// #include <genesis/placement/pquery.hpp>
// #include <genesis/placement/sample.hpp>
// #include <genesis/placement/sample_set.hpp>

// ---------------------------------------------------------------------
//   format
// ---------------------------------------------------------------------

// #include <genesis/placement/format/edge_color.hpp>
// #include <genesis/placement/format/jplace_reader.hpp>
// #include <genesis/placement/format/jplace_writer.hpp>
// #include <genesis/placement/format/newick_reader.hpp>
// #include <genesis/placement/format/newick_writer.hpp>
// #include <genesis/placement/format/serializer.hpp>

// ---------------------------------------------------------------------
//   function
// ---------------------------------------------------------------------

// #include <genesis/placement/function/cog.hpp>
// #include <genesis/placement/function/distance.hpp>
// #include <genesis/placement/function/emd.hpp>
// #include <genesis/placement/function/epca.hpp>
// #include <genesis/placement/function/function.hpp>
// #include <genesis/placement/function/helper.hpp>
// #include <genesis/placement/function/manipulation.hpp>
// #include <genesis/placement/function/mass.hpp>
// #include <genesis/placement/function/measure.hpp>
// #include <genesis/placement/function/nhd.hpp>
// #include <genesis/placement/function/operator.hpp>
// #include <genesis/placement/function/sample_set.hpp>
// #include <genesis/placement/function/tree.hpp>

// ---------------------------------------------------------------------
//   simulator
// ---------------------------------------------------------------------

// #include <genesis/placement/simulator/distribution.hpp>
// #include <genesis/placement/simulator/function.hpp>
// #include <genesis/placement/simulator/simulator.hpp>

// ---------------------------------------------------------------------
//   pquery
// ---------------------------------------------------------------------

// #include <genesis/placement/pquery/name.hpp>
// #include <genesis/placement/pquery/placement.hpp>
// #include <genesis/placement/pquery/plain.hpp>

// ================================================================================================
//   population
// ================================================================================================

// #include <genesis/population/genome_locus.hpp>
// #include <genesis/population/genome_locus_set.hpp>
// #include <genesis/population/genome_region.hpp>
// #include <genesis/population/genome_region_list.hpp>
// #include <genesis/population/sample_counts.hpp>
// #include <genesis/population/variant.hpp>

// ---------------------------------------------------------------------
//   filter
// ---------------------------------------------------------------------

// #include <genesis/population/filter/filter_stats.hpp>
// #include <genesis/population/filter/filter_status.hpp>
// #include <genesis/population/filter/sample_counts_filter.hpp>
// #include <genesis/population/filter/sample_counts_filter_numerical.hpp>
// #include <genesis/population/filter/sample_counts_filter_positional.hpp>
// #include <genesis/population/filter/variant_filter.hpp>
// #include <genesis/population/filter/variant_filter_numerical.hpp>
// #include <genesis/population/filter/variant_filter_positional.hpp>

// ---------------------------------------------------------------------
//   format
// ---------------------------------------------------------------------

// #include <genesis/population/format/bed_reader.hpp>
// #include <genesis/population/format/frequency_table_input_stream.hpp>
// #include <genesis/population/format/genome_region_reader.hpp>
// #include <genesis/population/format/gff_reader.hpp>
// #include <genesis/population/format/hts_file.hpp>
// #include <genesis/population/format/map_bim_reader.hpp>
// #include <genesis/population/format/sam_flags.hpp>
// #include <genesis/population/format/sam_variant_input_stream.hpp>
// #include <genesis/population/format/simple_pileup_common.hpp>
// #include <genesis/population/format/simple_pileup_input_stream.hpp>
// #include <genesis/population/format/simple_pileup_reader.hpp>
// #include <genesis/population/format/sync_common.hpp>
// #include <genesis/population/format/sync_input_stream.hpp>
// #include <genesis/population/format/sync_reader.hpp>
// #include <genesis/population/format/vcf_common.hpp>
// #include <genesis/population/format/vcf_format_iterator.hpp>
// #include <genesis/population/format/vcf_header.hpp>
// #include <genesis/population/format/vcf_input_stream.hpp>
// #include <genesis/population/format/vcf_record.hpp>

// ---------------------------------------------------------------------
//   function
// ---------------------------------------------------------------------

// #include <genesis/population/function/diversity_pool_calculator.hpp>
// #include <genesis/population/function/diversity_pool_function.hpp>
// #include <genesis/population/function/diversity_pool_processor.hpp>
// #include <genesis/population/function/fst_cathedral.hpp>
// #include <genesis/population/function/fst_pool_calculator.hpp>
// #include <genesis/population/function/fst_pool_function.hpp>
// #include <genesis/population/function/fst_pool_karlsson.hpp>
// #include <genesis/population/function/fst_pool_kofler.hpp>
// #include <genesis/population/function/fst_pool_processor.hpp>
// #include <genesis/population/function/fst_pool_unbiased.hpp>
// #include <genesis/population/function/function.hpp>
// #include <genesis/population/function/genome_locus.hpp>
// #include <genesis/population/function/genome_locus_set.hpp>
// #include <genesis/population/function/genome_region.hpp>
// #include <genesis/population/function/subsample.hpp>
// #include <genesis/population/function/variant_input_stream.hpp>
// #include <genesis/population/function/window_average.hpp>

// ---------------------------------------------------------------------
//   plotting
// ---------------------------------------------------------------------

// #include <genesis/population/plotting/af_spectrum.hpp>
// #include <genesis/population/plotting/cathedral_plot.hpp>
// #include <genesis/population/plotting/genome_heatmap.hpp>
// #include <genesis/population/plotting/heatmap_colorization.hpp>

// ---------------------------------------------------------------------
//   stream
// ---------------------------------------------------------------------

// #include <genesis/population/stream/variant_gapless_input_stream.hpp>
// #include <genesis/population/stream/variant_input_stream.hpp>
// #include <genesis/population/stream/variant_input_stream_adapter.hpp>
// #include <genesis/population/stream/variant_input_stream_source.hpp>
// #include <genesis/population/stream/variant_parallel_input_stream.hpp>

// ---------------------------------------------------------------------
//   window
// ---------------------------------------------------------------------

// #include <genesis/population/window/base_window.hpp>
// #include <genesis/population/window/base_window_stream.hpp>
// #include <genesis/population/window/chromosome_window_stream.hpp>
// #include <genesis/population/window/function.hpp>
// #include <genesis/population/window/genome_window_stream.hpp>
// #include <genesis/population/window/interval_window_stream.hpp>
// #include <genesis/population/window/position_window_stream.hpp>
// #include <genesis/population/window/queue_window_stream.hpp>
// #include <genesis/population/window/region_window_stream.hpp>
// #include <genesis/population/window/sliding_window_generator.hpp>
// #include <genesis/population/window/variant_window_stream.hpp>
// #include <genesis/population/window/vcf_window.hpp>
// #include <genesis/population/window/window.hpp>
// #include <genesis/population/window/window_view.hpp>
// #include <genesis/population/window/window_view_stream.hpp>

// ================================================================================================
//   sequence
// ================================================================================================

// #include <genesis/sequence/reference_genome.hpp>
// #include <genesis/sequence/sequence.hpp>
// #include <genesis/sequence/sequence_dict.hpp>
// #include <genesis/sequence/sequence_set.hpp>
// #include <genesis/sequence/site_counts.hpp>

// ---------------------------------------------------------------------
//   format
// ---------------------------------------------------------------------

// #include <genesis/sequence/format/fai_input_stream.hpp>
// #include <genesis/sequence/format/fasta_reader.hpp>
// #include <genesis/sequence/format/fasta_writer.hpp>
// #include <genesis/sequence/format/fastq_reader.hpp>
// #include <genesis/sequence/format/fastq_writer.hpp>
// #include <genesis/sequence/format/fastx_input_stream.hpp>
// #include <genesis/sequence/format/fastx_input_view_stream.hpp>
// #include <genesis/sequence/format/fastx_output_stream.hpp>
// #include <genesis/sequence/format/phylip_reader.hpp>
// #include <genesis/sequence/format/phylip_writer.hpp>

// ---------------------------------------------------------------------
//   function
// ---------------------------------------------------------------------

// #include <genesis/sequence/function/code.hpp>
// #include <genesis/sequence/function/consensus.hpp>
// #include <genesis/sequence/function/dict.hpp>
// #include <genesis/sequence/function/entropy.hpp>
// #include <genesis/sequence/function/function.hpp>
// #include <genesis/sequence/function/label.hpp>
// #include <genesis/sequence/function/quality.hpp>
// #include <genesis/sequence/function/signature_specifications.hpp>
// #include <genesis/sequence/function/signature.hpp>
// #include <genesis/sequence/function/statistic.hpp>

// ---------------------------------------------------------------------
//   kmer
// ---------------------------------------------------------------------

// #include <genesis/sequence/kmer/alphabet.hpp>
// #include <genesis/sequence/kmer/bitfield.hpp>
// #include <genesis/sequence/kmer/canonical_encoding.hpp>
// #include <genesis/sequence/kmer/color_gamut.hpp>
// #include <genesis/sequence/kmer/color_gamut_function.hpp>
// #include <genesis/sequence/kmer/extractor.hpp>
// #include <genesis/sequence/kmer/function.hpp>
// #include <genesis/sequence/kmer/kmer.hpp>
// #include <genesis/sequence/kmer/microvariant_scanner.hpp>

// ---------------------------------------------------------------------
//   printer
// ---------------------------------------------------------------------

// #include <genesis/sequence/printer/bitmap.hpp>
// #include <genesis/sequence/printer/simple.hpp>

// ================================================================================================
//   taxonomy
// ================================================================================================

// #include <genesis/taxonomy/accession_lookup.hpp>
// #include <genesis/taxonomy/taxon.hpp>
// #include <genesis/taxonomy/taxon_data.hpp>
// #include <genesis/taxonomy/taxonomy.hpp>
// #include <genesis/taxonomy/taxopath.hpp>

// ---------------------------------------------------------------------
//   format
// ---------------------------------------------------------------------

// #include <genesis/taxonomy/format/accession_lookup_reader.hpp>
// #include <genesis/taxonomy/format/json_reader.hpp>
// #include <genesis/taxonomy/format/json_writer.hpp>
// #include <genesis/taxonomy/format/ncbi.hpp>
// #include <genesis/taxonomy/format/taxonomy_reader.hpp>
// #include <genesis/taxonomy/format/taxonomy_writer.hpp>
// #include <genesis/taxonomy/format/taxopath_generator.hpp>
// #include <genesis/taxonomy/format/taxopath_parser.hpp>

// ---------------------------------------------------------------------
//   function
// ---------------------------------------------------------------------

// #include <genesis/taxonomy/function/entropy.hpp>
// #include <genesis/taxonomy/function/entropy_data.hpp>
// #include <genesis/taxonomy/function/kmer.hpp>
// #include <genesis/taxonomy/function/kmer_data.hpp>
// #include <genesis/taxonomy/function/kmer_grouping.hpp>
// #include <genesis/taxonomy/function/kmer_partitioning.hpp>
// #include <genesis/taxonomy/function/operator.hpp>
// #include <genesis/taxonomy/function/ranks.hpp>
// #include <genesis/taxonomy/function/taxonomy.hpp>
// #include <genesis/taxonomy/function/taxopath.hpp>
// #include <genesis/taxonomy/function/tree.hpp>

// ---------------------------------------------------------------------
//   iterator
// ---------------------------------------------------------------------

// #include <genesis/taxonomy/iterator/levelorder.hpp>
// #include <genesis/taxonomy/iterator/postorder.hpp>
// #include <genesis/taxonomy/iterator/preorder.hpp>

// ---------------------------------------------------------------------
//   printers
// ---------------------------------------------------------------------

// #include <genesis/taxonomy/printer/nested.hpp>

// ================================================================================================
//   tree
// ================================================================================================

// #include <genesis/tree/tree.hpp>
// #include <genesis/tree/tree_set.hpp>

// ---------------------------------------------------------------------
//   attribute_tree
// ---------------------------------------------------------------------

// #include <genesis/tree/attribute_tree/indexed_newick_reader.hpp>
// #include <genesis/tree/attribute_tree/keyed_newick_reader.hpp>
// #include <genesis/tree/attribute_tree/tree.hpp>

// ---------------------------------------------------------------------
//   bipartition
// ---------------------------------------------------------------------

// #include <genesis/tree/bipartition/bipartition.hpp>
// #include <genesis/tree/bipartition/function.hpp>
// #include <genesis/tree/bipartition/rf.hpp>

// ---------------------------------------------------------------------
//   common_tree
// ---------------------------------------------------------------------

// #include <genesis/tree/common_tree/distance.hpp>
// #include <genesis/tree/common_tree/edge_color.hpp>
// #include <genesis/tree/common_tree/function.hpp>
// #include <genesis/tree/common_tree/newick_reader.hpp>
// #include <genesis/tree/common_tree/newick_writer.hpp>
// #include <genesis/tree/common_tree/operator.hpp>
// #include <genesis/tree/common_tree/phyloxml_writer.hpp>
// #include <genesis/tree/common_tree/tree.hpp>

// ---------------------------------------------------------------------
//   drawing
// ---------------------------------------------------------------------

// #include <genesis/tree/drawing/circular_layout.hpp>
// #include <genesis/tree/drawing/function.hpp>
// #include <genesis/tree/drawing/heat_tree.hpp>
// #include <genesis/tree/drawing/layout_base.hpp>
// #include <genesis/tree/drawing/layout_tree.hpp>
// #include <genesis/tree/drawing/rectangular_layout.hpp>

// ---------------------------------------------------------------------
//   format
// ---------------------------------------------------------------------

// #include <genesis/tree/format/color_writer_plugin.hpp>
// #include <genesis/tree/format/newick/broker.hpp>
// #include <genesis/tree/format/newick/color_writer_plugin.hpp>
// #include <genesis/tree/format/newick/element.hpp>
// #include <genesis/tree/format/newick/input_iterator.hpp>
// #include <genesis/tree/format/newick/reader.hpp>
// #include <genesis/tree/format/newick/simple_reader.hpp>
// #include <genesis/tree/format/newick/simple_tree.hpp>
// #include <genesis/tree/format/newick/simple_writer.hpp>
// #include <genesis/tree/format/newick/writer.hpp>
// #include <genesis/tree/format/phyloxml/color_writer_plugin.hpp>
// #include <genesis/tree/format/phyloxml/writer.hpp>
// #include <genesis/tree/format/table/reader.hpp>

// ---------------------------------------------------------------------
//   function
// ---------------------------------------------------------------------

// #include <genesis/tree/function/accumulate.hpp>
// #include <genesis/tree/function/distance.hpp>
// #include <genesis/tree/function/function.hpp>
// #include <genesis/tree/function/lca_lookup.hpp>
// #include <genesis/tree/function/manipulation.hpp>
// #include <genesis/tree/function/operator.hpp>
// #include <genesis/tree/function/tree_set.hpp>

// ---------------------------------------------------------------------
//   iterator
// ---------------------------------------------------------------------

// #include <genesis/tree/iterator/eulertour.hpp>
// #include <genesis/tree/iterator/levelorder.hpp>
// #include <genesis/tree/iterator/node_links.hpp>
// #include <genesis/tree/iterator/path.hpp>
// #include <genesis/tree/iterator/path_set.hpp>
// #include <genesis/tree/iterator/postorder.hpp>
// #include <genesis/tree/iterator/preorder.hpp>

// ---------------------------------------------------------------------
//   mass_tree
// ---------------------------------------------------------------------

// #include <genesis/tree/mass_tree/balance.hpp>
// #include <genesis/tree/mass_tree/emd.hpp>
// #include <genesis/tree/mass_tree/function.hpp>
// #include <genesis/tree/mass_tree/kmeans.hpp>
// #include <genesis/tree/mass_tree/phylo_factor.hpp>
// #include <genesis/tree/mass_tree/phylo_factor_color.hpp>
// #include <genesis/tree/mass_tree/phylo_ilr.hpp>
// #include <genesis/tree/mass_tree/squash_clustering.hpp>
// #include <genesis/tree/mass_tree/tree.hpp>

// ---------------------------------------------------------------------
//   printer
// ---------------------------------------------------------------------

// #include <genesis/tree/printer/compact.hpp>
// #include <genesis/tree/printer/detailed.hpp>
// #include <genesis/tree/printer/table.hpp>

// ---------------------------------------------------------------------
//   tree
// ---------------------------------------------------------------------

// #include <genesis/tree/tree/edge.hpp>
// #include <genesis/tree/tree/edge_data.hpp>
// #include <genesis/tree/tree/link.hpp>
// #include <genesis/tree/tree/node.hpp>
// #include <genesis/tree/tree/node_data.hpp>
// #include <genesis/tree/tree/subtree.hpp>

// ================================================================================================
//   utils
// ================================================================================================

// ---------------------------------------------------------------------
//   bit
// ---------------------------------------------------------------------

#include <genesis/utils/bit/bit.hpp>
#include <genesis/utils/bit/bitvector.hpp>
#include <genesis/utils/bit/bitvector/function.hpp>
#include <genesis/utils/bit/bitvector/operator.hpp>
#include <genesis/utils/bit/twobit_vector.hpp>
#include <genesis/utils/bit/twobit_vector/function.hpp>
#include <genesis/utils/bit/twobit_vector/iterator_deletions.hpp>
#include <genesis/utils/bit/twobit_vector/iterator_insertions.hpp>
#include <genesis/utils/bit/twobit_vector/iterator_substitutions.hpp>

// ---------------------------------------------------------------------
//   color
// ---------------------------------------------------------------------

#include <genesis/utils/color/color.hpp>
#include <genesis/utils/color/function.hpp>
// #include <genesis/utils/color/heat_map.hpp>
#include <genesis/utils/color/helper.hpp>
#include <genesis/utils/color/list_diverging.hpp>
#include <genesis/utils/color/list_misc.hpp>
#include <genesis/utils/color/list_qualitative.hpp>
#include <genesis/utils/color/list_sequential.hpp>
#include <genesis/utils/color/map.hpp>
#include <genesis/utils/color/name.hpp>
#include <genesis/utils/color/norm_boundary.hpp>
#include <genesis/utils/color/norm_diverging.hpp>
#include <genesis/utils/color/norm_linear.hpp>
#include <genesis/utils/color/norm_logarithmic.hpp>
#include <genesis/utils/color/normalization.hpp>

// ---------------------------------------------------------------------
//   container
// ---------------------------------------------------------------------

// #include <genesis/utils/container/bitpacked_vector.hpp>
// #include <genesis/utils/container/dataframe.hpp>
// #include <genesis/utils/container/dataframe/operator.hpp>
// #include <genesis/utils/container/dataframe/reader.hpp>
// #include <genesis/utils/container/deref_iterator.hpp>
// #include <genesis/utils/container/filter_iterator.hpp>
// #include <genesis/utils/container/function_cache.hpp>
// #include <genesis/utils/container/generic_input_stream.hpp>
// #include <genesis/utils/container/hash_tuple.hpp>
// #include <genesis/utils/container/interval_tree.hpp>
// #include <genesis/utils/container/interval_tree/function.hpp>
// #include <genesis/utils/container/interval_tree/fwd.hpp>
// #include <genesis/utils/container/interval_tree/interval.hpp>
// #include <genesis/utils/container/interval_tree/iterator.hpp>
// #include <genesis/utils/container/interval_tree/node.hpp>
// #include <genesis/utils/container/matrix.hpp>
// #include <genesis/utils/container/matrix/col.hpp>
// #include <genesis/utils/container/matrix/operator.hpp>
// #include <genesis/utils/container/matrix/reader.hpp>
// #include <genesis/utils/container/matrix/row.hpp>
// #include <genesis/utils/container/matrix/simple_reader.hpp>
// #include <genesis/utils/container/matrix/writer.hpp>
// #include <genesis/utils/container/mru_cache.hpp>
// #include <genesis/utils/container/optional.hpp>
// #include <genesis/utils/container/range.hpp>
// #include <genesis/utils/container/transform_iterator.hpp>

// ---------------------------------------------------------------------
//   core
// ---------------------------------------------------------------------

#include <genesis/utils/core/algorithm.hpp>
#include <genesis/utils/core/exception.hpp>
#include <genesis/utils/core/fs.hpp>
#include <genesis/utils/core/info.hpp>
#include <genesis/utils/core/logging.hpp>
#include <genesis/utils/core/options.hpp>
#include <genesis/utils/core/resource_logger.hpp>
#include <genesis/utils/core/std.hpp>
#include <genesis/utils/core/version.hpp>

// ---------------------------------------------------------------------
//   format
// ---------------------------------------------------------------------

// #include <genesis/utils/format/bmp/writer.hpp>
// #include <genesis/utils/format/csv/input_iterator.hpp>
// #include <genesis/utils/format/csv/reader.hpp>
// #include <genesis/utils/format/json/document.hpp>
// #include <genesis/utils/format/json/iterator.hpp>
// #include <genesis/utils/format/json/reader.hpp>
// #include <genesis/utils/format/json/writer.hpp>
// #include <genesis/utils/format/nexus/block.hpp>
// #include <genesis/utils/format/nexus/document.hpp>
// #include <genesis/utils/format/nexus/taxa.hpp>
// #include <genesis/utils/format/nexus/trees.hpp>
// #include <genesis/utils/format/nexus/writer.hpp>
// #include <genesis/utils/format/svg/attribute.hpp>
// #include <genesis/utils/format/svg/axis.hpp>
// #include <genesis/utils/format/svg/color_bar.hpp>
// #include <genesis/utils/format/svg/definitions.hpp>
// #include <genesis/utils/format/svg/document.hpp>
// #include <genesis/utils/format/svg/gradient.hpp>
// #include <genesis/utils/format/svg/group.hpp>
// #include <genesis/utils/format/svg/helper.hpp>
// #include <genesis/utils/format/svg/image.hpp>
// #include <genesis/utils/format/svg/matrix.hpp>
// #include <genesis/utils/format/svg/object.hpp>
// #include <genesis/utils/format/svg/pie_chart.hpp>
// #include <genesis/utils/format/svg/shape.hpp>
// #include <genesis/utils/format/svg/svg.hpp>
// #include <genesis/utils/format/svg/text.hpp>
// #include <genesis/utils/format/xml/document.hpp>
// #include <genesis/utils/format/xml/helper.hpp>
// #include <genesis/utils/format/xml/writer.hpp>

// ---------------------------------------------------------------------
//   io
// ---------------------------------------------------------------------

#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/deserializer.hpp>
#include <genesis/utils/io/file_input_source.hpp>
#include <genesis/utils/io/file_output_target.hpp>
#include <genesis/utils/io/function.hpp>
#include <genesis/utils/io/gzip.hpp>
#include <genesis/utils/io/gzip_block_ostream.hpp>
#include <genesis/utils/io/gzip_input_source.hpp>
#include <genesis/utils/io/gzip_output_target.hpp>
#include <genesis/utils/io/gzip_stream.hpp>
#include <genesis/utils/io/input_buffer.hpp>
#include <genesis/utils/io/input_reader.hpp>
#include <genesis/utils/io/input_source.hpp>
#include <genesis/utils/io/input_stream.hpp>
#include <genesis/utils/io/output_stream.hpp>
#include <genesis/utils/io/output_target.hpp>
#include <genesis/utils/io/parser.hpp>
#include <genesis/utils/io/scanner.hpp>
#include <genesis/utils/io/serializer.hpp>
#include <genesis/utils/io/stream_input_source.hpp>
#include <genesis/utils/io/stream_output_target.hpp>
#include <genesis/utils/io/strict_fstream.hpp>
#include <genesis/utils/io/string_input_source.hpp>
#include <genesis/utils/io/string_output_target.hpp>

// ---------------------------------------------------------------------
//   math
// ---------------------------------------------------------------------

#include <genesis/utils/math/binomial.hpp>
#include <genesis/utils/math/common.hpp>
#include <genesis/utils/math/compensated_sum.hpp>
#include <genesis/utils/math/correlation.hpp>
// #include <genesis/utils/math/distance.hpp>
#include <genesis/utils/math/distribution.hpp>
// #include <genesis/utils/math/euclidean_kmeans.hpp>
// #include <genesis/utils/math/hac.hpp>
// #include <genesis/utils/math/hac/function.hpp>
// #include <genesis/utils/math/hac/linkage.hpp>
// #include <genesis/utils/math/histogram.hpp>
// #include <genesis/utils/math/histogram/accumulator.hpp>
// #include <genesis/utils/math/histogram/distance.hpp>
// #include <genesis/utils/math/histogram/operation.hpp>
// #include <genesis/utils/math/histogram/operator.hpp>
// #include <genesis/utils/math/histogram/statistic.hpp>
// #include <genesis/utils/math/kmeans.hpp>
// #include <genesis/utils/math/matrix.hpp>
// #include <genesis/utils/math/mds.hpp>
#include <genesis/utils/math/moments.hpp>
// #include <genesis/utils/math/pca.hpp>
#include <genesis/utils/math/random.hpp>
// #include <genesis/utils/math/range_minimum_query.hpp>
#include <genesis/utils/math/ranking.hpp>
// #include <genesis/utils/math/regression/dataframe.hpp>
// #include <genesis/utils/math/regression/factor.hpp>
// #include <genesis/utils/math/regression/family.hpp>
// #include <genesis/utils/math/regression/glm.hpp>
// #include <genesis/utils/math/regression/helper.hpp>
// #include <genesis/utils/math/regression/link.hpp>
// #include <genesis/utils/math/regression/slr.hpp>
#include <genesis/utils/math/statistic.hpp>

// ---------------------------------------------------------------------
//   text
// ---------------------------------------------------------------------

#include <genesis/utils/text/base64.hpp>
#include <genesis/utils/text/char.hpp>
#include <genesis/utils/text/convert.hpp>
#include <genesis/utils/text/light_string.hpp>
#include <genesis/utils/text/string.hpp>
#include <genesis/utils/text/style.hpp>
#include <genesis/utils/text/table.hpp>

// ---------------------------------------------------------------------
//   threading
// ---------------------------------------------------------------------

#include <genesis/utils/threading/concurrent_vector_guard.hpp>
#include <genesis/utils/threading/multi_future.hpp>
#include <genesis/utils/threading/sequential_output_buffer.hpp>
#include <genesis/utils/threading/serial_task_queue.hpp>
#include <genesis/utils/threading/thread_function.hpp>
#include <genesis/utils/threading/thread_local_cache.hpp>
#include <genesis/utils/threading/thread_pool.hpp>
#include <genesis/utils/threading/threadsafe_queue.hpp>

// ---------------------------------------------------------------------
//   tool
// ---------------------------------------------------------------------

#include <genesis/utils/tool/char_lookup.hpp>
#include <genesis/utils/tool/date_time.hpp>
#include <genesis/utils/tool/geodesy/function.hpp>
#include <genesis/utils/tool/geodesy/geo_coordinate.hpp>
#include <genesis/utils/tool/hash/function.hpp>
#include <genesis/utils/tool/hash/md5.hpp>
#include <genesis/utils/tool/hash/sha1.hpp>
#include <genesis/utils/tool/hash/sha256.hpp>
#include <genesis/utils/tool/tickmarks.hpp>
#include <genesis/utils/tool/timer.hpp>
