#ifndef GENESIS_POPULATION_STREAM_VARIANT_INPUT_STREAM_H_
#define GENESIS_POPULATION_STREAM_VARIANT_INPUT_STREAM_H_

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Generic Variant Stream
// =================================================================================================

/**
 * @brief Data storage for input-specific information when traversing a variant file.
 *
 * This is used by VariantInputStream, see there for details.
 *
 * The utils::GenericInputStream allows us to store some extra data. When traversing a file as a
 * list of Variant%s, one per genomic position, we can use this extra field to store information
 * such as the file name and the individual sample names.
 *
 * In the future, we might even want to store pointers to the underlying streams and readers
 * (useful for VCF for example), so that users can work with them when iterating. For now however,
 * we just store some basic information.
 */
struct VariantInputStreamData
{
    /**
     * @brief Full file path, when reading from a file.
     */
    std::string file_path;

    /**
     * @brief User-readable name of the input source.
     *
     * This can for example be the file base name, without path and extensions.
     */
    std::string source_name;

    /**
     * @brief Sample names, for example as found in the file header.
     *
     * Not all input file formats contain sample names. In that case, this field might be left empty,
     * or filled with names based on the source name and the number of samples contained in the file.
     * That means that downstream processing needs to check this if sample names are going to be
     * used (e.g., for output). The `make_variant_input_stream_from_...()` functions for instance
     * fill this list with names based on the source name, and a numbered list of samples, such as
     * `file-1`.
     */
    std::vector<std::string> sample_names;
};

/**
 * @brief Iterate Variant%s, using a variety of input file formats.
 *
 * This generic stream is an abstraction that is agnostic to the underlying file format,
 * and can be used with anything that can be converted to a Variant per genome position.
 * It offers to iterate a whole input file, and transform and filter the Variant as needed
 * in order to make downstream processing as easy as possible.
 *
 * This is useful for downstream processing, where we just want to work with the Variant%s along
 * the genome, but want to allow different file formats for their input. Use this stream
 * to achieve this. For example, use the `make_variant_input_stream_...()` functions to get such
 * an interator for different input file types.
 *
 * The stream furthermore offers a data field of type VariantInputStreamData, which gets
 * filled with basic data about the input file and sample names (if available in the file format).
 * Use the @link utils::GenericInputStream::data() data()@endlink function to access this data
 * while iterating.
 *
 * @see @link genesis::utils::GenericInputStream GenericInputStream@endlink for usage and details.
 */
using VariantInputStream = utils::GenericInputStream<Variant, VariantInputStreamData>;

} // namespace population
} // namespace genesis

#endif // include guard
