#ifndef GENESIS_POPULATION_FORMATS_VARIANT_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_VARIANT_INPUT_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/formats/simple_pileup_input_iterator.hpp"
#include "genesis/population/formats/sync_input_iterator.hpp"
#include "genesis/population/formats/sync_reader.hpp"
#include "genesis/population/formats/vcf_input_iterator.hpp"
#include "genesis/population/functions/base_counts.hpp"
#include "genesis/population/functions/filter_transform.hpp"
#include "genesis/population/functions/genome_region.hpp"
#include "genesis/population/functions/variant.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/containers/lambda_iterator.hpp"
#include "genesis/utils/containers/optional.hpp"
#include "genesis/utils/core/fs.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Generic Variant Iterator
// =================================================================================================

/**
 * @brief Data storage for input-specific information when traversing a variant file.
 *
 * This is used by VariantInputIterator, see there for details.
 *
 * The utils::LambdaIterator allows us to store some extra data. When traversing a file as
 * Variant%s, we can use this extra field to store information such as the file name and
 * the individual sample names.
 *
 * In the future, we might even want to store pointers to the underlying iterators and readers
 * (useful for VCF for example), so that users can work with those when iterating. For now however,
 * we just store some basic information.
 */
struct VariantInputIteratorData
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
     */
    std::vector<std::string> sample_names;
};

/**
 * @brief Iterate Variant%s, using a variety of input file formats.
 *
 * This generic iterator is an abstraction that is agnostic to the underlying file format,
 * and can be used with anything that can be converted to a Variant per genome position.
 * It offers to iterate a whole input file, and transform and filter the Variant as needed
 * in order to make downstream processing as easy as possible.
 *
 * This is useful for downstream processing, where we just want to work with the Variant%s along
 * the genome, but want to allow different file formats for their input. Use this iterator
 * to achieve this. For example, use the `make_variant_input_iterator_...()` functions to get such
 * an interator for different input file types.
 *
 * The iterator furthermore offers a data field of type VariantInputIteratorData, which gets
 * filled with basic data about the input file and sample names (if available in the file format).
 * Use the @link utils::LambdaIterator::data() data()@endlink function to access this data
 * while iterating.
 *
 * @see @link utils::LambdaIterator LambdaIterator@endlink for usage and details.
 */
using VariantInputIterator = utils::LambdaIterator<Variant, VariantInputIteratorData>;

// =================================================================================================
//     Input Sources
// =================================================================================================

/**
 * @brief Create a VariantInputIterator to iterate the contents of a (m)pileup file as Variant%s.
 */
inline VariantInputIterator make_variant_input_iterator_from_pileup_file(
    std::string const& filename,
    SimplePileupReader reader = {}
) {
    // Alternative signature that has to be fed into the VariantInputIterator:
    // inline std::function<std::shared_ptr<Variant>()> make_variant_input_iterator_from_pileup_file(
    // auto it = VariantInputIterator(
    //     variant_from_pileup_file( infile )
    // );

    // TODO with sample index list --> isnt that already part of the reader?!

    auto it = SimplePileupInputIterator<Variant>(
        utils::from_file( filename ),
        reader
    );

    // Get file base name without path and potential extensions.
    VariantInputIteratorData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".gz", ".pileup", ".mpileup" });

    return VariantInputIterator(
        [ it ]() mutable -> utils::Optional<Variant>{
            if( it ) {
                auto res = utils::make_optional<Variant>( *it );
                ++it;
                return res;
            } else {
                return utils::nullopt;
            }
        },
        std::move( data )
    );
}

/**
 * @brief Create a VariantInputIterator to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 */
inline VariantInputIterator make_variant_input_iterator_from_sync_file(
    std::string const& filename
) {
    auto it = SyncInputIterator( utils::from_file( filename ));

    // Get file base name without path and potential extensions.
    VariantInputIteratorData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".gz", ".sync" });

    return VariantInputIterator(
        [ it ]() mutable -> utils::Optional<Variant>{
            if( it ) {
                auto res = utils::make_optional<Variant>( *it );
                ++it;
                return res;
            } else {
                return utils::nullopt;
            }
        },
        std::move( data )
    );
}

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

/**
 * @brief Create a VariantInputIterator to iterate the contents of a VCF file as Variant%s.
 *
 * This required the VCF to have the "AD" FORMAT field. It only iterates over those VCF record
 * lines that actually have the "AD" FORMAT provided, as this is the information that we use
 * to convert the samples to Variant%s. All records without that field are skipped.
 */
inline VariantInputIterator make_variant_input_iterator_from_vcf_file(
    std::string const& filename
) {
    // Make an iterator over vcf, and check that the necessary format field AD is present
    // and of the correct form.
    auto it = VcfInputIterator( filename );
    if( ! it.header().has_format( "AD", VcfValueType::kInteger, VcfValueSpecial::kReference )) {
        throw std::runtime_error(
            "Cannot iterator over VCF file " + filename + " as Variants, " +
            "because it does not contain the required \"AD\" FORMAT field. "  +
            "Note that we expect the samples in the VCF file to represent pools of individuals " +
            "here, instead of single individuals."
        );
    }

    // Get file base name without path and potential extensions.
    VariantInputIteratorData data;
    data.file_path = filename;
    data.source_name = utils::file_basename( filename, { ".gz", ".vcf", ".bcf" });

    return VariantInputIterator(
        [ it ]() mutable -> utils::Optional<Variant>{

            // Only use the lines that have the AD field, skip all others.
            while( it && ! it->has_format( "AD" ) ) {
                ++it;
            }

            // Now we are either at a record that as the AD field, or at the end.
            if( it ) {
                assert( it->has_format( "AD" ) );
                auto res = utils::make_optional<Variant>( convert_to_variant( *it ));
                ++it;
                return res;
            } else {
                // If we reached the end of the input, return an empty optional to signal this.
                return utils::nullopt;
            }
        },
        std::move( data )
    );
}

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Filters
// =================================================================================================

/**
 * @brief Filter function to be used with VariantInputIterator to filter by a genome region.
 *
 * This function can be used as a filter with VariantInputIterator::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p region (if @p inclusive is `true`, default),
 * or only over Variant%s that are outside of the @p region (if @p inclusive is `false`).
 */
inline std::function<bool(Variant const&)> variant_filter_region(
    GenomeRegion const& region,
    bool inclusive = true
) {
    return [region, inclusive]( Variant const& variant ){
        return (!inclusive) ^ is_covered( region, variant );
    };
}

/**
 * @brief Filter function to be used with VariantInputIterator to filter by a list of genome regions.
 *
 * This function can be used as a filter with VariantInputIterator::add_filter(), in order
 * to only iterate over Variant%s that are in the given @p regions (if @p inclusive is `true`, default),
 * or only over Variant%s that are outside of the @p regions (if @p inclusive is `false`).
 *
 * The parameter @p copy_regions is an optimization. By default, the function stores a copy of the
 * @p regions, in order to make sure that it is available. However, if it is guaranteed that
 * the @p regions object stays in scope during the VariantInputIterator's livetime, this copy
 * can be avoided.
 */
inline std::function<bool(Variant const&)> variant_filter_region(
    GenomeRegionList const& regions,
    bool inclusive = true,
    bool copy_regions = true
) {
    if( copy_regions ) {
        return [regions, inclusive]( Variant const& variant ){
            return (!inclusive) ^ is_covered( regions, variant );
        };
    } else {
        return [&, inclusive]( Variant const& variant ){
            return (!inclusive) ^ is_covered( regions, variant );
        };
    }
}

// TODO

// for all and per sample?!
// we need an NA entry for samples! all zeroes?!
//
// --> total sum, and per sample.
// better: option: all of them have to be good, or at least one of them has to be good
//
// variant_filter_min_coverage
// variant_filter_max_coverage
// variant_filter_min_max_coverage
//
// variant_filter_min_frequency
// variant_filter_max_frequency
// variant_filter_min_max_frequency
//
// is snp
//
// is biallelic

// =================================================================================================
//     Transformations
// =================================================================================================

// bascially, all of the above filters, but as transforms that set stuff to 0 intead of filtering
//
// inline std::functiom<void(Variant&)> variant_transform_min_counts( size_t min_count )
// {
//     return [min_count]( Variant& variant ){
//         for( auto& sample : variant.samples ) {
//             transform_min_count( sample, min_count );
//             --> add this function for variants as well first, and use this
//             (basically just a loop over the other one)
//             -->> also make this for max and min max, and use these.
//
//             --->> then, these already have the function signature that is needed for the iterator~
//             no need to do a lambda that just calles it!
//             --> ah no, because we need to capture the min count setting....
//         }
//     };
// }
//
// min count to 0
// max count to 0
// min max count to 0

} // namespace population
} // namespace genesis

#endif // include guard
