#ifndef GENESIS_UTIL_H_
#define GENESIS_UTIL_H_

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
 * @brief This header includes all headers in 'util'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include <genesis/util/bit/bit.hpp>
#include <genesis/util/bit/bitvector/function.hpp>
#include <genesis/util/bit/bitvector.hpp>
#include <genesis/util/bit/bitvector/operator.hpp>
#include <genesis/util/bit/twobit_vector/function.hpp>
#include <genesis/util/bit/twobit_vector.hpp>
#include <genesis/util/bit/twobit_vector/iterator_deletions.hpp>
#include <genesis/util/bit/twobit_vector/iterator_insertions.hpp>
#include <genesis/util/bit/twobit_vector/iterator_substitutions.hpp>
#include <genesis/util/color/color.hpp>
#include <genesis/util/color/function.hpp>
#include <genesis/util/color/heat_map.hpp>
#include <genesis/util/color/helper.hpp>
#include <genesis/util/color/list_diverging.hpp>
#include <genesis/util/color/list_misc.hpp>
#include <genesis/util/color/list_qualitative.hpp>
#include <genesis/util/color/list_sequential.hpp>
#include <genesis/util/color/map.hpp>
#include <genesis/util/color/name.hpp>
#include <genesis/util/color/normalization.hpp>
#include <genesis/util/color/norm_boundary.hpp>
#include <genesis/util/color/norm_diverging.hpp>
#include <genesis/util/color/norm_linear.hpp>
#include <genesis/util/color/norm_logarithmic.hpp>
#include <genesis/util/container/bitpacked_vector.hpp>
#include <genesis/util/container/dataframe.hpp>
#include <genesis/util/container/dataframe/operator.hpp>
#include <genesis/util/container/dataframe/reader.hpp>
#include <genesis/util/container/deref_iterator.hpp>
#include <genesis/util/container/filter_iterator.hpp>
#include <genesis/util/container/function_cache.hpp>
#include <genesis/util/container/generic_input_stream.hpp>
#include <genesis/util/container/hash_tuple.hpp>
#include <genesis/util/container/interval_tree/function.hpp>
#include <genesis/util/container/interval_tree/fwd.hpp>
#include <genesis/util/container/interval_tree.hpp>
#include <genesis/util/container/interval_tree/interval.hpp>
#include <genesis/util/container/interval_tree/iterator.hpp>
#include <genesis/util/container/interval_tree/node.hpp>
#include <genesis/util/container/matrix/col.hpp>
#include <genesis/util/container/matrix.hpp>
#include <genesis/util/container/matrix/operator.hpp>
#include <genesis/util/container/matrix/reader.hpp>
#include <genesis/util/container/matrix/row.hpp>
#include <genesis/util/container/matrix/simple_reader.hpp>
#include <genesis/util/container/matrix/writer.hpp>
#include <genesis/util/container/mru_cache.hpp>
#include <genesis/util/container/optional.hpp>
#include <genesis/util/container/range.hpp>
#include <genesis/util/container/transform_iterator.hpp>
#include <genesis/util/core/algorithm.hpp>
#include <genesis/util/core/exception.hpp>
#include <genesis/util/core/fs.hpp>
#include <genesis/util/core/info.hpp>
#include <genesis/util/core/logging.hpp>
#include <genesis/util/core/options.hpp>
#include <genesis/util/core/resource_logger.hpp>
#include <genesis/util/core/std.hpp>
#include <genesis/util/core/version.hpp>
#include <genesis/util/format/bmp/writer.hpp>
#include <genesis/util/format/csv/input_iterator.hpp>
#include <genesis/util/format/csv/reader.hpp>
#include <genesis/util/format/json/document.hpp>
#include <genesis/util/format/json/iterator.hpp>
#include <genesis/util/format/json/reader.hpp>
#include <genesis/util/format/json/writer.hpp>
#include <genesis/util/format/nexus/block.hpp>
#include <genesis/util/format/nexus/document.hpp>
#include <genesis/util/format/nexus/taxa.hpp>
#include <genesis/util/format/nexus/trees.hpp>
#include <genesis/util/format/nexus/writer.hpp>
#include <genesis/util/format/svg/attribute.hpp>
#include <genesis/util/format/svg/axis.hpp>
#include <genesis/util/format/svg/color_bar.hpp>
#include <genesis/util/format/svg/definitions.hpp>
#include <genesis/util/format/svg/document.hpp>
#include <genesis/util/format/svg/gradient.hpp>
#include <genesis/util/format/svg/group.hpp>
#include <genesis/util/format/svg/helper.hpp>
#include <genesis/util/format/svg/image.hpp>
#include <genesis/util/format/svg/matrix.hpp>
#include <genesis/util/format/svg/object.hpp>
#include <genesis/util/format/svg/pie_chart.hpp>
#include <genesis/util/format/svg/shape.hpp>
#include <genesis/util/format/svg/svg.hpp>
#include <genesis/util/format/svg/text.hpp>
#include <genesis/util/format/xml/document.hpp>
#include <genesis/util/format/xml/helper.hpp>
#include <genesis/util/format/xml/writer.hpp>
#include <genesis/util/io/base_input_source.hpp>
#include <genesis/util/io/base_output_target.hpp>
#include <genesis/util/io/deserializer.hpp>
#include <genesis/util/io/file_input_source.hpp>
#include <genesis/util/io/file_output_target.hpp>
#include <genesis/util/io/function.hpp>
#include <genesis/util/io/gzip_block_ostream.hpp>
#include <genesis/util/io/gzip.hpp>
#include <genesis/util/io/gzip_input_source.hpp>
#include <genesis/util/io/gzip_output_target.hpp>
#include <genesis/util/io/gzip_stream.hpp>
#include <genesis/util/io/input_buffer.hpp>
#include <genesis/util/io/input_reader.hpp>
#include <genesis/util/io/input_source.hpp>
#include <genesis/util/io/input_stream.hpp>
#include <genesis/util/io/output_stream.hpp>
#include <genesis/util/io/output_target.hpp>
#include <genesis/util/io/parser.hpp>
#include <genesis/util/io/scanner.hpp>
#include <genesis/util/io/serializer.hpp>
#include <genesis/util/io/stream_input_source.hpp>
#include <genesis/util/io/stream_output_target.hpp>
#include <genesis/util/io/strict_fstream.hpp>
#include <genesis/util/io/string_input_source.hpp>
#include <genesis/util/io/string_output_target.hpp>
#include <genesis/util/math/binomial.hpp>
#include <genesis/util/math/common.hpp>
#include <genesis/util/math/compensated_sum.hpp>
#include <genesis/util/math/correlation.hpp>
#include <genesis/util/math/distance.hpp>
#include <genesis/util/math/distribution.hpp>
#include <genesis/util/math/euclidean_kmeans.hpp>
#include <genesis/util/math/hac/function.hpp>
#include <genesis/util/math/hac.hpp>
#include <genesis/util/math/hac/linkage.hpp>
#include <genesis/util/math/histogram/accumulator.hpp>
#include <genesis/util/math/histogram/distance.hpp>
#include <genesis/util/math/histogram.hpp>
#include <genesis/util/math/histogram/operation.hpp>
#include <genesis/util/math/histogram/operator.hpp>
#include <genesis/util/math/histogram/statistic.hpp>
#include <genesis/util/math/kmeans.hpp>
#include <genesis/util/math/matrix.hpp>
#include <genesis/util/math/mds.hpp>
#include <genesis/util/math/moments.hpp>
#include <genesis/util/math/pca.hpp>
#include <genesis/util/math/random.hpp>
#include <genesis/util/math/range_minimum_query.hpp>
#include <genesis/util/math/ranking.hpp>
#include <genesis/util/math/regression/dataframe.hpp>
#include <genesis/util/math/regression/factor.hpp>
#include <genesis/util/math/regression/family.hpp>
#include <genesis/util/math/regression/glm.hpp>
#include <genesis/util/math/regression/helper.hpp>
#include <genesis/util/math/regression/link.hpp>
#include <genesis/util/math/regression/slr.hpp>
#include <genesis/util/math/statistic.hpp>
#include <genesis/util/text/base64.hpp>
#include <genesis/util/text/char.hpp>
#include <genesis/util/text/convert.hpp>
#include <genesis/util/text/light_string.hpp>
#include <genesis/util/text/string.hpp>
#include <genesis/util/text/style.hpp>
#include <genesis/util/text/table.hpp>
#include <genesis/util/threading/concurrent_vector_guard.hpp>
#include <genesis/util/threading/multi_future.hpp>
#include <genesis/util/threading/sequential_output_buffer.hpp>
#include <genesis/util/threading/serial_task_queue.hpp>
#include <genesis/util/threading/thread_function.hpp>
#include <genesis/util/threading/thread_local_cache.hpp>
#include <genesis/util/threading/thread_pool.hpp>
#include <genesis/util/threading/threadsafe_queue.hpp>
#include <genesis/util/tool/char_lookup.hpp>
#include <genesis/util/tool/date_time.hpp>
#include <genesis/util/tool/geodesy/function.hpp>
#include <genesis/util/tool/geodesy/geo_coordinate.hpp>
#include <genesis/util/tool/hash/function.hpp>
#include <genesis/util/tool/hash/md5.hpp>
#include <genesis/util/tool/hash/sha1.hpp>
#include <genesis/util/tool/hash/sha256.hpp>
#include <genesis/util/tool/tickmarks.hpp>
#include <genesis/util/tool/timer.hpp>

#endif // include guard
