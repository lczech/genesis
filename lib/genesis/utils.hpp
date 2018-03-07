#ifndef GENESIS_UTILS_H_
#define GENESIS_UTILS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief This header includes all headers in 'utils'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include "genesis/utils/containers/dataframe.hpp"
#include "genesis/utils/containers/dataframe/reader.hpp"
#include "genesis/utils/containers/matrix/col.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/containers/matrix/reader.hpp"
#include "genesis/utils/containers/matrix/row.hpp"
#include "genesis/utils/containers/mru_cache.hpp"
#include "genesis/utils/containers/sorted_vector.hpp"
#include "genesis/utils/containers/sorted_vector/operators.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/indexed_list.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/options.hpp"
#include "genesis/utils/core/range.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/core/version.hpp"
#include "genesis/utils/formats/bmp/writer.hpp"
#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/iterator.hpp"
#include "genesis/utils/formats/json/reader.hpp"
#include "genesis/utils/formats/json/writer.hpp"
#include "genesis/utils/formats/nexus/block.hpp"
#include "genesis/utils/formats/nexus/document.hpp"
#include "genesis/utils/formats/nexus/taxa.hpp"
#include "genesis/utils/formats/nexus/trees.hpp"
#include "genesis/utils/formats/nexus/writer.hpp"
#include "genesis/utils/formats/svg/attributes.hpp"
#include "genesis/utils/formats/svg/color_bar.hpp"
#include "genesis/utils/formats/svg/definitions.hpp"
#include "genesis/utils/formats/svg/document.hpp"
#include "genesis/utils/formats/svg/gradient.hpp"
#include "genesis/utils/formats/svg/group.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/formats/svg/image.hpp"
#include "genesis/utils/formats/svg/object.hpp"
#include "genesis/utils/formats/svg/shapes.hpp"
#include "genesis/utils/formats/svg/svg.hpp"
#include "genesis/utils/formats/svg/text.hpp"
#include "genesis/utils/formats/xml/document.hpp"
#include "genesis/utils/formats/xml/helper.hpp"
#include "genesis/utils/formats/xml/writer.hpp"
#include "genesis/utils/io/input_buffer.hpp"
#include "genesis/utils/io/input_reader.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/io/serializer.hpp"
#include "genesis/utils/math/bitvector.hpp"
#include "genesis/utils/math/bitvector/operators.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/math/histogram/accumulator.hpp"
#include "genesis/utils/math/histogram/distances.hpp"
#include "genesis/utils/math/histogram.hpp"
#include "genesis/utils/math/histogram/operations.hpp"
#include "genesis/utils/math/histogram/operators.hpp"
#include "genesis/utils/math/histogram/stats.hpp"
#include "genesis/utils/math/kmeans.hpp"
#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/math/pca.hpp"
#include "genesis/utils/math/random.hpp"
#include "genesis/utils/math/range_minimum_query.hpp"
#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/math/twobit_vector/functions.hpp"
#include "genesis/utils/math/twobit_vector.hpp"
#include "genesis/utils/math/twobit_vector/iterator_deletions.hpp"
#include "genesis/utils/math/twobit_vector/iterator_insertions.hpp"
#include "genesis/utils/math/twobit_vector/iterator_substitutions.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/style.hpp"
#include "genesis/utils/text/table.hpp"
#include "genesis/utils/tools/char_lookup.hpp"
#include "genesis/utils/tools/color/diverging_lists.hpp"
#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/tools/color/helpers.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/map.hpp"
#include "genesis/utils/tools/color/names.hpp"
#include "genesis/utils/tools/color/normalization.hpp"
#include "genesis/utils/tools/color/norm_boundary.hpp"
#include "genesis/utils/tools/color/norm_diverging.hpp"
#include "genesis/utils/tools/color/norm_linear.hpp"
#include "genesis/utils/tools/color/norm_logarithmic.hpp"
#include "genesis/utils/tools/color/qualitative_lists.hpp"
#include "genesis/utils/tools/color/sequential_lists.hpp"
#include "genesis/utils/tools/date_time.hpp"
#include "genesis/utils/tools/geodesy/functions.hpp"
#include "genesis/utils/tools/geodesy.hpp"
#include "genesis/utils/tools/md5.hpp"
#include "genesis/utils/tools/sha1.hpp"
#include "genesis/utils/tools/sha256.hpp"
#include "genesis/utils/tools/tickmarks.hpp"

#endif // include guard
