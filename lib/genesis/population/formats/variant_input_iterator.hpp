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

#include "genesis/population/formats/sam_variant_input_iterator.hpp"
#include "genesis/population/formats/simple_pileup_input_iterator.hpp"
#include "genesis/population/formats/sync_input_iterator.hpp"
#include "genesis/population/formats/sync_reader.hpp"
#include "genesis/population/formats/vcf_input_iterator.hpp"
#include "genesis/population/base_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/containers/lambda_iterator.hpp"

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
 * The utils::LambdaIterator allows us to store some extra data. When traversing a file as a
 * list of Variant%s, one per genomic position, we can use this extra field to store information
 * such as the file name and the individual sample names.
 *
 * In the future, we might even want to store pointers to the underlying iterators and readers
 * (useful for VCF for example), so that users can work with them when iterating. For now however,
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
     *
     * Not all input file formats contain sample names. In that case, this field is left empty,
     * independent of the number of samples contained in the file. That means that downstream
     * processing needs to check this if sample names are going to be used (e.g., for output).
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

// -------------------------------------------------------------------------
//     SAM/BAM/CRAM
// -------------------------------------------------------------------------

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

/**
 * @brief Create a VariantInputIterator to iterate the contents of a SAM/BAM/CRAM file as Variant%s.
 *
 * An instance of SamVariantInputIterator can be provided from which the settings are copied.
 *
 * Depending on the settings used in the @p reader, this can either produce a single sample
 * (one BaseCounts object in the resulting Variant at each position in the genome),
 * or split the input file by the read group (RG) tag (potentially also allowing for an
 * "unaccounted" group of reads).
 */
VariantInputIterator make_variant_input_iterator_from_sam_file(
    std::string const& filename,
    SamVariantInputIterator const& reader = SamVariantInputIterator{}
);

#endif // GENESIS_HTSLIB

// -------------------------------------------------------------------------
//     Pileup
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantInputIterator to iterate the contents of a (m)pileup file as Variant%s.
 */
VariantInputIterator make_variant_input_iterator_from_pileup_file(
    std::string const& filename,
    SimplePileupReader const& reader = SimplePileupReader{}
);

// -------------------------------------------------------------------------
//     Sync
// -------------------------------------------------------------------------

/**
 * @brief Create a VariantInputIterator to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 */
VariantInputIterator make_variant_input_iterator_from_sync_file(
    std::string const& filename
);

// -------------------------------------------------------------------------
//     VCF
// -------------------------------------------------------------------------

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

/**
 * @brief Create a VariantInputIterator to iterate the contents of a VCF file as Variant%s,
 * treating each sample as a pool of individuals.
 *
 * See convert_to_variant_as_pool( VcfRecord const& ) for details on the conversion from
 * VcfRecord to Variant.
 *
 * This function requires the VCF to have the "AD" FORMAT field. It only iterates over those VCF
 * record lines that actually have the "AD" FORMAT provided, as this is the information that we use
 * to convert the samples to Variant%s. All records without that field are skipped.
 * Only SNP records are processed; that is, all non-SNPs (indels and others) are ignord.
 *
 * If @p only_biallelic is set to `true` (default), this is further restricted to only contain
 * biallelic SNPs, that is, only positions with exactly one alternative allele.
 *
 * If @p only_filter_pass is set to `true` (default), only those positions are considered that
 * have the FILTER field set to "PASS". That is, all variants that did not pass a filter in
 * the VCF processing are skipped.
 *
 * @see See make_variant_input_iterator_from_individual_vcf_file() for the function that instead
 * interprets the VCF as usual as a set of individuals.
 */
VariantInputIterator make_variant_input_iterator_from_pool_vcf_file(
    std::string const& filename,
    bool only_biallelic = true,
    bool only_filter_pass = true
);

/**
 * @copydoc make_variant_input_iterator_from_pool_vcf_file( std::string const& )
 *
 * Additionally, this version of the function takes a list of @p sample_names which are used as
 * filter so that only those samples (columns of the VCF records) are evaluated and accessible - or,
 * if @p inverse_sample_names is set to `true`, instead all <i>but</i> those samples.
 */
VariantInputIterator make_variant_input_iterator_from_pool_vcf_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names = false,
    bool only_biallelic = true,
    bool only_filter_pass = true
);

/**
 * @brief Create a VariantInputIterator to iterate the contents of a VCF file as Variant%s,
 * treating each sample as an individual, and combining them all into one BaseCounts sample.
 *
 * See convert_to_variant_as_individuals( VcfRecord const&, bool ) for details on the conversion
 * from VcfRecord to Variant. We only consider biallelic SNP positions here.
 *
 * If @p only_filter_pass is set to `true` (default), only those positions are considered that
 * have the FILTER field set to "PASS". That is, all variants that did not pass a filter in
 * the VCF processing are skipped.
 *
 * @see See make_variant_input_iterator_from_pool_vcf_file() for the function that instead
 * interprets each sample (column) as a pool of individuals, e.g., from pool sequencing.
 */
VariantInputIterator make_variant_input_iterator_from_individual_vcf_file(
    std::string const& filename,
    bool use_allelic_depth = false,
    bool only_biallelic = true,
    bool only_filter_pass = true
);

/**
 * @copydoc make_variant_input_iterator_from_individual_vcf_file( std::string const&, bool )
 *
 * Additionally, this version of the function takes a list of @p sample_names which are used as
 * filter so that only those samples (columns of the VCF records) are evaluated and accessible - or,
 * if @p inverse_sample_names is set to `true`, instead all <i>but</i> those samples.
 */
VariantInputIterator make_variant_input_iterator_from_individual_vcf_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names,
    bool inverse_sample_names = false,
    bool use_allelic_depth = false,
    bool only_biallelic = true,
    bool only_filter_pass = true
);

#endif // GENESIS_HTSLIB

} // namespace population
} // namespace genesis

#endif // include guard
