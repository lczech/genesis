#ifndef GENESIS_POPULATION_STREAM_VARIANT_INPUT_STREAM_SOURCES_H_
#define GENESIS_POPULATION_STREAM_VARIANT_INPUT_STREAM_SOURCES_H_

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

#include "genesis/population/format/frequency_table_input_stream.hpp"
#include "genesis/population/format/sam_variant_input_stream.hpp"
#include "genesis/population/format/simple_pileup_input_stream.hpp"
#include "genesis/population/format/sync_input_stream.hpp"
#include "genesis/population/format/sync_reader.hpp"
#include "genesis/population/format/vcf_input_stream.hpp"
#include "genesis/population/sample_counts.hpp"
#include "genesis/population/stream/variant_input_stream.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/sequence/reference_genome.hpp"
#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/utils/containers/generic_input_stream.hpp"

#include <cassert>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     vector
// =================================================================================================

/**
 * @brief Create a VariantInputStream to iterate the contents of `std::vector` containing Variant%s.
 *
 * This is a simple wrapper to bring a vector of in-memory Variant%s into the input stream
 * format that we use for file streaming as well. Meant as a speed-up for small files that fit into
 * memory, in cases where they for example have to processed multiple times.
 *
 * The user needs to make sure that the lifetime of the given input @p variants vector is longer
 * than the stream returned here, and that the vector is not modified after calling this function.
 */
VariantInputStream make_variant_input_stream_from_vector(
    std::vector<Variant> const& variants
);

// =================================================================================================
//     SAM/BAM/CRAM
// =================================================================================================

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

/**
 * @brief Create a VariantInputStream to iterate the contents of a SAM/BAM/CRAM file as Variant%s.
 *
 * An instance of SamVariantInputStream can be provided from which the settings are copied.
 *
 * Depending on the settings used in the @p reader, this can either produce a single sample
 * (one SampleCounts object in the resulting Variant at each position in the genome),
 * or split the input file by the read group (RG) tag (potentially also allowing for an
 * "unaccounted" group of reads).
 *
 * The other `make_variant_input_stream_...` functions offer settings to sub-set (filter) the
 * samples based on their names or indices. This can be achieved here as well, but has instead to
 * be done directly in the @p reader, instead of providing the fitler arguments to this function.
 */
VariantInputStream make_variant_input_stream_from_sam_file(
    std::string const& filename,
    SamVariantInputStream const& reader = SamVariantInputStream{}
);

#endif // GENESIS_HTSLIB

// =================================================================================================
//     Pileup
// =================================================================================================

/**
 * @brief Create a VariantInputStream to iterate the contents of a (m)pileup file as Variant%s.
 *
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    SimplePileupReader const& reader = SimplePileupReader{}
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a (m)pileup file as Variant%s.
 *
 * This uses only the samples at the zero-based indices given in the @p sample_indices list.
 * If @p inverse_sample_indices is `true`, this list is inversed, that is, all sample indices _but_
 * the ones listed are included in the output.
 *
 * For example, given a list `{ 0, 2 }` and a file with 4 samples, only the first and the third
 * sample will be in the output. When however @p inverse_sample_indices is also set, then the output
 * will contain the second and fourth sample.
 *
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices = false,
    SimplePileupReader const& reader = SimplePileupReader{}
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a (m)pileup file as Variant%s.
 *
 * This uses only the samples at the indices where the @p sample_filter is `true`.
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_pileup_file(
    std::string const& filename,
    std::vector<bool> const& sample_filter,
    SimplePileupReader const& reader = SimplePileupReader{}
);

// =================================================================================================
//     Sync
// =================================================================================================

/**
 * @brief Create a VariantInputStream to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 */
VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 *
 * This uses only the samples at the zero-based indices given in the @p sample_indices list.
 * If @p inverse_sample_indices is `true`, this list is inversed, that is, all sample indices _but_
 * the ones listed are included in the output.
 *
 * For example, given a list `{ 0, 2 }` and a file with 4 samples, only the first and the third
 * sample will be in the output. When however @p inverse_sample_indices is also set, then the output
 * will contain the second and fourth sample.
 */
VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename,
    std::vector<size_t> const& sample_indices,
    bool inverse_sample_indices = false
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a PoPoolation2 sync file
 * as Variant%s.
 *
 * This uses only the samples at the indices where the @p sample_filter is `true`.
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_sync_file(
    std::string const& filename,
    std::vector<bool> const& sample_filter
);

// =================================================================================================
//     Frequency Table
// =================================================================================================

/**
 * @brief Create a VariantInputStream to iterate the contents of a frequency table file as
 * Variant%s.
 *
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_frequency_table_file(
    std::string const& filename,
    char separator_char = '\t',
    FrequencyTableInputStream const& reader = FrequencyTableInputStream{}
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a frequency table file as
 * Variant%s.
 *
 * Additionally, this version of the function takes a list of @p sample_names which are used as
 * filter so that only those samples (columns of the frequency table) are evaluated and accessible -
 * or, if @p inverse_sample_names is set to `true`, instead all <i>but</i> those samples.
 *
 * Optionally, this takes a @p reader with settings to be used.
 */
VariantInputStream make_variant_input_stream_from_frequency_table_file(
    std::string const& filename,
    std::vector<std::string> const& sample_names_filter,
    bool inverse_sample_names_filter = false,
    char separator_char = '\t',
    FrequencyTableInputStream const& reader = FrequencyTableInputStream{}
);

// =================================================================================================
//     VCF
// =================================================================================================

// Only available if compiled with htslib
#ifdef GENESIS_HTSLIB

/**
 * @brief Parameters to use when streaming through a VCF file as Variant%s.
 *
 * This is used in make_variant_input_stream_from_pool_vcf_file() and
 * make_variant_input_stream_from_individual_vcf_file() to set the paramters for the reading
 * and conversion to Variant.
 *
 * If @p only_snps is set, only SNP records are processed; that is, all non-SNPs (indels and others)
 * are ignored. If @p only_biallelic_snps is set to `true`, this is further restricted to only contain
 * biallelic SNPs, that is, only positions with exactly one alternative allele.
 *
 * If @p only_filter_pass is set to `true`, only those positions are considered that
 * have the FILTER field set to "PASS" (or missing "."). That is, all variants that did not pass a
 * filter in the VCF processing are skipped. If @p only_filter_pass is set to `false` however
 * (default), these position are used, but the VariantFilterTag::kNotPassed is set to indicate
 * the failing filter.
 *
 * The list of @p sample_names is used as a filter so that only those samples (columns of the VCF
 * records) are evaluated and accessible - or, if @p inverse_sample_names is set to `true`,
 * instead all <i>but</i> those samples.
 */
struct VariantInputStreamFromVcfParams
{
    // Types of records to use or discard.
    bool only_snps            = false;
    bool only_biallelic_snps  = false;
    bool only_filter_pass     = false;

    // Subset the samples.
    std::vector<std::string> sample_names;
    bool inverse_sample_names = false;
};

/**
 * @brief Create a VariantInputStream to iterate the contents of a VCF file as Variant%s,
 * treating each sample as a pool of individuals.
 *
 * See convert_to_variant_as_pool( VcfRecord const& ) for details on the conversion from
 * VcfRecord to Variant.
 *
 * This function requires the VCF to have the "AD" FORMAT field. It only iterates over those VCF
 * record lines that actually have the "AD" FORMAT provided, as this is the information that we use
 * to convert the samples to Variant%s. All records without that field are skipped.
 *
 * @see See make_variant_input_stream_from_individual_vcf_file() for the function that instead
 * interprets the VCF as usual as a set of individuals.
 */
VariantInputStream make_variant_input_stream_from_pool_vcf_file(
    std::string const& filename,
    VariantInputStreamFromVcfParams const& params = VariantInputStreamFromVcfParams{}
);

/**
 * @brief Create a VariantInputStream to iterate the contents of a VCF file as Variant%s,
 * treating each sample as an individual, and combining them all into one SampleCounts sample.
 *
 * See convert_to_variant_as_individuals( VcfRecord const&, bool ) for details on the conversion
 * from VcfRecord to Variant.
 *
 * @see See make_variant_input_stream_from_pool_vcf_file() for the function that instead
 * interprets each sample (column) as a pool of individuals, e.g., from pool sequencing.
 */
VariantInputStream make_variant_input_stream_from_individual_vcf_file(
    std::string const& filename,
    VariantInputStreamFromVcfParams const& params = VariantInputStreamFromVcfParams{},
    bool use_allelic_depth = false
);

#endif // GENESIS_HTSLIB

} // namespace population
} // namespace genesis

#endif // include guard
